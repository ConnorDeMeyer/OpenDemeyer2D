#include "TextComponent.h"

#include "EngineFiles/GameObject.h"
#include "RenderComponent.h"
#include "ResourceWrappers/Font.h"
#include "Singletons/ResourceManager.h"
#include "Singletons/RenderManager.h"
#include "imgui.h"


void TextComponent::Update(float)
{
	if (m_NeedsUpdate)
	{
		//const auto surf = TTF_RenderText_Blended(m_Font->GetFont(), m_Text.c_str(), m_Color);
		//if (surf == nullptr)
		//{
		//	throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		//}
		//
		//m_TextTexture = RESOURCES.LoadTexture(surf);
		//
		//SDL_FreeSurface(surf);
		//
		//m_NeedsUpdate = false;
		//
		//if (m_pRenderComp)
		//{
		//	m_pRenderComp->SetTexture(m_TextTexture);
		//}
	}
}

void TextComponent::Initialize()
{
	//m_pRenderComp = GetParent()->GetComponent<RenderComponent>();
	//
	//if (m_pRenderComp && m_TextTexture) m_pRenderComp->SetTexture(m_TextTexture);
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
		//TODO
		//m_Font = RESOURCES.LoadFont(m_Font->GetPath(), size);
		m_NeedsUpdate = true;
	}
}

void TextComponent::RenderImGui()
{
	// Change text
	char buffer[128];
	strcpy(buffer, m_Text.c_str());
	ImGui::InputText("Text", buffer, 64);

	if (m_Text != std::string(buffer))
	{
		SetText(std::string(buffer));
	}

	// Change Color
	float colors[4]{ m_Color.r / 255.f, m_Color.g / 255.f, m_Color.b / 255.f, m_Color.a / 255.f };
	ImGui::ColorEdit4("Text color", colors);

	if (colors[0] != m_Color.r / 255.f || colors[1] != m_Color.g / 255.f || colors[2] != m_Color.b / 255.f || colors[3] != m_Color.a / 255.f)
	{
		SetColor({ Uint8(colors[0] * 255.f),Uint8(colors[1] * 255.f) ,Uint8(colors[2] * 255.f) ,Uint8(colors[3] * 255.f) });
	}

	// Change Char Size
	int fontSize = m_Font->GetSize();
	ImGui::InputInt("Character Size", &fontSize);

	if (fontSize != int(m_Font->GetSize()))
	{
		SetSize(fontSize);
	}
}
