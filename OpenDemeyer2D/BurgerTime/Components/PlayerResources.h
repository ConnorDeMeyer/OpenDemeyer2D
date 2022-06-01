#pragma once
#include "EngineFiles/ComponentBase.h"
#include "UtilityFiles/Delegate.h"

class PeterPepper;
class TextPixelComponent;

/** 
* Component Resposible for displaying the lives and chaning the pepper count
* The Game Object should not have any children attached to it as it will delete them at the start
*/
class PlayerResources final : public ComponentBase
{
	COMPONENT_BODY(PlayerResources)

public:

	void DefineUserFields(UserFieldBinder& binder) const override;
	
	void RenderImGui() override;
	
	void BeginPlay() override;

	size_t GetAmountOfResources() const { return m_Resources.size(); }


	void SetResourceTexture(const std::shared_ptr<Texture2D>& texture);
	void SetSourceRect(const SDL_FRect& rect);

	void GenerateLives();

private:

	void LosePepper();
	void LoseLife();
	void UpdateResources();

private:

	std::shared_ptr<Texture2D> m_pTexture;

	SDL_FRect m_SourceRect{};

	std::vector<GameObject*> m_Resources;

	std::weak_ptr<PeterPepper> m_Player;
	
	std::weak_ptr<TextPixelComponent> m_pPepperText{};

};

