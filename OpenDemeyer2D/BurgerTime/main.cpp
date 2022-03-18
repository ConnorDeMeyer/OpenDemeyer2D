#include "OD2pch.h"
#include "OpenDemeyer2D.h"

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "BurgerTimeGI.h"


int main(int, char* []) {

	ENGINE.Run(new BurgerTimeGI());
	return 0;
}