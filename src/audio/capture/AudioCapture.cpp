#include "src/audio/capture/AudioCapture.h"

#include <libs/miniaudio.h>
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

AudioCapture::AudioCapture(std::shared_ptr<CaptureQueue> outputQueue)
	: m_outputQueue(std::move(outputQueue))
{
	m_accumulator.reserve(AudioConfig::SAMPLES_PER_PACKET * 2);

	ma_device_config config      = ma_device_config_init(ma_device_type_capture);
	config.capture.format        = ma_format_f32;
	config.capture.channels      = AudioConfig::CHANNELS;
	config.sampleRate            = AudioConfig::SAMPLE_RATE;
	config.dataCallback          = CaptureCallback;
	config.pUserData             = this;

	AssertMaResult(
		ma_device_init(nullptr, &config, &m_device),
		"Не удалось инициализировать устройство захвата звука");

	AssertMaResult(
		ma_device_start(&m_device),
		"Не удалось запустить устройство захвата звука");
}

AudioCapture::~AudioCapture()
{
	ma_device_uninit(&m_device);
}

void AudioCapture::CaptureCallback(
	ma_device*  device,
	void*       /*output*/,
	const void* input,
	ma_uint32   frameCount)
{
	auto* self        = static_cast<AudioCapture*>(device->pUserData);
	const auto* samples = static_cast<const float*>(input);

	self->m_accumulator.insert(
		self->m_accumulator.end(),
		samples,
		samples + frameCount);

	while (self->m_accumulator.size() >= AudioConfig::SAMPLES_PER_PACKET)
	{
		std::vector<float> chunk(
			self->m_accumulator.begin(),
			self->m_accumulator.begin() + AudioConfig::SAMPLES_PER_PACKET);

		self->m_accumulator.erase(
			self->m_accumulator.begin(),
			self->m_accumulator.begin() + AudioConfig::SAMPLES_PER_PACKET);

		self->m_outputQueue->TryPush(std::move(chunk));
	}
}
