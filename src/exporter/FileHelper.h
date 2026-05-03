#pragma once

#include <filesystem>
#include <fstream>

namespace io
{
void AssertIsFileOpened(const std::ofstream& file, const std::filesystem::path& path);

template <typename T>
void SaveToFile(const std::filesystem::path& path, const T& data)
{
	std::ofstream file(path);
	AssertIsFileOpened(file, path);

	file << data << std::endl;
}
} // namespace io