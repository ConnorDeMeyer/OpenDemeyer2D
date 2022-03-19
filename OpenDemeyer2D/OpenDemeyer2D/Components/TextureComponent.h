#pragma once
#include "../ComponentBase.h"
#include "../Texture2D.h"
#include <string>

class RenderComponent;

class TextureComponent final : public ComponentBase
{
public:

	TextureComponent() = default;
	virtual ~TextureComponent() = default;

public:

	void BeginPlay() override;

	void SetTexture(const std::shared_ptr<Texture2D>& texture);

	void SetTexture(const std::string& filePath);

	void RenderImGui() override;

	const std::string GetComponentName() override { return "TextureComponent"; }

	void SetSourceRect(const SDL_FRect& sourceRect);

	const SDL_FRect& GetSourceRect() const { return m_SourceRect; }

	//void InitializeComponent(const Dictionary& dictionary) override;

	//Dictionary& GetClassDefault() override;

private:

	void UpdateRenderComponent();

	std::shared_ptr<Texture2D> m_Texture;

	RenderComponent* m_pRenderComponent;

	SDL_FRect m_SourceRect{};
};


