#pragma once
#include <SDL.h>

/** Simple RAII container for the sdl texture*/
class Texture2D final
{
public:
	SDL_Texture* GetSDLTexture() const { return m_Texture; }

	explicit Texture2D(SDL_Texture* texture) : m_Texture{ texture }{}
	~Texture2D(){ SDL_DestroyTexture(m_Texture); }
	
	Texture2D(const Texture2D&) = delete;
	Texture2D(Texture2D&&) = delete;
	Texture2D& operator= (const Texture2D&) = delete;
	Texture2D& operator= (const Texture2D&&) = delete;
private:
	SDL_Texture* m_Texture{};
};