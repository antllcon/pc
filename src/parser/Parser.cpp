#include "Parser.h"
#include <stdexcept>
#include <string>

namespace
{
void AssertArgCount(int argc, int min, const char* msg)
{
	if (argc < min)
	{
		throw std::runtime_error(msg);
	}
}

void AssertProcessCount(int count)
{
	if (count <= 0)
	{
		throw std::runtime_error("Количество процессов должно быть > 0");
	}
}

void AssertFilesExist(const std::vector<std::string>& files)
{
	if (files.empty())
	{
		throw std::runtime_error("Не указаны входные файлы");
	}
}
} // namespace

namespace Parser
{
ArchiveSettings ParseArchiveArgs(int argc, char* argv[])
{
	AssertArgCount(argc, 4, "Синтаксис: start make-archive -S/-P [ARGS]...");

	std::string mode = argv[2];
	ArchiveSettings settings;
	int argIndex = 0;

	if (mode == "-S")
	{
		settings.processes = 1;
		argIndex = 3;
	}
	else if (mode == "-P")
	{
		AssertArgCount(argc, 5, "Недостаточно аргументов для режима -P");

		int pCount = std::stoi(argv[3]);
		AssertProcessCount(pCount);

		settings.processes = static_cast<size_t>(pCount);
		argIndex = 4;
	}
	else
	{
		throw std::runtime_error("Неизвестный флаг: " + mode);
	}

	if (argIndex >= argc)
	{
		throw std::runtime_error("Не указано имя архива");
	}
	settings.archiveName = argv[argIndex++];

	for (; argIndex < argc; ++argIndex)
	{
		settings.inputFiles.emplace_back(argv[argIndex]);
	}

	AssertFilesExist(settings.inputFiles);

	return settings;
}

ExtractSettings ParseExtractArgs(int argc, char* argv[])
{
	AssertArgCount(argc, 5, "Синтаксис: start extract-files -S/-P [ARGS]...");

	std::string mode = argv[2];
	ExtractSettings settings;
	int argIndex = 0;

	if (mode == "-S")
	{
		settings.processes = 1;
		argIndex = 3;
	}
	else if (mode == "-P")
	{
		AssertArgCount(argc, 6, "Недостаточно аргументов для режима -P");

		int pCount = std::stoi(argv[3]);
		AssertProcessCount(pCount);

		settings.processes = static_cast<size_t>(pCount);
		argIndex = 4;
	}
	else
	{
		throw std::runtime_error("Неизвестный флаг: " + mode);
	}

	settings.archiveName = argv[argIndex++];
	settings.outputFolder = argv[argIndex++];

	return settings;
}
} // namespace Parser