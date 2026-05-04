#include "HistogramBuilder.h"

#include <array>
#include <stdexcept>

namespace
{
constexpr size_t RgbChannelsCount = 3;
constexpr size_t RedOffset = 0;
constexpr size_t GreenOffset = 1;
constexpr size_t BlueOffset = 2;

void AssertIsRGBImage(const Image& image)
{
	if (image.GetChannels() != RgbChannelsCount)
	{
		throw std::runtime_error("Изображение должно быть RGB (3 канала)");
	}
}

std::array<float, BinCount> NormalizeChannel(const std::array<size_t, BinCount>& counts, const size_t total)
{
	std::array<float, BinCount> result{};
	const float invTotal = 1.0f / static_cast<float>(total);
	for (int i = 0; i < BinCount; ++i)
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

	std::array<size_t, BinCount> countsRed{};
	std::array<size_t, BinCount> countsGreen{};
	std::array<size_t, BinCount> countsBlue{};

	const unsigned char* data = image.GetData();
	const size_t pixelCount = image.GetPixelCount();

	for (size_t i = 0; i < pixelCount; ++i)
	{
		const size_t pixelIndex = i * RgbChannelsCount;
		++countsRed[data[pixelIndex + RedOffset]];
		++countsGreen[data[pixelIndex + GreenOffset]];
		++countsBlue[data[pixelIndex + BlueOffset]];
	}

	Histogram histogram;
	histogram.r = NormalizeChannel(countsRed, pixelCount);
	histogram.g = NormalizeChannel(countsGreen, pixelCount);
	histogram.b = NormalizeChannel(countsBlue, pixelCount);

	return histogram;
}
} // namespace HistogramBuilder