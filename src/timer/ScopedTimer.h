#pragma once
#include <chrono>
#include <string>

class ScopedTimer
{
public:
	ScopedTimer(std::string  phaseName, std::ostream& outputStream);
	~ScopedTimer();

private:
	std::string m_phaseName;
	std::ostream& m_outputStream;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
};