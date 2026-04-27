#include "HistogramBuilder.h"

#include <array>
#include <atomic>
#include <cstdint>
#include <stdexcept>
#include <thread>
#include <vector>

namespace
{
void AssertIsRGBImage(const Image& image)
{
	if (image.GetChannels() != 3u)
	{
		throw std::runtime_error("Изображение должно быть RGB (3 канала)");
	}
}

void AssertIsValidThreadCount(unsigned int threadCount)
{
	if (threadCount == 0)
	{
		throw std::runtime_error("Число потоков должно быть положительным");
	}
}

std::pair<size_t, size_t> ComputeRange(const size_t pixelCount, const unsigned int threadCount, const unsigned int id)
{
	const size_t chunkSize = pixelCount / threadCount;
	const size_t start = id * chunkSize;
	const size_t end = id + 1 == threadCount ? pixelCount : start + chunkSize;
	return {start, end};
}

std::array<float, 256> NormalizeAtomic(
	const std::atomic<uint32_t>* counts,
	size_t total)
{
	std::array<float, 256> result{};
	const float invTotal = 1.0f / static_cast<float>(total);
	for (int i = 0; i < 256; ++i)
	{
		result[i] = static_cast<float>(counts[i].load(std::memory_order_relaxed)) * invTotal;
	}
	return result;
}
} // namespace

namespace HistogramBuilder
{
Histogram BuildAtomicInterleaved(const Image& image, const unsigned int threadCount)
{
	AssertIsRGBImage(image);
	AssertIsValidThreadCount(threadCount);

	std::array<std::atomic<uint32_t>, 256 * 3> hist{};

	const unsigned char* data = image.GetData();
	const size_t pixelCount = image.GetPixelCount();

	std::vector<std::jthread> threads;
	threads.reserve(threadCount);

	for (unsigned int t = 0; t < threadCount; ++t)
	{
		auto [start, end] = ComputeRange(pixelCount, threadCount, t);
		threads.emplace_back([&hist, data, start, end] {
			for (size_t i = start; i < end; ++i)
			{
				hist[data[i * 3 + 0] * 3 + 0].fetch_add(1, std::memory_order_relaxed);
				hist[data[i * 3 + 1] * 3 + 1].fetch_add(1, std::memory_order_relaxed);
				hist[data[i * 3 + 2] * 3 + 2].fetch_add(1, std::memory_order_relaxed);
			}
		});
	}
	threads.clear();

	Histogram histogram;
	const float invTotal = 1.0f / static_cast<float>(pixelCount);
	for (int v = 0; v < 256; ++v)
	{
		histogram.r[v] = static_cast<float>(hist[v * 3 + 0].load(std::memory_order_relaxed)) * invTotal;
		histogram.g[v] = static_cast<float>(hist[v * 3 + 1].load(std::memory_order_relaxed)) * invTotal;
		histogram.b[v] = static_cast<float>(hist[v * 3 + 2].load(std::memory_order_relaxed)) * invTotal;
	}
	return histogram;
}

Histogram BuildAtomicBlocked(const Image& image, const unsigned int threadCount)
{
	AssertIsRGBImage(image);
	AssertIsValidThreadCount(threadCount);

	std::array<std::atomic<uint32_t>, 256> histR{};
	std::array<std::atomic<uint32_t>, 256> histG{};
	std::array<std::atomic<uint32_t>, 256> histB{};

	const unsigned char* data = image.GetData();
	const size_t pixelCount = image.GetPixelCount();

	std::vector<std::jthread> threads;
	threads.reserve(threadCount);

	for (unsigned int t = 0; t < threadCount; ++t)
	{
		auto [start, end] = ComputeRange(pixelCount, threadCount, t);
		threads.emplace_back([&histR, &histG, &histB, data, start, end] {
			for (size_t i = start; i < end; ++i)
			{
				histR[data[i * 3 + 0]].fetch_add(1, std::memory_order_relaxed);
				histG[data[i * 3 + 1]].fetch_add(1, std::memory_order_relaxed);
				histB[data[i * 3 + 2]].fetch_add(1, std::memory_order_relaxed);
			}
		});
	}
	threads.clear();

	Histogram histogram;
	histogram.r = NormalizeAtomic(histR.data(), pixelCount);
	histogram.g = NormalizeAtomic(histG.data(), pixelCount);
	histogram.b = NormalizeAtomic(histB.data(), pixelCount);
	return histogram;
}
} // namespace HistogramBuilder
