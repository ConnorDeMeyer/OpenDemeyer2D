#include "Stage.h"

#include "GameObject.h"
#include "SDL.h"
#include "ResourceManager.h"
#include "BurgerPiece.h"
#include "BurgerPiece.h"
#include "Components/TextureComponent.h"
#include "GameplayPrefabs.h"
#include "imgui.h"
#include "PeterPepper.h"
#include "StageMovement.h"

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

			if (level1[x + stageWidth * y] & 0b01) // draw the platforms
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

	auto sheetTexture = RESOURCES.LoadTexture("Bitmaps/FullSheet.png");
	// burger holders at the bottom
	for (int i{}; i < 4; ++i)
	{
		auto go = new GameObject();
		auto render = go->AddComponent<RenderComponent>();
		auto texture = go->AddComponent<TextureComponent>();
		texture->SetTexture(sheetTexture);
		texture->SetSourceRect({ 112.f, 104.f, 48.f, 16.f });
		render->SetRenderAlignMode(eRenderAlignMode::left);

		go->GetTransform()->SetPosition({ 8.f + 48.f * i, -32.f });
		go->SetParent(GetParent());
	}
	
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
		auto burgerPiece = piece->AddComponent<BurgerPiece>();

		burgerPiece->SetType(info.type);
		piece->GetTransform()->SetPosition({ 32.f + 48.f * float(info.posX), 4.f + 16.f * float(info.posY) });

		piece->SetParent(GetParent());
	}
}

void Stage::SpawnPlayer()
{
	m_pPeterPepper = PeterPepperFactory();
	auto stageMovement = m_pPeterPepper->GetComponent<StageMovement>();
	stageMovement->SetStage(this);

	m_pPeterPepper->SetParent(GetParent());
	m_pPeterPepper->GetTransform()->SetPosition({ 104,3 });
}

constexpr int BigHorizontalTiles = stageWidth / 2;
constexpr int smallHorizontalTiles = stageWidth - BigHorizontalTiles;
constexpr int PositionMap[stageWidth + BigHorizontalTiles]
{
	0,1,1,2,3,3,4,5,5,6,7,7,8
};
constexpr float ladderPositionMap[stageWidth]
{
	8.f, 32.f, 56.f, 80.f, 104.f, 128.f, 152.f, 176.f, 200.f
};

bool Stage::CanMoveInDirection(const glm::vec2& position, movementDirection direction)
{

	assert(position.x >= 0.f && position.x < smallHorizontalTiles * 16.f + BigHorizontalTiles * 32.f);
	assert(position.y >= 0.f && position.y < stageHeight * 16.f);

	int xPos = PositionMap[int(position.x / 16.f)];
	int yPos = stageHeight - 1 - int((position.y) / 16.f);
	int arrayPos{ xPos + yPos * stageWidth };

	if (xPos < 0 || xPos >= stageWidth || yPos < 0 || yPos > stageHeight)
		return false;

	float xTilePos = fmod(position.x, 16.f);
	float yTilePos = fmod(position.y, 16.f);

	switch (direction)
	{
	case movementDirection::right:
	{
		bool isOnGround{ fabsf(yTilePos - 3.f) <= .5f && int(m_Tiles[arrayPos]) & 0b1 };
		if (isOnGround) {
			if (xTilePos <= 7.5f || // go until the center of a tile
					(xPos < stageWidth - 1 && 
					(int(m_Tiles[arrayPos + 1]) & 0b1))) // go right if the tile next to it is not empty 
				return true;
		}
		break;
	}
	case movementDirection::left:
	{
		bool isOnGround{ fabsf(yTilePos - 3.f) <= .5f && int(m_Tiles[arrayPos]) & 0b1 };
		if (isOnGround) {
			if (xTilePos >= 8.5f || // go until the center of a tile
				(xPos > 0 && (int(m_Tiles[arrayPos - 1]) & 0b1)))
				return true;
		}
		break;
	}
	case movementDirection::up:
	{
		bool isOnLadder{ bool(int(m_Tiles[arrayPos]) & 0b10) };
		bool isLadderDown{ yPos < stageHeight - 1 && bool(int(m_Tiles[arrayPos + stageWidth]) & 0b10) };
		bool isAboveGround{ yTilePos >= 2.5f };
		auto ladderPos = ladderPositionMap[xPos];
		if (fabsf(ladderPos - position.x) <= 1.f && (isOnLadder || (!isAboveGround && isLadderDown)))
			return true;
		break;
	}
	case movementDirection::down:
	{
		bool isOnLadder{ bool(int(m_Tiles[arrayPos]) & 0b10) };
		bool isLadderDown{yPos < stageHeight - 1 && bool(int(m_Tiles[arrayPos + stageWidth]) & 0b10)};
		bool isAboveGround{ yTilePos >= 3.5f };
		auto ladderPos = ladderPositionMap[xPos];
		if (fabsf(ladderPos - position.x) <= 1.f && 
			((isAboveGround && isOnLadder) || (!isAboveGround && isLadderDown)) )
			return true;
		break;
	}
	default:
		assert(false);
	}

	return false;
}

void Stage::SnapToGridX(Transform* transform)
{
	auto pos = transform->GetLocalPosition();
	int xPos = PositionMap[int(pos.x / 16.f)];
	auto ladderPos = ladderPositionMap[xPos];
	if (fabsf(ladderPos - pos.x) <= 1.f)
		transform->SetPosition({ ladderPositionMap[xPos], pos.y });
}

void Stage::SnapToGridY(Transform* transform)
{
	auto pos = transform->GetLocalPosition();
	int yPos = int(pos.y / 16.f);
	float yTilePos = fmod(pos.y, 16.f);
	if (fabsf(yTilePos - 3.f) <= 1.f)
		transform->SetPosition({ pos.x, yTilePos + float(yPos) * 16.f });
}

float Stage::GetNextPlatformDown(const glm::vec2& pos)
{
	int xPos = PositionMap[int(pos.x / 16.f)];
	int yPos = stageHeight - 0 - int((pos.y) / 16.f);
	while (yPos < stageHeight)
	{
		int arrayPos{ xPos + yPos * stageWidth };
		if (int(m_Tiles[arrayPos]) & 0b1)
		{
			return (stageHeight - yPos -  1) * 16.f + 5.f;
		}
		++yPos;
	}

	return -36.f + float(m_FallenHamburgers[xPos / 2]++) * 8.f;
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

	SpawnPlayer();

	if (auto renderComp{ GetParent()->GetRenderComponent() }) {
		renderComp->SetTexture(m_StageTexture);
		renderComp->SetRenderAlignMode(eRenderAlignMode::bottomLeft);
	}
}

void Stage::RenderImGui()
{
	// Show texture
	float ratio = float(m_StageTexture->GetWidth()) / float(m_StageTexture->GetHeight());
#pragma warning(disable : 4312)
	ImGui::Image((ImTextureID)(m_StageTexture->GetId()), { 16 * ratio,16 });
#pragma warning(default : 4312)
}