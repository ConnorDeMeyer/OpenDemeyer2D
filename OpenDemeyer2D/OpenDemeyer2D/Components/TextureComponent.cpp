#include "../OD2pch.h"
#include "TextureComponent.h"

#include "imgui.h"
#include "RenderComponent.h"
#include "../GameObject.h"
#include "../ResourceManager.h"

#include "../RenderManager.h"

//static Dictionary TextureDefaults
//{
//	Dictionary::EntryStruct<std::string>{"file", ""}
//};

void TextureComponent::BeginPlay()
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

//void TextureComponent::InitializeComponent(const Dictionary& dictionary)
//{
//	std::string path;
//	dictionary.GetData("file", path);
//
//	if (!path.empty())
//		SetTexture(path);
//}

//Dictionary& TextureComponent::GetClassDefault()
//{
//	return TextureDefaults;
//}

