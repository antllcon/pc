#include "src/net/receiver/RadioReceiver.h"

#include "src/net/packet/AudioPacket.h"

#include <asio.hpp>
#include <stdexcept>

RadioReceiver::RadioReceiver(
	const std::string&               address,
	uint16_t                         port,
	std::shared_ptr<PlaybackQueue>   playbackQueue,
	std::shared_ptr<VisualizerQueue> visualizerQueue,
	std::shared_ptr<ILogger>         logger)
	: m_ioContext()
	, m_socket(m_ioContext)
	, m_signals(m_ioContext, SIGINT, SIGTERM)
	, m_playbackQueue(std::move(playbackQueue))
	, m_visualizerQueue(std::move(visualizerQueue))
	, m_logger(std::move(logger))
	, m_buffer(PACKET_BYTE_SIZE)
{
	asio::ip::tcp::resolver           resolver(m_ioContext);
	const asio::ip::tcp::resolver::results_type endpoints =
		resolver.resolve(address, std::to_string(port));

	asio::error_code ec;
	asio::connect(m_socket, endpoints, ec);
	if (ec)
	{
		throw std::runtime_error(
			"Не удалось подключиться к серверу: " + ec.message());
	}

	asio::error_code optEc;
	m_socket.set_option(asio::ip::tcp::no_delay(true), optEc);

	m_logger->Log("Подключено к радиостанции " + address + ":" + std::to_string(port));

	m_signals.async_wait([this](const asio::error_code& ec, int /*signal*/)
	{
		if (!ec)
		{
			m_logger->Log("Получен сигнал завершения, отключаюсь...");
			asio::error_code closeEc;
			m_socket.close(closeEc);
			m_ioContext.stop();
			m_playbackQueue->Close();
		}
	});

	StartReceive();

	m_ioThread = std::jthread([this](std::stop_token /*stopToken*/)
	{
		m_ioContext.run();
	});
}

RadioReceiver::~RadioReceiver()
{
	m_playbackQueue->Close();

	asio::error_code ec;
	m_socket.close(ec);
	m_ioContext.stop();
}

void RadioReceiver::Run()
{
	m_ioThread.join();
}

void RadioReceiver::StartReceive()
{
	asio::async_read(
		m_socket,
		asio::buffer(m_buffer),
		[this](const asio::error_code& ec, size_t /*bytesRead*/)
		{
			if (ec)
			{
				if (ec == asio::error::eof || ec == asio::error::connection_reset)
				{
					m_logger->Log("Сервер завершил трансляцию");
				}
				else
				{
					m_logger->Log("Ошибка приёма данных: " + ec.message());
				}
				m_playbackQueue->Close();
				m_ioContext.stop();
				return;
			}

			try
			{
				auto samples = AudioPacket::Deserialize(m_buffer.data(), m_buffer.size());
				m_playbackQueue->Push(samples);
				m_visualizerQueue->TryPush(std::move(samples));
			}
			catch (const std::runtime_error& e)
			{
				m_logger->Log(std::string("Ошибка разбора пакета: ") + e.what());
			}

			StartReceive();
		});
}
