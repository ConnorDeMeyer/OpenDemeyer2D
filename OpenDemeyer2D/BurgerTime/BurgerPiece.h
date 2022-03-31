#pragma once
#include <ComponentBase.h>

class TextureComponent;
class RenderComponent;

enum class BurgerPieceType : uint8_t
{
	topBun		= 0,
	bottomBun	= 1,
	cheese		= 2,
	meat		= 3,
	tomato		= 4,
	lettuce		= 5
};

class BurgerPiece final : public ComponentBase
{
public:

	void BeginPlay() override;

	void SetType(BurgerPieceType type);

	BurgerPieceType GetBurgerPieceType() const { return m_Type; }

private:

	void UpdateTexture();

private:

	TextureComponent* m_pTextureComp{};
	RenderComponent* m_pRenderComp{};

	BurgerPieceType m_Type{ };

};


