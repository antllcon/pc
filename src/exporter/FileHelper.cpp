#include "FileHelper.h"
#include <stdexcept>

namespace io
{
void AssertIsFileOpened(const std::ofstream& file, const std::filesystem::path& path)
{
	if (!file.is_open())
	{
		throw std::runtime_error("Не удалось создать файл: " + path.string());
	}
}
} // namespace io