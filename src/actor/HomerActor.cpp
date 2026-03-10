#include "HomerActor.h"
#include "src/common/Economy.h"

namespace
{
constexpr std::string HOMER_NAME = "Homer";
} // namespace

HomerActor::HomerActor(
	Bank& bank,
	AccountId accountId,
	const AccountId margeAccountId,
	const AccountId powerPlantAccountId,
	Wallet& bartWallet,
	Wallet& lisaWallet,
	ILogger& logger)
	: BaseActor(HOMER_NAME, bank, accountId, logger)
	, m_margeAccountId(margeAccountId)
	, m_powerPlantAccountId(powerPlantAccountId)
	, m_bartWallet(bartWallet)
	, m_lisaWallet(lisaWallet)
{
}

void HomerActor::Step()
{
	if (m_bank.TrySendMoney(GetAccountId(), m_margeAccountId, Economy::HomerTransferToMarge))
	{
		m_logger.Log(GetName() + " перевел деньги Мардж");
	}

	if (m_bank.TrySendMoney(GetAccountId(), m_powerPlantAccountId, Economy::HomerElectricityBill))
	{
		m_logger.Log(GetName() + " оплатил электричество");
	}

	if (m_bank.TryWithdrawMoney(GetAccountId(), Economy::HomerCashWithdrawal))
	{
		m_wallet.AddCash(Economy::HomerCashWithdrawal);
		m_logger.Log(GetName() + " снял наличные");
	}

	try
	{
		m_wallet.TransferTo(m_bartWallet, Economy::HomerKidAllowance);
		m_logger.Log(GetName() + " дал карманные деньги Барту");
	}
	catch (const InsufficientFundsError&)
	{
	}

	try
	{
		m_wallet.TransferTo(m_lisaWallet, Economy::HomerKidAllowance);
		m_logger.Log(GetName() + " дал карманные деньги Лизе");
	}
	catch (const InsufficientFundsError&)
	{
	}
}