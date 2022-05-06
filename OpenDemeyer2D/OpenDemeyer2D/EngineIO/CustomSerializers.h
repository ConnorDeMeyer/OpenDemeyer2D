#pragma once
#include <string>
#include <iostream>
#include <memory>

#include <glm/glm.hpp>

#include <SDL_rect.h>

#include "ResourceWrappers/Texture2D.h"
#include "ResourceWrappers/Surface2D.h"
#include "ResourceWrappers/Sound.h"

#include "EngineFiles/ComponentBase.h"
#include "Deserializer.h"

// GLM
std::ostream& operator<<(std::ostream& stream, const glm::vec2& vec);
std::ostream& operator<<(std::ostream& stream, const glm::vec3& vec);
std::ostream& operator<<(std::ostream& stream, const glm::vec4& vec);
std::ostream& operator<<(std::ostream& stream, const glm::mat2x2& mat);
std::ostream& operator<<(std::ostream& stream, const glm::mat2x3& mat);
std::ostream& operator<<(std::ostream& stream, const glm::mat2x4& mat);
std::ostream& operator<<(std::ostream& stream, const glm::mat3x2& mat);
std::ostream& operator<<(std::ostream& stream, const glm::mat3x3& mat);
std::ostream& operator<<(std::ostream& stream, const glm::mat3x4& mat);
std::ostream& operator<<(std::ostream& stream, const glm::mat4x2& mat);
std::ostream& operator<<(std::ostream& stream, const glm::mat4x3& mat);
std::ostream& operator<<(std::ostream& stream, const glm::mat4x4& mat);

std::istream& operator>>(std::istream& stream, glm::vec2& vec);
std::istream& operator>>(std::istream& stream, glm::vec3& vec);
std::istream& operator>>(std::istream& stream, glm::vec4& vec);
std::istream& operator>>(std::istream& stream, glm::mat2x2& mat);
std::istream& operator>>(std::istream& stream, glm::mat2x3& mat);
std::istream& operator>>(std::istream& stream, glm::mat2x4& mat);
std::istream& operator>>(std::istream& stream, glm::mat3x2& mat);
std::istream& operator>>(std::istream& stream, glm::mat3x3& mat);
std::istream& operator>>(std::istream& stream, glm::mat3x4& mat);
std::istream& operator>>(std::istream& stream, glm::mat4x2& mat);
std::istream& operator>>(std::istream& stream, glm::mat4x3& mat);
std::istream& operator>>(std::istream& stream, glm::mat4x4& mat);

// SDL
std::ostream& operator<<(std::ostream& stream, const SDL_FRect& rect);
std::ostream& operator<<(std::ostream& stream, const SDL_Rect& rect);
std::ostream& operator<<(std::ostream& stream, const SDL_Color& color);

std::istream& operator>>(std::istream& stream, SDL_FRect& rect);
std::istream& operator>>(std::istream& stream, SDL_Rect& rect);
std::istream& operator>>(std::istream& stream, SDL_Color& color);

// Engine files
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<Sound>& sound);
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<Music>& music);
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<Texture2D>& texture2d);
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<Surface2D>& surface2d);

std::istream& operator>>(std::istream& stream, std::shared_ptr<Sound>& sound);
std::istream& operator>>(std::istream& stream, std::shared_ptr<Music>& music);
std::istream& operator>>(std::istream& stream, std::shared_ptr<Texture2D>& texture2d);
std::istream& operator>>(std::istream& stream, std::shared_ptr<Surface2D>& surface2d);




