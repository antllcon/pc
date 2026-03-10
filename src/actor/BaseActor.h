#pragma once
#include "src/actor/IActor.h"
#include "src/actor/Wallet.h"
#include "src/bank/Bank.h"
#include "src/logger/ILogger.h"

#include <optional>
#include <string>

class BaseActor : public IActor
{
public:
	BaseActor(
		std::string name,
		Bank& bank,
		std::optional<AccountId> id,
		ILogger& logger);

	[[nodiscard]] Money GetCashBalance() const override;
	[[nodiscard]] Money GetBankBalance() const override;
	[[nodiscard]] std::string GetName() const override;
	[[nodiscard]] Wallet& GetWallet();

protected:
	[[nodiscard]] AccountId GetAccountId() const;

	std::string m_name;
	Wallet m_wallet;
	Bank& m_bank;
	std::optional<AccountId> m_accountId;
	ILogger& m_logger;
};