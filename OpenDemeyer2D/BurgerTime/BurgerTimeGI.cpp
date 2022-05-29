#include "BurgerTimeGI.h"

#include "EngineIO/Deserializer.h"


void BurgerTimeGI::LoadGame()
{
	auto stream = std::ifstream("BurgerTime");
	Deserializer gameLoader{};
	gameLoader.DeserializeGame(stream);
}
