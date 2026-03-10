#pragma once

#include "src/bank/Bank.h"
#include "src/actor/IActor.h"
#include "src/logger/ConsoleLogger.h"
#include <vector>
#include <memory>
#include <atomic>

class Simulation
{
public:
	explicit Simulation(Money initialBankCash);

	void Run(bool isParallel) const;
	void Stop();
	void PrintResult() const;

private:
	void Init();
	void RunSingleThreaded() const;
	void RunMultiThreaded() const;

	Money m_initBankCash;
	Bank m_bank;
	ConsoleLogger m_logger;

	std::vector<std::shared_ptr<IActor>> m_actors;
	std::shared_ptr<std::atomic<AccountId>> m_sharedSmithersId;
	std::atomic<bool> m_isRunning;
};