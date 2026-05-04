#include "HistogramBuilder.h"

#include <stdexcept>
#include <thread>
#include <vector>

namespace
{
constexpr size_t RgbChannelsCount = 3;
constexpr size_t RedOffset = 0;
constexpr size_t GreenOffset = 1;
constexpr size_t BlueOffset = 2;

struct PixelRange
{
	size_t start;
	size_t end;
};

struct LocalCounts
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

std::vector<PixelRange> SplitPixels(const size_t pixelCount, const unsigned int threadCount)
{
	const size_t chunkSize = pixelCount / threadCount;
	std::vector<PixelRange> ranges;
	ranges.reserve(threadCount);

	for (unsigned int i = 0; i < threadCount; ++i)
	{
		const size_t start = i * chunkSize;
		const size_t end = i + 1 == threadCount ? pixelCount : start + chunkSize;
		ranges.push_back({start, end});
	}

	return ranges;
}

void ProcessLocalChunk(LocalCounts& local, const unsigned char* data, const PixelRange& range)
{
	for (size_t i = range.start; i < range.end; ++i)
	{
		const size_t pixelIndex = i * RgbChannelsCount;
		++local.r[data[pixelIndex + RedOffset]];
		++local.g[data[pixelIndex + GreenOffset]];
		++local.b[data[pixelIndex + BlueOffset]];
	}
}

void MergeLocalCounts(Histogram& histogram, const std::vector<LocalCounts>& locals)
{
	for (size_t i = 0; i < BinCount; ++i)
	{
		size_t r = 0;
		size_t g = 0;
		size_t b = 0;

		for (const auto& local : locals)
		{
			r += local.r[i];
			g += local.g[i];
			b += local.b[i];
		}

		histogram.r[i] = static_cast<float>(r);
		histogram.g[i] = static_cast<float>(g);
		histogram.b[i] = static_cast<float>(b);
	}
}
} // namespace

namespace HistogramBuilder
{
Histogram BuildLocalHistograms(const Image& image, const unsigned int threadCount)
{
	AssertIsRGBImage(image);

	const unsigned char* data = image.GetData();
	const auto ranges = SplitPixels(image.GetPixelCount(), threadCount);

	std::vector<LocalCounts> locals(threadCount);

	{
		std::vector<std::jthread> threads;
		threads.reserve(threadCount);

		for (unsigned int i = 0; i < threadCount; ++i)
		{
			threads.emplace_back([&locals, data, &ranges, i] {
				ProcessLocalChunk(locals[i], data, ranges[i]);
			});
		}
	}

	Histogram histogram;
	MergeLocalCounts(histogram, locals);

	return histogram;
}
} // namespace HistogramBuilder