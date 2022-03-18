#include "../OD2pch.h"
#include "RenderComponent.h"

#include "Transform.h"
#include "../GameObject.h"
#include "../RenderManager.h"

//static Dictionary RenderDefaults
//{
//	Dictionary::EntryStruct<Uint8>{"align", Uint8(eRenderAlignMode::centered)},
//};

void RenderComponent::Render() const
{
	if (m_Texture && m_pTransform)
	{
		RENDER.RenderTexture(m_Texture, m_pTransform->GetWorldPosition(), m_pTransform->GetWorldScale(), m_pTransform->GetWorldRotation(),
			eRenderAlignMode::centered, &m_SourceRect);
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

void RenderComponent::SetSourceRect(const SDL_FRect& srcRect)
{
	m_SourceRect = srcRect;
}

//void RenderComponent::InitializeComponent(const Dictionary& dictionary)
//{
//	Uint8 alignMode;
//	dictionary.GetData("align", alignMode);
//
//	m_RenderAlignMode = eRenderAlignMode(alignMode);
//}

//Dictionary& RenderComponent::GetClassDefault()
//{
//	return RenderDefaults;
//}
