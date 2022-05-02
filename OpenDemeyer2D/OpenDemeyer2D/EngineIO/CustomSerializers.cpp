#include "CustomSerializers.h"
#include "Scene.h"
#include "Component.h"
#include "ComponentBase.h"
#include "TypeInformation.h"
#include "GameObject.h"

#include <stack>
#include <fstream>
#include <ios>
#include <map>
#include <functional>
#include <sstream>
#include "CustomSerializers.h"

#include "ResourceManager.h"


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
	return stream << color.r << ' ' << color.g << ' ' << color.g << ' ' << color.a;
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
	return stream >> color.r >> color.g >> color.g >> color.a;
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

std::istream& operator>>(std::istream& stream, std::shared_ptr<Sound>& sound)
{
	std::string file;
	stream >> file;
	sound = RESOURCES.LoadSound(file);
	return stream;
}

std::istream& operator>>(std::istream& stream, std::shared_ptr<Music>& music)
{
	std::string file;
	stream >> file;
	music = RESOURCES.LoadMusic(file);
	return stream;
}

std::istream& operator>>(std::istream& stream, std::shared_ptr<Texture2D>& texture)
{
	std::string file;
	stream >> file;
	texture = RESOURCES.LoadTexture(file);
	return stream;
}

std::istream& operator>>(std::istream& stream, std::shared_ptr<Surface2D>& surface)
{
	std::string file;
	stream >> file;
	surface = RESOURCES.LoadSurface(file);
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const ComponentBase* component)
{
	return stream << component->GetParent()->GetId();
}

std::istream& operator>>(FileDeserializer& stream, ComponentBase* component)
{
	unsigned int id{};
	stream >> id;

	stream.GetComponentFromObject(component->GetComponentId(), id, &component);
	return stream;
}



