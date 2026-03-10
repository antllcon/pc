#include "SmithersActor.h"

#include "src/common/Economy.h"

#include <random>

namespace
{
const std::string SMITHERS_NAME = "Смиттерс";

bool ShouldTriggerParanoia()
{
	thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<int> distribution(1, 10);
	return distribution(generator) == 1;
}
}

SmithersActor::SmithersActor(
	Bank& bank,
	AccountId accountId,
	const AccountId apuAccountId,
	std::atomic<AccountId>& sharedBurnsAccountId,
	ILogger& logger)
	: BaseActor(SMITHERS_NAME, bank, accountId, logger)
	, m_apuAccountId(apuAccountId)
	, m_sharedBurnsAccountId(sharedBurnsAccountId)
{
}

void SmithersActor::Step()
{
	if (m_bank.TrySendMoney(GetAccountId(), m_apuAccountId, Economy::SmithersGroceries))
	{
		m_logger.Log(GetName() + " купил продукты у Апу");
	}

	if (ShouldTriggerParanoia())
	{
		Money withdrawnCash = m_bank.CloseAccount(GetAccountId());
		m_logger.Log(GetName() + " запаниковал и закрыл счет");

		m_accountId = m_bank.OpenAccount();
		m_bank.DepositMoney(GetAccountId(), withdrawnCash);

		m_sharedBurnsAccountId.store(GetAccountId(), std::memory_order_relaxed);
		m_logger.Log(GetName() + " открыл новый счет и перевел деньги");
	}
}