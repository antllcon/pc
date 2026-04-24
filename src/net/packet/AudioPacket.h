#pragma once

#include "src/audio/AudioConstants.h"

#include <cstdint>
#include <vector>

inline constexpr size_t PACKET_BYTE_SIZE =
	sizeof(uint32_t) + AudioConfig::SAMPLES_PER_PACKET * sizeof(float);

struct AudioPacket
{
	static std::vector<uint8_t> Serialize(const std::vector<float>& samples);
	static std::vector<float>   Deserialize(const uint8_t* data, size_t size);
};
