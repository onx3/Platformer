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
    , mStepDelay(0.001f)
    , mCurrentStep(0)
    , mDungeonGrid(mHeight, std::vector<EDungeonPiece>(mWidth, EDungeonPiece::Empty))
{
    mNeighborRules = {
    { EDungeonPiece::Floor, { EDungeonPiece::Floor, EDungeonPiece::WaterEdge, EDungeonPiece::Empty } },
    { EDungeonPiece::Water, { EDungeonPiece::Water, EDungeonPiece::WaterEdge, EDungeonPiece::Empty } },
    { EDungeonPiece::Empty, { EDungeonPiece::Floor, EDungeonPiece::Water } },
    { EDungeonPiece::WaterEdge, { EDungeonPiece::Floor, EDungeonPiece::Water } }
    };

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    Walker walker;
    walker.x = mWidth / 2;
    walker.y = mHeight / 2;
    walker.steps = 500;
    mWalkers.push_back(walker);
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

        std::vector<Walker> newWalkers;

        for (auto it = mWalkers.begin(); it != mWalkers.end();)
        {
            if (it->steps <= 0)
            {
                it = mWalkers.erase(it);
                continue;
            }
            int stepsPerUpdate = 10;
            for (int ii = 0; ii < stepsPerUpdate && it->steps > 0; ++ii)
            {
                RandomWalkStep(*it);
            }

            // 10% chance to spawn a new walker
            if (std::rand() % 100 < 10 && mWalkers.size() < 50)
            {
                Walker newWalker;
                newWalker.x = it->x;
                newWalker.y = it->y;
                newWalker.steps = 500;
                newWalkers.push_back(newWalker);
            }

            // 5% chance to delete the walker (ensure at least 1 remains)
            if (std::rand() % 100 < 5 && mWalkers.size() > 1)
            {
                it = mWalkers.erase(it);
            }
            else
            {
                ++it;
            }
        }

        mWalkers.insert(mWalkers.end(), newWalkers.begin(), newWalkers.end());
    }
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::RandomWalkStep(Walker & walker)
{
    if (walker.steps <= 0) return;

    // Weighted random chance for block placement
    EDungeonPiece blockToPlace;

    // Determine if the walker is near water
    bool nearWater = false;
    if (walker.y > 0 && mDungeonGrid[walker.y - 1][walker.x] == EDungeonPiece::Water) nearWater = true;
    if (walker.x > 0 && mDungeonGrid[walker.y][walker.x - 1] == EDungeonPiece::Water) nearWater = true;
    if (walker.y < mHeight - 1 && mDungeonGrid[walker.y + 1][walker.x] == EDungeonPiece::Water) nearWater = true;
    if (walker.x < mWidth - 1 && mDungeonGrid[walker.y][walker.x + 1] == EDungeonPiece::Water) nearWater = true;

    // Adjust placement chance
    if (nearWater && std::rand() % 100 < 80) // 80% chance to place water near water
    {
        blockToPlace = EDungeonPiece::Water;
    }
    else if (std::rand() % 100 < 10) // 10% chance for water in general
    {
        blockToPlace = EDungeonPiece::Water;
    }
    else
    {
        blockToPlace = EDungeonPiece::Floor;
    }

    // Attempt to place the block
    if (CanPlaceBlock(walker.x, walker.y, blockToPlace, mNeighborRules))
    {
        mDungeonGrid[walker.y][walker.x] = blockToPlace;
        walker.lastPlacedBlock = blockToPlace;
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
                mDungeonGrid[y][x] = EDungeonPiece::Water;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------