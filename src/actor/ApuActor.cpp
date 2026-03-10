#include "ApuActor.h"

#include "src/common/Economy.h"

namespace
{
const std::string APU_NAME = "Инд. Апу";
}

ApuActor::ApuActor(
	Bank& bank,
	AccountId accountId,
	AccountId powerPlantAccountId,
	ILogger& logger)
	: BaseActor(APU_NAME, bank, accountId, logger)
	, m_powerPlantAccountId(powerPlantAccountId)
{
}

void ApuActor::Step()
{
	Money currentCash = m_wallet.GetBalance();

	if (currentCash > 0)
	{
		if (m_wallet.TryTakeCash(currentCash))
		{
			m_bank.DepositMoney(GetAccountId(), currentCash);
			m_logger.Log(GetName() + " внес выручку на счет");
		}
	}

	if (m_bank.TrySendMoney(GetAccountId(), m_powerPlantAccountId, Economy::ApuElectricityBill))
	{
		m_logger.Log(GetName() + " оплатил электричество электростанции");
	}
}