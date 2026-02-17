#include "Process.h"
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

namespace
{
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

void AssertIsExitStatusCorrect(int status)
{
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	{
		throw std::runtime_error("Дочерний процесс завершился с ошибкой");
	}
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
{
	auto argv = PrepareExecArguments(program, args);

	m_pid = fork();
	AssertIsForkSuccessful(m_pid);

	if (m_pid == 0)
	{
		execvp(program.c_str(), argv.data());
		_exit(127);
	}
}

Process::~Process()
{
	DisposeProcess(m_pid, m_waited);
}

Process::Process(Process&& other) noexcept
	: m_pid(other.m_pid)
	, m_waited(other.m_waited)
{
	other.m_pid = -1;
	other.m_waited = true;
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
	}

	return *this;
}

void Process::Wait()
{
	if (m_waited)
	{
		return;
	}

	int status = 0;
	pid_t result = waitpid(m_pid, &status, 0);

	AssertIsWaitSuccessful(result);
	AssertIsExitStatusCorrect(status);

	m_waited = true;
}

void Process::MarkAsWaited()
{
	m_waited = true;
}

pid_t Process::GetPid() const
{
	return m_pid;
}