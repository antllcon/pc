#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#include <algorithm>
#include <ostream>
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

void AssertThreeChannels(const Image& image)
{
	if (image.GetChannels() != 3u)
	{
		throw std::runtime_error("Недостаточно каналов для RGB");
	}
}

unsigned int CalculateSideLimit(const unsigned int side, const unsigned int width, const unsigned int height)
{
    const unsigned int maxSide = std::min(width, height);
    return std::min(side, maxSide);
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

Image::View Image::operator()(const unsigned int count) const
{
	return { *this, count };
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

std::ostream& operator<<(std::ostream& os, const Image::View& view)
{
	const Image& image = view.image;
	AssertThreeChannels(image);


	auto width = image.GetWidth();
    auto height = image.GetHeight();
    auto channels = image.GetChannels();
    auto* data = image.GetData();

	const unsigned int limit = CalculateSideLimit(view.count, width, height);

	const unsigned int startX = (width - limit) / 2;
    const unsigned int startY = (height - limit) / 2;

	const unsigned int endX = startX + limit;
    const unsigned int endY = startY + limit;

	for (unsigned int y = startY; y < endY; ++y)
    {
        for (unsigned int x = startX; x < endX; ++x)
        {
            const size_t index = (static_cast<size_t>(y) * width + x) * channels;

            os << "pixel (" << x << "," << y << "): "
               << static_cast<int>(data[index]) << " "
               << static_cast<int>(data[index + 1]) << " "
               << static_cast<int>(data[index + 2]) << '\n';
        }
    }

	return os;
}

std::ostream& operator<<(std::ostream& os, const Image& image)
{
    return os << image(std::max(image.GetWidth(), image.GetHeight()));
}