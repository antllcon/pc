#include "Warehouse.h"
#include <stdexcept>

namespace
{
void AssertIsPositive(int value)
{
	if (value <= 0)
	{
		throw std::invalid_argument("Значение должно быть строго положительным");
	}
}

void AssertIsWithinCapacity(int count, int capacity)
{
	if (count > capacity)
	{
		throw std::invalid_argument("Количество превышает общую вместимость склада");
	}
}
} // namespace

Warehouse::Warehouse(int capacity)
	: m_capacity(capacity)
	, m_itemsCount(0)
	, m_isStopped(false)
{
	AssertIsPositive(m_capacity);
}

bool Warehouse::Add(int count)
{
	AssertIsPositive(count);
	AssertIsWithinCapacity(count, m_capacity);

	std::unique_lock<std::mutex> lock(m_mutex);

	m_cvSuppliers.wait(lock, [this, count] {
		return m_isStopped || (m_itemsCount + count <= m_capacity);
	});

	if (m_isStopped)
	{
		return false;
	}

	m_itemsCount += count;
	m_cvClients.notify_all();

	return true;
}

bool Warehouse::Take(int count)
{
	AssertIsPositive(count);
	AssertIsWithinCapacity(count, m_capacity);

	std::unique_lock<std::mutex> lock(m_mutex);

	m_cvClients.wait(lock, [this, count] {
		return m_isStopped || (m_itemsCount >= count);
	});

	if (m_isStopped)
	{
		return false;
	}

	m_itemsCount -= count;
	m_cvSuppliers.notify_all();

	return true;
}

int Warehouse::GetCount() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_itemsCount;
}

void Warehouse::Stop()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_isStopped = true;
	m_cvClients.notify_all();
	m_cvSuppliers.notify_all();
}