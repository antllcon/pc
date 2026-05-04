#include "HistogramBuilder.h"

#include <atomic>
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

using AtomicInterleavedArray = std::array<std::atomic<size_t>, BinCount * RgbChannelsCount>;
using AtomicChannelArray = std::array<std::atomic<size_t>, BinCount>;

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

void ProcessInterleavedChunk(AtomicInterleavedArray& hist, const unsigned char* data, const PixelRange& range)
{
	for (size_t i = range.start; i < range.end; ++i)
	{
		const size_t pixelIndex = i * RgbChannelsCount;
		const size_t r = data[pixelIndex + RedOffset];
		const size_t g = data[pixelIndex + GreenOffset];
		const size_t b = data[pixelIndex + BlueOffset];

		hist[r * RgbChannelsCount + RedOffset].fetch_add(1, std::memory_order_relaxed);
		hist[g * RgbChannelsCount + GreenOffset].fetch_add(1, std::memory_order_relaxed);
		hist[b * RgbChannelsCount + BlueOffset].fetch_add(1, std::memory_order_relaxed);
	}
}

void ProcessBlockedChunk(AtomicChannelArray& r, AtomicChannelArray& g, AtomicChannelArray& b, const unsigned char* data, const PixelRange& range)
{
	for (size_t i = range.start; i < range.end; ++i)
	{
		const size_t pixelIndex = i * RgbChannelsCount;
		r[data[pixelIndex + RedOffset]].fetch_add(1, std::memory_order_relaxed);
		g[data[pixelIndex + GreenOffset]].fetch_add(1, std::memory_order_relaxed);
		b[data[pixelIndex + BlueOffset]].fetch_add(1, std::memory_order_relaxed);
	}
}

void FillHistogramFromInterleaved(Histogram& histogram, const AtomicInterleavedArray& hist)
{
	for (size_t i = 0; i < BinCount; ++i)
	{
		histogram.r[i] = static_cast<float>(hist[i * RgbChannelsCount + RedOffset].load(std::memory_order_relaxed));
		histogram.g[i] = static_cast<float>(hist[i * RgbChannelsCount + GreenOffset].load(std::memory_order_relaxed));
		histogram.b[i] = static_cast<float>(hist[i * RgbChannelsCount + BlueOffset].load(std::memory_order_relaxed));
	}
}

void FillHistogramFromBlocked(Histogram& histogram, const AtomicChannelArray& r, const AtomicChannelArray& g, const AtomicChannelArray& b)
{
	for (size_t i = 0; i < BinCount; ++i)
	{
		histogram.r[i] = static_cast<float>(r[i].load(std::memory_order_relaxed));
		histogram.g[i] = static_cast<float>(g[i].load(std::memory_order_relaxed));
		histogram.b[i] = static_cast<float>(b[i].load(std::memory_order_relaxed));
	}
}
} // namespace

namespace HistogramBuilder
{
Histogram BuildAtomicInterleaved(const Image& image, const unsigned int threadCount)
{
	AssertIsRGBImage(image);

	AtomicInterleavedArray hist{};
	const unsigned char* data = image.GetData();
	auto ranges = SplitPixels(image.GetPixelCount(), threadCount);

	{
		std::vector<std::jthread> threads;
		threads.reserve(threadCount);
		for (const auto& range : ranges)
		{
			threads.emplace_back([&hist, data, range] {
				ProcessInterleavedChunk(hist, data, range);
			});
		}
	}

	Histogram histogram;
	FillHistogramFromInterleaved(histogram, hist);

	return histogram;
}

Histogram BuildAtomicBlocked(const Image& image, const unsigned int threadCount)
{
	AssertIsRGBImage(image);

	AtomicChannelArray r{};
	AtomicChannelArray g{};
	AtomicChannelArray b{};

	const unsigned char* data = image.GetData();
	const auto ranges = SplitPixels(image.GetPixelCount(), threadCount);

	{
		std::vector<std::jthread> threads;
		threads.reserve(threadCount);
		for (const auto& range : ranges)
		{
			threads.emplace_back([&r, &g, &b, data, range] {
				ProcessBlockedChunk(r, g, b, data, range);
			});
		}
	}

	Histogram histogram;
	FillHistogramFromBlocked(histogram, r, g, b);

	return histogram;
}
} // namespace HistogramBuilder