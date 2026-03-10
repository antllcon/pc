#pragma once
#include "src/common/Types.h"

#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

class Bank
{
public:
	explicit Bank(Money cash);

	Bank(const Bank&) = delete;
	Bank& operator=(const Bank&) = delete;
	Bank(Bank&&) = delete;
	Bank& operator=(Bank&&) = delete;

	[[nodiscard]] OperationsCount GetOperationsCount() const noexcept;

	[[nodiscard]] Money GetCash() const;
	[[nodiscard]] Money GetAccountBalance(AccountId id) const;

	void SendMoney(AccountId idSrc, AccountId idDst, Money amount);
	void WithdrawMoney(AccountId id, Money amount);
	void DepositMoney(AccountId id, Money amount);

	[[nodiscard]] bool TrySendMoney(AccountId idSrc, AccountId idDst, Money amount);
	[[nodiscard]] bool TryWithdrawMoney(AccountId id, Money amount);

	AccountId OpenAccount();
	Money CloseAccount(AccountId id);

private:
	struct Account
	{
		Money balance{0};
		std::mutex mutex;
	};

	using AccountsMap = std::unordered_map<AccountId, std::unique_ptr<Account>>;

	AccountsMap m_accounts;
	std::atomic<Money> m_cash;
	std::atomic<AccountId> m_nextId;
	mutable std::atomic<OperationsCount> m_operationsCount;
	mutable std::shared_mutex m_bankMutex;
};