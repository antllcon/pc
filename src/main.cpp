#include "image/Image.h"
#include "src/console/ConsoleEncoding.h"
#include "src/logger/logger/ConsoleLogger.h"
#include "src/logger/timer/ScopedTimer.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <syncstream>

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

std::filesystem::path ExtractImagePath(int argc, char* argv[])
{
	AssertIsArgumentsProvided(argc);
	std::filesystem::path path(argv[1]);
	AssertIsFileExists(path);
	AssertIsImageFile(path);
	return path;
}
} // namespace

int main(const int argc, char* argv[])
{
	try
	{
		ConsoleEncoding consoleEncoding;
		const auto logger = std::make_shared<ConsoleLogger>(true);

		const auto imagePath = ExtractImagePath(argc, argv);
		const auto image = Image(imagePath.string());

		{
			ScopedTimer("обработка файла изображения", logger);

		}
	}
	catch (const std::exception& e)
	{
		std::osyncstream(std::cerr) << "[Error]\t" << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
