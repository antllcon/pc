#include "console/ConsoleArgs.h"
#include "console/ConsoleEncoding.h"
#include "exporter/FileHelper.h"
#include "histogram/Histogram.h"
#include "histogram/HistogramBuilder.h"
#include "image/Image.h"
#include "logger/logger/ConsoleLogger.h"
#include "logger/timer/ScopedTimer.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <syncstream>

namespace
{
std::filesystem::path BuildOutputPath(const std::filesystem::path& imagePath)
{
	return imagePath.parent_path() / (imagePath.stem().string() + "-stats.txt");
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

		auto outputPath = BuildOutputPath(imagePath);

		{
			const ScopedTimer timer("построение гистограммы", logger);
			auto histogram = HistogramBuilder::BuildLocalHistograms(image, 1);
			io::SaveToFile(outputPath, histogram);
		}
	}
	catch (const std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Error]\t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
