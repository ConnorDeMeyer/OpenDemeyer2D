#include "TextPixelComponent.h"
#include "EngineFiles/GameObject.h"
#include "RenderComponent.h"
#include "Singletons/ResourceManager.h"
#include "Singletons/RenderManager.h"
#include "ResourceWrappers/Surface2D.h"

#include "imgui.h"

void TextPixelComponent::DefineUserFields(UserFieldBinder& binder) const
{
	binder.Add<std::shared_ptr<Surface2D>>("FontSurface", offsetof(TextPixelComponent, m_FontSurface));
	binder.Add<SDL_Color>("FontColor", offsetof(TextPixelComponent, m_Color));
	binder.Add<int>("CharSize", offsetof(TextPixelComponent, m_CharSize));
	binder.Add<std::string>("Text", offsetof(TextPixelComponent, m_Text));
}

void TextPixelComponent::Update(float)
{
	if (m_NeedsUpdate)
	{
		UpdateFontTexture();
	}
}

void TextPixelComponent::Initialize()
{
	UpdateFontTexture();
}

void TextPixelComponent::setFontTexture(const std::shared_ptr<Surface2D>& surface)
{
	m_FontSurface = surface;
	m_NeedsUpdate = true;
}

void TextPixelComponent::SetText(const std::string& text)
{
	m_Text = text;
	m_NeedsUpdate = true;
}

void TextPixelComponent::SetColor(const SDL_Color& color)
{
	m_Color = color;
	m_NeedsUpdate = true;
}

void TextPixelComponent::SetCharPixelSize(int size)
{
	m_CharSize = size;
	m_NeedsUpdate = true;
}

void TextPixelComponent::RenderImGui()
{
	// Change text
	char buffer[128];
	strcpy(buffer, m_Text.c_str());
	ImGui::InputText("Text", buffer, 64);
	
	if (m_Text != std::string(buffer))
	{
		SetText(std::string(buffer));
		UpdateFontTexture();
	}

	// Change Color
	float colors[4]{ m_Color.r / 255.f, m_Color.g / 255.f, m_Color.b / 255.f, m_Color.a / 255.f };
	ImGui::ColorEdit4("Text color", colors);

	if (colors[0] != m_Color.r / 255.f || colors[1] != m_Color.g / 255.f || colors[2] != m_Color.b / 255.f || colors[3] != m_Color.a / 255.f)
	{
		SetColor({ Uint8(colors[0] * 255.f),Uint8(colors[1] * 255.f) ,Uint8(colors[2] * 255.f) ,Uint8(colors[3] * 255.f) });
		UpdateFontTexture();
	}

	// Change Char Size
	int charSize = m_CharSize;
	ImGui::InputInt("Character Size", &charSize);

	if (charSize != m_CharSize)
	{
		m_CharSize = charSize;
		UpdateFontTexture();
	}
}

void TextPixelComponent::UpdateFontTexture()
{
	if (!GetRenderComponent())
		return;


	size_t textSize = m_Text.size();

	// create an empty surface
	auto pSurface = SDL_CreateRGBSurfaceWithFormat(0, int(textSize) * m_CharSize, m_CharSize, 32, SDL_PIXELFORMAT_RGBA32);
	if (!pSurface)
	{
		throw std::runtime_error(std::string("Failed to create surface: ") + SDL_GetError());
	}

	SDL_Rect fontSource{ 0,0,m_CharSize,m_CharSize };
	SDL_Rect destinationSource{ 0,0,m_CharSize,m_CharSize };

	int characterPerLine = m_FontSurface->GetSurface()->w / m_CharSize;

	for (size_t i{}; i < textSize; ++i)
	{
		auto character = m_Text[i];

		character -= 32; // remove the first 32 whitespaces

		fontSource.x = (character % characterPerLine) * m_CharSize;
		fontSource.y = (character / characterPerLine) * m_CharSize;

		destinationSource.x = int(i) * m_CharSize;

		SDL_BlitSurface(
			m_FontSurface->GetSurface(),
			&fontSource,
			pSurface,
			&destinationSource
		);
	}

	// Change the color of each pixel
	{
		float colorR = float(m_Color.r) / 255.f;
		float colorG = float(m_Color.g) / 255.f;
		float colorB = float(m_Color.b) / 255.f;
		float colorA = float(m_Color.a) / 255.f;

		const size_t amountOfPixel{ size_t(pSurface->w * pSurface->h) };
		Uint8* pixelsCol = static_cast<Uint8*>(pSurface->pixels);
		for (size_t i{}; i < amountOfPixel * 4; i += 4)
		{
			pixelsCol[i] = Uint8(float(pixelsCol[i]) * colorR);
			pixelsCol[i + 1] = Uint8(float(pixelsCol[i + 1]) * colorG);
			pixelsCol[i + 2] = Uint8(float(pixelsCol[i + 2]) * colorB);
			pixelsCol[i + 3] = Uint8(float(pixelsCol[i + 3]) * colorA);
		}
	}

	GetRenderComponent()->SetTexture(RESOURCES.LoadTexture(pSurface));
	SDL_FreeSurface(pSurface);

	m_NeedsUpdate = false;

}
