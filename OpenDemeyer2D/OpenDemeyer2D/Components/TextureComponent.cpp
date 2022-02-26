#include "../OD2pch.h"
#include "TextureComponent.h"

#include "imgui.h"
#include "RenderComponent.h"
#include "../GameObject.h"
#include "../ResourceManager.h"

#include "../RenderManager.h"

void TextureComponent::BeginPlay()
{
	m_pRenderComponent = GetParent()->GetComponent<RenderComponent>();

	if (m_pRenderComponent && m_Texture) m_pRenderComponent->SetTexture(m_Texture);
}

void TextureComponent::SetTexture(const std::shared_ptr<Texture2D>& texture)
{
	m_Texture = texture;
	if (m_pRenderComponent) m_pRenderComponent->SetTexture(m_Texture);
}

void TextureComponent::SetTexture(const std::string& filePath)
{
	m_Texture = RESOURCES.LoadTexture(filePath);
	if (m_pRenderComponent && m_Texture) m_pRenderComponent->SetTexture(m_Texture);
}

void TextureComponent::RenderImGui()
{
	ImGui::Image(reinterpret_cast<ImTextureID>(m_Texture->GetId()), { 100,100 });
}

