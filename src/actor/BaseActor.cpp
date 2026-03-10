#include "BaseActor.h"

namespace
{
void AssertHasAccount(bool hasAccount)
{
	if (!hasAccount)
	{
		throw std::logic_error("Попытка доступа к банковскому счету у персонажа без счета");
	}
}
} // namespace

BaseActor::BaseActor(
	std::string name,
	Bank& bank,
	const std::optional<AccountId> id,
	ILogger& logger)
	: m_name(std::move(name))
	, m_bank(bank)
	, m_accountId(id)
	, m_logger(logger)
{
}

Money BaseActor::GetCashBalance() const
{
	return m_wallet.GetBalance();
}

Money BaseActor::GetBankBalance() const
{
	if (m_accountId.has_value())
	{
		try
		{
			return m_bank.GetAccountBalance(m_accountId.value());
		}
		catch (const AccountNotFoundError&)
		{
			return 0;
		}
	}

	return 0;
}

std::string BaseActor::GetName() const
{
	return m_name;
}

Wallet& BaseActor::GetWallet()
{
	return m_wallet;
}

AccountId BaseActor::GetAccountId() const
{
	AssertHasAccount(m_accountId.has_value());
	return m_accountId.value();
}