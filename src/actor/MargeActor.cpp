#include "MargeActor.h"
#include "src/common/Economy.h"

namespace
{
constexpr std::string MARGE_NAME = "Мардж";
} // namespace

MargeActor::MargeActor(
	Bank& bank,
	AccountId accountId,
	const AccountId apuAccountId,
	ILogger& logger)
	: BaseActor(MARGE_NAME, bank, accountId, logger)
	, m_apuAccountId(apuAccountId)
{
}

void MargeActor::Step()
{
	if (m_bank.TrySendMoney(GetAccountId(), m_apuAccountId, Economy::MargeGroceries))
	{
		m_logger.Log(GetName() + " купила продукты у Апу (безнал)");
	}
}