#include "src/visualizer/VisualizerThread.h"

#include <iostream>
#include <syncstream>

VisualizerThread::VisualizerThread(std::shared_ptr<VisualizerQueue> queue)
	: m_queue(std::move(queue))
	, m_visualizer(40)
	, m_thread([this](std::stop_token stopToken) { DisplayLoop(std::move(stopToken)); })
{
}

VisualizerThread::~VisualizerThread()
{
	Stop();
}

void VisualizerThread::Stop()
{
	m_queue->Close();
	m_thread.request_stop();
}

void VisualizerThread::DisplayLoop(std::stop_token stopToken)
{
	while (!stopToken.stop_requested())
	{
		std::vector<float> samples;
		try
		{
			samples = m_queue->Pop();
		}
		catch (const std::runtime_error&)
		{
			break;
		}

		m_visualizer.Update(samples);
		std::osyncstream(std::cout) << '\r' << m_visualizer.Render() << std::flush;
	}

	std::osyncstream(std::cout) << '\n' << std::flush;
}
