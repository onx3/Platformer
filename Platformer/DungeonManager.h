#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "BaseManager.h"

enum class EDungeonPiece
{
    Empty,
    Water,
    Floor,
    WaterEdge
};

struct Walker
{
    int x;
    int y;
    int steps;
    EDungeonPiece lastPlacedBlock = EDungeonPiece::Empty;
};

class BaseManager;
class DungeonManager : public BaseManager
{
public:
    DungeonManager(GameManager * pGameManager, int width, int height);

    const std::vector<std::vector<EDungeonPiece>> & GetDungeonGrid() const;

    void Update(float deltaTime) override;
    void DebugUpdate(float deltaTime);

    void RandomWalkStep(Walker & walker);

    bool CanPlaceBlock(int x, int y, EDungeonPiece type, const std::unordered_map<EDungeonPiece, std::vector<EDungeonPiece>> & neighborRules) const;

private:
    void AddScreenBounds();

    int mWidth;
    int mHeight;
    float mTimeSinceLastStep;
    float mStepDelay;
    int mCurrentStep;
    std::vector<Walker> mWalkers;
    std::unordered_map<EDungeonPiece, std::vector<EDungeonPiece>> mNeighborRules;
    std::vector<std::vector<EDungeonPiece>> mDungeonGrid;
};