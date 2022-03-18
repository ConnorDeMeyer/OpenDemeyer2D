#include "OD2pch.h"
#include "InputManager.h"
#include "OpenDemeyer2D.h"
#include "Components/InputComponent.h"

#include "backends/imgui_impl_sdl.h"



void InputManager::ProcessInput()
{
	
	SDL_Event e;
	while (SDL_PollEvent(&e)) {

		ImGui_ImplSDL2_ProcessEvent(&e);

		if (e.type == SDL_QUIT) {
			ENGINE.Quit();
		}
		else if (e.type == SDL_KEYDOWN) {
			HandleKeyDown(e);
		}
		else if (e.type == SDL_KEYUP) {
			HandleKeyUp(e);
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			HandleMouseDown(e);
		}
		else if (e.type == SDL_MOUSEBUTTONUP) {
			HandleMouseUp(e);
		}
		else if (e.type == SDL_MOUSEWHEEL) {
			HandleMouseWheel(e);
		}
		else if (e.type == SDL_MOUSEMOTION) {
			HandleMouseMotion(e);
		}
		else if (e.type == SDL_CONTROLLERBUTTONUP) {
			HandleControllerButtonUp(e);
		}
		else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
			HandleControllerButtonDown(e);
		}
		else if (e.type == SDL_CONTROLLERAXISMOTION) {
			HandleControllerAxis(e);
		}
	}
}

int InputManager::RegisterInputComponent(InputComponent* comp)
{
	if (!comp) return -1;

	for (uint8_t i{}; i < MaxControllers; ++i)
	{
		if (!m_pControllerInputComps[i]) {

			m_pJoysticks[i] = SDL_JoystickOpen(i);
			m_pControllers[i] = SDL_GameControllerOpen(i);

			if (m_pControllers[i] == nullptr)
			{
				printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
			}

			m_pControllerInputComps[i] = comp;
			comp->m_ControllerId = i;
			return i;
		}
	}
	return -1;
}

void InputManager::UnregisterInputComponent(InputComponent* comp)
{
	if (!comp) return;

	for (uint8_t i{}; i < MaxControllers; ++i)
	{
		if (m_pControllerInputComps[i] == comp) {

			SDL_GameControllerClose(m_pControllers[i]);
			SDL_JoystickClose(m_pJoysticks[i]);
			m_pControllers[i] = nullptr;

			m_pControllerInputComps[i] = nullptr;
			comp->m_ControllerId = 0;
		}
	}
}

bool InputManager::IsKeyPressed(SDL_Keycode SDLK_key) const
{
	const Uint8* keyState = SDL_GetKeyboardState(nullptr);

	return (keyState[SDLK_key]);
}

bool InputManager::IsMouseButtonPressed(Uint8 SDL_BUTTON) const
{
	Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);

	return mouseState & SDL_BUTTON;
}

bool InputManager::IsControllerButtonDown(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId) const
{
	return SDL_GameControllerGetButton(m_pControllers[controllerId], SDL_CONTROLLER_BUTTON);
}

void InputManager::HandleKeyDown(const SDL_Event& e)
{
	auto it = m_KeyDownActions.find(e.key.keysym.sym);
	if (it != m_KeyDownActions.end())
	{
		it->second.BroadCast();
	}
}

void InputManager::HandleKeyUp(const SDL_Event& e)
{
	auto it = m_KeyUpActions.find(e.key.keysym.sym);
	if (it != m_KeyUpActions.end())
	{
		it->second.BroadCast();
	}
}

void InputManager::HandleMouseDown(const SDL_Event& e)
{
	auto it = m_MouseDownActions.find(e.button.button);
	if (it != m_MouseDownActions.end())
	{
		it->second.BroadCast(float(e.button.x), float(e.button.y));
	}
}

void InputManager::HandleMouseUp(const SDL_Event& e)
{
	auto it = m_MouseUpActions.find(e.button.button);
	if (it != m_MouseUpActions.end())
	{
		it->second.BroadCast(float(e.button.x), float(e.button.y));
	}
}

void InputManager::HandleMouseMotion(const SDL_Event& e)
{
	m_MouseScrollAxis.BroadCast(float(e.motion.xrel), float(e.motion.yrel));
}

void InputManager::HandleMouseWheel(const SDL_Event& e)
{
	m_MouseScrollAxis.BroadCast(e.wheel.preciseY);
}

void InputManager::HandleControllerButtonUp(const SDL_Event& e)
{
	auto it = m_pControllerUpActions[e.cbutton.which].find(SDL_GameControllerButton(e.cbutton.button));
	if (it != m_pControllerUpActions[e.cbutton.which].end())
	{
		it->second.BroadCast();
	}
}

void InputManager::HandleControllerButtonDown(const SDL_Event& e)
{
	auto it = m_pControllerDownActions[e.cbutton.which].find(SDL_GameControllerButton(e.cbutton.button));
	if (it != m_pControllerDownActions[e.cbutton.which].end())
	{
		it->second.BroadCast();
	}
}

void InputManager::HandleControllerAxis(const SDL_Event& e)
{
	auto it = m_pControllerAxis[e.cbutton.which].find(SDL_GameControllerAxis(e.caxis.axis));
	if (it != m_pControllerAxis[e.cbutton.which].end())
	{
		it->second.BroadCast(float(e.caxis.value) / float(SDL_JOYSTICK_AXIS_MAX));
	}
}
