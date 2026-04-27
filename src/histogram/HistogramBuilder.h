#pragma once

#include "Histogram.h"
#include "src/image/Image.h"

namespace HistogramBuilder
{
Histogram Build(const Image& image);
} // namespace HistogramBuilder
