#pragma once
#include "ComponentBase.h"

class PeterPepper;

class PlayerUI final : public ComponentBase
{
public:

	void LinkPlayer(PeterPepper* pPeterPepper);

private:

	void UpdateScore();

	PeterPepper* m_pPeterPepper{};

	class TextComponent* m_pScoreText{};

};
