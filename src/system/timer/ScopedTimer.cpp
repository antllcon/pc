#include "ScopedTimer.h"
#include <iomanip>
#include <stdexcept>
#include <utility>

namespace
{
void AssertIsPhaseNameValid(const std::string& name)
{
	if (name.empty())
	{
		throw std::invalid_argument("Имя фазы для таймера не может быть пустым");
	}
}
}

ScopedTimer::ScopedTimer(std::string phaseName, std::ostream& outputStream)
	: m_phaseName(std::move(phaseName))
	, m_outputStream(outputStream)
	, m_startTime(std::chrono::high_resolution_clock::now())
{
	AssertIsPhaseNameValid(m_phaseName);
}

ScopedTimer::~ScopedTimer()
{
	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = endTime - m_startTime;

	m_outputStream << "[Timer] " << m_phaseName << ": "
				   << std::fixed << std::setprecision(3) << duration.count()
				   << " s" << std::endl;
}