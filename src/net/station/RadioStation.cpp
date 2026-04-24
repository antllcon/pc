#include "src/net/station/RadioStation.h"

#include "src/net/packet/AudioPacket.h"

#include <asio.hpp>

namespace
{
std::string FormatEndpoint(const asio::ip::tcp::endpoint& endpoint)
{
	return endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
}
} // namespace

bool RadioStation::ClientSession::Send(const std::vector<uint8_t>& data)
{
	asio::error_code ec;
	asio::write(socket, asio::buffer(data), ec);
	if (ec)
	{
		alive = false;
	}
	return alive.load();
}

RadioStation::RadioStation(
	uint16_t                         port,
	std::shared_ptr<CaptureQueue>    captureQueue,
	std::shared_ptr<VisualizerQueue> visualizerQueue,
	std::shared_ptr<ILogger>         logger)
	: m_ioContext()
	, m_acceptor(m_ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	, m_signals(m_ioContext, SIGINT, SIGTERM)
	, m_captureQueue(std::move(captureQueue))
	, m_visualizerQueue(std::move(visualizerQueue))
	, m_logger(std::move(logger))
{
	m_signals.async_wait([this](const asio::error_code& ec, int /*signal*/)
	{
		if (!ec)
		{
			m_logger->Log("Получен сигнал завершения, останавливаю радиостанцию...");
			m_acceptor.close();
			m_ioContext.stop();
			m_captureQueue->Close();
		}
	});

	StartAccept();

	m_ioThread = std::jthread([this](std::stop_token /*stopToken*/)
	{
		m_ioContext.run();
	});

	m_broadcastThread = std::jthread([this](std::stop_token stopToken)
	{
		BroadcastLoop(std::move(stopToken));
	});
}

RadioStation::~RadioStation()
{
	m_captureQueue->Close();

	asio::error_code ec;
	m_acceptor.close(ec);
	m_ioContext.stop();
}

void RadioStation::Run()
{
	m_ioThread.join();
}

void RadioStation::StartAccept()
{
	m_acceptor.async_accept([this](const asio::error_code& ec, asio::ip::tcp::socket socket)
	{
		if (ec)
		{
			return;
		}

		asio::error_code optEc;
		socket.set_option(asio::ip::tcp::no_delay(true), optEc);

		const std::string endpoint = FormatEndpoint(socket.remote_endpoint(optEc));
		m_logger->Log("Подключился клиент: " + endpoint);

		{
			std::lock_guard lock(m_sessionsMutex);
			m_sessions.push_back(std::make_shared<ClientSession>(std::move(socket)));
		}

		StartAccept();
	});
}

void RadioStation::BroadcastLoop(std::stop_token stopToken)
{
	while (!stopToken.stop_requested())
	{
		std::vector<float> chunk;
		try
		{
			chunk = m_captureQueue->Pop();
		}
		catch (const std::runtime_error&)
		{
			break;
		}

		const auto packet = AudioPacket::Serialize(chunk);

		{
			std::lock_guard lock(m_sessionsMutex);
			for (auto& session : m_sessions)
			{
				if (session->alive.load())
				{
					session->Send(packet);
				}
			}
		}

		m_visualizerQueue->TryPush(chunk);
		RemoveDeadSessions();
	}
}

void RadioStation::RemoveDeadSessions()
{
	std::lock_guard lock(m_sessionsMutex);
	const size_t before = m_sessions.size();

	m_sessions.erase(
		std::remove_if(
			m_sessions.begin(),
			m_sessions.end(),
			[](const std::shared_ptr<ClientSession>& s) { return !s->alive.load(); }),
		m_sessions.end());

	const size_t removed = before - m_sessions.size();
	if (removed > 0)
	{
		m_logger->Log("Отключилось клиентов: " + std::to_string(removed));
	}
}
