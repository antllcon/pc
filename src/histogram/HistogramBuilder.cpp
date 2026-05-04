#include "HistogramBuilder.h"
#include <stdexcept>

namespace
{
constexpr size_t RgbChannelsCount = 3;
constexpr size_t RedOffset = 0;
constexpr size_t GreenOffset = 1;
constexpr size_t BlueOffset = 2;

struct RawCounts
{
	std::array<size_t, BinCount> r{};
	std::array<size_t, BinCount> g{};
	std::array<size_t, BinCount> b{};
};

void AssertIsRGBImage(const Image& image)
{
	if (image.GetChannels() != RgbChannelsCount)
	{
		throw std::runtime_error("Изображение должно быть RGB (3 канала)");
	}
}

RawCounts CalculateRawCounts(const Image& image)
{
	RawCounts counts;
	const unsigned char* data = image.GetData();
	const size_t pixelCount = image.GetPixelCount();

	for (size_t i = 0; i < pixelCount; ++i)
	{
		const size_t pixelIndex = i * RgbChannelsCount;
		++counts.r[data[pixelIndex + RedOffset]];
		++counts.g[data[pixelIndex + GreenOffset]];
		++counts.b[data[pixelIndex + BlueOffset]];
	}

	return counts;
}

float GetDivisor(const size_t pixelCount, const bool normalize)
{
	return normalize ? static_cast<float>(pixelCount) : 1.0f;
}

void FillHistogram(Histogram& histogram, const RawCounts& counts, const float divisor)
{
	for (size_t i = 0; i < BinCount; ++i)
	{
		histogram.r[i] = static_cast<float>(counts.r[i]) / divisor;
		histogram.g[i] = static_cast<float>(counts.g[i]) / divisor;
		histogram.b[i] = static_cast<float>(counts.b[i]) / divisor;
	}
}
} // namespace

namespace HistogramBuilder
{
Histogram Build(const Image& image, const bool normalize)
{
	AssertIsRGBImage(image);

	auto counts = CalculateRawCounts(image);
	auto divisor = GetDivisor(image.GetPixelCount(), normalize);

	Histogram histogram;
	FillHistogram(histogram, counts, divisor);

	return histogram;
}
} // namespace HistogramBuilder