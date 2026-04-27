#include "src/histogram/HistogramBuilder.h"
#include "src/image/Image.h"

#include <fstream>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>

using namespace testing;

namespace
{
constexpr unsigned int TEST_THREAD_COUNT = 4;
constexpr float SUM_EPSILON = 1e-5f;
constexpr float MATCH_EPSILON = 1e-6f;

float SumChannel(const std::array<float, 256>& channel)
{
	return std::accumulate(channel.begin(), channel.end(), 0.0f);
}

bool ChannelsMatch(const std::array<float, 256>& a, const std::array<float, 256>& b, float eps)
{
	for (int i = 0; i < 256; ++i)
	{
		if (std::abs(a[i] - b[i]) > eps)
		{
			return false;
		}
	}
	return true;
}

void WriteUint16LE(std::vector<unsigned char>& buf, int offset, uint16_t val)
{
	buf[offset] = val & 0xFF;
	buf[offset + 1] = (val >> 8) & 0xFF;
}

void WriteUint32LE(std::vector<unsigned char>& buf, int offset, uint32_t val)
{
	buf[offset] = val & 0xFF;
	buf[offset + 1] = (val >> 8) & 0xFF;
	buf[offset + 2] = (val >> 16) & 0xFF;
	buf[offset + 3] = (val >> 24) & 0xFF;
}

void WriteSolidBMP(
	const std::string& path,
	int width,
	int height,
	unsigned char r,
	unsigned char g,
	unsigned char b)
{
	const int rowStride = ((width * 3 + 3) / 4) * 4;
	const int pixelDataSize = rowStride * height;
	const int fileSize = 54 + pixelDataSize;

	std::vector<unsigned char> buf(fileSize, 0);

	// File header
	buf[0] = 'B';
	buf[1] = 'M';
	WriteUint32LE(buf, 2, static_cast<uint32_t>(fileSize));
	WriteUint32LE(buf, 10, 54);

	// DIB header (BITMAPINFOHEADER)
	WriteUint32LE(buf, 14, 40);
	WriteUint32LE(buf, 18, static_cast<uint32_t>(width));
	WriteUint32LE(buf, 22, static_cast<uint32_t>(height));
	WriteUint16LE(buf, 26, 1);
	WriteUint16LE(buf, 28, 24);

	// Pixel data: BMP stores BGR, rows bottom-to-top
	for (int row = 0; row < height; ++row)
	{
		int rowOffset = 54 + row * rowStride;
		for (int col = 0; col < width; ++col)
		{
			buf[rowOffset + col * 3 + 0] = b;
			buf[rowOffset + col * 3 + 1] = g;
			buf[rowOffset + col * 3 + 2] = r;
		}
	}

	std::ofstream file(path, std::ios::binary);
	file.write(reinterpret_cast<const char*>(buf.data()), buf.size());
}
} // namespace

class HistogramTest : public Test
{
public:
	static void SetUpTestSuite()
	{
		s_image = std::make_unique<Image>("res/images/mojave.jpg");
		s_reference = std::make_unique<Histogram>(HistogramBuilder::Build(*s_image));

		WriteSolidBMP("test_solid.bmp", 64, 64, 128, 0, 255);
		s_solidImage = std::make_unique<Image>("test_solid.bmp");
	}

	static void TearDownTestSuite()
	{
		s_solidImage.reset();
		s_reference.reset();
		s_image.reset();
		std::remove("test_solid.bmp");
	}

protected:
	static std::unique_ptr<Image> s_image;
	static std::unique_ptr<Histogram> s_reference;
	static std::unique_ptr<Image> s_solidImage;
};

std::unique_ptr<Image> HistogramTest::s_image;
std::unique_ptr<Histogram> HistogramTest::s_reference;
std::unique_ptr<Image> HistogramTest::s_solidImage;

// Проверка суммы каналов однопоточной реализации
TEST_F(HistogramTest, SingleThreadSumIsOne)
{
	EXPECT_NEAR(SumChannel(s_reference->r), 1.0f, SUM_EPSILON);
	EXPECT_NEAR(SumChannel(s_reference->g), 1.0f, SUM_EPSILON);
	EXPECT_NEAR(SumChannel(s_reference->b), 1.0f, SUM_EPSILON);
}

// Проверка корректности на изображении с одним известным цветом (128, 0, 255)
TEST_F(HistogramTest, SingleThreadKnownColor)
{
	const auto hist = HistogramBuilder::Build(*s_solidImage);

	EXPECT_NEAR(hist.r[128], 1.0f, SUM_EPSILON);
	EXPECT_NEAR(hist.g[0], 1.0f, SUM_EPSILON);
	EXPECT_NEAR(hist.b[255], 1.0f, SUM_EPSILON);
}

// Проверка совпадения варианта 2a с однопоточным эталоном
TEST_F(HistogramTest, AtomicInterleavedMatchesSingle)
{
	const auto hist = HistogramBuilder::BuildAtomicInterleaved(*s_image, TEST_THREAD_COUNT);

	EXPECT_TRUE(ChannelsMatch(hist.r, s_reference->r, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist.g, s_reference->g, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist.b, s_reference->b, MATCH_EPSILON));
}

// Проверка совпадения варианта 2b с однопоточным эталоном
TEST_F(HistogramTest, AtomicBlockedMatchesSingle)
{
	const auto hist = HistogramBuilder::BuildAtomicBlocked(*s_image, TEST_THREAD_COUNT);

	EXPECT_TRUE(ChannelsMatch(hist.r, s_reference->r, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist.g, s_reference->g, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist.b, s_reference->b, MATCH_EPSILON));
}

// Проверка совпадения варианта 3 с однопоточным эталоном
TEST_F(HistogramTest, LocalHistogramsMatchSingle)
{
	const auto hist = HistogramBuilder::BuildLocalHistograms(*s_image, TEST_THREAD_COUNT);

	EXPECT_TRUE(ChannelsMatch(hist.r, s_reference->r, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist.g, s_reference->g, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist.b, s_reference->b, MATCH_EPSILON));
}

// Проверка взаимной согласованности всех параллельных вариантов
TEST_F(HistogramTest, AllVariantsConsistent)
{
	const auto hist2a = HistogramBuilder::BuildAtomicInterleaved(*s_image, TEST_THREAD_COUNT);
	const auto hist2b = HistogramBuilder::BuildAtomicBlocked(*s_image, TEST_THREAD_COUNT);
	const auto hist3 = HistogramBuilder::BuildLocalHistograms(*s_image, TEST_THREAD_COUNT);

	EXPECT_TRUE(ChannelsMatch(hist2a.r, hist2b.r, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist2a.r, hist3.r, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist2a.g, hist2b.g, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist2a.g, hist3.g, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist2a.b, hist2b.b, MATCH_EPSILON));
	EXPECT_TRUE(ChannelsMatch(hist2a.b, hist3.b, MATCH_EPSILON));
}
