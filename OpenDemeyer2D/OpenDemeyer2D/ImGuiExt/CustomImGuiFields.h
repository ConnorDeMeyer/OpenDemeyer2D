#pragma once
#include "imgui.h"

#include <SDL_rect.h>


namespace ImGui
{
	bool CustomInput(const char* label, SDL_Rect& rect);
	bool CustomInput(const char* label, SDL_FRect& rect);
}