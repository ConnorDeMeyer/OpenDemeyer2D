#pragma once
#include "EngineFiles/GameInstance.h"

class BurgerTimeGI final : public GameInstance
{
public:

	virtual ~BurgerTimeGI() = default;

	void LoadGame() override;

};
