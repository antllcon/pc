#pragma once
#include <filesystem>
#include <string>
#include <vector>

class Archiver
{
public:
	static void CreateArchive(
		const std::string& archiveName,
		const std::vector<std::string>& inputFiles,
		size_t parallelProcesses);

	static void ExtractArchive(
		const std::string& archiveName,
		const std::string& outputFolder,
		size_t parallelProcesses);
};