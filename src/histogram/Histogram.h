#pragma once

#include <array>
#include <ostream>

constexpr size_t BinCount = 256;

struct Histogram
{
	std::array<float, BinCount> r{};
	std::array<float, BinCount> g{};
	std::array<float, BinCount> b{};
};

std::ostream& operator<<(std::ostream& os, const Histogram& histogram);