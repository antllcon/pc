#pragma once

#include <condition_variable>
#include <mutex>

class Warehouse
{
public:
	explicit Warehouse(int capacity);

	bool Add(int count);
	bool Take(int count);
	int GetCount() const;
	void Stop();

private:
	int m_capacity;
	int m_itemsCount;
	mutable std::mutex m_mutex;
	std::condition_variable m_cvClients;
	std::condition_variable m_cvSuppliers;
	bool m_isStopped;
};