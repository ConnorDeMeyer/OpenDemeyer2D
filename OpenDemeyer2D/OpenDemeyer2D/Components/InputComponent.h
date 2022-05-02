#pragma once

#include "Component.h"
#include "../InputManager.h"

class InputComponent : public Component<InputComponent>
{
	friend class InputManager;

public:
	virtual ~InputComponent();

	void SetActive(bool bActive) { m_bIsActive = bActive; }

	void BindKeyDown(SDL_Keycode SDLK_key, const std::function<void()>& function) const;
	void BindKeyUp(SDL_Keycode SDLK_key, const std::function<void()>& function) const;
	void BindKeyPressed(SDL_Keycode SDLK_key, const std::function<void()>& function) const;

	void BindMouseDown(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function) const;
	void BindMouseUp(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function) const;
	void BindMousePressed(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function) const;
	void BindMouseMove(const std::function<void(float, float)>& function) const;
	void BindMouseWheel(const std::function<void(float)>& function) const;

	void BindControllerUp(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId, const std::function<void()>& function) const;
	void BindControllerDown(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId, const std::function<void()>& function) const;
	void BindControllerPressed(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId, const std::function<void()>& function) const;
	void BindControllerAxis(SDL_GameControllerAxis SDL_CONTROLLER_AXIS, Uint8 controllerId, const std::function<void(float)>& function) const;

	int BindToController();
	int GetControllerId() const { return m_ControllerId; }

	//const std::string GetComponentName() override { return "InputComponent"; }

	void RenderImGui() override;

private:

	bool m_bIsActive{ true };
	char m_ControllerId{};
};
