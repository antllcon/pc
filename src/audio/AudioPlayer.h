#pragma once

#include <vector>

class AudioPlayer final
{
public:
	AudioPlayer() = delete;

	static void Play(const std::vector<double>& audioBuffer);
};