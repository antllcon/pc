#pragma once

#include "src/audio/playback/AudioPlayback.h"
#include "src/logger/ILogger.h"
#include "src/visualizer/VisualizerThread.h"

#include <asio.hpp>
#include <array>
#include <memory>
#include <string>
#include <thread>

class RadioReceiver
{
public:
	RadioReceiver(
		const std::string&               address,
		uint16_t                         port,
		std::shared_ptr<PlaybackQueue>   playbackQueue,
		std::shared_ptr<VisualizerQueue> visualizerQueue,
		std::shared_ptr<ILogger>         logger);

	~RadioReceiver();

	RadioReceiver(const RadioReceiver&)            = delete;
	RadioReceiver& operator=(const RadioReceiver&) = delete;

	void Run();

private:
	void StartReceive();

	asio::io_context                             m_ioContext;
	asio::ip::tcp::socket                        m_socket;
	asio::signal_set                             m_signals;
	std::shared_ptr<PlaybackQueue>               m_playbackQueue;
	std::shared_ptr<VisualizerQueue>             m_visualizerQueue;
	std::shared_ptr<ILogger>                     m_logger;
	std::vector<uint8_t>                         m_buffer;
	std::jthread                                 m_ioThread;
};
