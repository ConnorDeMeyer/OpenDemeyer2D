#pragma once

#include "Singleton.h"
#include <unordered_map>
#include "Delegate.h"
#include <SDL_gamecontroller.h>

#define INPUT InputManager::GetInstance()

constexpr uint8_t MaxControllers{ 4 };

class InputComponent;

class InputManager final : public Singleton<InputManager>
{

	friend class Singleton<InputManager>;

private:

	InputManager() = default;
	virtual ~InputManager() = default;

public:

	void ProcessInput();

	int RegisterInputComponent(InputComponent* comp);
	void UnregisterInputComponent(InputComponent* comp);

public:

	bool IsKeyPressed(SDL_Keycode SDLK_key)		const;
	bool IsMouseButtonPressed(Uint8 SDL_BUTTON)	const;
	bool IsControllerButtonDown(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId) const;

public:

	MultiDelegate<>& GetKeyDownActions(SDL_Keycode SDLK_key) { return m_KeyDownActions[SDLK_key]; }
	MultiDelegate<>& GetKeyUpActions(SDL_Keycode SDLK_key) { return m_KeyUpActions[SDLK_key]; }
	MultiDelegate<>& GetKeyPressedActions(SDL_Keycode SDLK_key) { return m_KeyPressedActions[SDLK_key]; }

	MultiDelegate<float, float>& GetMouseDownActions(Uint8 SDL_BUTTON) { return m_MouseDownActions[SDL_BUTTON]; }
	MultiDelegate<float, float>& GetMouseUpActions(Uint8 SDL_BUTTON) { return m_MouseUpActions[SDL_BUTTON]; }
	MultiDelegate<float, float>& GetMousePressedActions(Uint8 SDL_BUTTON) { return m_MousePressedActions[SDL_BUTTON]; }
	MultiDelegate<float>& GetMouseScrollActions() { return m_MouseScrollAxis; }
	MultiDelegate<float, float>& GetMouseMoveActions() { return m_MouseMove2DAxis; }

	MultiDelegate<>& GetControllerUpActions(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId)
	{return m_pControllerUpActions[controllerId][SDL_CONTROLLER_BUTTON];}
	MultiDelegate<>& GetControllerDownActions(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId)
	{return m_pControllerDownActions[controllerId][SDL_CONTROLLER_BUTTON];}
	MultiDelegate<>& GetControllerPressedActions(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId)
	{return m_pControllerPressedActions[controllerId][SDL_CONTROLLER_BUTTON];}
	MultiDelegate<float>& GetControllerAxisActions(SDL_GameControllerAxis SDL_CONTROLLER_AXIS, Uint8 controllerId)
	{return m_pControllerAxis[controllerId][SDL_CONTROLLER_AXIS];}

private:

	void HandleKeyDown				(const SDL_Event& e);
	void HandleKeyUp				(const SDL_Event& e);

	void HandleMouseDown			(const SDL_Event& e);
	void HandleMouseUp				(const SDL_Event& e);
	void HandleMouseMotion			(const SDL_Event& e);
	void HandleMouseWheel			(const SDL_Event& e);

	void HandleControllerButtonUp	(const SDL_Event& e);
	void HandleControllerButtonDown	(const SDL_Event& e);
	void HandleControllerAxis		(const SDL_Event& e);

private:

	/**
	 * INPUT MAPPINGS
	 */

	std::unordered_map<SDL_Keycode, MultiDelegate<>> m_KeyDownActions;
	std::unordered_map<SDL_Keycode, MultiDelegate<>> m_KeyUpActions;
	std::unordered_map<SDL_Keycode, MultiDelegate<>> m_KeyPressedActions;

	std::unordered_map<Uint8, MultiDelegate<float,float>> m_MouseDownActions;
	std::unordered_map<Uint8, MultiDelegate<float,float>> m_MouseUpActions;
	std::unordered_map<Uint8, MultiDelegate<float, float>> m_MousePressedActions;
	MultiDelegate<float> m_MouseScrollAxis;
	MultiDelegate<float, float> m_MouseMove2DAxis;

	std::unordered_map<SDL_GameControllerButton, MultiDelegate<>> m_pControllerUpActions[MaxControllers]{};
	std::unordered_map<SDL_GameControllerButton, MultiDelegate<>> m_pControllerDownActions[MaxControllers]{};
	std::unordered_map<SDL_GameControllerButton, MultiDelegate<>> m_pControllerPressedActions[MaxControllers]{};
	std::unordered_map<SDL_GameControllerAxis, MultiDelegate<float>> m_pControllerAxis[MaxControllers]{};

	InputComponent* m_pControllerInputComps[MaxControllers]{};
	SDL_GameController* m_pControllers[MaxControllers]{};
	SDL_Joystick *m_pJoysticks[MaxControllers]{};

	int m_MouseX{};
	int m_MouseY{};

};


