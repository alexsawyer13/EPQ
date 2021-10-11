#include <Core/Game.h>

int main()
{
#ifndef NDEBUG
	static_assert(false);
#endif

	Setup();
	Run();
	Cleanup();
}