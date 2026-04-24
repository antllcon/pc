#include "src/console/ConsoleEncoding.h"
#include "src/logger/logger/ConsoleLogger.h"

#include <iostream>
#include <memory>
#include <string>
#include <syncstream>

int main(const int argc, char* argv[])
{
	try
	{
		ConsoleEncoding consoleEncoding;
		auto logger = std::make_shared<ConsoleLogger>(true);
	
		
	}
	catch (const std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Error]\t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
