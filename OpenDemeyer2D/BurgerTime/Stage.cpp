﻿#include "Stage.h"

#include "GameObject.h"
#include "SDL.h"
#include "ResourceManager.h"
#include "BurgerPiece.h"
#include "BurgerPiece.h"
#include "Components/TextureComponent.h"

constexpr char level1[stageSize]
{
	1,1,1,1,1,1,1,1,1,
	2,0,2,2,2,0,2,0,2,
	3,1,3,2,3,1,3,1,3,
	0,2,3,3,3,2,2,0,2,
	1,3,3,0,2,2,3,1,3,
	2,2,3,1,3,3,3,2,0,
	2,2,2,0,2,0,3,3,1,
	3,3,3,1,3,1,3,2,2,
	2,0,2,0,2,0,2,2,2,
	3,1,3,1,3,1,3,3,3
};

void Stage::LoadStageTexture()
{

	SDL_Surface* pLevelLayout = SDL_CreateRGBSurfaceWithFormat(0, 208, 160, 32, SDL_PIXELFORMAT_RGBA32);
	//SDL_Rect rect{ 0,0,208,160 };
	//SDL_FillRect(pLevelLayout, &rect, 0xEEEEEEFF);

	SDL_Surface* pLadderSurface = SDL_CreateRGBSurfaceWithFormat(0, 16, 16, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_Surface* pPlatformSurface = SDL_CreateRGBSurfaceWithFormat(0, 16, 16, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_Surface* pLargePlatformSurface = SDL_CreateRGBSurfaceWithFormat(0, 32, 16, 32, SDL_PIXELFORMAT_RGBA32);

	SDL_Rect LadderConnectorRect{ 3,15,10,1 };

	// Create the ladder surface
	SDL_Rect ladderRect{ 3,1,10,1 };
	for (int i{}; i < 8; ++i)
	{
		SDL_FillRect(pLadderSurface, &ladderRect, SDL_MapRGB(pLadderSurface->format, 179, 182, 167));
		ladderRect.y += 2;
	}

	// Create platform surface
	SDL_Rect platformRect = { 0,13,16,2 };
	SDL_FillRect(pPlatformSurface, &platformRect, SDL_MapRGB(pPlatformSurface->format, 95, 212, 191));
	SDL_FillRect(pPlatformSurface, &LadderConnectorRect, SDL_MapRGB(pPlatformSurface->format, 179, 182, 167));

	// Create the large platform surface
	SDL_Rect LargePlatformRect = { 0,13,32,2 };
	LadderConnectorRect.x = 11;
	SDL_FillRect(pLargePlatformSurface, &LargePlatformRect, SDL_MapRGB(pLargePlatformSurface->format, 14, 15, 108));
	SDL_FillRect(pLargePlatformSurface, &LadderConnectorRect, SDL_MapRGB(pLargePlatformSurface->format, 179, 182, 167));

	// first we draw the platforms
	SDL_Rect cursorPos{ 0,0,16,16 };
	for (int y{}; y < stageHeight; ++y)
	{
		for (int x{}; x < stageWidth; ++x)
		{
			bool isOdd{ bool(x & 0b1) };

			if (level1[x + stageWidth * y] & 0b10) // draw the ladders
			{
				if (isOdd)
				{
					cursorPos.x += 8;
					SDL_BlitSurface(pLadderSurface, nullptr, pLevelLayout, &cursorPos);
					cursorPos.x -= 8;
				}
				else
				{
					SDL_BlitSurface(pLadderSurface, nullptr, pLevelLayout, &cursorPos);
				}
			}

			if (level1[x + stageWidth * y] & 0b1) // draw the platforms
			{
				if (isOdd)
				{
					SDL_BlitSurface(pLargePlatformSurface, nullptr, pLevelLayout, &cursorPos);
				}
				else
				{
					SDL_BlitSurface(pPlatformSurface, nullptr, pLevelLayout, &cursorPos);
				}
			}

			cursorPos.x += isOdd ? 32 : 16;
			cursorPos.w = 16;
		}
		cursorPos.y += 16;
		cursorPos.x = 0;
	}

	m_StageTexture = RESOURCES.LoadTexture(pLevelLayout);

	SDL_FreeSurface(pLevelLayout);

	
}

void Stage::LoadStageItems()
{
	struct burgerPieceInfo
	{
		BurgerPieceType type;
		uint8_t posX;
		uint8_t posY;
	};

	constexpr int burgerPiecesAmount{ 16 };
	constexpr burgerPieceInfo burgerPiecesLayout[burgerPiecesAmount]
	{
		{BurgerPieceType::topBun, 0,7},
		{BurgerPieceType::topBun, 1,9},
		{BurgerPieceType::topBun, 2,9},
		{BurgerPieceType::topBun, 3,9},

		{BurgerPieceType::lettuce, 0,5},
		{BurgerPieceType::lettuce, 1,4},
		{BurgerPieceType::lettuce, 2,7},
		{BurgerPieceType::lettuce, 3,7},

		{BurgerPieceType::meat, 0,2},
		{BurgerPieceType::meat, 1,2},
		{BurgerPieceType::meat, 2,4},
		{BurgerPieceType::meat, 3,5},

		{BurgerPieceType::bottomBun, 0,0},
		{BurgerPieceType::bottomBun, 1,0},
		{BurgerPieceType::bottomBun, 2,0},
		{BurgerPieceType::bottomBun, 3,3}
	};

	for (int i{}; i < burgerPiecesAmount; ++i)
	{
		auto& info = burgerPiecesLayout[i];

		auto piece = new GameObject();
		piece->AddComponent<RenderComponent>();
		piece->AddComponent<TextureComponent>();
		auto burgerPiece = piece->AddComponent<BurgerPiece>();

		burgerPiece->SetType(info.type);
		piece->GetTransform()->SetPosition({ 32.f + 48.f * float(info.posX), 4.f + 16.f * float(info.posY) });

		piece->SetParent(GetParent());
	}
}

bool Stage::CanMoveInDirection(const glm::vec2& position, movementDirection direction)
{
	constexpr int BigHorizontalTiles = stageWidth / 2;
	constexpr int smallHorizontalTiles = stageWidth - BigHorizontalTiles;
	constexpr int PositionMap[stageWidth + BigHorizontalTiles]
	{
		0,1,1,2,3,3,4,5,5,6,7,7,8
	};

	assert(position.x >= 0.f && position.x < smallHorizontalTiles * 16.f + BigHorizontalTiles * 32.f);
	assert(position.y >= 0.f && position.y < stageHeight * 16.f);

	int xPos = PositionMap[int(position.x / 16.f)];
	int yPos = int(position.y / 16.f);
	int arrayPos{ xPos + yPos * stageWidth };

	switch (direction)
	{
	case movementDirection::right:
		if (fmod(position.x, 16.f) <= 8.f) // go until the center of a tile
			return true;
		if (xPos < stageWidth - 1 && (int(m_Tiles[arrayPos + 1]) & 0b1))
			return true;
		break;
	case movementDirection::left:
		if (fmod(position.x, 16.f) >= 8.f) // go until the center of a tile
			return true;
		if (xPos > 0 && (int(m_Tiles[arrayPos - 1]) & 0b1))
			return true;
		break;
	case movementDirection::up:
	{
		if ((!(xPos & 0b1) && fabsf(fmod(position.x, 16.f) - 8.f) < 1.f) ||
			((xPos & 0b1) && fabsf(fmod(position.x, 32.f) - 16.f) < 1.f))// has te be in the center of the tile
			if (int(m_Tiles[arrayPos]) & 0b10) // if on platform tile
				return true;
		break;
	}
	case movementDirection::down:
		if ((!(xPos & 0b1) && fabsf(fmod(position.x, 16.f) - 8.f) < 1.f) ||
			((xPos & 0b1) && fabsf(fmod(position.x, 32.f) - 16.f) < 1.f))// has te be in the center of the tile
			if (yPos > 0 && int(m_Tiles[arrayPos - stageWidth]) & 0b10) // if above platform tile
				return true;
		break;
	default:
		assert(false);
	}

	return false;
}

void Stage::BeginPlay()
{
	for (int y{}; y < stageHeight; ++y)
		for (int x{}; x < stageWidth; ++x)
		{
			m_Tiles[x + y * stageWidth] = tiles(level1[x + y * stageWidth]);
		}

	LoadStageTexture();
	LoadStageItems();

	if (auto renderComp{ GetParent()->GetRenderComponent() }) {
		renderComp->SetTexture(m_StageTexture);
		renderComp->SetRenderAlignMode(eRenderAlignMode::bottomLeft);
	}
}