#pragma once

#include <ostream>
#include <span>
#include <string>
#include <vector>

class ConsoleHistogram
{
public:
	explicit ConsoleHistogram(int height, int widthPerChannel = 40);

	void AddChannel(const std::string& name, std::span<const float> data);
	void Print(std::ostream& stream) const;

private:
	struct Channel
	{
		std::string name;
		std::vector<int> normalizedData;
	};

	int m_height;
	int m_width;
	std::vector<Channel> m_channels;
};