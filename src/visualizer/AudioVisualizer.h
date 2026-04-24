#pragma once

#include <string>
#include <vector>

class AudioVisualizer
{
public:
	explicit AudioVisualizer(size_t barCount = 40);

	void        Update(const std::vector<float>& samples);
	std::string Render() const;

private:
	size_t             m_barCount;
	std::vector<float> m_levels;
};
