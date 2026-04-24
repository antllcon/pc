#pragma once

#include "src/audio/capture/AudioCapture.h"
#include "src/logger/ILogger.h"
#include "src/visualizer/VisualizerThread.h"

#include <asio.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class RadioStation
{
public:
	RadioStation(
		uint16_t                         port,
		std::shared_ptr<CaptureQueue>    captureQueue,
		std::shared_ptr<VisualizerQueue> visualizerQueue,
		std::shared_ptr<ILogger>         logger);

	~RadioStation();

	RadioStation(const RadioStation&)            = delete;
	RadioStation& operator=(const RadioStation&) = delete;

	void Run();

private:
	struct ClientSession
	{
		asio::ip::tcp::socket socket;
		std::atomic<bool>     alive{true};

		explicit ClientSession(asio::ip::tcp::socket s)
			: socket(std::move(s))
		{
		}

		bool Send(const std::vector<uint8_t>& data);
	};

	void StartAccept();
	void BroadcastLoop(std::stop_token stopToken);
	void RemoveDeadSessions();

	asio::io_context                             m_ioContext;
	asio::ip::tcp::acceptor                      m_acceptor;
	asio::signal_set                             m_signals;
	std::shared_ptr<CaptureQueue>                m_captureQueue;
	std::shared_ptr<VisualizerQueue>             m_visualizerQueue;
	std::shared_ptr<ILogger>                     m_logger;
	std::vector<std::shared_ptr<ClientSession>>  m_sessions;
	std::mutex                                   m_sessionsMutex;
	std::jthread                                 m_ioThread;
	std::jthread                                 m_broadcastThread;
};
