#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <memory>

class Scene;
class ComponentBase;
class GameObject;

class Deserializer
{
public:

	/** 
	* Register the object so others may be able to use it at a reference
	* the streamId is the id the object has inside of the stream, not inside the game
	*/
	void RegisterGameObject(unsigned int streamId, GameObject* object);

	void GetComponentFromObject(uint32_t objectId, uint32_t compId, std::function<void(std::weak_ptr<ComponentBase>*)> linker);

	bool CanContinue();
	bool IsEnd();
	char PeekChar();

	void DeserializeGame(std::istream& iStream);
	Scene* DeserializeScene(std::istream& iStream);
	GameObject* DeserializeObject(std::istream& iStream);

	std::istream* GetStream() { return m_pIStream; }

private:

	/**
	* Struct used to keep information for when we try linking components to each other but one of the objects/components has yet to be initialized
	*/
	struct LinkingInfo
	{
		uint32_t objectId; // the object id from the object that contains the component
		//std::weak_ptr<ComponentBase>* addressOfReference; // the address of where the reference to the component will be stored
		uint32_t typeId; //type id of the component its supposed to get
		std::function<void(std::weak_ptr<ComponentBase>*)> pointerLinker;
	};

private:

	std::istream* m_pIStream{};

	std::vector<LinkingInfo> m_LinkingInfos;

	std::unordered_map<unsigned int, GameObject*> m_RegisteredObjects;
};

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

inline bool CanContinue(std::istream& stream)
{
	char buffer{};
	stream >> buffer;
	if (buffer != '{' && buffer != '}')
		throw ParsingError("{ or } not found");
	return buffer == '{';
}

inline char PeekNextChar(std::istream& stream)
{
	char buffer{};
	stream >> buffer;
	stream.unget();
	return buffer;
}

inline bool IsEnd(std::istream& stream)
{
	if (PeekNextChar(stream) == '}')
	{
		char buffer{};
		stream >> buffer;
		return true;
	}
	return false;
}

//https://www.cplusplus.com/forum/beginner/251052/
inline void TrimWhitespace(std::string& string)
{
	constexpr const char* whiteSpace{ " \t\v\r\n" };
	size_t begin = string.find_first_not_of(whiteSpace);
	size_t end = string.find_last_not_of(whiteSpace);
	string = begin == end ? std::string() : string.substr(begin, end - begin + 1);
}