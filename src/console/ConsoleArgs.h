#pragma once

#include <filesystem>

namespace ConsoleArgs
{
std::filesystem::path ExtractImagePath(int argc, char* argv[]);
} // namespace ConsoleArgs
