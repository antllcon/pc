#include "SnakeActor.h"
#include "src/common/Economy.h"

namespace
{
const std::string SNAKE_NAME = "Змей";
} // namespace

SnakeActor::SnakeActor(
	Bank& bank,
	AccountId accountId,
	AccountId homerAccountId,
	AccountId apuAccountId,
	ILogger& logger)
	: BaseActor(SNAKE_NAME, bank, accountId, logger)
	, m_homerAccountId(homerAccountId)
	, m_apuAccountId(apuAccountId)
{
}

void SnakeActor::Step()
{
	if (m_bank.TrySendMoney(m_homerAccountId, GetAccountId(), Economy::SnakeStealAmount))
	{
		m_logger.Log(GetName() + " украл деньги со счета Гомера");
	}

	if (m_bank.TrySendMoney(GetAccountId(), m_apuAccountId, Economy::SnakeGroceries))
	{
		m_logger.Log(GetName() + " купил продукты у Апу");
	}
}