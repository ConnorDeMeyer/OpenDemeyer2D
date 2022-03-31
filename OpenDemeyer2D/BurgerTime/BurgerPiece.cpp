#include "BurgerPiece.h"

#include "GameObject.h"
#include "ResourceManager.h"
#include "Components/TextureComponent.h"

void BurgerPiece::BeginPlay()
{
	m_pRenderComp = GetParent()->GetComponent<RenderComponent>();
	m_pTextureComp = GetParent()->GetComponent<TextureComponent>();

	UpdateTexture();
}

void BurgerPiece::SetType(BurgerPieceType type)
{
	m_Type = type;

	UpdateTexture();
}

void BurgerPiece::UpdateTexture()
{
	if (m_pTextureComp)
	{
		if (!m_pTextureComp->GetTexture())
		{
			m_pTextureComp->SetTexture(RESOURCES.LoadTexture("Bitmaps/FullSheet.png"));
		}

		m_pTextureComp->SetSourceRect(SDL_FRect{ 112.f,48.f + 8.f * float(m_Type),32.f,8.f });
	}

	if (m_pRenderComp)
		m_pRenderComp->SetRenderAlignMode(eRenderAlignMode::centered);
}