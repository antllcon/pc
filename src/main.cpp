#include "histogram/Histogram.h"
#include "histogram/HistogramBuilder.h"
#include "image/Image.h"
#include "src/console/ConsoleArgs.h"
#include "src/console/ConsoleEncoding.h"
#include "src/logger/logger/ConsoleLogger.h"
#include "src/logger/timer/ScopedTimer.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <syncstream>

namespace
{
std::filesystem::path BuildOutputPath(const std::filesystem::path& imagePath)
{
	return imagePath.parent_path() / (imagePath.stem().string() + "_histogram.txt");
}

void AssertIsFileOpened(const std::ofstream& file, const std::filesystem::path& path)
{
	if (!file.is_open())
	{
		throw std::runtime_error("Не удалось создать файл: " + path.string());
	}
}
} // namespace

int main(const int argc, char* argv[])
{
	try
	{
		ConsoleEncoding consoleEncoding;
		auto logger = std::make_shared<ConsoleLogger>(true);

		auto imagePath = ConsoleArgs::ExtractImagePath(argc, argv);
		auto image = Image(imagePath.string());

		{
			const ScopedTimer timer("построение гистограммы", logger);
			auto histogram = HistogramBuilder::Build(image);

			auto outputPath = BuildOutputPath(imagePath);
			std::ofstream file(outputPath);
			AssertIsFileOpened(file, outputPath);
			file << histogram;
		}
	}
	catch (const std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Error]\t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
