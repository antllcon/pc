#include "src/Process/Process.h"
#include <gtest/gtest.h>

using namespace testing;

// Проверка запуска простой команды
TEST(ProcessTest, EchoCommandReturnsSuccess)
{
	std::vector<std::string> args = { "Hello" };
	Process process("echo", args);

	EXPECT_NO_THROW(process.Wait());
}

// Проверка обработки ошибки (несуществующая программа)
TEST(ProcessTest, InvalidProgramThrowsException)
{
	std::vector<std::string> args;
	Process process("program_that_does_not_exist_12345", args);
	EXPECT_THROW(process.Wait(), std::runtime_error);
}