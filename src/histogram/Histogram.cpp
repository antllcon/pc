#include "Histogram.h"

namespace
{
constexpr std::string_view SEPARATOR = ";";
}

std::ostream& operator<<(std::ostream& os, const Histogram& histogram)
{
	os << "Intensity,Red,Green,Blue" << std::endl;

	for (size_t i = 0; i < BinCount; ++i)
	{
		os << i << SEPARATOR
		   << histogram.r[i] << SEPARATOR
		   << histogram.g[i] << SEPARATOR
		   << histogram.b[i] << std::endl;
	}

	return os;
}