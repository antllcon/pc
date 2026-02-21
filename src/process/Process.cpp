#include "Process.h"

#include <iostream>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

namespace
{
constexpr int COMMAND_NOT_FOUND_EXIT_CODE = 127;

void AssertIsForkSuccessful(pid_t pid)
{
	if (pid < 0)
	{
		throw std::runtime_error("Не удалось создать дочерний процесс (fork вернул ошибку)");
	}
}

void AssertIsWaitSuccessful(pid_t result)
{
	if (result == -1)
	{
		throw std::runtime_error("Ошибка при ожидании завершения процесса");
	}
}

bool EvaluateProcessStatus(int status, pid_t pid)
{
	if (WIFEXITED(status))
	{
		int code = WEXITSTATUS(status);
		if (code == 0)
		{
			return true;
		}

		std::cerr << "Процесс (PID: " << pid << ") завершился с кодом ошибки: " << code << std::endl;
		return false;
	}

	if (WIFSIGNALED(status))
	{
		std::cerr << "Процесс (PID: " << pid << ") убит сигналом: " << WTERMSIG(status) << std::endl;
	}

	return false;
}

std::vector<char*> PrepareExecArguments(const std::string& program, const std::vector<std::string>& args)
{
	std::vector<char*> cArgs;
	cArgs.reserve(args.size() + 2);

	cArgs.push_back(const_cast<char*>(program.c_str()));

	for (const auto& arg : args)
	{
		cArgs.push_back(const_cast<char*>(arg.c_str()));
	}

	cArgs.push_back(nullptr);
	return cArgs;
}

void DisposeProcess(pid_t pid, bool waited)
{
	if (pid > 0 && !waited)
	{
		int status = 0;
		waitpid(pid, &status, 0);
	}
}
} // namespace

Process::Process(const std::string& program, const std::vector<std::string>& args)
	: m_pid(-1)
	, m_waited(false)
	, m_success(false)
{
	auto argv = PrepareExecArguments(program, args);

	m_pid = fork();
	AssertIsForkSuccessful(m_pid);

	if (m_pid == 0)
	{
		execvp(program.c_str(), argv.data());
		_exit(COMMAND_NOT_FOUND_EXIT_CODE);
	}
}

Process::~Process()
{
	DisposeProcess(m_pid, m_waited);
}

Process::Process(Process&& other) noexcept
	: m_pid(other.m_pid)
	, m_waited(other.m_waited)
	, m_success(other.m_success)
{
	other.m_pid = -1;
	other.m_waited = true;
	other.m_success = false;
}

Process& Process::operator=(Process&& other) noexcept
{
	if (this != &other)
	{
		DisposeProcess(m_pid, m_waited);

		m_pid = other.m_pid;
		m_waited = other.m_waited;

		other.m_pid = -1;
		other.m_waited = true;
		other.m_success = false;
	}

	return *this;
}

bool Process::Wait()
{
	if (m_waited)
	{
		return m_success;
	}

	int status = 0;
	pid_t result = waitpid(m_pid, &status, 0);

	AssertIsWaitSuccessful(result);
	m_success = EvaluateProcessStatus(status, m_pid);
	m_waited = true;

	return m_success;
}

void Process::MarkAsWaited(bool success)
{
	m_waited = true;
	m_success = success;
}

pid_t Process::GetPid() const
{
	return m_pid;
}