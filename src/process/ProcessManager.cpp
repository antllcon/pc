#include "ProcessManager.h"
#include <algorithm>
#include <stdexcept>
#include <sys/wait.h>

namespace
{
void AssertIsWaitAnySuccessful(pid_t pid)
{
	if (pid == -1)
	{
		throw std::runtime_error("Ошибка ожидания произвольного процесса");
	}
}

void AssertIsChildSuccess(int status)
{
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	{
		throw std::runtime_error("Один из процессов завершился с ошибкой");
	}
}
}

ProcessManager::ProcessManager() = default;

ProcessManager::~ProcessManager()
{
	WaitAll();
}

void ProcessManager::Add(Process&& process)
{
	m_activeProcesses.push_back(std::move(process));
}

void ProcessManager::WaitAny()
{
	if (m_activeProcesses.empty())
	{
		return;
	}

	int status = 0;
	pid_t finishedPid = waitpid(-1, &status, 0);

	AssertIsWaitAnySuccessful(finishedPid);
	AssertIsChildSuccess(status);

	HandleFinishedProcess(finishedPid);
}

void ProcessManager::WaitAll()
{
	for (auto& process : m_activeProcesses)
	{
		process.Wait();
	}

	m_activeProcesses.clear();
}

size_t ProcessManager::Count() const
{
	return m_activeProcesses.size();
}

bool ProcessManager::Empty() const
{
	return m_activeProcesses.empty();
}

void ProcessManager::HandleFinishedProcess(pid_t pid)
{
	auto it = std::ranges::find_if(m_activeProcesses,
		[pid](const Process& process) {
			return process.GetPid() == pid;
		});

	if (it != m_activeProcesses.end())
	{
		it->MarkAsWaited();
		m_activeProcesses.erase(it);
	}
}