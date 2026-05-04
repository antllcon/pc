#include "FileHelper.h"
#include "src/histogram/Histogram.h"
#include <fstream>
#include <stdexcept>

namespace
{
void AssertIsFileOpened(const std::ofstream& file, const std::filesystem::path& path)
{
	if (!file.is_open())
	{
		throw std::runtime_error("Не удалось создать файл: " + path.string());
	}
}
} // namespace

namespace io
{
template <typename T>
void Save(const std::filesystem::path& path, const T& data)
{
	std::ofstream file(path);
	AssertIsFileOpened(file, path);

	file << data << std::endl;
}
} // namespace io