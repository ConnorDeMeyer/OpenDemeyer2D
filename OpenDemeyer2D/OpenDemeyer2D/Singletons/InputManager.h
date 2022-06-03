#pragma once
#include <SDL.h>
#include "UtilityFiles/Singleton.h"
#include <unordered_map>
#include "UtilityFiles/Delegate.h"
#include <SDL_gamecontroller.h>
#include "Components/InputComponent.h"
#include "UtilityFiles/ODArray.h"

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

	bool IsKeyPressed(SDL_Scancode key)		const;
	bool IsMouseButtonPressed(Uint8 SDL_BUTTON)	const;
	bool IsControllerButtonPressed(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId) const;

public:

	Delegate<>& GetKeyDownActions(SDL_Keycode SDLK_key) { return m_KeyDownActions[SDLK_key]; }
	Delegate<>& GetKeyUpActions(SDL_Keycode SDLK_key) { return m_KeyUpActions[SDLK_key]; }
	Delegate<>& GetKeyPressedActions(SDL_Keycode SDLK_key) { return m_KeyPressedActions[SDLK_key]; }

	Delegate<float, float>& GetMouseDownActions(Uint8 SDL_BUTTON) { return m_MouseDownActions[SDL_BUTTON]; }
	Delegate<float, float>& GetMouseUpActions(Uint8 SDL_BUTTON) { return m_MouseUpActions[SDL_BUTTON]; }
	Delegate<float, float>& GetMousePressedActions(Uint8 SDL_BUTTON) { return m_MousePressedActions[SDL_BUTTON]; }
	Delegate<float>& GetMouseScrollActions() { return m_MouseScrollAxis; }
	Delegate<float, float>& GetMouseMoveActions() { return m_MouseMove2DAxis; }

	Delegate<>& GetControllerUpActions(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId)
	{return m_pControllerUpActions[controllerId][SDL_CONTROLLER_BUTTON];}
	Delegate<>& GetControllerDownActions(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId)
	{return m_pControllerDownActions[controllerId][SDL_CONTROLLER_BUTTON];}
	Delegate<>& GetControllerPressedActions(SDL_GameControllerButton SDL_CONTROLLER_BUTTON, Uint8 controllerId)
	{return m_pControllerPressedActions[controllerId][SDL_CONTROLLER_BUTTON];}
	Delegate<float>& GetControllerAxisActions(SDL_GameControllerAxis SDL_CONTROLLER_AXIS, Uint8 controllerId)
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

	void UpdateControllersAxis();

private:

	void HandleWindowEvent			(const SDL_Event& e);

	void ClearAllInputs();

private:

	/**
	 * INPUT MAPPINGS
	 */

	std::unordered_map<SDL_Keycode, Delegate<>> m_KeyDownActions;
	std::unordered_map<SDL_Keycode, Delegate<>> m_KeyUpActions;
	std::unordered_map<SDL_Keycode, Delegate<>> m_KeyPressedActions;

	std::unordered_map<Uint8, Delegate<float,float>> m_MouseDownActions;
	std::unordered_map<Uint8, Delegate<float,float>> m_MouseUpActions;
	std::unordered_map<Uint8, Delegate<float, float>> m_MousePressedActions;
	Delegate<float> m_MouseScrollAxis;
	Delegate<float, float> m_MouseMove2DAxis;

	std::unordered_map<SDL_GameControllerButton, Delegate<>> m_pControllerUpActions[MaxControllers]{};
	std::unordered_map<SDL_GameControllerButton, Delegate<>> m_pControllerDownActions[MaxControllers]{};
	std::unordered_map<SDL_GameControllerButton, Delegate<>> m_pControllerPressedActions[MaxControllers]{};
	std::unordered_map<SDL_GameControllerAxis, Delegate<float>> m_pControllerAxis[MaxControllers]{};

	InputComponent* m_pControllerInputComps[MaxControllers]{};
	SDL_GameController* m_pControllers[MaxControllers]{};
	SDL_Joystick *m_pJoysticks[MaxControllers]{};

	int m_MouseX{};
	int m_MouseY{};

	ODArray<SDL_Keycode> m_PressedKeys;
	ODArray<Uint8> m_PressedMouseButtons;
	ODArray<SDL_GameControllerButton> m_PressedControllerButtons[MaxControllers];

};


