#include "BurnsActor.h"

#include "src/common/Economy.h"

namespace
{
const std::string BURNS_NAME = "Бернс";
}

BurnsActor::BurnsActor(
	Bank& bank,
	AccountId accountId,
	AccountId homerAccountId,
	std::atomic<AccountId>& smithersAccountId,
	ILogger& logger)
	: BaseActor(BURNS_NAME, bank, accountId, logger)
	, m_homerAccountId(homerAccountId)
	, m_smithersAccountId(smithersAccountId)
{
}

void BurnsActor::Step()
{
	if (m_bank.TrySendMoney(GetAccountId(), m_homerAccountId, Economy::HomerSalary))
	{
		m_logger.Log(GetName() + " выплатил зарплату Гомеру");
	}

	try
	{
		AccountId currentSmithersId = m_smithersAccountId.load(std::memory_order_relaxed);

		if (m_bank.TrySendMoney(GetAccountId(), currentSmithersId, Economy::SmithersSalary))
		{
			m_logger.Log(GetName() + " выплатил зарплату Смиттерсу");
		}
	}
	catch (const AccountNotFoundError&)
	{
		m_logger.Log(GetName() + " не смог перевести деньги: счет Смиттерса недействителен");
	}
}