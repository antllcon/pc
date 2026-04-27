#include "src/histogram/HistogramBuilder.h"
#include "src/image/Image.h"

#include <benchmark/benchmark.h>
#include <memory>

namespace
{
std::unique_ptr<Image> g_image;
} // namespace

class HistogramFixture : public benchmark::Fixture
{
public:
	void SetUp(const benchmark::State&) override
	{
		if (!g_image)
		{
			g_image = std::make_unique<Image>("res/images/mojave.jpg");
		}
	}
};

BENCHMARK_F(HistogramFixture, Variant1)(benchmark::State& state)
{
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::Build(*g_image));
	}
}

BENCHMARK_F(HistogramFixture, Variant2a_Interleaved)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildAtomicInterleaved(*g_image, threadCount));
	}
}

BENCHMARK_F(HistogramFixture, Variant2b_Blocked)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildAtomicBlocked(*g_image, threadCount));
	}
}

BENCHMARK_F(HistogramFixture, Variant3_Local)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildLocalHistograms(*g_image, threadCount));
	}
}

BENCHMARK_REGISTER_F(HistogramFixture, Variant2a_Interleaved)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);
BENCHMARK_REGISTER_F(HistogramFixture, Variant2b_Blocked)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);
BENCHMARK_REGISTER_F(HistogramFixture, Variant3_Local)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);
