#include "TextPixelComponent.h"
#include "../GameObject.h"
#include "RenderComponent.h"
#include "../ResourceManager.h"
#include "../RenderManager.h"
#include "../Surface2D.h"

void TextPixelComponent::Update(float)
{
	if (m_NeedsUpdate)
	{
		size_t textSize = m_Text.size();

		// create an empty surface
		auto pSurface = SDL_CreateRGBSurfaceWithFormat(0, textSize * m_CharSize, m_CharSize, 32, SDL_PIXELFORMAT_RGBA32);
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
				pixelsCol[i]	= Uint8(float(pixelsCol[i]) * colorR);
				pixelsCol[i+1]	= Uint8(float(pixelsCol[i+1]) * colorG);
				pixelsCol[i+2]	= Uint8(float(pixelsCol[i+2]) * colorB);
				pixelsCol[i+3]	= Uint8(float(pixelsCol[i+3]) * colorA);
			}
		}

		m_TextTexture = RESOURCES.LoadTexture(pSurface);
		SDL_FreeSurface(pSurface);

		m_NeedsUpdate = false;

		if (m_pRenderComp)
		{
			m_pRenderComp->SetTexture(m_TextTexture);
		}
	}
}

void TextPixelComponent::BeginPlay()
{
	m_pRenderComp = GetParent()->GetComponent<RenderComponent>();

	if (m_pRenderComp && m_TextTexture) m_pRenderComp->SetTexture(m_TextTexture);
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
