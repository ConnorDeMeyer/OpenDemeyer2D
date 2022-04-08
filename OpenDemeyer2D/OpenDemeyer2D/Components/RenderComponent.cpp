#include "../OD2pch.h"
#include "RenderComponent.h"

#include "Transform.h"
#include "../GameObject.h"
#include "../RenderManager.h"


void RenderComponent::Render() const
{
	if (m_Texture && m_pTransform)
	{
		RENDER.RenderTexture(m_Texture, m_pTransform->GetWorldPosition(), m_pTransform->GetWorldScale(), m_pTransform->GetWorldRotation(),
			m_Pivot, &m_SourceRect, m_RenderLayer);
	}
}

void RenderComponent::BeginPlay()
{
	m_pTransform = GetParent()->GetTransform();
}

void RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
	m_Texture = texture;

	m_SourceRect = SDL_FRect{ 0.f,0.f,float(texture->GetWidth()),float(texture->GetHeight()) };
}

void RenderComponent::SetRenderAlignMode(eRenderAlignMode mode)
{
	switch (mode)
	{
	case eRenderAlignMode::centered:
		m_Pivot = { 0.5f,0.5f };
		break;
	case eRenderAlignMode::bottomLeft:
		m_Pivot = { 1,1 };
		break;
	case eRenderAlignMode::bottomRight:
		m_Pivot = { 0,1 };
		break;
	case eRenderAlignMode::topLeft:
		m_Pivot = { 1,0 };
		break;
	case eRenderAlignMode::topRight:
		m_Pivot = { 0,0 };
		break;
	case eRenderAlignMode::top:
		m_Pivot.y = 0;
		break;
	case eRenderAlignMode::bottom:
		m_Pivot.y = 1;
		break;
	case eRenderAlignMode::left:
		m_Pivot.x = 1;
		break;
	case eRenderAlignMode::right:
		m_Pivot.x = 0;
		break;
	}
}

void RenderComponent::SetSourceRect(const SDL_FRect& srcRect)
{
	m_SourceRect = srcRect;
}

void RenderComponent::RenderImGui()
{
	// Show texture
	float ratio = float(m_Texture->GetWidth()) / float(m_Texture->GetHeight());
#pragma warning(disable : 4312)
	ImGui::Image((ImTextureID)(m_Texture->GetId()), { 16 * ratio,16 });
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

	// Pivot
	float dims[2]{ m_Pivot.x, m_Pivot.y };
	ImGui::InputFloat2("Pivot", dims);

	m_Pivot.x = dims[0];
	m_Pivot.y = dims[1];

	// Render Layer
	ImGui::SliderInt("Render Layer", &m_RenderLayer, 0, int(RENDER.GetRenderLayersAmount()) - 1);

	// Render Align modes
	const char* items[] = { "Top Left", "Top Right", "Bottom Left", "Bottom Right", "Top", "Bottom", "Right", "Left", "Centered" };
	if (ImGui::BeginCombo("Render Align Modes", "Render Align Modes")) {

		for (int i{}; i < 9; ++i)
		{
			if (ImGui::Selectable(items[i]))
			{
				SetRenderAlignMode(eRenderAlignMode(i));
			}
		}
		ImGui::EndCombo();
	}
}
