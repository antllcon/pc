#pragma once
#include "Process.h"
#include <list>

class ProcessManager
{
public:
	ProcessManager();
	~ProcessManager();

	void Add(Process&& process);

	void WaitAny();
	void WaitAll();

	[[nodiscard]] size_t Count() const;
	[[nodiscard]] bool Empty() const;

private:
	void HandleFinishedProcess(pid_t pid);

	std::list<Process> m_activeProcesses;
};