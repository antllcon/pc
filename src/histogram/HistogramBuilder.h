#pragma once

#include "Histogram.h"
#include "src/image/Image.h"

namespace HistogramBuilder
{
constexpr bool DefaultNormalize = false;

Histogram Build(const Image& image, bool normalize = DefaultNormalize);
Histogram BuildAtomicInterleaved(const Image& image, unsigned int threadCount);
Histogram BuildAtomicBlocked(const Image& image, unsigned int threadCount);
Histogram BuildLocalHistograms(const Image& image, unsigned int threadCount);
} // namespace HistogramBuilder
