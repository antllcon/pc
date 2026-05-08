#include "utils/console/ConsoleEncoding.h"
#include "utils/logger/timer/ScopedTimer.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

#include <filesystem>
#include <iostream>
#include <syncstream>

using namespace boost;

int main()
{
	ConsoleEncoding consoleEncoding;

	try
	{
		ScopedTimer timer("Работа программы");
		asio::io_context ctx;
		ctx.run();
	}
	catch (const std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Error]\t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
