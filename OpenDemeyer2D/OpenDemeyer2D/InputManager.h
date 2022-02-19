#pragma once

#include "Singleton.h"
#define INPUT InputManager::GetInstance()

class InputManager final : public Singleton<InputManager>
{

	friend class Singleton<InputManager>;

private:

	InputManager() = default;
	virtual ~InputManager() = default;

public:

	void ProcessInput();

};
