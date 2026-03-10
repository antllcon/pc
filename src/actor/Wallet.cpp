#include "Wallet.h"

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

void AssertAreWalletsDifferent(const Wallet* src, const Wallet* dst)
{
	if (src == dst)
	{
		throw BankOperationError("Нельзя передать наличные в тот же самый кошелек");
	}
}
} // namespace

Wallet::Wallet(Money initialCash)
	: m_cash(initialCash)
{
	AssertIsAmountValid(initialCash);
}

Money Wallet::GetBalance() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_cash;
}

void Wallet::AddCash(const Money amount)
{
	AssertIsAmountValid(amount);

	std::lock_guard<std::mutex> lock(m_mutex);
	m_cash += amount;
}

void Wallet::TakeCash(const Money amount)
{
	AssertIsAmountValid(amount);

	std::lock_guard<std::mutex> lock(m_mutex);

	AssertIsBalanceSufficient(m_cash, amount);
	m_cash -= amount;
}

bool Wallet::TryTakeCash(Money amount)
{
	AssertIsAmountValid(amount);

	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_cash < amount)
	{
		return false;
	}

	m_cash -= amount;
	return true;
}

void Wallet::TransferTo(Wallet& targetWallet, const Money amount)
{
	AssertIsAmountValid(amount);
	AssertAreWalletsDifferent(this, &targetWallet);

	std::scoped_lock lock(m_mutex, targetWallet.m_mutex);

	AssertIsBalanceSufficient(m_cash, amount);

	m_cash -= amount;
	targetWallet.m_cash += amount;
}