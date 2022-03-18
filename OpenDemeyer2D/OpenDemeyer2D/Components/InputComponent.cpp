#include "InputComponent.h"
#include "../InputManager.h"



InputComponent::~InputComponent()
{
	INPUT.UnregisterInputComponent(this);
}

void InputComponent::BindKeyDown(SDL_Keycode SDLK_key, const std::function<void()>& function) const
{
	INPUT.GetKeyDownActions(SDLK_key).BindFunction(GetParent(), [this, function] { if (m_bIsActive) function(); });
}

void InputComponent::BindKeyUp(SDL_Keycode SDLK_key, const std::function<void()>& function) const
{
	INPUT.GetKeyUpActions(SDLK_key).BindFunction(GetParent(), [this, function] { if (m_bIsActive) function(); });
}

void InputComponent::BindMouseDown(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function) const
{
	INPUT.GetMouseDownActions(SDL_BUTTON).BindFunction(GetParent(), [this, function](float x, float y) { if (m_bIsActive) function(x,y); });
}

void InputComponent::BindMouseUp(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function) const
{
	INPUT.GetMouseUpActions(SDL_BUTTON).BindFunction(GetParent(), [this, function](float x, float y) { if (m_bIsActive) function(x, y); });
}

void InputComponent::BindMouseMove(const std::function<void(float, float)>& function) const
{
	INPUT.GetMouseMoveActions().BindFunction(GetParent(), [this, function](float x, float y) { if (m_bIsActive) function(x, y); });
}

void InputComponent::BindMouseWheel(const std::function<void(float)>& function) const
{
	INPUT.GetMouseScrollActions().BindFunction(GetParent(), [this, function](float value) { if (m_bIsActive) function(value); });
}

void InputComponent::BindControllerUp(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId,
	const std::function<void()>& function) const
{
	if (m_ControllerId != -1)
	INPUT.GetControllerUpActions(SDL_CONTROLLER_BUTTON, controllerId).BindFunction(GetParent(), [this, function] { if (m_bIsActive) function(); });
}

void InputComponent::BindControllerDown(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId,
	const std::function<void()>& function) const
{
	if (m_ControllerId != -1)
	INPUT.GetControllerDownActions(SDL_CONTROLLER_BUTTON, controllerId).BindFunction(GetParent(), [this, function] { if (m_bIsActive) function(); });
}

void InputComponent::BindControllerAxis(SDL_GameControllerAxis SDL_CONTROLLER_AXIS, Uint8 controllerId,
	const std::function<void(float)>& function) const
{
	if (m_ControllerId != -1)
	INPUT.GetControllerAxisActions(SDL_CONTROLLER_AXIS, controllerId).BindFunction(GetParent(), [this, function](float value) { if (m_bIsActive) function(value); });
}

int InputComponent::BindToController()
{
	return INPUT.RegisterInputComponent(this);
}