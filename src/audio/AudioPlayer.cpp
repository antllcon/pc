#define MINIAUDIO_IMPLEMENTATION
#include <libs/miniaudio.h>

#include "AudioPlayer.h"
#include "AudioTypes.h"
#include <chrono>
#include <span>
#include <stdexcept>
#include <thread>

namespace
{
struct PlaybackContext
{
	std::span<const double> AudioData;
	size_t CurrentOffset = 0;
};

void AssertMiniaudioSuccess(const ma_result result)
{
	if (result != MA_SUCCESS)
	{
		throw std::runtime_error("Произошла ошибка при работе с аудиоустройством ОС");
	}
}

void AudioDataCallback(ma_device* device, void* output, const void* input, const ma_uint32 frameCount)
{
	(void)input;

	auto* context = static_cast<PlaybackContext*>(device->pUserData);
	auto* floatOutput = static_cast<float*>(output);

	for (ma_uint32 i = 0; i < frameCount; ++i)
	{
		if (context->CurrentOffset < context->AudioData.size())
		{
			floatOutput[i] = static_cast<float>(context->AudioData[context->CurrentOffset]);
			context->CurrentOffset++;
		}
		else
		{
			floatOutput[i] = 0.0f;
		}
	}
}

class ScopedAudioDevice
{
public:
	explicit ScopedAudioDevice(PlaybackContext& context)
	{
		ma_device_config config = ma_device_config_init(ma_device_type_playback);

		config.playback.format = ma_format_f32;
		config.playback.channels = 1;
		config.sampleRate = STANDARD_SAMPLE_RATE;
		config.dataCallback = AudioDataCallback;
		config.pUserData = &context;

		AssertMiniaudioSuccess(ma_device_init(nullptr, &config, &m_device));
		AssertMiniaudioSuccess(ma_device_start(&m_device));
	}

	~ScopedAudioDevice()
	{
		ma_device_uninit(&m_device);
	}

	ScopedAudioDevice(const ScopedAudioDevice&) = delete;
	ScopedAudioDevice& operator=(const ScopedAudioDevice&) = delete;

private:
	ma_device m_device;
};

void WaitUntilPlaybackFinishes(const PlaybackContext& context)
{
	while (context.CurrentOffset < context.AudioData.size())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
} // namespace

void AudioPlayer::Play(const std::vector<double>& audioBuffer)
{
	PlaybackContext context{audioBuffer, 0};
	ScopedAudioDevice device(context);

	WaitUntilPlaybackFinishes(context);
}