#pragma once
#include "AstroidsPrivate.h"
#include "BaseManager.h"

class CameraManager : public BaseManager
{
public:
	CameraManager(GameManager * pGameManager);

	void Update(float deltaTime) override;
	void OnGameEnd() override;

	void SetTarget(GameObject * pPlayer);

private:

	sf::Vector2f Lerp(sf::Vector2f start, sf::Vector2f end, float t);

	GameObject * mpPlayer;
	sf::View mView;	
};

