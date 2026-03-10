#include "simulation/Simulation.h"
#include <chrono>
#include <csignal>
#include <iostream>
#include <syncstream>
#include <windows.h>

namespace
{
std::atomic<bool> g_stopRequested{false};

void SignalHandler(int)
{
	g_stopRequested.store(true, std::memory_order_relaxed);
}

void AssertIsOsApiSuccessful(BOOL result)
{
	if (!result)
	{
		throw std::runtime_error("Ошибка вызова API операционной системы");
	}
}

class ConsoleCodePageRaii
{
public:
	ConsoleCodePageRaii()
		: m_oldOutCp(GetConsoleOutputCP())
		, m_oldCp(GetConsoleCP())
	{
		AssertIsOsApiSuccessful(SetConsoleOutputCP(CP_UTF8));
		AssertIsOsApiSuccessful(SetConsoleCP(CP_UTF8));
	}

	~ConsoleCodePageRaii()
	{
		SetConsoleOutputCP(m_oldOutCp);
		SetConsoleCP(m_oldCp);
	}

	ConsoleCodePageRaii(const ConsoleCodePageRaii&) = delete;
	ConsoleCodePageRaii& operator=(const ConsoleCodePageRaii&) = delete;

private:
	UINT m_oldOutCp;
	UINT m_oldCp;
};
} // namespace

int main(int argc, char* argv[])
{
	try
	{
		ConsoleCodePageRaii consoleGuard;

		std::signal(SIGINT, SignalHandler);
		std::signal(SIGTERM, SignalHandler);

		auto duration = std::chrono::seconds(10);
		bool isParallel = true;

		if (argc > 1)
		{
			duration = std::chrono::seconds(std::stoi(argv[1]));
		}
		if (argc > 2)
		{
			isParallel = std::stoi(argv[2]) != 0;
		}

		Simulation engine(5000);

		std::jthread engineThread([&engine, isParallel]() {
			engine.Run(isParallel);
		});

		const auto startTime = std::chrono::steady_clock::now();

		while (!g_stopRequested.load(std::memory_order_relaxed))
		{
			if (std::chrono::steady_clock::now() - startTime >= duration)
			{
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		engine.Stop();
		engineThread.join();

		engine.PrintResult();
	}
	catch (std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Error] " << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}