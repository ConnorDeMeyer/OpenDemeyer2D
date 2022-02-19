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
		int w, h;
		SDL_QueryTexture(m_Texture->GetSDLTexture(), NULL, NULL, &w, &h);

		float width = float(w) * scale.x;
		float height = float(h) * scale.y;

		if (m_RenderAlignMode == RenderAlignMode::center)
			RENDER.RenderTexture(m_Texture, pos.x - width * 0.5f, pos.y - height * 0.5f, width, height);

		RENDER.RenderTexture(m_Texture, 
			pos.x - width * float(int(m_RenderAlignMode) & 0b1),
			pos.y - height * float((int(m_RenderAlignMode) >> 1) & 0b1),
			w * scale.x, 
			h * scale.y);
	}
}

void RenderComponent::BeginPlay()
{
	m_pTransform = GetParent()->GetTransform();
}
