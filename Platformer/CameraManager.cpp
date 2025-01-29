#include "CameraManager.h"
#include "GameObject.h"

CameraManager::CameraManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
	, mpPlayer(nullptr)
{

}

//------------------------------------------------------------------------------------------------------------------------

void CameraManager::Update(float deltaTime)
{
	if (!mpPlayer)
	{
		return;
	}

	sf::Vector2f targetPos = mpPlayer->GetPosition();

	mView.setCenter(Lerp(mView.getCenter(), targetPos, .1f));
	if (mpGameManager)
	{
		if (mpGameManager->mpWindow)
		{
			mpGameManager->mpWindow->setView(mView);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------

void CameraManager::OnGameEnd()
{
}

//------------------------------------------------------------------------------------------------------------------------

void CameraManager::SetTarget(GameObject * pPlayer)
{
	mpPlayer = pPlayer;
}

//------------------------------------------------------------------------------------------------------------------------

sf::Vector2f CameraManager::Lerp(sf::Vector2f start, sf::Vector2f end, float t)
{
	return start + (end - start) * t;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------