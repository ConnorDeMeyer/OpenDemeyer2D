#pragma once
#include "ComponentBase.h"

class PeterPepper;

class PlayerUI final : public ComponentBase
{
public:

	void LinkPlayer(PeterPepper* pPeterPepper);

	const std::string GetComponentName() override { return "Player UI"; }

private:

	void UpdateScore();

	PeterPepper* m_pPeterPepper{};

	class TextComponent* m_pScoreText{};

};
