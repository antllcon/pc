#pragma once

#include <cstdint>

namespace AudioConfig
{
	inline constexpr unsigned int SAMPLE_RATE        = 44100;
	inline constexpr unsigned int CHANNELS           = 1;
	inline constexpr uint32_t     SAMPLES_PER_PACKET = 1024;
}