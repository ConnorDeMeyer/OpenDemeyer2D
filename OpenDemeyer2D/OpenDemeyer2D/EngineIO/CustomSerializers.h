#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include <SDL_rect.h>

#include "ResourceWrappers/Texture2D.h"
#include "ResourceWrappers/Surface2D.h"
#include "ResourceWrappers/Sound.h"
#include "ResourceWrappers/Prefab.h"

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
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<Prefab>& prefab);

std::istream& operator>>(std::istream& stream, std::shared_ptr<Sound>& sound);
std::istream& operator>>(std::istream& stream, std::shared_ptr<Music>& music);
std::istream& operator>>(std::istream& stream, std::shared_ptr<Texture2D>& texture2d);
std::istream& operator>>(std::istream& stream, std::shared_ptr<Surface2D>& surface2d);
std::istream& operator>>(std::istream& stream, std::shared_ptr<Prefab>& prefab);

//Box2D
struct b2BodyDef;
struct b2FixtureDef;
class b2Shape;
struct b2Vec2;

std::ostream& operator<<(std::ostream& stream, const b2Vec2& vec2);
std::ostream& operator<<(std::ostream& stream, const b2BodyDef& body);
std::ostream& operator<<(std::ostream& stream, const b2FixtureDef& fixture);
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<b2Shape>& shape);

std::istream& operator>>(std::istream& stream, b2Vec2& vec2);
std::istream& operator>>(std::istream& stream, b2BodyDef& body);
std::istream& operator>>(std::istream& stream, b2FixtureDef& fixture);
std::istream& operator>>(std::istream& stream, std::shared_ptr<b2Shape>& shape);

// std::vector
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& arr)
{
	os << "{\n";
	for (auto& ele : arr)
	{
		os << ele << ' ';
	}
	os << "\n}";
	return os;
}

template <typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& arr)
{
	if (CanContinue(is))
	{
		arr.clear();
		while (!IsEnd(is))
		{
			T element{};
			is >> element;
			arr.emplace_back(element);
		}
	}
	return is;
}

// std::pair
template <typename T0, typename T1>
std::ostream& operator<<(std::ostream& os, const std::pair<T0, T1>& pair)
{
	return os << pair.first << ' ' << pair.second;
}

template <typename T0, typename T1>
std::istream& operator>>(std::istream& is, std::pair<T0, T1>& pair)
{
	return is >> pair.first >> pair.second;
}

// std::unique_ptr
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::unique_ptr<T>& ptr)
{
	return os << ptr.get();
}

template <typename T>
std::istream& operator>>(std::istream& is, std::unique_ptr<T>& ptr)
{
	ptr = std::unique_ptr<T>(new T());
	return is >> ptr.get();
}

// std::shared_ptr
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<T>& ptr)
{
	return os << ptr.get();
}

template <typename T>
std::istream& operator>>(std::istream& is, std::shared_ptr<T>& ptr)
{
	if constexpr (std::is_abstract_v<T>)
	{
		return is >> ptr.get();
	}
	else
	{
		ptr = std::unique_ptr<T>(new T());
		return is >> ptr.get();
	}
}

// ENUMS
#define ENUM_ENABLE_STREAMING(enum_class)\
std::ostream& operator<<(std::ostream& os, enum_class _enum)\
{\
	return os << int(_enum);\
}\
std::istream& operator>>(std::istream& is, enum_class& _enum)\
{\
	int enumInt{};\
	is >> enumInt;\
	_enum = enum_class(enumInt);\
	return is;\
}