#include "Archiver.h"
#include "src/process/ProcessManager.h"
#include <iostream>

namespace fs = std::filesystem;

namespace
{
constexpr auto GZIP = "gzip";
constexpr auto TAR = "tar";
constexpr auto FLAG_CREATE_ARCHIVE = "-cf";
constexpr auto FLAG_EXTRACT_ARCHIVE = "-xf";
constexpr auto FLAG_OUTPUT_DIR = "-C";
constexpr auto FLAG_DECOMPRESS = "-d";
constexpr auto FLAG_FORCE = "-f";
constexpr auto FLAG_KEEP_ORIGINAL = "-k";

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

void AssertIsSuccessfulTempFilesNotEmpty(const std::vector<std::string>& successfulTempFiles)
{
	if (successfulTempFiles.empty())
	{
		throw ::std::logic_error("Архивация отменена: нет успешно сжатых файлов.");
	}
}

void RunTarCreate(const std::string& archiveName, const std::vector<std::string>& gzFiles)
{
	std::vector<std::string> args = {FLAG_CREATE_ARCHIVE, archiveName};
	args.insert(args.end(), gzFiles.begin(), gzFiles.end());

	Process tar(TAR, args);
	tar.Wait();
}

void RunTarExtract(const std::string& archiveName, const std::string& outputFolder)
{
	Process tar(TAR, {FLAG_EXTRACT_ARCHIVE, archiveName, FLAG_OUTPUT_DIR, outputFolder});
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

	manager.Add(Process(GZIP, {FLAG_KEEP_ORIGINAL, FLAG_FORCE, inputFile}));
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

	manager.Add(Process(GZIP, {FLAG_DECOMPRESS, FLAG_FORCE, gzFile}));
}

std::vector<std::string> FilterSuccessfulFiles(
	const std::vector<std::string>& tempFiles,
	const std::vector<std::string>& originalFiles)
{
	std::vector<std::string> validFiles;
	validFiles.reserve(tempFiles.size());

	for (size_t i = 0; i < tempFiles.size(); ++i)
	{
		if (fs::exists(tempFiles[i]) && fs::file_size(tempFiles[i]) > 0)
		{
			validFiles.push_back(tempFiles[i]);
		}
		else
		{
			std::cerr << "Внимание: файл '" << originalFiles[i] << "' был пропущен из-за ошибки сжатия." << std::endl;

			if (fs::exists(tempFiles[i]))
			{
				fs::remove(tempFiles[i]);
			}
		}
	}

	AssertIsSuccessfulTempFilesNotEmpty(validFiles);

	return validFiles;
}

void CleanupTempFiles(const std::vector<std::string>& files)
{
	for (const auto& file : files)
	{
		fs::remove(file);
	}
}

void CleanupFailedExtractions(const std::string& outputFolder)
{
	for (const auto& entry : fs::directory_iterator(outputFolder))
	{
		if (entry.path().extension() == ".gz")
		{
			std::cerr << "Внимание: не удалось распаковать файл '" << entry.path().string() << "'." << std::endl;
			fs::remove(entry.path());
		}
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

	auto successfulTempFiles = FilterSuccessfulFiles(tempFiles, inputFiles);
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
				parallelProcesses);
		}
	}

	if (!processManager.WaitAll())
	{
		CleanupFailedExtractions(outputFolder);
	}
}