#include "audio/AudioPlayer.h"
#include "console/ConsoleEncoding.h"
#include "logger/logger/ConsoleLogger.h"
#include "logger/timer/ScopedTimer.h"
#include "parser/TrackParser.h"
#include "synth/Synthesizer.h"

#include <csignal>
#include <filesystem>
#include <iostream>
#include <memory>
#include <syncstream>
#include <windows.h>

namespace
{
void AssertArgumentCount(const int argc)
{
	if (argc != 2)
	{
		throw std::invalid_argument("Формат вызова: start file-name + [.txt]");
	}
}

std::filesystem::path BuildFilePath(const char* argument)
{
	std::filesystem::path path(argument);

	if (!path.has_extension())
	{
		path.replace_extension(".txt");
	}

	return path;
}
} // namespace

int main(const int argc, char* argv[])
{
	try
	{
		ConsoleEncoding consoleEncoding;
		AssertArgumentCount(argc);

		auto logger = std::make_shared<ConsoleLogger>(true);
		auto path = BuildFilePath(argv[1]);

		MusicComposition composition;
		{
			ScopedTimer timer("Парсинг файла", logger);
			composition = TrackParser::Parse(path);
		}

		std::vector<double> audioBuffer;
		{
			ScopedTimer timer("Синтез аудио", logger);
			audioBuffer = Synthesizer::Render(composition);
		}

		ScopedTimer timer("Воспроизведение", logger);
		AudioPlayer::Play(audioBuffer);
	}
	catch (const std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Error]\t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}