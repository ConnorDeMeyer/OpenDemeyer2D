#include "../OD2.pch"
#include "RenderComponent.h"

#include "Transform.h"
#include "../GameObject.h"
#include "../RenderManager.h"

void RenderComponent::Render() const
{
	if (m_Texture && m_pTransform)
	{
		const auto& pos = m_pTransform->GetPosition();
		const auto& scale = m_pTransform->GetScale();
		const auto& angle = m_pTransform->GetRotation();
		int w, h;
		SDL_QueryTexture(m_Texture->GetSDLTexture(), NULL, NULL, &w, &h);

		float width = float(w) * scale.x;
		float height = float(h) * scale.y;

		SDL_FRect DestRect{};

		if (m_RenderAlignMode == RenderAlignMode::center)
			DestRect = {
			pos.x - width * 0.5f,
			pos.y - height * 0.5f,
			width, height };
		else
			DestRect = {
			pos.x - width * float(int(m_RenderAlignMode) & 0b1),
			pos.y - height * float((int(m_RenderAlignMode) >> 1) & 0b1),
			width, height };

		SDL_RenderCopyExF(
			RENDER.GetSDLRenderer(),
			m_Texture->GetSDLTexture(),
			&m_SourceRect,
			&DestRect,
			static_cast<double>(angle),
			nullptr,
			SDL_FLIP_NONE);
	}
}

void RenderComponent::BeginPlay()
{
	m_pTransform = GetParent()->GetTransform();
}

void RenderComponent::SetTexture(std::shared_ptr<Texture2D> texture)
{
	m_Texture = texture;

	int w, h;
	SDL_QueryTexture(m_Texture->GetSDLTexture(), NULL, NULL, &w, &h);

	m_SourceRect = { 0,0,w,h };
}

void RenderComponent::SetSourceRect(const SDL_Rect& srcRect)
{
	m_SourceRect = srcRect;
}
