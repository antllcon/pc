#pragma once

#include <array>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <stdexcept>

template <typename T, size_t Capacity>
class AudioQueue
{
public:
	void Push(T item)
	{
		std::unique_lock lock(m_mutex);
		m_notFull.wait(lock, [this] { return m_count < Capacity || m_closed; });
		if (m_closed)
		{
			return;
		}
		m_buffer[m_tail] = std::move(item);
		m_tail           = (m_tail + 1) % Capacity;
		++m_count;
		m_notEmpty.notify_one();
	}

	bool TryPush(T item)
	{
		std::unique_lock lock(m_mutex);
		if (m_count >= Capacity || m_closed)
		{
			return false;
		}
		m_buffer[m_tail] = std::move(item);
		m_tail           = (m_tail + 1) % Capacity;
		++m_count;
		m_notEmpty.notify_one();
		return true;
	}

	T Pop()
	{
		std::unique_lock lock(m_mutex);
		m_notEmpty.wait(lock, [this] { return m_count > 0 || m_closed; });
		if (m_count == 0)
		{
			throw std::runtime_error("Очередь закрыта");
		}
		T item      = std::move(m_buffer[m_head]);
		m_head      = (m_head + 1) % Capacity;
		--m_count;
		m_notFull.notify_one();
		return item;
	}

	std::optional<T> TryPop()
	{
		std::unique_lock lock(m_mutex);
		if (m_count == 0)
		{
			return std::nullopt;
		}
		T item = std::move(m_buffer[m_head]);
		m_head = (m_head + 1) % Capacity;
		--m_count;
		m_notFull.notify_one();
		return item;
	}

	void Close()
	{
		std::unique_lock lock(m_mutex);
		m_closed = true;
		m_notEmpty.notify_all();
		m_notFull.notify_all();
	}

	bool IsClosed() const
	{
		std::unique_lock lock(m_mutex);
		return m_closed;
	}

private:
	std::array<T, Capacity>  m_buffer{};
	size_t                   m_head{0};
	size_t                   m_tail{0};
	size_t                   m_count{0};
	bool                     m_closed{false};
	mutable std::mutex       m_mutex;
	std::condition_variable  m_notEmpty;
	std::condition_variable  m_notFull;
};
