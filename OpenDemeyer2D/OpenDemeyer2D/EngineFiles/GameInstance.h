#pragma once

class GameInstance
{
public:
	GameInstance() = default;
	virtual ~GameInstance() = default;
	GameInstance(const GameInstance& other) = delete;
	GameInstance(GameInstance&& other) noexcept = delete;
	GameInstance& operator=(const GameInstance& other) = delete;
	GameInstance& operator=(GameInstance&& other) noexcept = delete;

	virtual void LoadGame() = 0;
};