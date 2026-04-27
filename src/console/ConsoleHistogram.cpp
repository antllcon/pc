#include "ConsoleHistogram.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace
{
void AssertIsPositive(int value)
{
	if (value <= 0)
	{
		throw std::runtime_error("Размер гистограммы должен быть положительным");
	}
}

std::vector<int> ResampleAndNormalize(std::span<const float> data, int targetWidth, int targetHeight)
{
	std::vector<int> result(targetWidth, 0);
	float maxValue = *std::ranges::max_element(data);
	if (maxValue <= 0.0f)
	{
		return result;
	}

	float binSize = static_cast<float>(data.size()) / static_cast<float>(targetWidth);

	for (int i = 0; i < targetWidth; ++i)
	{
		int start = static_cast<int>(std::floor(i * binSize));
		int end = static_cast<int>(std::floor((i + 1) * binSize));
		end = std::min(end, static_cast<int>(data.size()));

		float sum = 0;
		for (int j = start; j < end; ++j)
		{
			sum += data[j];
		}

		float average = sum / (end - start);
		result[i] = static_cast<int>(std::round((average / maxValue) * targetHeight));
	}

	return result;
}
} // namespace

ConsoleHistogram::ConsoleHistogram(int height, int widthPerChannel)
	: m_height(height)
	, m_width(widthPerChannel)
{
	AssertIsPositive(height);
	AssertIsPositive(widthPerChannel);
}

void ConsoleHistogram::AddChannel(const std::string& name, std::span<const float> data)
{
	m_channels.push_back({name,
		ResampleAndNormalize(data, m_width, m_height)});
}

void ConsoleHistogram::Print(std::ostream& stream) const
{
	constexpr std::string separator = "      ";

	// 1. Печать имен
	for (const auto& [name, normalizedData] : m_channels)
	{
		std::string label = name;
		label.resize(m_width + 4, ' ');
		stream << label << separator;
	}
	stream << std::endl;

	// 2. Печать тела гистограмм (сверху вниз)
	for (int row = m_height; row >= 1; --row)
	{
		for (const auto& [name, normalizedData] : m_channels)
		{
			// Левая граница
			if (row == m_height)
				stream << "^ |";
			else
				stream << "| |";

			// Данные
			for (int val : normalizedData)
			{
				stream << (val >= row ? "#" : " ");
			}
			stream << separator;
		}
		stream << std::endl;
	}

	// 3. Печать основания
	for (size_t i = 0; i < m_channels.size(); ++i)
	{
		stream << "+ +" << std::string(m_width, '-') << separator;
	}
	stream << std::endl;
}