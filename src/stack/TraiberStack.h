#pragma once

#include <atomic>
#include <memory>
#include <optional>

template <typename T>
class TreiberStack
{
public:
	TreiberStack() = default;
	~TreiberStack();

	TreiberStack(const TreiberStack&) = delete;
	TreiberStack& operator=(const TreiberStack&) = delete;

	void Push(T value);
	std::optional<T> Pop();

	[[nodiscard]] bool Empty() const;

private:
	struct Node
	{
		T data;
		Node* next;

		explicit Node(T value)
			: data(std::move(value))
			, next(nullptr)
		{
		}
	};

	std::atomic<Node*> m_top{nullptr};
};

template <typename T>
TreiberStack<T>::~TreiberStack()
{
	Node* current = m_top.load(std::memory_order_relaxed);
	while (current != nullptr)
	{
		Node* next = current->next;
		delete current;
		current = next;
	}
}

template <typename T>
void TreiberStack<T>::Push(T value)
{
	Node* newNode = new Node(std::move(value));
	newNode->next = m_top.load(std::memory_order_relaxed);

	while (!m_top.compare_exchange_weak(newNode->next, newNode, std::memory_order_release, std::memory_order_relaxed))
	{
	}
}

template <typename T>
std::optional<T> TreiberStack<T>::Pop()
{
	Node* oldTop = m_top.load(std::memory_order_relaxed);

	while (oldTop != nullptr
	// Вот тут возникает ABA проблема
											// тут нельзя обращаться к oldTop->next (может уже быть удалён)
	&& !m_top.compare_exchange_weak(oldTop, oldTop->next, std::memory_order_acquire, std::memory_order_relaxed))
	{
	}

	if (oldTop == nullptr)
	{
		return std::nullopt;
	}

	T result = std::move(oldTop->data);

	// Нельзя так удалять (проблема use after delete)
	delete oldTop;

	return result;
}

template <typename T>
bool TreiberStack<T>::Empty() const
{
	return m_top.load(std::memory_order_relaxed) == nullptr;
}