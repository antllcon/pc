#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#include <algorithm>
#include <stdexcept>

namespace
{
void AssertIsLoaded(const unsigned char* data)
{
	if (!data)
	{
		throw std::runtime_error(std::string("Ошибка загрузки изображения: ") + stbi_failure_reason());
	}
}

void AssertIsDimensionsValid(const int width, const int height, const int channels)
{
	if (width <= 0 || height <= 0 || channels <= 0)
	{
		throw std::runtime_error("Некорректные размеры изображения");
	}
}

void AssertIsHexColorValid(const std::string& hexColor)
{
	if (hexColor.length() != 6)
	{
		throw std::invalid_argument("Цвет должен быть строкой из 6 символов RRGGBB");
	}
	for (const char c : hexColor)
	{
		if (!std::isxdigit(static_cast<unsigned char>(c)))
		{
			throw std::invalid_argument("Цвет должен содержать только шестнадцатеричные символы");
		}
	}
}

void AssertIsMemoryAllocated(const unsigned char* data)
{
	if (!data)
	{
		throw std::runtime_error("Не удалось выделить память под изображение");
	}
}

unsigned char ParseHexChannel(const std::string& hexColor, const size_t offset)
{
	const std::string channelStr = hexColor.substr(offset, 2);
	return static_cast<unsigned char>(std::stoul(channelStr, nullptr, 16));
}
} // namespace

Image::Image(const std::string& path)
{
	Load(path);
}

Image::Image(const unsigned int width, const unsigned int height, const std::string& hexColor)
	: m_width(width)
	, m_height(height)
	, m_channels(3)
{
	AssertIsHexColorValid(hexColor);
	AssertIsDimensionsValid(static_cast<int>(width), static_cast<int>(height), static_cast<int>(m_channels));

	const unsigned char r = ParseHexChannel(hexColor, 0);
	const unsigned char g = ParseHexChannel(hexColor, 2);
	const unsigned char b = ParseHexChannel(hexColor, 4);

	const size_t pixelCount = GetPixelCount();
	const size_t bytesCount = pixelCount * m_channels;

	m_data = static_cast<unsigned char*>(std::malloc(bytesCount));
	AssertIsMemoryAllocated(m_data);

	for (size_t i = 0; i < pixelCount; ++i)
	{
		const size_t pixelIndex = i * m_channels;
		m_data[pixelIndex] = r;
		m_data[pixelIndex + 1] = g;
		m_data[pixelIndex + 2] = b;
	}
}

Image::~Image()
{
	Free();
}

Image::Image(Image&& other) noexcept
{
	m_width = other.m_width;
	m_height = other.m_height;
	m_channels = other.m_channels;
	m_data = other.m_data;

	other.m_data = nullptr;
}

Image& Image::operator=(Image&& other) noexcept
{
	if (this != &other)
	{
		Free();

		m_width = other.m_width;
		m_height = other.m_height;
		m_channels = other.m_channels;
		m_data = other.m_data;

		other.m_data = nullptr;
	}
	return *this;
}

bool Image::IsRGB() const
{
	return m_channels == 3u;
}

unsigned int Image::GetWidth() const
{
	return m_width;
}

unsigned int Image::GetHeight() const
{
	return m_height;
}

unsigned int Image::GetChannels() const
{
	return m_channels;
}

size_t Image::GetPixelCount() const
{
	return m_width * m_height;
}

const unsigned char* Image::GetData() const
{
	return m_data;
}

unsigned char* Image::GetData()
{
	return m_data;
}

void Image::Load(const std::string& path)
{
	int width = 0;
	int height = 0;
	int channels = 0;

	auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	AssertIsLoaded(data);
	AssertIsDimensionsValid(width, height, channels);

	m_width = static_cast<unsigned int>(width);
	m_height = static_cast<unsigned int>(height);
	m_channels = static_cast<unsigned int>(channels);
	m_data = data;
}

void Image::Free() noexcept
{
	if (m_data)
	{
		stbi_image_free(m_data);
		m_data = nullptr;
	}
}