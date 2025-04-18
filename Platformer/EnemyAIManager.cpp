#include "AstroidsPrivate.h"
#include "EnemyAIManager.h"
#include "GameObject.h"
#include <memory>
#include <cassert>
#include <iostream>
#include "SpriteComponent.h"
#include "CollisionComponent.h"
#include "HealthComponent.h"
#include "ExplosionComponent.h"
#include "DropManager.h"
#include "ResourceManager.h"
#include "AIPathComponent.h"
#include "FollowComponent.h"
#include "TrackingComponent.h"
#include "PlayerManager.h"
#include "EnemyBulletComponent.h"

namespace
{
    int skBossHealth = 100;
    int skMaxBossHealth = 100;
}

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
    for (auto enemyHandle : mEnemyHandles)
    {
        auto * pEnemy = GetGameManager().GetGameObject(enemyHandle);
        if (pEnemy && !pEnemy->IsDestroyed())
        {
            auto pHealthComp = pEnemy->GetComponent<HealthComponent>().lock();
            if (pHealthComp)
            {
                pHealthComp->SetDeathCallBack([this, enemyHandle]() {
                    GameObject * pEnemySafe = GetGameManager().GetGameObject(enemyHandle);
                    if (pEnemySafe)
                    {
                        OnDeath(pEnemySafe);
                    }
                    });
            }
        }
    }
	CleanUpDeadEnemies();

	while (mEnemyHandles.size() < mkMaxEnemies)
	{
		sf::Vector2f spawnPosition = sf::Vector2f(float(1183.723), float(851.008));
		RespawnEnemy(EEnemy::Ogre, spawnPosition);
	}
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::OnGameEnd()
{
    mEnemyHandles.clear();
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
        BD::Handle enemyHandle = gameManager.CreateNewGameObject(ETeam::Enemy, gameManager.GetRootGameObjectHandle());
        auto * pEnemy = gameManager.GetGameObject(enemyHandle);
        mEnemyHandles.push_back(enemyHandle);

        auto pSpriteComp = pEnemy->GetComponent<SpriteComponent>().lock();

        if (!pSpriteComp)
        {
            return;
        }
        // Sprite Comp
        SetUpSprite(*pSpriteComp, type);
        pSpriteComp->SetPosition(pos);

        // AI Path Movement
        auto pAIPathComponentComp = std::make_shared<AIPathComponent>(pEnemy, gameManager);
        pEnemy->AddComponent(pAIPathComponentComp);

        // Health Component
        auto pHealthComponent = std::make_shared<HealthComponent>(pEnemy, gameManager, skBossHealth, skMaxBossHealth, 1, 1);
        pEnemy->AddComponent(pHealthComponent);

        // Physics and Collision
        {
            pEnemy->CreatePhysicsBody(&gameManager.GetPhysicsWorld(), pEnemy->GetSize(), true);
            auto pCollisionComp = std::make_shared<CollisionComponent>(
                pEnemy,
                gameManager,
                &gameManager.GetPhysicsWorld(),
                pEnemy->GetPhysicsBody(),
                pEnemy->GetSize(),
                true
            );
            pEnemy->AddComponent(pCollisionComp);
        }

#if 0
        // Extra Tank GameObject Logic
        {
            // Tank Logic
            BD::Handle gunHandle = gameManager.CreateNewGameObject(ETeam::Enemy, pEnemy->GetHandle());
            auto * pGunGameObject = gameManager.GetGameObject(gunHandle);
            if (!pGunGameObject)
            {
                return;
            }
            auto pGunSpriteComp = pGunGameObject->GetComponent<SpriteComponent>().lock();
            if (pGunSpriteComp)
            {
                SetUpSprite(*pGunSpriteComp, EEnemy::TankGuns);
                pGunSpriteComp->SetPosition(pos);

                auto * pPlayerManager = gameManager.GetManager<PlayerManager>();
                if (!pPlayerManager)
                {
                    return;
                }
                auto & players = pPlayerManager->GetPlayers();
                if (players.empty())
                {
                    return;
                }
                BD::Handle playerHandle = players[0];
                GameObject * pPlayerObject = gameManager.GetGameObject(playerHandle);
                if (!pPlayerObject)
                {
                    return;
                }

                // Setup Components
                {
                    auto pFollowParentComponent = std::make_shared<FollowComponent>(pGunGameObject, gameManager, enemyHandle);
                    pGunGameObject->AddComponent(pFollowParentComponent);
                    auto pTrackingComponent = std::make_shared<TrackingComponent>(pGunGameObject, gameManager, playerHandle);
                    pGunGameObject->AddComponent(pTrackingComponent);
                    auto pEnemyBulletComponent = std::make_shared<EnemyBulletComponent>(pGunGameObject, gameManager);
                    pGunGameObject->AddComponent(pEnemyBulletComponent);
                }
            }
        }
#endif
    }
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::DestroyAllEnemies()
{
    for (auto enemyHandle : mEnemyHandles)
    {
        auto * pEnemy = GetGameManager().GetGameObject(enemyHandle);
        pEnemy->Destroy();
    }
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::CleanUpDeadEnemies()
{
    auto & gameManager = GetGameManager();

    for (BD::Handle enemyHandle : mEnemyHandles)
    {
        GameObject * pEnemy = gameManager.GetGameObject(enemyHandle);
        if (pEnemy && !pEnemy->IsDestroyed())
        {
            auto explosionComp = pEnemy->GetComponent<ExplosionComponent>().lock();
            if (explosionComp && explosionComp->IsAnimationFinished())
            {
                pEnemy->Destroy();
            }
        }
    }

    auto removeStart = std::remove_if(mEnemyHandles.begin(), mEnemyHandles.end(),
        [&gameManager](BD::Handle handle)
        {
            GameObject * pObj = gameManager.GetGameObject(handle);
            return pObj == nullptr || pObj->IsDestroyed();
        });

    mEnemyHandles.erase(removeStart, mEnemyHandles.end());
}

//------------------------------------------------------------------------------------------------------------------------

std::string EnemyAIManager::GetEnemyFile(EEnemy type)
{
	switch (type)
	{
        case (EEnemy::LizardF):
        {
            return "Art/Enemies/LizardF/lizard_f_idle_anim_f0.png";
        }
        case (EEnemy::Ogre):
        {
            return "Art/Enemies/Ogre/ogre_idle_anim_f0.png";
        }
		default:
		{
			return "Art/Enemies/Ogre/ogre_idle_anim_f0.png";
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------

const std::vector<BD::Handle> & EnemyAIManager::GetEnemies() const
{
	return mEnemyHandles;
}

//------------------------------------------------------------------------------------------------------------------------

void EnemyAIManager::OnDeath(GameObject * pEnemy)
{
    auto & gameManager = GetGameManager();
    // Explosion
    if (!pEnemy->GetComponent<ExplosionComponent>().lock())
    {
        auto explosionComp = std::make_shared<ExplosionComponent>(
            pEnemy, gameManager, "Art/explosion.png", 32, 32, 7, 0.1f, sf::Vector2f(2.f, 2.f), pEnemy->GetPosition());
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
        case (EEnemy::LizardF):
        {
            scale = sf::Vector2f(.2f, .2f);
            break;
        }
        case (EEnemy::Ogre):
        {
            scale = sf::Vector2f(1.2f, 1.2f);
            break;
        }
        default :
        {
            scale = sf::Vector2f(.2f, .2f);
            break;
        }
    }
    spriteComp.SetSprite(pTexture, scale);
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------