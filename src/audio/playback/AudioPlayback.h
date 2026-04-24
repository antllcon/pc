#pragma once

#include "src/audio/AudioConstants.h"
#include "src/queue/AudioQueue.h"

#include <libs/miniaudio.h>
#include <memory>
#include <vector>

using PlaybackQueue = AudioQueue<std::vector<float>, 8>;

class AudioPlayback
{
public:
	explicit AudioPlayback(std::shared_ptr<PlaybackQueue> inputQueue);
	~AudioPlayback();

	AudioPlayback(const AudioPlayback&)            = delete;
	AudioPlayback& operator=(const AudioPlayback&) = delete;
	AudioPlayback(AudioPlayback&&)                 = delete;
	AudioPlayback& operator=(AudioPlayback&&)      = delete;

private:
	static void PlaybackCallback(
		ma_device*  device,
		void*       output,
		const void* input,
		ma_uint32   frameCount);

	std::shared_ptr<PlaybackQueue> m_inputQueue;
	std::vector<float>             m_currentPacket;
	size_t                         m_packetOffset{0};
	ma_device                      m_device{};
};
