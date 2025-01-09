#pragma once
#include <vector>
#include <string>
#include "BaseManager.h"

enum class EDungeonPiece
{
	Empty,
	Wall,
	Floor,
};

class BaseManager;
class DungeonManager : public BaseManager
{
public:
	DungeonManager(GameManager * pGameManger, int width, int height);

	void GenerateDungeon();
	const std::vector<std::vector<EDungeonPiece>> & GetDungeonGrid() const;

	void Update(float deltaTime) override;	

private:

	void AddWalls();

	bool CanPlaceRoom(int x, int y, int width, int height) const;

	void AddRoomWithWalls(int x, int y, int width, int height);

	void AddScreenBounds();

	int mWidth;
	int mHeight;
	std::vector<std::vector<EDungeonPiece>> mDungeonGrid;
};

