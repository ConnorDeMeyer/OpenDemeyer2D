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
