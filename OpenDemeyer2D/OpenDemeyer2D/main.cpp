#include "OD2.pch"
#include "OpenDemeyer2D.h"

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif



int main(int, char* []) {

	ENGINE.Run();
	return 0;
}