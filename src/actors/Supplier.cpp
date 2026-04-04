#include "Supplier.h"
#include <chrono>
#include <random>
#include <string>
#include <thread>

namespace
{
int GenerateRandomCount(int min, int max)
{
	thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}
}

Supplier::Supplier(std::shared_ptr<Warehouse> warehouse, std::shared_ptr<ILogger> logger)
	: m_warehouse(std::move(warehouse))
	, m_logger(std::move(logger))
	, m_totalSupplied(0)
{
}

void Supplier::Run(const std::atomic<bool>& isTerminated)
{
	while (!isTerminated.load(std::memory_order_relaxed))
	{
		int itemsToSupply = GenerateRandomCount(10, 50);

		if (m_warehouse->Add(itemsToSupply))
		{
			m_totalSupplied += itemsToSupply;
			m_logger->Log("Поставщик добавил: " + std::to_string(itemsToSupply));
		}
		else
		{
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(GenerateRandomCount(100, 300)));
	}
}

int Supplier::GetTotalSupplied() const
{
	return m_totalSupplied;
}