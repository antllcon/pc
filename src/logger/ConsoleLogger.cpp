#include "ConsoleLogger.h"
#include <iostream>
#include <syncstream>

ConsoleLogger::ConsoleLogger(bool isEnabled)
	: m_isEnabled(isEnabled)
{
}

void ConsoleLogger::Log(const std::string& message)
{
	if (m_isEnabled.load(std::memory_order_relaxed))
	{
		std::osyncstream(std::cout) << message << std::endl;
	}
}

void ConsoleLogger::SetEnabled(bool isEnabled)
{
	m_isEnabled.store(isEnabled, std::memory_order_relaxed);
}