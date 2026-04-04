#pragma once

#include "src/logger/ILogger.h"
#include "src/warehouse/Warehouse.h"
#include <atomic>
#include <memory>

class Supplier
{
public:
	Supplier(std::shared_ptr<Warehouse> warehouse, std::shared_ptr<ILogger> logger);

	void Run(const std::atomic<bool>& isTerminated);
	[[nodiscard]] int GetTotalSupplied() const;

private:
	std::shared_ptr<Warehouse> m_warehouse;
	std::shared_ptr<ILogger> m_logger;
	int m_totalSupplied;
};