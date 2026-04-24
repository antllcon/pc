#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

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
} // namespace

Image::Image(const std::string& path)
{
	Load(path);
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

	m_data = data;
	m_width = static_cast<unsigned int>(width);
	m_height = static_cast<unsigned int>(height);
	m_channels = static_cast<unsigned int>(channels);
}

void Image::Free() noexcept
{
	if (m_data)
	{
		stbi_image_free(m_data);
		m_data = nullptr;
	}
}
