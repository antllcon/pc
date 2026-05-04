#include "src/histogram/HistogramBuilder.h"
#include "src/image/Image.h"

#include <benchmark/benchmark.h>
#include <filesystem>
#include <memory>

#ifndef PROJECT_ROOT_DIR
#define PROJECT_ROOT_DIR "."
#endif

namespace
{
std::unique_ptr<Image> g_realImage;
std::unique_ptr<Image> g_syntheticImage;
std::unique_ptr<Image> g_hugeImage;

void EnsureRealImageLoaded()
{
	if (!g_realImage)
	{
		std::filesystem::path path(PROJECT_ROOT_DIR);
		path /= "res/images/mojave.jpg";
		g_realImage = std::make_unique<Image>(path.string());
	}
}

void EnsureSyntheticImageGenerated()
{
	if (!g_syntheticImage)
	{
		EnsureRealImageLoaded();
		g_syntheticImage = std::make_unique<Image>(
			g_realImage->GetWidth(),
			g_realImage->GetHeight(),
			"FFFFFF");
	}
}

void EnsureHugeImageGenerated()
{
	if (!g_hugeImage)
	{
		g_hugeImage = std::make_unique<Image>(8000, 8000, "808080");
	}
}
} // namespace

class RealImageFixture : public benchmark::Fixture
{
public:
	void SetUp(const benchmark::State&) override
	{
		EnsureRealImageLoaded();
	}
};

class SyntheticImageFixture : public benchmark::Fixture
{
public:
	void SetUp(const benchmark::State&) override
	{
		EnsureSyntheticImageGenerated();
	}
};

class HugeImageFixture : public benchmark::Fixture
{
public:
	void SetUp(const benchmark::State&) override
	{
		EnsureHugeImageGenerated();
	}
};

BENCHMARK_F(RealImageFixture, Variant1_Single)(benchmark::State& state)
{
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::Build(*g_realImage, false));
	}
}

BENCHMARK_DEFINE_F(RealImageFixture, Variant2a_Interleaved)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildAtomicInterleaved(*g_realImage, threadCount));
	}
}

BENCHMARK_DEFINE_F(RealImageFixture, Variant2b_Blocked)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildAtomicBlocked(*g_realImage, threadCount));
	}
}

BENCHMARK_DEFINE_F(RealImageFixture, Variant3_Local)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildLocalHistograms(*g_realImage, threadCount));
	}
}

BENCHMARK_REGISTER_F(RealImageFixture, Variant2a_Interleaved)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);
BENCHMARK_REGISTER_F(RealImageFixture, Variant2b_Blocked)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);
BENCHMARK_REGISTER_F(RealImageFixture, Variant3_Local)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);

BENCHMARK_F(SyntheticImageFixture, Variant1_Single)(benchmark::State& state)
{
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::Build(*g_syntheticImage, false));
	}
}

BENCHMARK_DEFINE_F(SyntheticImageFixture, Variant2a_Interleaved)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildAtomicInterleaved(*g_syntheticImage, threadCount));
	}
}

BENCHMARK_DEFINE_F(SyntheticImageFixture, Variant2b_Blocked)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildAtomicBlocked(*g_syntheticImage, threadCount));
	}
}

BENCHMARK_DEFINE_F(SyntheticImageFixture, Variant3_Local)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildLocalHistograms(*g_syntheticImage, threadCount));
	}
}

BENCHMARK_REGISTER_F(SyntheticImageFixture, Variant2a_Interleaved)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);
BENCHMARK_REGISTER_F(SyntheticImageFixture, Variant2b_Blocked)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);
BENCHMARK_REGISTER_F(SyntheticImageFixture, Variant3_Local)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);

BENCHMARK_F(HugeImageFixture, Variant1_Single)(benchmark::State& state)
{
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::Build(*g_hugeImage, false));
	}
}

BENCHMARK_DEFINE_F(HugeImageFixture, Variant2a_Interleaved)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildAtomicInterleaved(*g_hugeImage, threadCount));
	}
}

BENCHMARK_DEFINE_F(HugeImageFixture, Variant2b_Blocked)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildAtomicBlocked(*g_hugeImage, threadCount));
	}
}

BENCHMARK_DEFINE_F(HugeImageFixture, Variant3_Local)(benchmark::State& state)
{
	const auto threadCount = static_cast<unsigned int>(state.range(0));
	for (auto _ : state)
	{
		benchmark::DoNotOptimize(HistogramBuilder::BuildLocalHistograms(*g_hugeImage, threadCount));
	}
}

BENCHMARK_REGISTER_F(HugeImageFixture, Variant2a_Interleaved)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);
BENCHMARK_REGISTER_F(HugeImageFixture, Variant2b_Blocked)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);
BENCHMARK_REGISTER_F(HugeImageFixture, Variant3_Local)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(16);