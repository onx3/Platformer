#include "AstroidsPrivate.h"
#include "DungeonManager.h"
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

//------------------------------------------------------------------------------------------------------------------------

DungeonManager::DungeonManager(GameManager * pGameManager, int width, int height)
    : BaseManager(pGameManager)
    , mWidth(width)
    , mHeight(height)
    , mTimeSinceLastStep(0.f)
    , mStepDelay(0.001f)
    , mCurrentStep(0)
    , mDungeonGrid(mHeight, std::vector<EDungeonPiece>(mWidth, EDungeonPiece::Empty))
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    mNeighborRules = {
        { EDungeonPiece::Water, { EDungeonPiece::Water, EDungeonPiece::Sand, EDungeonPiece::Empty} },
        { EDungeonPiece::Sand,  { EDungeonPiece::Water, EDungeonPiece::Sand, EDungeonPiece::Grass, EDungeonPiece::Empty } },
        { EDungeonPiece::Grass, { EDungeonPiece::Sand, EDungeonPiece::Grass, EDungeonPiece::Empty} }
    };

    GenerateDungeonGrid();
}

//------------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<EDungeonPiece>> & DungeonManager::GetDungeonGrid() const
{
    return mDungeonGrid;
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::Update(float deltaTime)
{
    // Implement any runtime updates if necessary
}

//------------------------------------------------------------------------------------------------------------------------

void DungeonManager::GenerateDungeonGrid()
{
    float scale = 0.1f;  // Adjust to control the size of terrain features
    int seed = std::rand(); // Use a random seed for variation
    float maxOffset = .05f; // Allow slightly more variation in noise

    std::vector<std::vector<float>> noiseGrid(mHeight, std::vector<float>(mWidth, 0.0f));

    for (int y = 0; y < mHeight; ++y)
    {
        for (int x = 0; x < mWidth; ++x)
        {
            // Generate Perlin noise
            float baseValue = Perlin(x * scale, y * scale, seed);

            // Apply clamping for smoother transitions
            if (x > 0)
                baseValue = std::clamp(baseValue, noiseGrid[y][x - 1] - maxOffset, noiseGrid[y][x - 1] + maxOffset);
            if (y > 0)
                baseValue = std::clamp(baseValue, noiseGrid[y - 1][x] - maxOffset, noiseGrid[y - 1][x] + maxOffset);
            noiseGrid[y][x] = baseValue;

            // Normalize noise to [0, 1]
            float normalized = (baseValue + 1.0f) / 2.0f;

            // Assign terrain types based on thresholds and neighbor rules
            if (normalized < 0.4f && CanPlaceTile(x, y, EDungeonPiece::Water))
            {
                mDungeonGrid[y][x] = EDungeonPiece::Water;
            }
            else if (normalized < 0.5f && CanPlaceTile(x, y, EDungeonPiece::Sand))
            {
                mDungeonGrid[y][x] = EDungeonPiece::Sand;
            }
            else if (CanPlaceTile(x, y, EDungeonPiece::Grass))
            {
                mDungeonGrid[y][x] = EDungeonPiece::Grass;
            }
            else
            {
                // Fallback case: default to Sand
                mDungeonGrid[y][x] = EDungeonPiece::Sand;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

bool DungeonManager::CanPlaceTile(int x, int y, EDungeonPiece type) const
{
    if (x < 0 || x >= mWidth || y < 0 || y >= mHeight) return false;

    const auto & validNeighbors = mNeighborRules.at(type);

    // Check neighbors
    if (y > 0 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y - 1][x]) == validNeighbors.end()) return false; // Up
    if (x < mWidth - 1 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y][x + 1]) == validNeighbors.end()) return false; // Right
    if (y < mHeight - 1 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y + 1][x]) == validNeighbors.end()) return false; // Down
    if (x > 0 && std::find(validNeighbors.begin(), validNeighbors.end(), mDungeonGrid[y][x - 1]) == validNeighbors.end()) return false; // Left

    return true;
}

//------------------------------------------------------------------------------------------------------------------------

float DungeonManager::Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

//------------------------------------------------------------------------------------------------------------------------

float DungeonManager::Fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

//------------------------------------------------------------------------------------------------------------------------

int DungeonManager::Hash(int x, int y, int seed = 0)
{
    int h = x * 374761393 + y * 668265263 + seed * 982451653;
    h = (h ^ (h >> 13)) * 1274126177;
    return h & 0x7FFFFFFF;
}

//------------------------------------------------------------------------------------------------------------------------

float DungeonManager::Gradient(int x, int y, int seed = 0)
{
    int h = Hash(x, y, seed);
    return (h % 256) / 255.0f * 2.0f - 1.0f;
}

//------------------------------------------------------------------------------------------------------------------------

float DungeonManager::Perlin(float x, float y, int seed = 0)
{
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

    float sx = x - (float)x0;
    float sy = y - (float)y0;

    float n0, n1, ix0, ix1;

    n0 = Gradient(x0, y0, seed);
    n1 = Gradient(x1, y0, seed);
    ix0 = Lerp(n0, n1, Fade(sx));

    n0 = Gradient(x0, y1, seed);
    n1 = Gradient(x1, y1, seed);
    ix1 = Lerp(n0, n1, Fade(sx));

    return Lerp(ix0, ix1, Fade(sy)); // Final value in range [-1, 1]
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------