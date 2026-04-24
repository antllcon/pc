#include "src/visualizer/AudioVisualizer.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace
{
constexpr std::array<char, 9> LEVELS = {' ', '_', '.', '-', '=', '+', '*', '#', '@'};
} // namespace

AudioVisualizer::AudioVisualizer(size_t barCount)
	: m_barCount(barCount)
	, m_levels(barCount, 0.0f)
{
}

void AudioVisualizer::Update(const std::vector<float>& samples)
{
	if (samples.empty())
	{
		return;
	}

	const size_t samplesPerBar = std::max(size_t{1}, samples.size() / m_barCount);

	for (size_t bar = 0; bar < m_barCount; ++bar)
	{
		const size_t start = bar * samplesPerBar;
		const size_t end   = std::min(start + samplesPerBar, samples.size());

		float peak = 0.0f;
		for (size_t i = start; i < end; ++i)
		{
			peak = std::max(peak, std::abs(samples[i]));
		}
		m_levels[bar] = std::min(1.0f, peak);
	}
}

std::string AudioVisualizer::Render() const
{
	std::string result;
	result.reserve(m_barCount + 4);
	result += '[';

	for (float level : m_levels)
	{
		const size_t idx = static_cast<size_t>(std::round(level * (LEVELS.size() - 1)));
		result += LEVELS[idx];
	}

	result += ']';
	return result;
}
