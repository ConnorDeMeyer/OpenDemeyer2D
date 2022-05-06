#include "TextureComponent.h"

#include "imgui.h"
#include "RenderComponent.h"
#include "EngineFiles/GameObject.h"
#include "Singletons/ResourceManager.h"

#include "Singletons/RenderManager.h"

void TextureComponent::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<SDL_FRect>("SourceRect", offsetof(TextureComponent, m_SourceRect));
	binder.Add<std::shared_ptr<Texture2D>>("Texture2D", offsetof(TextureComponent, m_Texture));
}

void TextureComponent::Initialize()
{
	m_pRenderComponent = GetParent()->GetComponent<RenderComponent>();

	UpdateRenderComponent();
}

void TextureComponent::SetTexture(const std::shared_ptr<Texture2D>& texture)
{
	m_Texture = texture;
	if (m_SourceRect.w == 0 || m_SourceRect.h == 0) m_SourceRect = { 0,0,float(m_Texture->GetWidth()), float(m_Texture->GetHeight()) };
	UpdateRenderComponent();
}

void TextureComponent::SetTexture(const std::string& filePath)
{
	m_Texture = RESOURCES.LoadTexture(filePath);
	UpdateRenderComponent();
}

void TextureComponent::RenderImGui()
{
#pragma warning(disable : 4312)
	ImGui::Image((ImTextureID)(m_Texture->GetId()), { 100,100 });
#pragma warning(default : 4312)

	//Change source rect
	ImGui::Text("Source Rectangle");
	float x{ m_SourceRect.x }, y{ m_SourceRect.y }, w{ m_SourceRect.w }, h{ m_SourceRect.h };
	ImGui::InputFloat("X Coordinate", &x);
	ImGui::InputFloat("Y Coordinate", &y);
	ImGui::InputFloat("Width", &w);
	ImGui::InputFloat("Height", &h);

	if (m_SourceRect.x != x || m_SourceRect.y != y || m_SourceRect.w != w || m_SourceRect.h != h)
	{
		SetSourceRect({ x,y,w,h });
	}
}

void TextureComponent::SetSourceRect(const SDL_FRect& sourceRect)
{
	m_SourceRect = sourceRect;
	if (m_pRenderComponent) m_pRenderComponent->SetSourceRect(sourceRect);
}

void TextureComponent::UpdateRenderComponent()
{
	if (m_pRenderComponent) {
		if (m_Texture)
			m_pRenderComponent->SetTexture(m_Texture);
		m_pRenderComponent->SetSourceRect(m_SourceRect);
	}
}
