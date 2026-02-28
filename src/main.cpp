#include "Windows.h"
#include "system/Application.h"

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	auto app = std::make_shared<Application>();
	app->Init();
	app->Run();

	return EXIT_SUCCESS;
}