#include "AstroidsPrivate.h"
#include <queue>
#include <unordered_map>
#include "AIPathComponent.h"
#include "PlayerManager.h"
#include "imgui.h"
#include "LevelManager.h"

AIPathComponent::AIPathComponent(GameObject * pGameObject)
    : GameComponent(pGameObject)
    , mName("AIPathComponent")
    , mPath()
    , mPathIndex(0)
    , mLastPlayerTile()
    , mStopDistance(250.f)
    , mMovementSpeed(.001f)
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

    float distanceSquared = (playerPos.x - myPosition.x) * (playerPos.x - myPosition.x) +
                            (playerPos.y - myPosition.y) * (playerPos.y - myPosition.y);

    if (distanceSquared <= mStopDistance * mStopDistance)
    {
        return;
    }

	auto cellSize = BD::gsPixelCount;
	sf::Vector2i myTile(static_cast<int>(myPosition.x / cellSize), static_cast<int>(myPosition.y / cellSize));
	sf::Vector2i playerTile(static_cast<int>(playerPos.x / cellSize), static_cast<int>(playerPos.y / cellSize));

	if (playerTile != mLastPlayerTile || mPath.empty())
	{
		mPath = FindPath(myTile, playerTile);
		mPathIndex = 0;
		mLastPlayerTile = playerTile;
	}

	if (!mPath.empty() && mPathIndex < mPath.size())
	{
		sf::Vector2i nextTile = mPath[mPathIndex];
		sf::Vector2f newPosition(
			nextTile.x * cellSize + cellSize * mMovementSpeed,
			nextTile.y * cellSize + cellSize * mMovementSpeed
		);

		GetGameObject().SetPosition(newPosition);

		if (myTile == nextTile)
		{
			++mPathIndex;
		}
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

std::vector<sf::Vector2i> AIPathComponent::FindPath(sf::Vector2i start, sf::Vector2i goal)
{
    auto * pLevelManager = GetGameManager().GetManager<LevelManager>();
    std::vector<sf::Vector2i> path;
    if (!pLevelManager || !pLevelManager->IsTileWalkableAI(goal.x, goal.y))
    {
        return path;
    }

    auto heuristic = [](sf::Vector2i a, sf::Vector2i b) {
        return abs(a.x - b.x) + abs(a.y - b.y);
        };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    std::unordered_map<int, Node> allNodes;

    auto getKey = [](sf::Vector2i pos) { return pos.y * 1000 + pos.x; };

    Node startNode = { start, 0, heuristic(start, goal), nullptr };
    openList.push(startNode);
    allNodes[getKey(start)] = startNode;

    std::vector<sf::Vector2i> directions = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

    while (!openList.empty())
    {
        Node current = openList.top();
        openList.pop();

        if (current.position == goal)
        {
            Node * pathNode = &allNodes[getKey(goal)];
            while (pathNode)
            {
                path.push_back(pathNode->position);
                pathNode = pathNode->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const auto & dir : directions)
        {
            sf::Vector2i neighborPos = { current.position.x + dir.x, current.position.y + dir.y };

            if (!pLevelManager->IsTileWalkableAI(neighborPos.x, neighborPos.y)) continue;

            int gCost = current.gCost + 1;
            int hCost = heuristic(neighborPos, goal);
            int key = getKey(neighborPos);

            if (allNodes.find(key) == allNodes.end() || gCost < allNodes[key].gCost)
            {
                Node neighborNode = { neighborPos, gCost, hCost, &allNodes[getKey(current.position)] };
                allNodes[key] = neighborNode;
                openList.push(neighborNode);
            }
        }
    }
    return path;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------