#include "Histogram.h"
#include "src/console/ConsoleHistogram.h"
#include <cmath>

namespace
{
constexpr int HISTOGRAM_HEIGHT = 10;
} // namespace

std::ostream& operator<<(std::ostream& os, const Histogram& histogram)
{
	ConsoleHistogram visualizer(HISTOGRAM_HEIGHT, 256);

	visualizer.AddChannel("Red Channel", histogram.r);
	visualizer.AddChannel("Green Channel", histogram.g);
	visualizer.AddChannel("Blue Channel", histogram.b);

	visualizer.Print(os);

	return os;
}