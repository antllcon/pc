#pragma once

#include <memory>
#include <mutex>
#include <stack>

template <typename T>
class MutexStack
{
public:
	MutexStack() = default;
	~MutexStack() = default;

	MutexStack(const MutexStack& other);
	MutexStack& operator=(const MutexStack& other) = delete;

	void Push(T value);
	void Pop();

	[[nodiscard]] T Top() const;
	[[nodiscard]] bool Empty() const;

private:
	std::stack<T> m_storage;
	mutable std::mutex m_mutex;
};

template <typename T>
MutexStack<T>::MutexStack(const MutexStack& other)
{
	std::lock_guard<std::mutex> lock(other.m_mutex);
	m_storage = other.m_storage;
}

template <typename T>
void MutexStack<T>::Push(T value)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_storage.push(std::move(value));
}

template <typename T>
void MutexStack<T>::Pop()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_storage.empty())
	{
		throw std::runtime_error("Невозможно удалить элемент: стек пуст");
	}
	m_storage.pop();
}

template <typename T>
T MutexStack<T>::Top() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_storage.empty())
	{
		throw std::runtime_error("Невозможно получить верхний элемент: стек пуст");
	}
	return m_storage.top();
}

template <typename T>
bool MutexStack<T>::Empty() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_storage.empty();
}