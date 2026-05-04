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
	return imagePath.parent_path() / (imagePath.stem().string() + "-stats.csv");
}
} // namespace

int main(const int argc, char* argv[])
{
	ConsoleEncoding consoleEncoding;
	try
	{
		auto logger = std::make_shared<ConsoleLogger>(true);

		auto imagePath = ConsoleArgs::ExtractImagePath(argc, argv);
		auto outputPath = BuildOutputPath(imagePath);
		auto image = Image(imagePath.string());

		const unsigned int threadCount = std::thread::hardware_concurrency();

		{
			ScopedTimer timer("Однопоточное построение", logger);
			auto histogram = HistogramBuilder::Build(image, false);
			io::Save(outputPath, histogram);
		}

		{
			ScopedTimer timer("Многопоточное (Interleaved)", logger);
			auto histogram = HistogramBuilder::BuildAtomicInterleaved(image, threadCount);
		}

		{
			ScopedTimer timer("Многопоточное (Blocked)", logger);
			auto histogram = HistogramBuilder::BuildAtomicBlocked(image, threadCount);
		}

		{
			ScopedTimer timer("Многопоточное (Local histograms)", logger);
			auto histogram = HistogramBuilder::BuildLocalHistograms(image, threadCount);
		}
	}
	catch (const std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Error]\t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
