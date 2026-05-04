#pragma once

#include "histogram/Histogram.h"
#include <filesystem>

namespace io
{
void Save(const std::filesystem::path& path, const Histogram& histogram);
} // namespace io