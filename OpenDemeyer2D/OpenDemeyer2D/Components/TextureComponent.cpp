#include "TextureComponent.h"

#include "imgui.h"
#include "ImGuiExt/imgui_helpers.h"

#include "RenderComponent.h"
#include "EngineFiles/GameObject.h"
#include "Singletons/ResourceManager.h"

#include "Singletons/RenderManager.h"
#include "EngineIO/Reflection.h"

void TextureComponent::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<std::shared_ptr<Texture2D>>("Texture2D", offsetof(TextureComponent, m_Texture));
}

void TextureComponent::Initialize()
{
	UpdateRenderComponent();
}

void TextureComponent::SetTexture(const std::shared_ptr<Texture2D>& texture)
{
	m_Texture = texture;
	UpdateRenderComponent();
}

void TextureComponent::SetTexture(const std::string& filePath)
{
	m_Texture = RESOURCES.LoadTexture(filePath);
	UpdateRenderComponent();
}

void TextureComponent::RenderImGui()
{
	
	ImGui::ResourceSelect("Texture2D", m_Texture);

	if (m_Texture)
	{
		float width = ImGui::GetWindowSize().x - 20.f;
		float aspectRatio = float(m_Texture->GetWidth()) / float(m_Texture->GetHeight());

#pragma warning(disable : 4312)
		ImGui::Image((ImTextureID)(m_Texture->GetId()), { width,width / aspectRatio });
#pragma warning(default : 4312)
	}

}

void TextureComponent::UpdateRenderComponent()
{
	if (auto render = GetRenderComponent()) {
		if (m_Texture)
			render->SetTexture(m_Texture);
	}
}
