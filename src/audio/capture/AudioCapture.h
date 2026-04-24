#pragma once

#include "src/audio/AudioConstants.h"
#include "src/queue/AudioQueue.h"

#include <libs/miniaudio.h>
#include <memory>
#include <vector>

using CaptureQueue = AudioQueue<std::vector<float>, 8>;

class AudioCapture
{
public:
	explicit AudioCapture(std::shared_ptr<CaptureQueue> outputQueue);
	~AudioCapture();

	AudioCapture(const AudioCapture&)            = delete;
	AudioCapture& operator=(const AudioCapture&) = delete;
	AudioCapture(AudioCapture&&)                 = delete;
	AudioCapture& operator=(AudioCapture&&)      = delete;

private:
	static void CaptureCallback(
		ma_device*  device,
		void*       output,
		const void* input,
		ma_uint32   frameCount);

	std::shared_ptr<CaptureQueue> m_outputQueue;
	std::vector<float>            m_accumulator;
	ma_device                     m_device{};
};
