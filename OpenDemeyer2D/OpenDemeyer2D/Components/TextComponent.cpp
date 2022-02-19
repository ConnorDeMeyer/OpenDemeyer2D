#include "TextComponent.h"

#include "../GameObject.h"
#include "RenderComponent.h"
#include "../Font.h"
#include "../ResourceManager.h"
#include "../RenderManager.h"

void TextComponent::Update(float)
{
	if (m_NeedsUpdate)
	{
		const auto surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), m_Color);
		if (surf == nullptr)
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}
		auto texture = SDL_CreateTextureFromSurface(RENDER.GetSDLRenderer(), surf);
		if (texture == nullptr)
		{
			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}
		SDL_FreeSurface(surf);
		m_TextTexture = std::make_shared<Texture2D>(texture);
		m_NeedsUpdate = false;

		if (m_pRenderComp)
		{
			m_pRenderComp->SetTexture(m_TextTexture);
		}
	}
}

void TextComponent::BeginPlay()
{
	m_pRenderComp = GetParent()->GetComponent<RenderComponent>();

	if (m_pRenderComp && m_TextTexture) m_pRenderComp->SetTexture(m_TextTexture);
}

void TextComponent::SetFont(const std::shared_ptr<Font>& font)
{
	m_Font = font;
	m_NeedsUpdate = true;
}

void TextComponent::SetText(const std::string& text)
{
	m_Text = text;
	m_NeedsUpdate = true;
}

void TextComponent::SetColor(const SDL_Color& color)
{
	m_Color = color;
	m_NeedsUpdate = true;
}

void TextComponent::SetSize(unsigned size)
{
	if (m_Font->GetSize() != size)
	{
		m_Font = RESOURCES.LoadFont(m_Font->GetPath(), size);
		m_NeedsUpdate = true;
	}
}
