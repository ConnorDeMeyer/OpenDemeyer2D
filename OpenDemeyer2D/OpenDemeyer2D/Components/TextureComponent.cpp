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
#pragma warning(disable : 4312)
	ImGui::Image((ImTextureID)(m_Texture->GetId()), { 100,100 });
#pragma warning(default : 4312)
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

