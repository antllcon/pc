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

void WriteCsvHeader(std::ofstream& file)
{
	file << "Intensity,Red,Green,Blue" << std::endl;
}

void WriteCsvData(std::ofstream& file, const Histogram& histogram)
{
	for (size_t i = 0; i < COLOR_RANGE; ++i)
	{
		file << i << ","
			 << histogram.r[i] << ","
			 << histogram.g[i] << ","
			 << histogram.b[i] << std::endl;
	}
}
} // namespace

namespace io
{
inline void Save(const std::filesystem::path& path, const Histogram& histogram)
{
	std::ofstream file(path);
	AssertIsFileOpened(file, path);

	WriteCsvHeader(file);
	WriteCsvData(file, histogram);
}
} // namespace io