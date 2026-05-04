#include "src/histogram/HistogramBuilder.h"
#include "src/image/Image.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <numeric>
#include <stdexcept>

using namespace testing;

namespace
{
void AssertIsFileOpened(const std::ofstream& file)
{
	if (!file.is_open())
	{
		throw std::runtime_error("Не удалось создать тестовый файл изображения");
	}
}

void DeleteTempPath(std::filesystem::path* path)
{
	if (path && std::filesystem::exists(*path))
	{
		std::filesystem::remove(*path);
	}
	delete path;
}

using TempFileGuard = std::unique_ptr<std::filesystem::path, decltype(&DeleteTempPath)>;

TempFileGuard CreateTestImage()
{
	auto path = new std::filesystem::path(std::filesystem::temp_directory_path() / "test_image.ppm");
	std::ofstream file(*path, std::ios::binary);

	AssertIsFileOpened(file);

	file << "P6\n2 2\n255\n";
	const unsigned char pixels[] = {
		255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 255};
	file.write(reinterpret_cast<const char*>(pixels), sizeof(pixels));

	return {path, DeleteTempPath};
}

void AssertIsHistogramsEqual(const Histogram& left, const Histogram& right)
{
	for (size_t i = 0; i < BinCount; ++i)
	{
		EXPECT_FLOAT_EQ(left.r[i], right.r[i]);
		EXPECT_FLOAT_EQ(left.g[i], right.g[i]);
		EXPECT_FLOAT_EQ(left.b[i], right.b[i]);
	}
}
} // namespace

// Проверка корректности однопоточного построения ненормализованной гистограммы
TEST(HistogramBuilderTest, SingleThreadCorrectness)
{
	const auto pathGuard = CreateTestImage();
	Image image(pathGuard->string());

	const Histogram histogram = HistogramBuilder::Build(image, false);

	EXPECT_FLOAT_EQ(histogram.r[255], 2.0f);
	EXPECT_FLOAT_EQ(histogram.r[0], 2.0f);
	EXPECT_FLOAT_EQ(histogram.g[255], 2.0f);
	EXPECT_FLOAT_EQ(histogram.g[0], 2.0f);
	EXPECT_FLOAT_EQ(histogram.b[255], 2.0f);
	EXPECT_FLOAT_EQ(histogram.b[0], 2.0f);
}

// Проверка нормализации гистограммы
TEST(HistogramBuilderTest, NormalizationCorrectness)
{
	const auto pathGuard = CreateTestImage();
	Image image(pathGuard->string());

	const Histogram histogram = HistogramBuilder::Build(image, true);

	const float sumR = std::accumulate(histogram.r.begin(), histogram.r.end(), 0.0f);
	const float sumG = std::accumulate(histogram.g.begin(), histogram.g.end(), 0.0f);
	const float sumB = std::accumulate(histogram.b.begin(), histogram.b.end(), 0.0f);

	EXPECT_FLOAT_EQ(sumR, 1.0f);
	EXPECT_FLOAT_EQ(sumG, 1.0f);
	EXPECT_FLOAT_EQ(sumB, 1.0f);
}

// Проверка идентичности результатов Interleaved и однопоточного алгоритмов
TEST(HistogramBuilderTest, InterleavedEqualsSingle)
{
	const auto pathGuard = CreateTestImage();
	Image image(pathGuard->string());
	const unsigned int threadCount = 2;

	const Histogram expected = HistogramBuilder::Build(image, false);
	const Histogram actual = HistogramBuilder::BuildAtomicInterleaved(image, threadCount);

	AssertIsHistogramsEqual(expected, actual);
}

// Проверка идентичности результатов Blocked и однопоточного алгоритмов
TEST(HistogramBuilderTest, BlockedEqualsSingle)
{
	const auto pathGuard = CreateTestImage();
	Image image(pathGuard->string());
	const unsigned int threadCount = 2;

	const Histogram expected = HistogramBuilder::Build(image, false);
	const Histogram actual = HistogramBuilder::BuildAtomicBlocked(image, threadCount);

	AssertIsHistogramsEqual(expected, actual);
}

// Проверка идентичности результатов Local и однопоточного алгоритмов
TEST(HistogramBuilderTest, LocalEqualsSingle)
{
	const auto pathGuard = CreateTestImage();
	Image image(pathGuard->string());
	const unsigned int threadCount = 2;

	const Histogram expected = HistogramBuilder::Build(image, false);
	const Histogram actual = HistogramBuilder::BuildLocalHistograms(image, threadCount);

	AssertIsHistogramsEqual(expected, actual);
}