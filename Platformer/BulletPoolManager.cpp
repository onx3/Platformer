#include "AstroidsPrivate.h"
#include "BulletPoolManager.h"
#include "BulletComponent.h"
#include "CollisionComponent.h"

BulletPoolManager::BulletPoolManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
{
}

//------------------------------------------------------------------------------------------------------------------------

GameObject * BulletPoolManager::GetBullet()
{
	for (auto * pBullet : mBullets)
	{
		if (pBullet && !pBullet->IsActive())
		{
			pBullet->Activate();
			return pBullet;
		}
	}
	
	// Create a new Bullet
	{
		auto & gameManager = GetGameManager();
		GameObject * pNewBullet = gameManager.CreateNewGameObject(ETeam::Enemy, gameManager.GetRootGameObject());

		// Bullet Component
		{
			pNewBullet->AddComponent(std::make_shared<BulletComponent>(pNewBullet, 700.f, 5.f));
		}
		// Collision Component
		{
			auto pCollisionComponent = pNewBullet->GetComponent<CollisionComponent>().lock();
			if (!pCollisionComponent)
			{
				pNewBullet->CreatePhysicsBody(&gameManager.GetPhysicsWorld(), pNewBullet->GetSize(), true);
				pNewBullet->AddComponent(std::make_shared<CollisionComponent>(
					pNewBullet,
					&gameManager.GetPhysicsWorld(),
					pNewBullet->GetPhysicsBody(),
					pNewBullet->GetSize(),
					true
				));
			}
		}
		mBullets.push_back(pNewBullet);
		return pNewBullet;
	}
}

//------------------------------------------------------------------------------------------------------------------------

void BulletPoolManager::ReturnBullet(GameObject * pBullet)
{
	pBullet->Deactivate();
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------