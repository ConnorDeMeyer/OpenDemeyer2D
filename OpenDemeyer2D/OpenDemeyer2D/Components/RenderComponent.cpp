#include "../OD2pch.h"
#include "RenderComponent.h"

#include "Transform.h"
#include "../GameObject.h"
#include "../RenderManager.h"

void RenderComponent::Render() const
{
	if (m_Texture && m_pTransform)
	{
		RENDER.RenderTexture(m_Texture, m_pTransform->GetPosition(), m_pTransform->GetScale(), m_pTransform->GetRotation());
	}
}

void RenderComponent::BeginPlay()
{
	m_pTransform = GetParent()->GetTransform();
}

void RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
	m_Texture = texture;

	m_SourceRect = { 0,0,texture->GetWidth(),texture->GetHeight() };
}

void RenderComponent::SetSourceRect(const SDL_Rect& srcRect)
{
	m_SourceRect = srcRect;
}
