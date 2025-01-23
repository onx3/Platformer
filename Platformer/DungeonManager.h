#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "BaseManager.h"

enum class EDungeonPiece : unsigned char
{
    Empty,
    Water,
    Grass,
    Sand
};

class BaseManager;
class DungeonManager : public BaseManager
{
public:
    DungeonManager(GameManager * pGameManager, int width, int height);

    const std::vector<std::vector<EDungeonPiece>> & GetDungeonGrid() const;

    void Update(float deltaTime) override;

    void GenerateDungeonGrid();

    bool IsTileWalkable(EDungeonPiece tile);

private:
    bool CanPlaceTile(int x, int y, EDungeonPiece type) const;

    // Perlin Noise Helpers
    float Lerp(float a, float b, float t);
    float Fade(float t);
    int Hash(int x, int y, int seed);
    float Gradient(int x, int y, int seed);
    float Perlin(float x, float y, int seed);

    int mWidth;
    int mHeight;
    float mTimeSinceLastStep;
    float mStepDelay;
    int mCurrentStep;
    std::unordered_map<EDungeonPiece, std::vector<EDungeonPiece>> mNeighborRules;
    std::vector<std::vector<EDungeonPiece>> mDungeonGrid;
};