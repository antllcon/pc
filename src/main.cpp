#include "histogram/Histogram.h"
#include "histogram/HistogramBuilder.h"
#include "image/Image.h"
#include "src/console/ConsoleArgs.h"
#include "src/console/ConsoleEncoding.h"
#include "src/logger/logger/ConsoleLogger.h"
#include "src/logger/timer/ScopedTimer.h"

#include <iostream>
#include <memory>
#include <syncstream>

int main(const int argc, char* argv[])
{
	try
	{
		ConsoleEncoding consoleEncoding;
		const auto logger = std::make_shared<ConsoleLogger>(true);

		auto path = ConsoleArgs::ExtractImagePath(argc, argv);
		const auto image = Image(path.string());

		{
			const ScopedTimer timer("построение гистограммы", logger);
			const auto histogram = HistogramBuilder::Build(image);
			std::cout << histogram;
		}
	}
	catch (const std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Error]\t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
