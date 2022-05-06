#include "Singletons/OpenDemeyer2D.h"

#if _DEBUG
#include <vld.h>
#endif

#include <SDL.h>

#include "BurgerTimeGI.h"
#include "EngineFiles/ComponentBase.h"

int main(int, char* []) {

	ENGINE.Run(new BurgerTimeGI());
	return 0;
}