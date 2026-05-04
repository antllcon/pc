#pragma once

#include <array>
#include <ostream>

constexpr unsigned int COLOR_RANGE = 256;

struct Histogram
{
	std::array<float, COLOR_RANGE> r;
	std::array<float, COLOR_RANGE> g;
	std::array<float, COLOR_RANGE> b;
};