#pragma once
#include "EngineFiles/Component.h"
#include <string>
#include <SDL.h>

class Font;
class Texture2D;
class RenderComponent;
class TextureComponent;

class TextComponent final : public Component<TextComponent>
{
public:

	TextComponent() = default;
	virtual ~TextComponent() = default;

public:

	void Update(float) override;

	void Initialize() override;

	void SetFont(const std::shared_ptr<Font>& font);

	void SetText(const std::string& text);
	void SetColor(const SDL_Color& color);
	void SetSize(unsigned int size);

	const std::shared_ptr<Font>& getFont() const { return m_Font; }

	//const std::string GetComponentName() override { return "TextComponent"; }

	void RenderImGui() override;

private:

	bool m_NeedsUpdate{};

	std::string m_Text;
	std::shared_ptr<Font> m_Font;
	SDL_Color m_Color{ 255,255,255,255 };
};



