#include "src/net/packet/AudioPacket.h"

#include <asio.hpp>
#include <cstring>
#include <stdexcept>

namespace
{
uint32_t FloatToNetwork(float f)
{
	uint32_t bits;
	std::memcpy(&bits, &f, sizeof(bits));
	return htonl(bits);
}

float FloatFromNetwork(uint32_t bits)
{
	bits = ntohl(bits);
	float f;
	std::memcpy(&f, &bits, sizeof(f));
	return f;
}
} // namespace

std::vector<uint8_t> AudioPacket::Serialize(const std::vector<float>& samples)
{
	std::vector<uint8_t> buffer(PACKET_BYTE_SIZE);

	const auto count       = static_cast<uint32_t>(samples.size());
	const uint32_t netCount = htonl(count);
	std::memcpy(buffer.data(), &netCount, sizeof(netCount));

	for (size_t i = 0; i < samples.size(); ++i)
	{
		const uint32_t netSample = FloatToNetwork(samples[i]);
		std::memcpy(
			buffer.data() + sizeof(uint32_t) + i * sizeof(float),
			&netSample,
			sizeof(netSample));
	}

	return buffer;
}

std::vector<float> AudioPacket::Deserialize(const uint8_t* data, size_t size)
{
	if (size < sizeof(uint32_t))
	{
		throw std::runtime_error("Повреждённый аудио-пакет: слишком короткий заголовок");
	}

	uint32_t netCount;
	std::memcpy(&netCount, data, sizeof(netCount));
	const uint32_t count = ntohl(netCount);

	const size_t expectedSize = sizeof(uint32_t) + count * sizeof(float);
	if (size < expectedSize)
	{
		throw std::runtime_error("Повреждённый аудио-пакет: недостаточно данных семплов");
	}

	std::vector<float> samples(count);
	for (uint32_t i = 0; i < count; ++i)
	{
		uint32_t netSample;
		std::memcpy(
			&netSample,
			data + sizeof(uint32_t) + i * sizeof(float),
			sizeof(netSample));
		samples[i] = FloatFromNetwork(netSample);
	}

	return samples;
}
