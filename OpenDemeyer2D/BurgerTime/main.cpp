#include "OD2pch.h"
#include "OpenDemeyer2D.h"

#if _DEBUG
#include <vld.h>
#endif

#include "BurgerTimeGI.h"
#include "ComponentBase.h"

int main(int, char* []) {

	ENGINE.Run(new BurgerTimeGI());
	return 0;
}