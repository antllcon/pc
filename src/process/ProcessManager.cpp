#include "ProcessManager.h"
#include <algorithm>
#include <iostream>
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

bool EvaluateManagerProcessStatus(int status, pid_t pid)
{
	if (WIFEXITED(status))
	{
		int code = WEXITSTATUS(status);
		if (code == 0)
		{
			return true;
		}

		std::cerr << "Процесс (PID: " << pid << ") завершился с ошибкой, код: " << code << std::endl;
		return false;
	}

	if (WIFSIGNALED(status))
	{
		std::cerr << "Процесс (PID: " << pid << ") убит сигналом: " << WTERMSIG(status) << std::endl;
	}

	return false;
}
} // namespace

ProcessManager::ProcessManager() = default;

ProcessManager::~ProcessManager()
{
	(void)WaitAll();
}

void ProcessManager::Add(Process&& process)
{
	m_activeProcesses.push_back(std::move(process));
}

bool ProcessManager::WaitAny()
{
	if (m_activeProcesses.empty())
	{
		return true;
	}

	int status = 0;
	pid_t finishedPid = waitpid(WAIT_MYPGRP, &status, 0);

	AssertIsWaitAnySuccessful(finishedPid);
	bool success = EvaluateManagerProcessStatus(status, finishedPid);
	HandleFinishedProcess(finishedPid, success);

	return success;
}

bool ProcessManager::WaitAll()
{
	bool allSuccess = true;

	for (auto& process : m_activeProcesses)
	{
		if (!process.Wait())
		{
			allSuccess = false;
		}
	}

	m_activeProcesses.clear();
	return allSuccess;
}

size_t ProcessManager::Count() const
{
	return m_activeProcesses.size();
}

bool ProcessManager::Empty() const
{
	return m_activeProcesses.empty();
}

void ProcessManager::HandleFinishedProcess(pid_t pid, bool success)
{
	auto it = std::ranges::find_if(m_activeProcesses,
		[pid](const Process& process) {
			return process.GetPid() == pid;
		});

	if (it != m_activeProcesses.end())
	{
		it->MarkAsWaited(success);
		m_activeProcesses.erase(it);
	}
}