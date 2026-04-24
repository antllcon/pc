#include "ConsoleArgs.h"

#include <filesystem>
#include <set>
#include <stdexcept>
#include <string>

namespace
{
const std::set<std::string> IMAGE_EXTENSIONS = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".webp"};

void AssertIsArgumentsProvided(int argc)
{
	if (argc < 2)
	{
		throw std::invalid_argument("Путь до изображения не передали");
	}
}

void AssertIsFileExists(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
	{
		throw std::invalid_argument("Файл не найден: " + path.string());
	}
}

void AssertIsImageFile(const std::filesystem::path& path)
{
	std::string ext = path.extension().string();
	for (auto& ch : ext)
	{
		ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
	}
	if (!IMAGE_EXTENSIONS.contains(ext))
	{
		throw std::invalid_argument("Файл не является изображением: " + path.string());
	}
}
} // namespace

namespace ConsoleArgs
{
std::filesystem::path ExtractImagePath(int argc, char* argv[])
{
	AssertIsArgumentsProvided(argc);
	std::filesystem::path path(argv[1]);
	AssertIsFileExists(path);
	AssertIsImageFile(path);
	return path;
}
} // namespace ConsoleArgs
