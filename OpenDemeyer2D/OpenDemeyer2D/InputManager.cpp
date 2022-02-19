#include "OD2pch.h"
#include "InputManager.h"
#include "OpenDemeyer2D.h"

void InputManager::ProcessInput()
{

	//ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
	//XInputGetState(0, &m_CurrentState);

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			ENGINE.Quit();
		}
		if (e.type == SDL_KEYDOWN) {

		}
		if (e.type == SDL_MOUSEBUTTONDOWN) {

		}
	}
	
}
