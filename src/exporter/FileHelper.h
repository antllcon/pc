#pragma once

#include <filesystem>

namespace io
{
template <typename T>
void Save(const std::filesystem::path& path, const T& data);
} // namespace io