#include <cds/container/treiber_stack.h>
#include <cds/gc/hp.h>
#include <cds/init.h>
#include <cds/threading/model.h>
#include <iostream>

struct EliminationTraits : cds::container::treiber_stack::make_traits<cds::opt::enable_elimination<true>>::type
{
};

using BasicTreiberStack = cds::container::TreiberStack<cds::gc::HP, int>;
using EliminationStack = cds::container::TreiberStack<cds::gc::HP, int, EliminationTraits>;

namespace
{
void RunBasicStackTest()
{
	BasicTreiberStack stack;

	stack.push(67);
	stack.push(67);

	int value = 0;
	if (stack.pop(value))
	{
		std::cout << "Basic Stack Pop: " << value << std::endl;
	}
}

void RunEliminationStackTest()
{
	EliminationStack stack;

	stack.push(670);
	stack.push(670);

	int value = 0;
	if (stack.pop(value))
	{
		std::cout << "Elimination Stack Pop: " << value << std::endl;
	}
}
} // namespace

int main()
{
	cds::Initialize();

	{
		// TODO: RAII обертки надо бы
		cds::gc::HP hazardPointers;
		cds::threading::Manager::attachThread();

		RunBasicStackTest();
		RunEliminationStackTest();

		cds::threading::Manager::detachThread();
	}

	cds::Terminate();

	return 0;
}