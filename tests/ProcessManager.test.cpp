#include <gtest/gtest.h>
#include "src/process/ProcessManager.h"

using namespace testing;

// Проверка добавления процессов и изменения счетчика
TEST(ProcessManagerTest, AddIncreasesCount)
{
	ProcessManager manager;
	manager.Add(Process("true", {}));

	EXPECT_EQ(manager.Count(), 1);
	EXPECT_FALSE(manager.Empty());
}

// Проверка ожидания всех процессов и очистки списка
TEST(ProcessManagerTest, WaitAllClearsList)
{
	ProcessManager manager;
	manager.Add(Process("true", {}));
	manager.Add(Process("true", {}));

	manager.WaitAll();

	EXPECT_EQ(manager.Count(), 0);
	EXPECT_TRUE(manager.Empty());
}

// Проверка ожидания произвольного процесса
TEST(ProcessManagerTest, WaitAnyDecreasesCount)
{
	ProcessManager manager;
	manager.Add(Process("true", {}));
	manager.Add(Process("sleep", {"0.1"}));

	manager.WaitAny();

	EXPECT_EQ(manager.Count(), 1);
}

// Проверка обработки ошибки при WaitAll
TEST(ProcessManagerTest, WaitAllThrowsOnFailure)
{
	ProcessManager manager;
	manager.Add(Process("false", {}));

	EXPECT_THROW(manager.WaitAll(), std::runtime_error);
}