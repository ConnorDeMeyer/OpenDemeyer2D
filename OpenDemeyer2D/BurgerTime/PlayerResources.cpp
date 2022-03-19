#include "PlayerResources.h"
#include "Components/TextureComponent.h"

void PlayerResources::SetResources(size_t amount)
{
	if (m_Resources.size() >= amount)
	{
		for (size_t i{m_Resources.size()}; i > amount; --i)
		{
			LoseResource();
		}
	}
	else
	{
		size_t amountToAdd(amount - m_Resources.size());
		for (size_t i{}; i < amountToAdd; ++i)
		{
			AddResource();
		}
	}
}

void PlayerResources::LoseResource()
{
	if (m_Resources.empty()) return;

	m_Resources.back()->Destroy();
	m_Resources.pop_back();
	OnLoseResource.BroadCast();

	if (m_Resources.empty())
		OnLostAllResources.BroadCast();
}

void PlayerResources::AddResource()
{
	auto go = new GameObject();

	go->AddComponent<RenderComponent>();
	auto texture = go->AddComponent<TextureComponent>();

	if (m_pTexture) {
		texture->SetTexture(m_pTexture);
		go->GetTransform()->SetPosition({ 0, (m_Resources.size() + 1) * m_pTexture->GetHeight() });
	}

	go->SetParent(GetParent());
	m_Resources.push_back(go);
	UpdateResources();
}

void PlayerResources::SetResourceTexture(const std::shared_ptr<Texture2D>& texture)
{
	m_pTexture = texture;
	UpdateResources();
}

void PlayerResources::SetSourceRect(const SDL_FRect& rect)
{
	m_SourceRect = rect;
	UpdateResources();
}

void PlayerResources::UpdateResources()
{
	float TexturePosY{};
	for (auto go : m_Resources)
	{
		auto textureComp = go->GetComponent<TextureComponent>();
		textureComp->SetTexture(m_pTexture);
		textureComp->SetSourceRect(m_SourceRect);
		go->GetTransform()->SetPosition({ 0,TexturePosY });
		TexturePosY += m_SourceRect.h;
	}
}
