#pragma once
#include "../ComponentBase.h"
#include <string>
#include <SDL.h>

class Font;
class Texture2D;
class RenderComponent;
class TextureComponent;

class TextComponent final : public ComponentBase
{
public:

	TextComponent() = default;
	virtual ~TextComponent() = default;

public:

	void Update(float) override;

	void BeginPlay() override;

	void SetFont(const std::shared_ptr<Font>& font);

	void SetText(const std::string& text);
	void SetColor(const SDL_Color& color);
	void SetSize(unsigned int size);

	const std::string GetComponentName() override { return "TextComponent"; }

	void InitializeComponent(const Dictionary& dictionary) override;

	Dictionary& GetClassDefault() override;

private:

	bool m_NeedsUpdate;

	std::string m_Text;
	std::shared_ptr<Font> m_Font;
	std::shared_ptr<Texture2D> m_TextTexture;
	SDL_Color m_Color{ 255,255,255,255 };

	RenderComponent* m_pRenderComp{};

};



