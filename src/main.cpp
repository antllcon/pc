#include "src/audio/capture/AudioCapture.h"
#include "src/audio/playback/AudioPlayback.h"
#include "src/console/ConsoleEncoding.h"
#include "src/logger/logger/ConsoleLogger.h"
#include "src/net/receiver/RadioReceiver.h"
#include "src/net/station/RadioStation.h"
#include "src/visualizer/VisualizerThread.h"

#include <iostream>
#include <memory>
#include <string>
#include <syncstream>

namespace
{
void AssertArgumentsValid(int argc)
{
	if (argc != 2 && argc != 3)
	{
		throw std::invalid_argument(
			"Формат вызова:\n"
			"  Радиостанция: radio PORT\n"
			"  Радиоприёмник: radio ADDRESS PORT");
	}
}

uint16_t ParsePort(const char* arg)
{
	try
	{
		const int value = std::stoi(arg);
		if (value < 1 || value > 65535)
		{
			throw std::out_of_range("");
		}
		return static_cast<uint16_t>(value);
	}
	catch (const std::exception&)
	{
		throw std::invalid_argument(
			std::string("Неверный номер порта: ") + arg);
	}
}

void RunServer(uint16_t port, std::shared_ptr<ILogger> logger)
{
	logger->Log("Запуск радиостанции на порту " + std::to_string(port));

	auto captureQueue    = std::make_shared<CaptureQueue>();
	auto visualizerQueue = std::make_shared<VisualizerQueue>();

	AudioCapture     capture(captureQueue);
	VisualizerThread visualizer(visualizerQueue);
	RadioStation     station(port, captureQueue, visualizerQueue, logger);

	station.Run();
}

void RunClient(
	const std::string&       address,
	uint16_t                 port,
	std::shared_ptr<ILogger> logger)
{
	logger->Log("Подключение к радиостанции " + address + ":" + std::to_string(port));

	auto playbackQueue   = std::make_shared<PlaybackQueue>();
	auto visualizerQueue = std::make_shared<VisualizerQueue>();

	AudioPlayback    playback(playbackQueue);
	VisualizerThread visualizer(visualizerQueue);
	RadioReceiver    receiver(address, port, playbackQueue, visualizerQueue, logger);

	receiver.Run();
}
} // namespace

int main(const int argc, char* argv[])
{
	try
	{
		ConsoleEncoding consoleEncoding;
		AssertArgumentsValid(argc);

		auto logger = std::make_shared<ConsoleLogger>(true);

		if (argc == 2)
		{
			RunServer(ParsePort(argv[1]), logger);
		}
		else
		{
			RunClient(argv[1], ParsePort(argv[2]), logger);
		}
	}
	catch (const std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Ошибка] " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
