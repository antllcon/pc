#pragma once

#include <string>
#include <vector>
#include <sys/types.h>

class Process
{
public:
	Process(const std::string& program, const std::vector<std::string>& args);
	~Process();

	Process(const Process&) = delete;
	Process& operator=(const Process&) = delete;

	Process(Process&& other) noexcept;
	Process& operator=(Process&& other) noexcept;

	void Wait();
	void MarkAsWaited();
	[[nodiscard]] pid_t GetPid() const;

private:
	pid_t m_pid;
	bool m_waited;
};