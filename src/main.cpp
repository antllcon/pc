#include "console/ConsoleEncoding.h"
#include "logger/logger/ConsoleLogger.h"
#include "src/actors/Client.h"
#include "src/actors/Supplier.h"
#include "src/warehouse/Warehouse.h"
#include <atomic>
#include <csignal>
#include <iostream>
#include <memory>
#include <string>
#include <syncstream>
#include <thread>
#include <vector>
#include <windows.h>

namespace
{
std::atomic<bool> g_isTerminated{false};

void SignalHandler(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
	{
		g_isTerminated.store(true, std::memory_order_relaxed);
	}
}

void AssertArgumentCount(int argc)
{
	if (argc != 4)
	{
		throw std::invalid_argument("Формат: warehouse NUM_SUPPLIERS NUM_CLIENTS NUM_AUDITORS");
	}
}

int ParseArgument(const char* arg)
{
	int value = std::stoi(arg);
	if (value < 0)
	{
		throw std::invalid_argument("Аргументы потоков не могут быть отрицательными");
	}
	return value;
}
} // namespace

int main(int argc, char* argv[])
{
	std::signal(SIGINT, SignalHandler);
	std::signal(SIGTERM, SignalHandler);

	try
	{
		ConsoleEncoding consoleEncoding;
		AssertArgumentCount(argc);

		const int numSuppliers = ParseArgument(argv[1]);
		const int numClients = ParseArgument(argv[2]);
		const int numAuditors = ParseArgument(argv[3]);

		auto logger = std::make_shared<ConsoleLogger>(true);
		auto warehouse = std::make_shared<Warehouse>(1000);

		std::vector<std::unique_ptr<Supplier>> suppliers;
		std::vector<std::unique_ptr<Client>> clients;
		std::vector<std::jthread> threads;

		for (int i = 0; i < numSuppliers; ++i)
		{
			suppliers.push_back(std::make_unique<Supplier>(warehouse, logger));
			threads.emplace_back(&Supplier::Run, suppliers.back().get(), std::ref(g_isTerminated));
		}

		for (int i = 0; i < numClients; ++i)
		{
			clients.push_back(std::make_unique<Client>(warehouse, logger));
			threads.emplace_back(&Client::Run, clients.back().get(), std::ref(g_isTerminated));
		}

		for (int i = 0; i < numAuditors; ++i)
		{
			threads.emplace_back([warehouse, logger] {
				while (!g_isTerminated.load(std::memory_order_relaxed))
				{
					logger->Log("Инвентаризация, надо тереперь. На складе: " + std::to_string(warehouse->GetCount()));
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			});
		}

		logger->Log("Склад запущен. Ctrl+C для выхода");

		while (!g_isTerminated.load(std::memory_order_relaxed))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		logger->Log("Остановка склада");
		warehouse->Stop();

		threads.clear();

		int totalSupplied = 0;
		for (const auto& supplier : suppliers)
		{
			totalSupplied += supplier->GetTotalSupplied();
		}

		int totalBought = 0;
		for (const auto& client : clients)
		{
			totalBought += client->GetTotalBought();
		}

		std::cout << std::endl;
		std::cout << "Итоги работы" << std::endl;
		std::cout << "Всего привезено: " << totalSupplied << std::endl;
		std::cout << "Всего куплено: " << totalBought << std::endl;
		std::cout << "Остаток на складе: " << warehouse->GetCount() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Ошибка] " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}