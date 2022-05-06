#pragma once
#include "EngineFiles/Component.h"
#include "UtilityFiles/Delegate.h"

class PlayerResources final : public Component<PlayerResources>
{
public:

	void SetResources(size_t amount);

	size_t GetAmountOfResources() const { return m_Resources.size(); }

	void LoseResource();

	void AddResource();

	void SetResourceTexture(const std::shared_ptr<Texture2D>& texture);

	void SetSourceRect(const SDL_FRect& rect);

	Delegate<> OnLoseResource;

	Delegate<> OnLostAllResources;

	//const std::string GetComponentName() override { return "PlayerResource"; }

private:

	void UpdateResources();

	std::shared_ptr<Texture2D> m_pTexture;

	SDL_FRect m_SourceRect{};

	std::vector<GameObject*> m_Resources;

};

