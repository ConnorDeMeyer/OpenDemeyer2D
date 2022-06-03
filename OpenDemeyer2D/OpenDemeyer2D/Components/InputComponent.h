#pragma once

#include "EngineFiles/ComponentBase.h"
#include "Singletons/InputManager.h"

class InputComponent final : public ComponentBase
{
	COMPONENT_BODY(InputComponent)

	friend class InputManager;

public:
	~InputComponent() override;

	void SetActive(bool bActive) { m_bIsActive = bActive; }

	void BindKeyDown(SDL_Keycode SDLK_key, const std::function<void()>& function);
	void BindKeyUp(SDL_Keycode SDLK_key, const std::function<void()>& function);
	void BindKeyPressed(SDL_Keycode SDLK_key, const std::function<void()>& function);

	void BindMouseDown(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function);
	void BindMouseUp(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function);
	void BindMousePressed(Uint8 SDL_BUTTON, const std::function<void(float, float)>& function);
	void BindMouseMove(const std::function<void(float, float)>& function);
	void BindMouseWheel(const std::function<void(float)>& function);

	void BindControllerUp(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, const std::function<void()>& function);
	void BindControllerDown(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, const std::function<void()>& function);
	void BindControllerPressed(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, const std::function<void()>& function);
	void BindControllerAxis(SDL_GameControllerAxis SDL_CONTROLLER_AXIS, const std::function<void(float)>& function);

	/** Will bind to controller if not already bound to one*/
	int GetControllerId();

	/** Binds the input to the next available controller*/
	int BindToController();

	void RenderImGui() override;

private:


	bool m_bIsActive{ true };
	char m_ControllerId{-1};
};
