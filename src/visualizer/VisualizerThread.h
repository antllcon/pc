#pragma once

#include "src/visualizer/AudioVisualizer.h"
#include "src/queue/AudioQueue.h"

#include <memory>
#include <thread>
#include <vector>

using VisualizerQueue = AudioQueue<std::vector<float>, 4>;

class VisualizerThread
{
public:
	explicit VisualizerThread(std::shared_ptr<VisualizerQueue> queue);
	~VisualizerThread();

	VisualizerThread(const VisualizerThread&)            = delete;
	VisualizerThread& operator=(const VisualizerThread&) = delete;

	void Stop();

private:
	void DisplayLoop(std::stop_token stopToken);

	std::shared_ptr<VisualizerQueue> m_queue;
	AudioVisualizer                  m_visualizer;
	std::jthread                     m_thread;
};
