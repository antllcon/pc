#include "Histogram.h"

std::ostream& operator<<(std::ostream& os, const Histogram& histogram)
{
	os << "Intensity,Red,Green,Blue" << std::endl;

	for (size_t i = 0; i < BinCount; ++i)
	{
		os << i << ","
		   << histogram.r[i] << ","
		   << histogram.g[i] << ","
		   << histogram.b[i] << std::endl;
	}

	return os;
}