#include "Client.h"
#include <chrono>
#include <random>
#include <string>
#include <thread>

namespace
{
int GenerateRandomPurchase(int min, int max)
{
	thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}
} // namespace

Client::Client(std::shared_ptr<Warehouse> warehouse, std::shared_ptr<ILogger> logger)
	: m_warehouse(std::move(warehouse))
	, m_logger(std::move(logger))
	, m_totalBought(0)
{
}

void Client::Run(const std::atomic<bool>& isTerminated)
{
	while (!isTerminated.load(std::memory_order_relaxed))
	{
		int itemsToBuy = GenerateRandomPurchase(5, 30);

		if (m_warehouse->Take(itemsToBuy))
		{
			m_totalBought += itemsToBuy;
			m_logger->Log("Покупатель забрал: " + std::to_string(itemsToBuy));
		}
		else
		{
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(GenerateRandomPurchase(200, 500)));
	}
}

int Client::GetTotalBought() const
{
	return m_totalBought;
}