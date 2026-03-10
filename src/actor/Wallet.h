#pragma once

#include "src/common/Types.h"
#include <mutex>

class Wallet
{
public:
	explicit Wallet(Money initialCash = 0);

	Wallet(const Wallet&) = delete;
	Wallet& operator=(const Wallet&) = delete;
	Wallet(Wallet&&) = delete;
	Wallet& operator=(Wallet&&) = delete;

	[[nodiscard]] Money GetBalance() const;

	void AddCash(Money amount);
	void TakeCash(Money amount);
	[[nodiscard]] bool TryTakeCash(Money amount);

	void TransferTo(Wallet& targetWallet, Money amount);

private:
	Money m_cash;
	mutable std::mutex m_mutex;
};