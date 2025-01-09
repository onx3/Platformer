#include "AstroidsPrivate.h"
#include "DungeonManager.h"

DungeonManager::DungeonManager(GameManager * pGameManger, int width, int height)
	: BaseManager(pGameManger)
	, mWidth(width)
	, mHeight(height)
	, mDungeonGrid()
{
	mDungeonGrid = std::vector<std::vector<EDungeonPiece>>(mHeight, std::vector<EDungeonPiece>(mWidth, EDungeonPiece::Empty));
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::GenerateDungeon()
{
    // Initialize the grid
    mDungeonGrid = std::vector<std::vector<EDungeonPiece>>(mHeight, std::vector<EDungeonPiece>(mWidth, EDungeonPiece::Empty));

    int roomCount = 10; // Number of rooms to generate
    for (int i = 0; i < roomCount; ++i)
    {
        bool roomPlaced = false;
        int attemptLimit = 100; // Prevent infinite loops
        int attempts = 0;

        while (!roomPlaced && attempts < attemptLimit)
        {
            ++attempts;

            // Generate random room dimensions and position
            int roomWidth = rand() % 6 + 3; // Room width between 3 and 8
            int roomHeight = rand() % 6 + 3; // Room height between 3 and 8
            int roomX = rand() % (mWidth - roomWidth - 2) + 1; // Ensure space for walls
            int roomY = rand() % (mHeight - roomHeight - 2) + 1; // Ensure space for walls

            // Check if the room overlaps with existing rooms or walls
            if (CanPlaceRoom(roomX, roomY, roomWidth, roomHeight))
            {
                AddRoomWithWalls(roomX, roomY, roomWidth, roomHeight);
                roomPlaced = true;
            }
        }

        if (!roomPlaced)
        {
            std::cerr << "Failed to place room after " << attemptLimit << " attempts.\n";
        }
    }

    // Add screen bounds
    AddScreenBounds();
}

//------------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<EDungeonPiece>> & DungeonManager::GetDungeonGrid() const
{
    return mDungeonGrid;
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::Update(float deltaTime)
{
    // No dynamic updates required for static dungeon generation.
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::AddWalls()
{
    for (int y = 1; y < mHeight - 1; ++y)
    {
        for (int x = 1; x < mWidth - 1; ++x)
        {
            if (mDungeonGrid[y][x] == EDungeonPiece::Empty)
            {
                if (mDungeonGrid[y - 1][x] == EDungeonPiece::Floor ||
                    mDungeonGrid[y + 1][x] == EDungeonPiece::Floor ||
                    mDungeonGrid[y][x - 1] == EDungeonPiece::Floor ||
                    mDungeonGrid[y][x + 1] == EDungeonPiece::Floor)
                {
                    mDungeonGrid[y][x] = EDungeonPiece::Wall;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool DungeonManager::CanPlaceRoom(int x, int y, int width, int height) const
{
    // Check if the room overlaps with any existing walls or floors
    for (int i = y - 1; i <= y + height; ++i)
    {
        for (int j = x - 1; j <= x + width; ++j)
        {
            if (mDungeonGrid[i][j] != EDungeonPiece::Empty)
            {
                return false; // Overlap detected
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::AddRoomWithWalls(int x, int y, int width, int height)
{
    // Add walls around the room
    for (int i = y - 1; i <= y + height; ++i)
    {
        for (int j = x - 1; j <= x + width; ++j)
        {
            if (i == y - 1 || i == y + height || j == x - 1 || j == x + width)
            {
                mDungeonGrid[i][j] = EDungeonPiece::Wall;
            }
            else
            {
                mDungeonGrid[i][j] = EDungeonPiece::Floor;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::AddScreenBounds()
{
    // Add black border around the grid to indicate screen bounds
    for (int y = 0; y < mHeight; ++y)
    {
        for (int x = 0; x < mWidth; ++x)
        {
            if (y == 0 || y == mHeight - 1 || x == 0 || x == mWidth - 1)
            {
                mDungeonGrid[y][x] = EDungeonPiece::Empty; // Set border to Empty (black)
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------