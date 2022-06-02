#include "CustomSerializers.h"
#include "EngineFiles/Scene.h"
#include "EngineFiles/ComponentBase.h"
#include "EngineFiles/ComponentBase.h"
#include "TypeInformation.h"
#include "EngineFiles/GameObject.h"

#include <stack>
#include <fstream>
#include <ios>
#include <map>
#include <functional>
#include <sstream>
#include "CustomSerializers.h"

#include "Singletons/ResourceManager.h"
#include "box2d.h"

std::ostream& operator<<(std::ostream& stream, const glm::vec2& vec)
{
	return stream << vec.x << ' ' << vec.y;
}

std::ostream& operator<<(std::ostream& stream, const glm::vec3& vec)
{
	return stream << vec.x << ' ' << vec.y << ' ' << vec.z;
}

std::ostream& operator<<(std::ostream& stream, const glm::vec4& vec)
{
	return stream << vec.x << ' ' << vec.y << ' ' << vec.z << ' ' << vec.w;
}

std::ostream& operator<<(std::ostream& stream, const glm::mat2x2& mat)
{
	return stream	<< mat[0][0] << ' ' << mat[0][1] << '\n'
					<< mat[1][0] << ' ' << mat[1][1];
}

std::ostream& operator<<(std::ostream& stream, const glm::mat2x3& mat)
{
	return stream	<< mat[0][0] << ' ' << mat[0][1] << ' ' << mat[0][2] << '\n'
					<< mat[1][0] << ' ' << mat[1][1] << ' ' << mat[0][2];
}

std::ostream& operator<<(std::ostream& stream, const glm::mat2x4& mat)
{
	return stream	<< mat[0][0] << ' ' << mat[0][1] << ' ' << mat[0][2] << ' ' << mat[0][3] << '\n'
					<< mat[1][0] << ' ' << mat[1][1] << ' ' << mat[0][2] << ' ' << mat[0][3];
}

std::ostream& operator<<(std::ostream& stream, const glm::mat3x2& mat)
{
	return stream	<< mat[0][0] << ' ' << mat[0][1] << '\n'
					<< mat[1][0] << ' ' << mat[1][1] << ' '
					<< mat[2][0] << ' ' << mat[2][1];
}

std::ostream& operator<<(std::ostream& stream, const glm::mat3x3& mat)
{
	return stream	<< mat[0][0] << ' ' << mat[0][1] << ' ' << mat[0][2] << '\n'
					<< mat[1][0] << ' ' << mat[1][1] << ' ' << mat[1][2] << ' '
					<< mat[2][0] << ' ' << mat[2][1] << ' ' << mat[2][2];
}

std::ostream& operator<<(std::ostream& stream, const glm::mat3x4& mat)
{
	return stream	<< mat[0][0] << ' ' << mat[0][1] << ' ' << mat[0][2] << ' ' << mat[0][3] << '\n'
					<< mat[1][0] << ' ' << mat[1][1] << ' ' << mat[1][2] << ' ' << mat[1][3] << ' '
					<< mat[2][0] << ' ' << mat[2][1] << ' ' << mat[2][2] << ' ' << mat[2][3];
}

std::ostream& operator<<(std::ostream& stream, const glm::mat4x2& mat)
{
	return stream	<< mat[0][0] << ' ' << mat[0][1] << '\n'
					<< mat[1][0] << ' ' << mat[1][1] << ' '
					<< mat[2][0] << ' ' << mat[2][1] << ' '
					<< mat[3][0] << ' ' << mat[3][1];
}

std::ostream& operator<<(std::ostream& stream, const glm::mat4x3& mat)
{
	return stream	<< mat[0][0] << ' ' << mat[0][1] << ' ' << mat[0][2] << '\n'
					<< mat[1][0] << ' ' << mat[1][1] << ' ' << mat[1][2] << ' '
					<< mat[2][0] << ' ' << mat[2][1] << ' ' << mat[2][2] << ' '
					<< mat[3][0] << ' ' << mat[3][1] << ' ' << mat[3][2];
}

std::ostream& operator<<(std::ostream& stream, const glm::mat4x4& mat)
{
	return stream	<< mat[0][0] << ' ' << mat[0][1] << ' ' << mat[0][2] << ' ' << mat[0][3] << '\n'
					<< mat[1][0] << ' ' << mat[1][1] << ' ' << mat[1][2] << ' ' << mat[1][3] << ' '
					<< mat[2][0] << ' ' << mat[2][1] << ' ' << mat[2][2] << ' ' << mat[2][3] << ' '
					<< mat[3][0] << ' ' << mat[3][1] << ' ' << mat[3][2] << ' ' << mat[3][3];
}

std::istream& operator>>(std::istream& stream, glm::vec2& vec)
{
	return stream	>> vec.x >> vec.y;
}

std::istream& operator>>(std::istream& stream, glm::vec3& vec)
{
	return stream	>> vec.x >> vec.y >> vec.z;
}

std::istream& operator>>(std::istream& stream, glm::vec4& vec)
{
	return stream	>> vec.x >> vec.y >> vec.z >> vec.w;
}

std::istream& operator>>(std::istream& stream, glm::mat2x2& mat)
{
	return stream	>> mat[0][0] >> mat[0][1] 
					>> mat[1][0] >> mat[1][1];
}

std::istream& operator>>(std::istream& stream, glm::mat2x3& mat)
{
	return stream	>> mat[0][0] >> mat[0][1] >> mat[0][2] 
					>> mat[1][0] >> mat[1][1] >> mat[0][2];
}

std::istream& operator>>(std::istream& stream, glm::mat2x4& mat)
{
	return stream	>> mat[0][0] >> mat[0][1] >> mat[0][2] >> mat[0][3] 
					>> mat[1][0] >> mat[1][1] >> mat[0][2] >> mat[0][3];
}

std::istream& operator>>(std::istream& stream, glm::mat3x2& mat)
{
	return stream	>> mat[0][0] >> mat[0][1] 
					>> mat[1][0] >> mat[1][1] 
					>> mat[2][0] >> mat[2][1];
}

std::istream& operator>>(std::istream& stream, glm::mat3x3& mat)
{
	return stream	>> mat[0][0] >> mat[0][1] >> mat[0][2] 
					>> mat[1][0] >> mat[1][1] >> mat[1][2] 
					>> mat[2][0] >> mat[2][1] >> mat[2][2];
}

std::istream& operator>>(std::istream& stream, glm::mat3x4& mat)
{
	return stream	>> mat[0][0] >> mat[0][1] >> mat[0][2] >> mat[0][3] 
					>> mat[1][0] >> mat[1][1] >> mat[1][2] >> mat[1][3] 
					>> mat[2][0] >> mat[2][1] >> mat[2][2] >> mat[2][3];
}

std::istream& operator>>(std::istream& stream, glm::mat4x2& mat)
{
	return stream	>> mat[0][0] >> mat[0][1] 
					>> mat[1][0] >> mat[1][1] 
					>> mat[2][0] >> mat[2][1] 
					>> mat[3][0] >> mat[3][1];
}

std::istream& operator>>(std::istream& stream, glm::mat4x3& mat)
{
	return stream	>> mat[0][0] >> mat[0][1] >> mat[0][2] 
					>> mat[1][0] >> mat[1][1] >> mat[1][2] 
					>> mat[2][0] >> mat[2][1] >> mat[2][2] 
					>> mat[3][0] >> mat[3][1] >> mat[3][2];
}

std::istream& operator>>(std::istream& stream, glm::mat4x4& mat)
{
	return stream	>> mat[0][0] >> mat[0][1] >> mat[0][2] >> mat[0][3] 
					>> mat[1][0] >> mat[1][1] >> mat[1][2] >> mat[1][3] 
					>> mat[2][0] >> mat[2][1] >> mat[2][2] >> mat[2][3] 
					>> mat[3][0] >> mat[3][1] >> mat[3][2] >> mat[3][3];
}

std::ostream& operator<<(std::ostream& stream, const SDL_FRect& rect)
{
	return stream << rect.x << ' ' << rect.y << ' ' << rect.w << ' ' << rect.h;
}

std::ostream& operator<<(std::ostream& stream, const SDL_Rect& rect)
{
	return stream << rect.x << ' ' << rect.y << ' ' << rect.w << ' ' << rect.h;
}

std::ostream& operator<<(std::ostream& stream, const SDL_Color& color)
{
	int r{ color.r }, g{ color.g }, b{ color.b }, a{ color.a };
	return stream << r << ' ' << g << ' ' << b << ' ' << a;
}

std::istream& operator>>(std::istream& stream, SDL_FRect& rect)
{
	return stream >> rect.x >> rect.y >> rect.w >> rect.h;
}

std::istream& operator>>(std::istream& stream, SDL_Rect& rect)
{
	return stream >> rect.x >> rect.y >> rect.w >> rect.h;
}

std::istream& operator>>(std::istream& stream, SDL_Color& color)
{
	int r, g, b, a;
	stream >> r >> g >> b >> a;
	color = { uint8(r),uint8(g),uint8(b),uint8(a) };
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<Sound>& sound)
{
	return stream << sound->GetFilePath();
}

std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<Music>& music)
{
	return stream << music->GetFilePath();
}

std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<Texture2D>& texture)
{
	return stream << texture->GetFilePath();
}

std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<Surface2D>& surface)
{
	return stream << surface->GetFilePath();
}

std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<Prefab>& prefab)
{
	return stream << prefab->GetPath();
}

std::istream& operator>>(std::istream& stream, std::shared_ptr<Sound>& sound)
{
	std::filesystem::path file;
	stream >> file;
	sound = RESOURCES.LoadSound(file);
	return stream;
}

std::istream& operator>>(std::istream& stream, std::shared_ptr<Music>& music)
{
	std::filesystem::path file;
	stream >> file;
	music = RESOURCES.LoadMusic(file);
	return stream;
}

std::istream& operator>>(std::istream& stream, std::shared_ptr<Texture2D>& texture)
{
	std::filesystem::path file;
	stream >> file;
	texture = RESOURCES.LoadTexture(file);
	return stream;
}

std::istream& operator>>(std::istream& stream, std::shared_ptr<Surface2D>& surface)
{
	std::filesystem::path file;
	stream >> file;
	surface = RESOURCES.LoadSurface(file);
	return stream;
}

std::istream& operator>>(std::istream& stream, std::shared_ptr<Prefab>& prefab)
{
	std::filesystem::path file;
	stream >> file;
	prefab = RESOURCES.LoadPrefab(file);
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const b2Vec2& vec2)
{
	return stream << vec2.x << ' ' << vec2.y;
}

std::ostream& operator<<(std::ostream& stream, const b2BodyDef& body)
{
	return stream
		<< body.allowSleep << ' '
		<< body.bullet << ' '
		<< body.fixedRotation << ' '
		<< body.angularDamping << ' '
		<< body.gravityScale << ' '
		<< body.linearDamping << ' '
		<< int(body.type);
}

std::ostream& operator<<(std::ostream& stream, const b2FixtureDef& fixture)
{
	return stream
		<< fixture.isSensor << ' '
		<< fixture.friction << ' '
		<< fixture.restitution << ' '
		<< fixture.restitutionThreshold << ' '
		<< fixture.density << ' '
		<< fixture.filter.categoryBits << ' '
		<< fixture.filter.maskBits << ' '
		<< fixture.filter.groupIndex;
}

std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<b2Shape>& shape)
{
	stream << int(shape->m_type);

	switch (shape->GetType())
	{
	case b2Shape::Type::e_circle:
	{
		auto circle = std::reinterpret_pointer_cast<const b2CircleShape>(shape);
		return stream << ' ' << circle->m_p << ' ' << circle->m_radius;
	}
		break;
	case b2Shape::Type::e_polygon:
	{
		auto polygon = std::reinterpret_pointer_cast<const b2PolygonShape>(shape);
		stream << ' ' << polygon->m_count << ' ';
		for (int i{}; i < polygon->m_count; ++i)
		{
			stream << polygon->m_vertices[i];
			if (i + 1 != polygon->m_count)
				stream << ' ';
		}
		return stream;
	}
		break;
	case b2Shape::Type::e_edge:
	{
		//TODO
	}
		break;
	}

	return stream;
}

std::istream& operator>>(std::istream& stream, b2Vec2& vec2)
{
	return stream >> vec2.x >> vec2.y;
}

std::istream& operator>>(std::istream& stream, b2BodyDef& body)
{
	int bodyType{};
	stream
		>> body.allowSleep
		>> body.bullet 
		>> body.fixedRotation
		>> body.angularDamping
		>> body.gravityScale
		>> body.linearDamping
		>> bodyType;
	body.type = b2BodyType(bodyType);
	return stream;
}

std::istream& operator>>(std::istream& stream, b2FixtureDef& fixture)
{
	return stream
		>> fixture.isSensor
		>> fixture.friction
		>> fixture.restitution
		>> fixture.restitutionThreshold 
		>> fixture.density
		>> fixture.filter.categoryBits
		>> fixture.filter.maskBits 
		>> fixture.filter.groupIndex;
}

std::istream& operator>>(std::istream& stream, std::shared_ptr<b2Shape>& shape)
{
	int shapeType{};
	stream >> shapeType;

	switch (b2Shape::Type(shapeType))
	{
	case b2Shape::Type::e_circle:
	{
		auto circle = new b2CircleShape();
		shape = std::shared_ptr<b2Shape>(circle);
		return stream >> circle->m_p >> circle->m_radius;
	}
	break;
	case b2Shape::Type::e_polygon:
	{
		auto polygon = new b2PolygonShape();
		shape = std::shared_ptr<b2Shape>(polygon);
		stream >> polygon->m_count;
		for (int i{}; i < polygon->m_count; ++i)
		{
			stream >> polygon->m_vertices[i];
		}
		return stream;
	}
	break;
	case b2Shape::Type::e_edge:
	{
		//TODO
	}
	break;
	}

	shape->m_type = b2Shape::Type(shapeType);

	return stream;
}
