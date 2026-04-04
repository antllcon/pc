#pragma once

#include "src/audio/AudioTypes.h"
#include <vector>

class Synthesizer final
{
public:
	Synthesizer() = delete;

	static std::vector<double> Render(const MusicComposition& composition);
};