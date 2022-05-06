#pragma once
#include "Component.h"
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

	const std::shared_ptr<Texture2D>& getTexture() const { return m_TextTexture; }

	const std::shared_ptr<Surface2D> GetFontTexture() const { return m_FontSurface; }

	const std::string& GetText() const { return m_Text; }

	void RenderImGui() override;

private:

	void UpdateFontTexture();

private:

	bool m_NeedsUpdate{};

	std::string m_Text;
	std::shared_ptr<Surface2D> m_FontSurface;
	std::shared_ptr<Texture2D> m_TextTexture;
	SDL_Color m_Color{ 255,255,255,255 };
	int m_CharSize{ 8 };

	RenderComponent* m_pRenderComp{};
	
};
