#include "AstroidsPrivate.h"
#include "DungeonManager.h"
#include <unordered_map>
#include <cstdlib>
#include <ctime>

DungeonManager::DungeonManager(GameManager * pGameManager, int width, int height)
    : BaseManager(pGameManager)
    , mWidth(width)
    , mHeight(height)
    , mTimeSinceLastStep(0.f)
    , mStepDelay(0.001f) // Set a default delay
    , mCurrentStep(0)
    , mWalkers()
    , mWalkerSteps()
    , mDungeonGrid(mHeight, std::vector<EDungeonPiece>(mWidth, EDungeonPiece::Empty))
{
    mNeighborRules = {
        { EDungeonPiece::Floor, { EDungeonPiece::Floor, EDungeonPiece::Wall, EDungeonPiece::Empty } },
        { EDungeonPiece::Wall, { EDungeonPiece::Floor, EDungeonPiece::Wall, EDungeonPiece::Empty } },
        { EDungeonPiece::Empty, { EDungeonPiece::Floor, EDungeonPiece::Wall } }
    };

    // Seed the random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Initialize 3 walkers
    for (int i = 0; i < 15; ++i)
    {
        Walker walker;
        walker.x = rand() % mWidth;
        walker.y = rand() % mHeight;
        walker.steps = 500;
        mWalkers.push_back(walker);
    }
}

//------------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<EDungeonPiece>> & DungeonManager::GetDungeonGrid() const
{
    return mDungeonGrid;
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::Update(float deltaTime)
{

}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::DebugUpdate(float deltaTime)
{
    mTimeSinceLastStep += deltaTime;

    if (mTimeSinceLastStep >= mStepDelay)
    {
        mTimeSinceLastStep -= mStepDelay;

        // Perform one step for all walkers
        for (auto & walker : mWalkers)
        {
            if (walker.steps > 0)
            {
                RandomWalkStep(walker);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::RandomWalkStep(Walker & walker)
{
    if (walker.steps <= 0) return;

    // Place a tile
    if (CanPlaceBlock(walker.x, walker.y, EDungeonPiece::Floor, mNeighborRules))
    {
        mDungeonGrid[walker.y][walker.x] = EDungeonPiece::Floor;
        mWalkerSteps.push_back({ walker.x, walker.y }); // Log the position
    }

    // Move the walker
    int direction = std::rand() % 4;
    switch (direction)
    {
        case 0: if (walker.y > 0) --walker.y; break;               // Up
        case 1: if (walker.x < mWidth - 1) ++walker.x; break;     // Right
        case 2: if (walker.y < mHeight - 1) ++walker.y; break;    // Down
        case 3: if (walker.x > 0) --walker.x; break;              // Left
    }

    --walker.steps; // Decrement steps
}

//------------------------------------------------------------------------------------------------------------------------

bool DungeonManager::CanPlaceBlock(int x, int y, EDungeonPiece type, const std::unordered_map<EDungeonPiece, std::vector<EDungeonPiece>> & neighborRules) const
{
    if (x < 0 || x >= mWidth || y < 0 || y >= mHeight) return false;
    if (mDungeonGrid[y][x] != EDungeonPiece::Empty) return false; // Only place on empty tiles

    // Validate neighbors
    std::vector<EDungeonPiece> validNeighbors = neighborRules.at(type);

    if (y > 0 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y - 1][x]) == validNeighbors.end()) return false; // Up
    if (x < mWidth - 1 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y][x + 1]) == validNeighbors.end()) return false; // Right
    if (y < mHeight - 1 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y + 1][x]) == validNeighbors.end()) return false; // Down
    if (x > 0 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y][x - 1]) == validNeighbors.end()) return false; // Left

    return true;
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::AddScreenBounds()
{
    for (int y = 0; y < mHeight; ++y)
    {
        for (int x = 0; x < mWidth; ++x)
        {
            if (y == 0 || y == mHeight - 1 || x == 0 || x == mWidth - 1)
            {
                mDungeonGrid[y][x] = EDungeonPiece::Wall;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------