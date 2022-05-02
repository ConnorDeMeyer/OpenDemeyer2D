#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeindex>

class Scene;
class ComponentBase;
class GameObject;

class DeserializerBase
{
public:

	//static void DeserializeGame(const std::string& file);
	//static void DeserializeGame(std::istream& stream);
	//static Scene* DeserializeScene(const std::string& file);
	//static Scene* DeserializeScene(std::istream& stream);
	//static GameObject* DeserializeObject(const std::string& file);
	//static GameObject* DeserializeObject(std::istream& stream);

	/** 
	* Register the object so others may be able to use it at a reference
	* the streamId is the id the object has inside of the stream, not inside the game
	*/
	void RegisterGameObject(unsigned int streamId, GameObject* object);

	void GetComponentFromObject(std::type_index typeId, unsigned int objectId, ComponentBase** ComponentAddress);

	virtual void DeserializeGame() = 0;
	virtual Scene* DeserializeScene() = 0;
	virtual GameObject* DeserializeObject() = 0;

private:

	/**
	* Struct used to keep information for when we try linking components to each other but one of the objects/components has yet to be initialized
	*/
	struct LinkingInfo
	{
		unsigned int objectId; // the object id from the object that contains the component
		ComponentBase** addressOfReference; // the address of where the reference to the component will be stored
		std::type_index typeId; //type id of the component its supposed to get
	};

private:

	std::vector<LinkingInfo> m_LinkingInfos;

	std::unordered_map<unsigned int, GameObject*> m_RegisteredObjects;
};

class FileDeserializer final : public std::ifstream, public DeserializerBase
{
	/**
	* std::Ifstream constructors
	*/
	FileDeserializer() = default;
	explicit FileDeserializer(const char* filename, std::ios_base::openmode mode = std::ios_base::in)
		: std::ifstream(filename, mode)
	{}
	explicit FileDeserializer(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
		: std::ifstream(filename, mode)
	{}
	explicit FileDeserializer(const wchar_t* filename, std::ios_base::openmode mode = std::ios_base::in)
		: std::ifstream(filename, mode)
	{}
	explicit FileDeserializer(const std::wstring& filename, std::ios_base::openmode mode = std::ios_base::in)
		: std::ifstream(filename, mode)
	{}
	explicit FileDeserializer(const unsigned short* filename, std::ios_base::openmode mode = std::ios_base::in)
		: std::ifstream(filename, mode)
	{}
	FileDeserializer(FILE* _File)
		: std::ifstream(_File)
	{}
	FileDeserializer(FileDeserializer&& x) noexcept
		: std::ifstream(std::move(x))
	{}

	/**
	* Deserializer Interfaces
	*/

	void DeserializeGame() override;
	Scene* DeserializeScene() override;
	GameObject* DeserializeObject() override;
};

//TODO complete string parser
//class StringDeserializer final : public std::istringstream, public DeserializerBase
//{
//	/**
//	* std::IStringstream constructors
//	*/
//	StringDeserializer(StringDeserializer&& x)
//		: std::istringstream(std::move(x))
//	{}
//	explicit StringDeserializer(std::ios_base::openmode which = std::ios_base::in)
//		: std::istringstream(which)
//	{}
//	explicit StringDeserializer(const std::string& str, std::ios_base::openmode which = std::ios_base::in)
//		: std::istringstream(str, which)
//	{}
//	explicit StringDeserializer(std::string&& str, std::ios_base::openmode which = std::ios_base::in)
//		: std::istringstream(std::move(str), which)
//	{}
//
//	/**
//	* Deserializer Interfaces
//	*/
//
//	void DeserializeGame() override;
//	Scene* DeserializeScene() override;
//	GameObject* DeserializeObject() override;
//};

struct ParsingError : public std::exception
{
	ParsingError() = default;
	ParsingError(const std::string& error)
		: errorMessage{ error }
	{};

	std::string errorMessage;

	const char* what() const override
	{
		return errorMessage.c_str();
	}
};

__forceinline bool CanContinue(std::istream& stream)
{
	char buffer{};
	stream >> buffer;
	if (buffer != '{' && buffer != '}')
		throw ParsingError("{ or } not found");
	return buffer == '{';
}

__forceinline bool IsEnd(std::istream& stream)
{
	if (stream.peek() == '}')
	{
		char buffer{};
		stream >> buffer;
		return true;
	}
	return false;
}