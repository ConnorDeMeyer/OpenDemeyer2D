#include "InputManager.h"
#include "OpenDemeyer2D.h"
#include "Components/InputComponent.h"

#include "backends/imgui_impl_sdl.h"
#include <SDL.h>


void InputManager::ProcessInput()
{
	SDL_GetMouseState(&m_MouseX, &m_MouseY);
	
	SDL_Event e;
	while (SDL_PollEvent(&e)) {

#ifdef _DEBUG
		ImGui_ImplSDL2_ProcessEvent(&e);
#endif

		if (e.type == SDL_KEYDOWN) {
			if (e.key.repeat) break;
			HandleKeyDown(e);
		}
		else if (e.type == SDL_KEYUP) {
			if (e.key.repeat) break;
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
		//else if (e.type == SDL_CONTROLLERAXISMOTION) {
		//	HandleControllerAxis(e);
		//}
		else if (e.type == SDL_WINDOWEVENT) {
			HandleWindowEvent(e);
		}
		else if (e.type == SDL_QUIT) {
			ENGINE.Quit();
		}
	}

	for (auto& mousePress : m_PressedKeys)
		m_KeyPressedActions[mousePress].BroadCast();

	for (auto& keyPress : m_PressedMouseButtons)
		m_MousePressedActions[keyPress].BroadCast(float(m_MouseX), float(m_MouseY));

	for (int i{}; i < MaxControllers; ++i)
		for (auto& controllerBtn : m_PressedControllerButtons[i])
			m_pControllerPressedActions[i][controllerBtn].BroadCast();
	
	UpdateControllersAxis();
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
				comp->m_ControllerId = -1;
				return -1;
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

bool InputManager::IsKeyPressed(SDL_Scancode key) const
{
	const Uint8* keyState = SDL_GetKeyboardState(nullptr);

	return (keyState[key]);
}

bool InputManager::IsMouseButtonPressed(Uint8 SDL_BUTTON) const
{
	Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);

	return mouseState & SDL_BUTTON;
}

bool InputManager::IsControllerButtonPressed(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId) const
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

	// add it to the list of pressed keys if it isn't in there yet
	auto& key = e.key.keysym.sym;
	if (std::find(m_PressedKeys.begin(), m_PressedKeys.end(), key) == m_PressedKeys.end())
		m_PressedKeys.emplace_back(key);
}

void InputManager::HandleKeyUp(const SDL_Event& e)
{
	auto it = m_KeyUpActions.find(e.key.keysym.sym);
	if (it != m_KeyUpActions.end())
	{
		it->second.BroadCast();
	}
	m_PressedKeys.SwapRemove(e.key.keysym.sym);
}

void InputManager::HandleMouseDown(const SDL_Event& e)
{
	auto it = m_MouseDownActions.find(e.button.button);
	if (it != m_MouseDownActions.end())
	{
		it->second.BroadCast(float(e.button.x), float(e.button.y));
	}
	m_PressedMouseButtons.emplace_back(e.button.button);
}

void InputManager::HandleMouseUp(const SDL_Event& e)
{
	auto it = m_MouseUpActions.find(e.button.button);
	if (it != m_MouseUpActions.end())
	{
		it->second.BroadCast(float(e.button.x), float(e.button.y));
	}
	m_PressedMouseButtons.SwapRemove(e.button.button);
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
	m_PressedControllerButtons[e.cbutton.which].SwapRemove(SDL_GameControllerButton(e.cbutton.button));
}

void InputManager::HandleControllerButtonDown(const SDL_Event& e)
{
	auto it = m_pControllerDownActions[e.cbutton.which].find(SDL_GameControllerButton(e.cbutton.button));
	if (it != m_pControllerDownActions[e.cbutton.which].end())
	{
		it->second.BroadCast();
	}
	m_PressedControllerButtons[e.cbutton.which].emplace_back(SDL_GameControllerButton(e.cbutton.button));
}

void InputManager::HandleControllerAxis(const SDL_Event& e)
{
	auto it = m_pControllerAxis[e.cbutton.which].find(SDL_GameControllerAxis(e.caxis.axis));
	if (it != m_pControllerAxis[e.cbutton.which].end())
	{
		it->second.BroadCast(float(e.caxis.value) / float(SDL_JOYSTICK_AXIS_MAX));
	}
}

void InputManager::UpdateControllersAxis()
{
	for (int i{}; i < MaxControllers; ++i)
	{
		if (auto pController = m_pControllers[i])
		{
			float invMax{ 1.f / float(SDL_JOYSTICK_AXIS_MAX) };
			auto& ctrAxis = m_pControllerAxis[i];
			ctrAxis[SDL_CONTROLLER_AXIS_LEFTX].BroadCast(float(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_LEFTX)) * invMax);
			ctrAxis[SDL_CONTROLLER_AXIS_LEFTY].BroadCast(float(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_LEFTY)) * invMax);
			ctrAxis[SDL_CONTROLLER_AXIS_RIGHTX].BroadCast(float(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_RIGHTX)) * invMax);
			ctrAxis[SDL_CONTROLLER_AXIS_RIGHTY].BroadCast(float(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_RIGHTY)) * invMax);
			ctrAxis[SDL_CONTROLLER_AXIS_TRIGGERLEFT].BroadCast(float(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) * invMax);
			ctrAxis[SDL_CONTROLLER_AXIS_TRIGGERRIGHT].BroadCast(float(SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) * invMax);
		}
	}
}

void InputManager::HandleWindowEvent(const SDL_Event& e)
{
	switch (e.window.event)
	{
	case SDL_WINDOWEVENT_FOCUS_LOST:
		ClearAllInputs();
		break;
	}
}

void InputManager::ClearAllInputs()
{
	m_PressedKeys.clear();
	m_PressedMouseButtons.clear();
	for (int i{}; i < MaxControllers; ++i)
		m_PressedControllerButtons[i].clear();
}
