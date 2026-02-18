#pragma once

#include <string>
#include <vector>

namespace Parser
{
struct MakeSettings
{
	size_t processes;
	std::string archiveName;
	std::vector<std::string> inputFiles;
};

struct ExtractSettings
{
	size_t processes;
	std::string archiveName;
	std::string outputFolder;
};

MakeSettings ParseMakeArgs(int argc, char* argv[]);
ExtractSettings ParseExtractArgs(int argc, char* argv[]);
}