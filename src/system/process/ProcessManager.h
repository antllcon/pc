#pragma once
#include "Process.h"
#include <list>

class ProcessManager
{
public:
	ProcessManager();
	~ProcessManager();

	void Add(Process&& process);

	bool WaitAny();
	bool WaitAll();

	[[nodiscard]] size_t Count() const;
	[[nodiscard]] bool Empty() const;

private:
	void HandleFinishedProcess(pid_t pid, bool success);

	std::list<Process> m_activeProcesses;
};