#include "console/ConsoleEncoding.h"
#include <chrono>
#include <csignal>
#include <iostream>
#include <syncstream>
#include <windows.h>

int main(int argc, char* argv[])
{
	try
	{
		ConsoleEncoding consoleEncoding;

	}
	catch (std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Error] " << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}