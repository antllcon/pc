#pragma once

#include "Histogram.h"
#include "src/image/Image.h"

namespace HistogramBuilder
{
Histogram Build(const Image& image);
Histogram BuildAtomicInterleaved(const Image& image, unsigned int threadCount);
Histogram BuildAtomicBlocked(const Image& image, unsigned int threadCount);
Histogram BuildLocalHistograms(const Image& image, unsigned int threadCount);
} // namespace HistogramBuilder
