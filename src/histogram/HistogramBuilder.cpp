#include "HistogramBuilder.h"

#include <array>
#include <stdexcept>

namespace
{
void AssertIsRGBImage(const Image& image)
{
	if (image.GetChannels() != 3u)
	{
		throw std::runtime_error("Изображение должно быть RGB (3 канала)");
	}
}

std::array<float, 256> NormalizeChannel(const std::array<unsigned int, 256>& counts, size_t total)
{
	std::array<float, 256> result{};
	const float invTotal = 1.0f / static_cast<float>(total);
	for (int i = 0; i < 256; ++i)
	{
		result[i] = static_cast<float>(counts[i]) * invTotal;
	}
	return result;
}
} // namespace

namespace HistogramBuilder
{
Histogram Build(const Image& image)
{
	AssertIsRGBImage(image);

	std::array<unsigned int, 256> countsR{};
	std::array<unsigned int, 256> countsG{};
	std::array<unsigned int, 256> countsB{};

	const unsigned char* data = image.GetData();
	const size_t pixelCount = image.GetPixelCount();

	for (size_t i = 0; i < pixelCount; ++i)
	{
		++countsR[data[i * 3 + 0]];
		++countsG[data[i * 3 + 1]];
		++countsB[data[i * 3 + 2]];
	}

	Histogram histogram;
	histogram.r = NormalizeChannel(countsR, pixelCount);
	histogram.g = NormalizeChannel(countsG, pixelCount);
	histogram.b = NormalizeChannel(countsB, pixelCount);

	return histogram;
}
} // namespace HistogramBuilder
