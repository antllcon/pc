#include "Simulation.h"
#include "src/actor/ApuActor.h"
#include "src/actor/BurnsActor.h"
#include "src/actor/HomerActor.h"
#include "src/actor/KidActor.h"
#include "src/actor/MargeActor.h"
#include "src/actor/NelsonActor.h"
#include "src/actor/SmithersActor.h"
#include "src/actor/SnakeActor.h"
#include "src/common/Economy.h"

#include <iostream>
#include <thread>

Simulation::Simulation(Money initialBankCash)
	: m_initBankCash(initialBankCash)
	, m_bank(initialBankCash)
	, m_logger(true)
	, m_isRunning(true)
{
	Init();
}

void Simulation::Init()
{
	AccountId homerId = m_bank.OpenAccount();
	AccountId margeId = m_bank.OpenAccount();
	AccountId apuId = m_bank.OpenAccount();
	AccountId burnsId = m_bank.OpenAccount();
	AccountId snakeId = m_bank.OpenAccount();
	AccountId smithersId = m_bank.OpenAccount();

	m_bank.DepositMoney(burnsId, Economy::BurnsStartingBalance);
	m_bank.DepositMoney(homerId, Economy::HomerStartingBalance);
	m_bank.DepositMoney(apuId, Economy::ApuStartingBalance);

	m_sharedSmithersId = std::make_shared<std::atomic<AccountId>>(smithersId);

	auto apuActor = std::make_shared<ApuActor>(m_bank, apuId, burnsId, m_logger);
	auto bartActor = std::make_shared<KidActor>("Барт", m_bank, apuActor->GetWallet(), m_logger);
	auto lisaActor = std::make_shared<KidActor>("Лиза", m_bank, apuActor->GetWallet(), m_logger);
	auto homerActor = std::make_shared<HomerActor>(m_bank, homerId, margeId, burnsId, bartActor->GetWallet(), lisaActor->GetWallet(), m_logger);
	auto margeActor = std::make_shared<MargeActor>(m_bank, margeId, apuId, m_logger);
	auto burnsActor = std::make_shared<BurnsActor>(m_bank, burnsId, homerId, *m_sharedSmithersId, m_logger);
	auto snakeActor = std::make_shared<SnakeActor>(m_bank, snakeId, homerId, apuId, m_logger);
	auto nelsonActor = std::make_shared<NelsonActor>(m_bank, bartActor->GetWallet(), apuActor->GetWallet(), m_logger);
	auto smithersActor = std::make_shared<SmithersActor>(m_bank, smithersId, apuId, *m_sharedSmithersId, m_logger);

	m_actors = {homerActor, margeActor, bartActor, lisaActor, apuActor, burnsActor, snakeActor, nelsonActor, smithersActor};
}

void Simulation::Run(bool isParallel) const
{
	if (isParallel)
	{
		RunMultiThreaded();
	}
	else
	{
		RunSingleThreaded();
	}
}

void Simulation::Stop()
{
	m_isRunning.store(false, std::memory_order_relaxed);
}

void Simulation::RunSingleThreaded() const
{
	while (m_isRunning.load(std::memory_order_relaxed))
	{
		for (const auto& actor : m_actors)
		{
			actor->Step();
		}
	}
}

void Simulation::RunMultiThreaded() const
{
	std::vector<std::jthread> threads;

	for (const auto& actor : m_actors)
	{
		threads.emplace_back([this, actor]() {
			while (m_isRunning.load(std::memory_order_relaxed))
			{
				actor->Step();
			}
		});
	}
}

void Simulation::PrintResult() const
{
	Money totalActorsCash = 0;
	Money totalBalances = 0;

	for (const auto& actor : m_actors)
	{
		totalActorsCash += actor->GetCashBalance();
		totalBalances += actor->GetBankBalance();
	}

	const Money bankCashCounter = m_bank.GetCash();
	const Money totalSystemMoney = totalActorsCash + totalBalances;

	std::cout << "\n[Баланс участников]\n";
	for (const auto& actor : m_actors)
	{
		std::cout << actor->GetName()
				  << ": Наличные =\t" << actor->GetCashBalance()
				  << "\t| На счету =\t" << actor->GetBankBalance()
				  << std::endl;
	}

	std::cout << "\n[Simulation result]\n";
	std::cout << "Выполнено банковских операций: " << m_bank.GetOperationsCount() << std::endl;
	std::cout << "Наличные в кошельках: " << totalActorsCash << std::endl;
	std::cout << "Счетчик наличных в банке: " << bankCashCounter << std::endl;
	std::cout << "Сумма на счетах: " << totalBalances << std::endl;
	std::cout << "Всего денег в системе: " << totalSystemMoney << " (Изначально: " << m_initBankCash << ")" << std::endl;

	bool isCashConsistent = (totalActorsCash == bankCashCounter);
	bool isTotalConsistent = (totalSystemMoney == m_initBankCash);

	if (isCashConsistent && isTotalConsistent)
	{
		std::cout << "[Success] Согласованность данных подтверждена" << std::endl;
	}
	else
	{
		std::cout << "[Error]: Нарушена согласованность системы" << std::endl;
	}
}