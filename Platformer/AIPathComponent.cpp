#include "AstroidsPrivate.h"
#include "AIPathComponent.h"
#include "PlayerManager.h"
#include "imgui.h"
#include "LevelManager.h"

AIPathComponent::AIPathComponent(GameObject * pGameObject)
	: GameComponent(pGameObject)
	, mName("AIPathComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

AIPathComponent::~AIPathComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void AIPathComponent::Update(float deltaTime)
{
	auto * pPlayer = GetGameManager().GetManager<PlayerManager>()->GetPlayers()[0];
	auto * pLevelManager = GetGameManager().GetManager<LevelManager>();
	if (!pPlayer || !pLevelManager)
	{
		return;
	}

	auto myPosition = GetGameObject().GetPosition();
	auto playerPos = pPlayer->GetPosition();

	auto cellSize = BD::gsPixelCount;
	int tileX = static_cast<int>(myPosition.x / cellSize);
	int tileY = static_cast<int>(myPosition.y / cellSize);

	int playerTileX = static_cast<int>(playerPos.x / cellSize);
	int playerTileY = static_cast<int>(playerPos.y / cellSize);

	sf::Vector2i direction(
		playerTileX > tileX ? 1 : (playerTileX < tileX ? -1 : 0),
		playerTileY > tileY ? 1 : (playerTileY < tileY ? -1 : 0)
	);

	int newTileX = tileX + direction.x;
	int newTileY = tileY + direction.y;

	if (pLevelManager->IsTileWalkableAI(newTileX, newTileY))
	{
		sf::Vector2f newPosition(
			newTileX * cellSize + cellSize * 0.5f,
			newTileY * cellSize + cellSize * 0.5f
		);

		GetGameObject().SetPosition(newPosition);
	}
}

//------------------------------------------------------------------------------------------------------------------------

void AIPathComponent::DebugImGuiComponentInfo()
{
	auto gameObjPos = GetGameObject().GetPosition();
	ImGui::Text("Position x,y: %.3f, %.3f", gameObjPos.x, gameObjPos.y);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & AIPathComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------