#include "Archiver.h"
#include "src/process/ProcessManager.h"

namespace fs = std::filesystem;

namespace
{
constexpr auto GZIP = "gzip";
constexpr auto TAR = "tar";

void AssertProcessesCountValid(size_t processes)
{
	if (processes == 0)
	{
		throw std::runtime_error("Количество процессов должно быть больше 0");
	}
}

void AssertIsFileExists(const fs::path& path)
{
	if (!fs::exists(path) || !fs::is_regular_file(path))
	{
		throw std::runtime_error("Файл не найден: " + path.string());
	}
}

void AssertIsDirectoryReady(const fs::path& path)
{
	if (fs::exists(path) && !fs::is_directory(path))
	{
		throw std::runtime_error("Путь занят файлом: " + path.string());
	}
}

void RunTarCreate(const std::string& archiveName, const std::vector<std::string>& gzFiles)
{
	std::vector<std::string> args = {"-cf", archiveName};
	args.insert(args.end(), gzFiles.begin(), gzFiles.end());

	Process tar(TAR, args);
	tar.Wait();
}

void RunTarExtract(const std::string& archiveName, const std::string& outputFolder)
{
	Process tar(TAR, {"-xf", archiveName, "-C", outputFolder});
	tar.Wait();
}

std::string ScheduleCompression(
	ProcessManager& manager,
	const std::string& inputFile,
	size_t maxProcesses)
{
	if (manager.Count() >= maxProcesses)
	{
		manager.WaitAny();
	}

	manager.Add(Process(GZIP, {"-k", "-f", inputFile}));
	return inputFile + ".gz";
}

void ScheduleDecompression(
		ProcessManager& manager,
		const std::string& gzFile,
		size_t maxProcesses)
{
	if (manager.Count() >= maxProcesses)
	{
		manager.WaitAny();
	}

	manager.Add(Process(GZIP, {"-d", "-f", gzFile}));
}

void CleanupTempFiles(const std::vector<std::string>& files)
{
	for (const auto& file : files)
	{
		fs::remove(file);
	}
}
} // namespace

void Archiver::CreateArchive(
	const std::string& archiveName,
	const std::vector<std::string>& inputFiles,
	size_t parallelProcesses)
{
	AssertProcessesCountValid(parallelProcesses);
	std::vector<std::string> tempFiles;

	tempFiles.reserve(inputFiles.size());
	ProcessManager processManager;

	for (const auto& inputFile : inputFiles)
	{
		AssertIsFileExists(inputFile);
		std::string name = ScheduleCompression(processManager, inputFile, parallelProcesses);
		tempFiles.push_back(name);
	}

	processManager.WaitAll();
	RunTarCreate(archiveName, tempFiles);
	CleanupTempFiles(tempFiles);
}

void Archiver::ExtractArchive(
	const std::string& archiveName,
	const std::string& outputFolder,
	size_t parallelProcesses)
{
	AssertProcessesCountValid(parallelProcesses);
	AssertIsFileExists(archiveName);
	AssertIsDirectoryReady(outputFolder);

	if (!fs::exists(outputFolder))
	{
		fs::create_directories(outputFolder);
	}

	RunTarExtract(archiveName, outputFolder);

	ProcessManager processManager;

	for (const auto& entry : fs::directory_iterator(outputFolder))
	{
		if (entry.path().extension() == ".gz")
		{
			ScheduleDecompression(
				processManager,
				entry.path().string(),
				parallelProcesses
			);
		}
	}

	processManager.WaitAll();
}