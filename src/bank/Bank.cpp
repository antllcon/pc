#include "Bank.h"

namespace
{
void AssertIsAmountValid(const Money amount)
{
	if (amount < 0)
	{
		throw InvalidAmountError();
	}
}

void AssertIsBalanceSufficient(const Money balance, const Money amount)
{
	if (balance < amount)
	{
		throw InsufficientFundsError();
	}
}

void AssertIsCashSufficient(const Money cash, const Money amount)
{
	if (cash < amount)
	{
		throw BankOperationError("Недостаточно наличных денег в обороте");
	}
}

void AssertIsAccountValid(const bool isValid)
{
	if (!isValid)
	{
		throw AccountNotFoundError();
	}
}

void AssertAreAccountsDifferent(const AccountId idSrc, const AccountId idDst)
{
	if (idSrc == idDst)
	{
		throw BankOperationError("Счета отправителя и получателя совпадают");
	}
}

void RegisterOperation(std::atomic<OperationsCount>& counter)
{
	counter.fetch_add(1, std::memory_order_relaxed);
}

void TakeCashFromCirculation(std::atomic<Money>& cash, const Money amount)
{
	auto currentCash = cash.load(std::memory_order_relaxed);

	while (true)
	{
		AssertIsCashSufficient(currentCash, amount);

		if (cash.compare_exchange_weak(currentCash, currentCash - amount, std::memory_order_relaxed))
		{
			break;
		}
	}
}
} // namespace

Bank::Bank(const Money cash)
	: m_cash(cash)
	, m_nextId(1)
	, m_operationsCount(0)
{
}

OperationsCount Bank::GetOperationsCount() const noexcept
{
	return m_operationsCount.load(std::memory_order_relaxed);
}

Money Bank::GetCash() const
{
	RegisterOperation(m_operationsCount);
	return m_cash.load(std::memory_order_relaxed);
}

Money Bank::GetAccountBalance(const AccountId id) const
{
	std::shared_lock<std::shared_mutex> bankLock(m_bankMutex);

	const auto data = m_accounts.find(id);
	AssertIsAccountValid(data != m_accounts.end());

	std::lock_guard<std::mutex> accountLock(data->second->mutex);
	const auto currentBalance = data->second->balance;

	RegisterOperation(m_operationsCount);
	return currentBalance;
}

void Bank::SendMoney(
	const AccountId idSrc,
	const AccountId idDst,
	const Money amount)
{
	AssertIsAmountValid(amount);
	AssertAreAccountsDifferent(idSrc, idDst);
	std::shared_lock<std::shared_mutex> bankLock(m_bankMutex);

	const auto dataSrc = m_accounts.find(idSrc);
	AssertIsAccountValid(dataSrc != m_accounts.end());

	const auto dataDst = m_accounts.find(idDst);
	AssertIsAccountValid(dataDst != m_accounts.end());

	std::scoped_lock accountsLock(
		dataSrc->second->mutex,
		dataDst->second->mutex);

	AssertIsBalanceSufficient(dataSrc->second->balance, amount);

	dataSrc->second->balance -= amount;
	dataDst->second->balance += amount;

	RegisterOperation(m_operationsCount);
}

void Bank::WithdrawMoney(const AccountId id, const Money amount)
{
	AssertIsAmountValid(amount);

	std::shared_lock<std::shared_mutex> bankLock(m_bankMutex);

	const auto data = m_accounts.find(id);
	AssertIsAccountValid(data != m_accounts.end());

	std::lock_guard<std::mutex> accountLock(data->second->mutex);

	AssertIsBalanceSufficient(data->second->balance, amount);
	data->second->balance -= amount;
	m_cash.fetch_add(amount, std::memory_order_relaxed);

	RegisterOperation(m_operationsCount);
}

void Bank::DepositMoney(const AccountId id, const Money amount)
{
	AssertIsAmountValid(amount);

	std::shared_lock<std::shared_mutex> bankLock(m_bankMutex);

	const auto data = m_accounts.find(id);
	AssertIsAccountValid(data != m_accounts.end());

	TakeCashFromCirculation(m_cash, amount);

	std::lock_guard<std::mutex> accountLock(data->second->mutex);
	data->second->balance += amount;

	RegisterOperation(m_operationsCount);
}

bool Bank::TrySendMoney(
	const AccountId idSrc,
	const AccountId idDst,
	const Money amount)
{
	AssertIsAmountValid(amount);
	AssertAreAccountsDifferent(idSrc, idDst);
	std::shared_lock<std::shared_mutex> bankLock(m_bankMutex);

	const auto dataSrc = m_accounts.find(idSrc);
	AssertIsAccountValid(dataSrc != m_accounts.end());

	const auto dataDst = m_accounts.find(idDst);
	AssertIsAccountValid(dataDst != m_accounts.end());

	std::scoped_lock accountsLock(
		dataSrc->second->mutex,
		dataDst->second->mutex);

	if (dataSrc->second->balance < amount)
	{
		RegisterOperation(m_operationsCount);
		return false;
	}

	dataSrc->second->balance -= amount;
	dataDst->second->balance += amount;

	RegisterOperation(m_operationsCount);
	return true;
}

bool Bank::TryWithdrawMoney(const AccountId id, const Money amount)
{
	AssertIsAmountValid(amount);

	std::shared_lock<std::shared_mutex> bankLock(m_bankMutex);

	const auto data = m_accounts.find(id);
	AssertIsAccountValid(data != m_accounts.end());

	std::lock_guard<std::mutex> accountLock(data->second->mutex);

	if (data->second->balance < amount)
	{
		RegisterOperation(m_operationsCount);
		return false;
	}

	data->second->balance -= amount;
	m_cash.fetch_add(amount, std::memory_order_relaxed);

	RegisterOperation(m_operationsCount);
	return true;
}

AccountId Bank::OpenAccount()
{
	std::unique_lock<std::shared_mutex> bankLock(m_bankMutex);

	AccountId id = m_nextId.fetch_add(1, std::memory_order_relaxed);
	m_accounts.emplace(id, std::make_unique<Account>());

	RegisterOperation(m_operationsCount);
	return id;
}

Money Bank::CloseAccount(const AccountId id)
{
	std::unique_lock<std::shared_mutex> bankLock(m_bankMutex);

	const auto data = m_accounts.find(id);
	AssertIsAccountValid(data != m_accounts.end());

	const auto lastBalance = data->second->balance;
	m_cash.fetch_add(lastBalance, std::memory_order_relaxed);

	m_accounts.erase(data);

	RegisterOperation(m_operationsCount);
	return lastBalance;
}