#include "OD2.pch"
#include "InputManager.h"
#include "OpenDemeyer2D.h"

#include "backends/imgui_impl_sdl.h"

void InputManager::ProcessInput()
{

	//ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
	//XInputGetState(0, &m_CurrentState);

	SDL_Event e;
	while (SDL_PollEvent(&e)) {

		ImGui_ImplSDL2_ProcessEvent(&e);

		if (e.type == SDL_QUIT) {
			ENGINE.Quit();
		}
		if (e.type == SDL_KEYDOWN) {

		}
		if (e.type == SDL_MOUSEBUTTONDOWN) {

		}
	}
	
}
