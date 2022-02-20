#pragma once
#include <iostream>

enum class ParseCommands : uint8_t
{
	NOOP			= 0x00,
	ADD_GAMEOBJECT	= 0x01,
	ADD_COMPONENT	= 0x02,
	TREE_UP			= 0x03,
	TREE_DOWN		= 0x04,
	NEW_SCENE		= 0x05,

	//Components
	COMP_RENDER,
	COMP_TEXT,
	COMP_TEXTURE,
	TRANSFORM
};

class IEngingeFunction
{
public:

	virtual void BinaryDataOut(std::ostream&) = 0;

	virtual void BinaryDataIn(std::istream&) = 0;
};