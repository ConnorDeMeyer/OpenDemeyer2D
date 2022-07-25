#include "pch.h"
#include "InputComponent.h"
#include "Singletons/InputManager.h"
#include <functional>
#include <SDL.h>

#include "imgui.h"


InputComponent::~InputComponent()
{
	INPUT.UnregisterInputComponent(this);
}

void InputComponent::BindKeyDown(SDL_Keycode SDLK_key, const std::function<void()>& function)
{
	INPUT.GetKeyDownActions(SDLK_key).BindFunction(this, [this, function] { if (m_bIsActive) function(); });
}

void InputComponent::BindKeyUp(SDL_Keycode SDLK_key, const std::function<void()>& function)
{
	INPUT.GetKeyUpActions(SDLK_key).BindFunction(this, [this, function] { if (m_bIsActive) function(); });
}

void InputComponent::BindKeyPressed(SDL_Keycode SDLK_key, const std::function<void()>& function)
{
	INPUT.GetKeyPressedActions(SDLK_key).BindFunction(this, [this, function] { if (m_bIsActive) function(); });
}

void InputComponent::BindMouseDown(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function)
{
	INPUT.GetMouseDownActions(SDL_BUTTON).BindFunction(this, [this, function](float x, float y) { if (m_bIsActive) function(x,y); });
}

void InputComponent::BindMouseUp(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function)
{
	INPUT.GetMouseUpActions(SDL_BUTTON).BindFunction(this, [this, function](float x, float y) { if (m_bIsActive) function(x, y); });
}

void InputComponent::BindMousePressed(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function)
{
	INPUT.GetMousePressedActions(SDL_BUTTON).BindFunction(this, [this, function](float x, float y) { if (m_bIsActive) function(x, y); });
}

void InputComponent::BindMouseMove(const std::function<void(float, float)>& function)
{
	INPUT.GetMouseMoveActions().BindFunction(this, [this, function](float x, float y) { if (m_bIsActive) function(x, y); });
}

void InputComponent::BindMouseWheel(const std::function<void(float)>& function)
{
	INPUT.GetMouseScrollActions().BindFunction(this, [this, function](float value) { if (m_bIsActive) function(value); });
}

void InputComponent::BindControllerUp(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, const std::function<void()>& function)
{
	if (m_ControllerId != -1 || BindToController() != -1)
		INPUT.GetControllerUpActions(SDL_CONTROLLER_BUTTON, m_ControllerId).BindFunction(this, [this, function] { if (m_bIsActive) function(); });
}

void InputComponent::BindControllerDown(SDL_GameControllerButton SDL_CONTROLLER_BUTTON,	const std::function<void()>& function)
{
	if (m_ControllerId != -1 || BindToController() != -1)
		INPUT.GetControllerDownActions(SDL_CONTROLLER_BUTTON, m_ControllerId).BindFunction(this, [this, function] { if (m_bIsActive) function(); });
}

void InputComponent::BindControllerPressed(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, const std::function<void()>& function)
{
	if (m_ControllerId != -1 || BindToController() != -1)
		INPUT.GetControllerPressedActions(SDL_CONTROLLER_BUTTON, m_ControllerId).BindFunction(this, [this, function] { if (m_bIsActive) function(); });
}

void InputComponent::BindControllerAxis(SDL_GameControllerAxis SDL_CONTROLLER_AXIS, const std::function<void(float)>& function)
{
	if (m_ControllerId != -1 || BindToController() != -1)
		INPUT.GetControllerAxisActions(SDL_CONTROLLER_AXIS, m_ControllerId).BindFunction(this, [this, function](float value) { if (m_bIsActive) function(value); });
}

int InputComponent::BindToController()
{
	return INPUT.RegisterInputComponent(this);
}

void InputComponent::RenderImGui()
{
	ImGui::Checkbox("Is Active", &m_bIsActive);
	ImGui::Text("Controller Id: %.d", m_ControllerId);
}

int InputComponent::GetControllerId()
{
	if (m_ControllerId == -1)
		BindToController();
	return m_ControllerId;
}