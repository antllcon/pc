#pragma once
#include "src/audio/AudioTypes.h"
#include <filesystem>

class TrackParser
{
public:
	static MusicComposition Parse(const std::filesystem::path& filePath);
};