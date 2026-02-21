#include "arhiver/Archiver.h"
#include "parser/Parser.h"

#include <complex>
#include <iostream>

namespace
{
const std::string MAKE_ARCHIVE = "make-archive";
const std::string EXTRACT_ARCHIVE = "extract-archive";

void AssertIsArgcValid(int argc)
{
	if (argc < 2)
	{
		throw std::runtime_error("Не указана команда (make-archive или extract-files)");
	}
}

void RunMakeArchive(int argc, char* argv[])
{
	auto settings = Parser::ParseArchiveArgs(argc, argv);
	Archiver::CreateArchive(settings.archiveName, settings.inputFiles, settings.processes);
}

void RunExtractFiles(int argc, char* argv[])
{
	auto settings = Parser::ParseExtractArgs(argc, argv);
	Archiver::ExtractArchive(settings.archiveName, settings.outputFolder, settings.processes);
}
}

int main(int argc, char* argv[])
{
	try
	{
		AssertIsArgcValid(argc);

		std::string command = argv[1];

		if (command == MAKE_ARCHIVE)
		{
			RunMakeArchive(argc, argv);
		}
		else if (command == EXTRACT_ARCHIVE)
		{
			RunExtractFiles(argc, argv);
		}
		else
		{
			throw std::runtime_error("Неизвестная команда: " + command);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}