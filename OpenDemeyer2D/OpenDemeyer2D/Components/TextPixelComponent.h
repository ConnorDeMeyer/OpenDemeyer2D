#pragma once
#include "EngineFiles/Component.h"
#include <string>
#include <SDL.h>

class Surface2D;
class Font;
class Texture2D;
class RenderComponent;
class TextureComponent;

class TextPixelComponent final : public Component<TextPixelComponent>
{
public:
	TextPixelComponent() = default;
	virtual ~TextPixelComponent() = default;

public:

	virtual void DefineUserFields(UserFieldBinder& binder) const;

	void Update(float) override;

	void Initialize() override;

	void setFontTexture(const std::shared_ptr<Surface2D>& surface);

	void SetText(const std::string& text);

	void SetColor(const SDL_Color& color);

	void SetCharPixelSize(int size);

	const std::shared_ptr<Surface2D> GetFontTexture() const { return m_FontSurface; }

	const std::string& GetText() const { return m_Text; }

	void RenderImGui() override;

private:

	void UpdateFontTexture();

private:

	bool m_NeedsUpdate{};

	std::string m_Text;
	std::shared_ptr<Surface2D> m_FontSurface;
	SDL_Color m_Color{ 255,255,255,255 };
	int m_CharSize{ 8 };
};
