#include "src/process/Process.h"
#include <gtest/gtest.h>

using namespace testing;

// Проверка успешного выполнения простой команды
TEST(ProcessTest, ExecuteTrueSuccess)
{
	Process process("true", {});
	process.Wait();
	EXPECT_GT(process.GetPid(), 0);
}

// Проверка выброса исключения при ошибке в дочернем процессе
TEST(ProcessTest, ExecuteFalseThrows)
{
	Process process("false", {});
	EXPECT_THROW(process.Wait(), std::runtime_error);
}

// Проверка перемещения процесса
TEST(ProcessTest, MoveConstructorWorks)
{
	Process original("true", {});
	pid_t originalPid = original.GetPid();

	Process moved(std::move(original));

	EXPECT_EQ(moved.GetPid(), originalPid);
	EXPECT_NO_THROW(moved.Wait());
}