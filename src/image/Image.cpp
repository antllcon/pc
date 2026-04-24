#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

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

int Image::GetWidth() const
{
	return m_width;
}

int Image::GetHeight() const
{
	return m_height;
}

int Image::GetChannels() const
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
	m_data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);

	if (!m_data)
	{
		throw std::runtime_error(std::string("Ошибка загрузки: ") + stbi_failure_reason());
	}
}

void Image::Free() noexcept
{
	if (m_data)
	{
		stbi_image_free(m_data);
		m_data = nullptr;
	}
}
