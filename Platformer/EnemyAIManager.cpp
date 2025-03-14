#include "AstroidsPrivate.h"
#include "EnemyAIManager.h"
#include "GameObject.h"
#include <memory>
#include <cassert>
#include <iostream>
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "HealthComponent.h"
#include "RandomMovementComponent.h"
#include "ExplosionComponent.h"
#include "DropManager.h"
#include "ResourceManager.h"
#include "AIPathComponent.h"
#include "FollowParentComponent.h"

EnemyAIManager::EnemyAIManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
{

}

//------------------------------------------------------------------------------------------------------------------------

EnemyAIManager::~EnemyAIManager()
{

}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::Update(float deltaTime)
{
    for (auto * pEnemy : mEnemyObjects)
    {
        if (pEnemy && !pEnemy->IsDestroyed())
        {
            auto pHealthComp = pEnemy->GetComponent<HealthComponent>().lock();
            if (pHealthComp)
            {
                pHealthComp->SetDeathCallBack([this, pEnemy]() {
                    OnDeath(pEnemy);
                    });
            }
        }
    }
	CleanUpDeadEnemies();

	while (mEnemyObjects.size() < mMaxEnemies)
	{
		sf::Vector2f spawnPosition = sf::Vector2f(float(1183.723), float(851.008));
		RespawnEnemy(EEnemy::TankBody, spawnPosition);
	}
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::OnGameEnd()
{
    mEnemyObjects.clear();
}

//------------------------------------------------------------------------------------------------------------------------

sf::Vector2f EnemyAIManager::GetRandomSpawnPosition()
{
    auto windowSize = GetGameManager().GetWindow().getSize();
    const int screenWidth = windowSize.x;
    const int screenHeight = windowSize.y;

    const float spawnOffset = 500.0f;

    sf::Vector2f spawnPosition;

    int edge = rand() % 4;

    switch (edge)
    {
        case 0: // Top
            spawnPosition.x = static_cast<float>(rand() % screenWidth);
            spawnPosition.y = -spawnOffset;
            break;

        case 1: // Bottom
            spawnPosition.x = static_cast<float>(rand() % screenWidth);
            spawnPosition.y = static_cast<float>(screenHeight) + spawnOffset;
            break;

        case 2: // Left
            spawnPosition.x = -spawnOffset;
            spawnPosition.y = static_cast<float>(rand() % screenHeight);
            break;

        case 3: // Right
            spawnPosition.x = static_cast<float>(screenWidth) + spawnOffset;
            spawnPosition.y = static_cast<float>(rand() % screenHeight);
            break;
    }
    return spawnPosition;
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::RemoveEnemy(GameObject * pEnemy)
{
    pEnemy->Destroy();
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::RespawnEnemy(EEnemy type, sf::Vector2f pos)
{
	AddEnemies(1, type, pos);
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::AddEnemies(int count, EEnemy type, sf::Vector2f pos)
{
    auto & gameManager = GetGameManager();
    for (int i = 0; i < count; ++i)
    {
        auto * pEnemy = gameManager.CreateNewGameObject(ETeam::Enemy, gameManager.GetRootGameObject());
        mEnemyObjects.push_back(pEnemy);

        auto pSpriteComp = pEnemy->GetComponent<SpriteComponent>().lock();

        if (pSpriteComp)
        {
            {
                SetUpSprite(*pSpriteComp, type);
                pSpriteComp->SetPosition(pos);
            }
            {
                // AI Path Movement
                auto pAIPathComponentComp = std::make_shared<AIPathComponent>(pEnemy);
                pEnemy->AddComponent(pAIPathComponentComp);

                // Health Component
                auto pHealthComponent = std::make_shared<HealthComponent>(pEnemy, 10, 100, 1, 1);
                pEnemy->AddComponent(pHealthComponent);
            }
            {
                pEnemy->CreatePhysicsBody(&gameManager.GetPhysicsWorld(), pEnemy->GetSize(), true);
                auto pCollisionComp = std::make_shared<CollisionComponent>(
                    pEnemy,
                    &gameManager.GetPhysicsWorld(),
                    pEnemy->GetPhysicsBody(),
                    pEnemy->GetSize(),
                    true
                );
                pEnemy->AddComponent(pCollisionComp);
            }
            {
                // Tank Logic
                auto * pGunGameObject = pEnemy->GetGameManager().CreateNewGameObject(ETeam::Enemy, pEnemy);
                auto pGunSpriteComp = pGunGameObject->GetComponent<SpriteComponent>().lock();
                if (pGunSpriteComp)
                {
                    SetUpSprite(*pGunSpriteComp, EEnemy::TankGuns);
                    pGunSpriteComp->SetPosition(pos);

                    auto pFollowParentComponent = std::make_shared<FollowParentComponent>(pGunGameObject);
                    pGunGameObject->AddComponent(pFollowParentComponent);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::DestroyAllEnemies()
{
    for (auto * pEnemy : mEnemyObjects)
    {
        pEnemy->Destroy();
    }
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::CleanUpDeadEnemies()
{
    for (auto * pEnemy : mEnemyObjects)
    {
        if (pEnemy && !pEnemy->IsDestroyed())
        {
            auto explosionComp = pEnemy->GetComponent<ExplosionComponent>().lock();
            if (explosionComp && explosionComp->IsAnimationFinished())
            {
                pEnemy->Destroy();
            }
        }
    }	

    auto removeStart = std::remove_if(mEnemyObjects.begin(), mEnemyObjects.end(),
        [](GameObject * pObj)
        {
            return pObj->IsDestroyed();
        });
	mEnemyObjects.erase(removeStart, mEnemyObjects.end());
}

//------------------------------------------------------------------------------------------------------------------------

std::string EnemyAIManager::GetEnemyFile(EEnemy type)
{
	switch (type)
	{
        case (EEnemy::TankBody):
        {
            return "Art/Tanks/PNG/Hulls_Color_C/Hull_01.png";
        }
        case (EEnemy::TankGuns):
        {
            return "Art/Tanks/PNG/Weapon_Color_C/Gun_01.png";
        }
        case (EEnemy::TankTracks):
        {
            return "Art/Tanks/PNG/Tracks/Track_1_A.png";
        }
		default:
		{
			return "Art/Astroid.png";
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------

const std::vector<GameObject *> & EnemyAIManager::GetEnemies() const
{
	return mEnemyObjects;
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::OnDeath(GameObject * pEnemy)
{
    auto & gameManager = GetGameManager();
    // Explosion
    if (!pEnemy->GetComponent<ExplosionComponent>().lock())
    {
        auto explosionComp = std::make_shared<ExplosionComponent>(
            pEnemy, "Art/explosion.png", 32, 32, 7, 0.1f, sf::Vector2f(2.f, 2.f), pEnemy->GetPosition());
        pEnemy->AddComponent(explosionComp);
    }
    // Add Score
    {
        auto pScoreManager = gameManager.GetManager<ScoreManager>();
        pScoreManager->AddScore(1000);
    }
    // Drops
    {
        EDropType dropType = DetermineDropType();
        auto pDropManager = gameManager.GetManager<DropManager>();
        if (pDropManager)
        {
            sf::Vector2f position = pEnemy->GetPosition();
            pDropManager->SpawnDrop(dropType, position);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

EDropType EnemyAIManager::DetermineDropType() const
{
    int randomValue = rand() % 100;

    if (randomValue < 3)
    {
        return EDropType::NukePickup;
    }
    else if (randomValue < 7)
    {
        return EDropType::LifePickup;
    }

    return EDropType::None;
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::SetUpSprite(SpriteComponent & spriteComp, EEnemy type)
{
    std::string file = GetEnemyFile(type);
    ResourceId resourceId(file);
    auto pTexture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resourceId);

    auto scale = sf::Vector2f();
    switch (type)
    {
        case (EEnemy::TankBody):
        {
            scale = sf::Vector2f(.2f, .2f);
            break;
        }
        case (EEnemy::TankGuns):
        {
            scale = sf::Vector2f(.2f, .2f);
            break;
        }
        case (EEnemy::TankTracks):
        {
            scale = sf::Vector2f(.2f, .2f);
            break;
        }
        default :
        {
            scale = sf::Vector2f(1.f, 1.f);
            break;
        }
    }
    spriteComp.SetSprite(pTexture, scale);
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------