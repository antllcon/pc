#pragma once

#include "src/logger/ILogger.h"
#include "src/warehouse/Warehouse.h"
#include <atomic>
#include <memory>

class Client
{
public:
	Client(std::shared_ptr<Warehouse> warehouse, std::shared_ptr<ILogger> logger);

	void Run(const std::atomic<bool>& isTerminated);
	[[nodiscard]] int GetTotalBought() const;

private:
	std::shared_ptr<Warehouse> m_warehouse;
	std::shared_ptr<ILogger> m_logger;
	int m_totalBought;
};