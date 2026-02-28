#include "ThreadPool.h"
#include <iostream>
#include <stdexcept>

namespace
{
void AssertIsThreadCountValid(unsigned count)
{
	if (count == 0)
	{
		throw std::invalid_argument("Пул потоков должен содержать хотя бы один поток");
	}
}

void AssertIsPoolRunning(bool isStopped)
{
	if (isStopped)
	{
		throw std::runtime_error("Добавление задачи в остановленный пул потоков запрещено");
	}
}
} // namespace

ThreadPool::ThreadPool(unsigned numThreads)
{
	AssertIsThreadCountValid(numThreads);

	m_threads.reserve(numThreads);
	for (unsigned i = 0; i < numThreads; ++i)
	{
		m_threads.emplace_back(
			[this]() {
				ThreadMainLoop();
			});
	}
}

ThreadPool::~ThreadPool() noexcept
{
	Wait();
}

void ThreadPool::Dispatch(Task task)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		AssertIsPoolRunning(m_stop);
		m_tasks.push(std::move(task));
	}

	m_condition.notify_one();
}

void ThreadPool::Wait()
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_stop = true;
	}

	m_condition.notify_all();

	for (auto& thread : m_threads)
	{
		if (thread.joinable())
		{
			try
			{
				thread.join();
			}
			catch (const std::system_error& e)
			{
				std::cerr << "Ошибка присоединения потока: " << e.code() << std::endl;
			}
		}
	}
}

void ThreadPool::ThreadMainLoop()
{
	while (true)
	{
		Task task;

		{
			std::unique_lock<std::mutex> lock(m_mutex);

			m_condition.wait(lock, [this]() {
				return m_stop || !m_tasks.empty();
			});

			if (m_stop && m_tasks.empty())
			{
				return;
			}

			task = std::move(m_tasks.front());
			m_tasks.pop();
		}

		try
		{
			task();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Ошибка выполнения задачи: " << e.what() << std::endl;
		}
		catch (...)
		{
			std::cerr << "Неизвестная ошибка выполнения задачи" << std::endl;
		}
	}
}