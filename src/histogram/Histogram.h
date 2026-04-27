#pragma once

#include <array>
#include <ostream>

struct Histogram
{
	std::array<float, 256> r{};
	std::array<float, 256> g{};
	std::array<float, 256> b{};
};

std::ostream& operator<<(std::ostream& os, const Histogram& histogram);
