#pragma once
#include "EngineFiles/ComponentBase.h"

class PeterPepper;

class PlayerUI final : public ComponentBase
{
	COMPONENT_BODY(PlayerUI)

public:

	void LinkPlayer(PeterPepper* pPeterPepper);

private:

	void UpdateScore();

	PeterPepper* m_pPeterPepper{};

	class TextComponent* m_pScoreText{};

};
