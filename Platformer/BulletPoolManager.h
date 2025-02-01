#pragma once
#include "BaseManager.h"

class BulletPoolManager : public BaseManager
{
public:
	BulletPoolManager(GameManager * pGameManager);

	GameObject * GetBullet();
	void ReturnBullet(GameObject * pBullet);

	std::vector<GameObject *> mBullets;

};

