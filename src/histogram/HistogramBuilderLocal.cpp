#include "HistogramBuilder.h"

#include <array>
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
	const size_t end = (id + 1 == threadCount) ? pixelCount : start + chunkSize;
	return {start, end};
}

struct LocalHistogram
{
	std::array<uint32_t, 256> r{};
	std::array<uint32_t, 256> g{};
	std::array<uint32_t, 256> b{};
};

std::array<float, 256> NormalizeChannel(const std::array<uint32_t, 256>& counts, size_t total)
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
Histogram BuildLocalHistograms(const Image& image, const unsigned int threadCount)
{
	AssertIsRGBImage(image);
	AssertIsValidThreadCount(threadCount);

	const unsigned char* data = image.GetData();
	const size_t pixelCount = image.GetPixelCount();

	std::vector<LocalHistogram> locals(threadCount);
	std::vector<std::jthread> threads;
	threads.reserve(threadCount);

	for (unsigned int t = 0; t < threadCount; ++t)
	{
		auto [start, end] = ComputeRange(pixelCount, threadCount, t);
		threads.emplace_back([&locals, data, start, end, t] {
			auto& [r, g, b] = locals[t];
			for (size_t i = start; i < end; ++i)
			{
				++r[data[i * 3 + 0]];
				++g[data[i * 3 + 1]];
				++b[data[i * 3 + 2]];
			}
		});
	}
	threads.clear();

	std::array<uint32_t, 256> totalR{};
	std::array<uint32_t, 256> totalG{};
	std::array<uint32_t, 256> totalB{};

	for (const auto& local : locals)
	{
		for (int v = 0; v < 256; ++v)
		{
			totalR[v] += local.r[v];
			totalG[v] += local.g[v];
			totalB[v] += local.b[v];
		}
	}

	Histogram histogram;
	histogram.r = NormalizeChannel(totalR, pixelCount);
	histogram.g = NormalizeChannel(totalG, pixelCount);
	histogram.b = NormalizeChannel(totalB, pixelCount);
	return histogram;
}
} // namespace HistogramBuilder
