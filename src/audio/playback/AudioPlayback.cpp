#include "src/audio/playback/AudioPlayback.h"

#include <libs/miniaudio.h>
#include <cstring>
#include <stdexcept>

namespace
{
void AssertMaResult(ma_result result, const char* message)
{
	if (result != MA_SUCCESS)
	{
		throw std::runtime_error(message);
	}
}
} // namespace

AudioPlayback::AudioPlayback(std::shared_ptr<PlaybackQueue> inputQueue)
	: m_inputQueue(std::move(inputQueue))
{
	ma_device_config config       = ma_device_config_init(ma_device_type_playback);
	config.playback.format        = ma_format_f32;
	config.playback.channels      = AudioConfig::CHANNELS;
	config.sampleRate             = AudioConfig::SAMPLE_RATE;
	config.dataCallback           = PlaybackCallback;
	config.pUserData              = this;

	AssertMaResult(
		ma_device_init(nullptr, &config, &m_device),
		"Не удалось инициализировать устройство воспроизведения звука");

	AssertMaResult(
		ma_device_start(&m_device),
		"Не удалось запустить устройство воспроизведения звука");
}

AudioPlayback::~AudioPlayback()
{
	ma_device_uninit(&m_device);
}

void AudioPlayback::PlaybackCallback(
	ma_device* device,
	void*      output,
	const void* /*input*/,
	ma_uint32  frameCount)
{
	auto* self    = static_cast<AudioPlayback*>(device->pUserData);
	auto* out     = static_cast<float*>(output);

	for (ma_uint32 i = 0; i < frameCount; ++i)
	{
		if (self->m_packetOffset >= self->m_currentPacket.size())
		{
			auto packet = self->m_inputQueue->TryPop();
			if (packet.has_value())
			{
				self->m_currentPacket = std::move(*packet);
				self->m_packetOffset  = 0;
			}
			else
			{
				out[i] = 0.0f;
				continue;
			}
		}
		out[i] = self->m_currentPacket[self->m_packetOffset++];
	}
}
