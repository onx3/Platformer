#include "AstroidsPrivate.h"
#include "CameraManager.h"
#include "GameObject.h"
#include "ResourceManager.h"

CameraManager::CameraManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
	, mpPlayer(nullptr)
	, mCursorSprite()
	, mPreviousViewCenter()
{
	ResourceId resourceId("Art/Crosshair.png");
	auto pTexture = mpGameManager->GetManager<ResourceManager>()->GetTexture(resourceId);
	mCursorSprite.setTexture(*pTexture);
	mCursorSprite.setScale(.25f, .25f);

	sf::FloatRect localBounds = mCursorSprite.getLocalBounds();
	mCursorSprite.setOrigin(
		localBounds.width / 2.0f,
		localBounds.height / 2.0f
	);
}

//------------------------------------------------------------------------------------------------------------------------

void CameraManager::Update(float deltaTime)
{
	if (!mpPlayer)
	{
		return;
	}

	sf::Vector2f previousCenter = mView.getCenter();
	sf::Vector2f targetPos = mpPlayer->GetPosition();
	mView.setCenter(Lerp(mView.getCenter(), targetPos, 0.1f));

	sf::Vector2f cameraDelta = mView.getCenter() - previousCenter;

	if (mpGameManager)
	{
		mpGameManager->GetWindow().setView(mView);

		sf::Vector2f cursorWorldPos = mpGameManager->GetWindow().mapPixelToCoords(
			sf::Mouse::getPosition(mpGameManager->GetWindow()), mView
		);

		mCursorSprite.setPosition(cursorWorldPos);
	}
}

//------------------------------------------------------------------------------------------------------------------------

void CameraManager::OnGameEnd()
{
}

//------------------------------------------------------------------------------------------------------------------------

void CameraManager::Render(sf::RenderWindow & window)
{
	window.draw(mCursorSprite);
}

//------------------------------------------------------------------------------------------------------------------------

void CameraManager::SetTarget(GameObject * pPlayer)
{
	mpPlayer = pPlayer;
}

//------------------------------------------------------------------------------------------------------------------------

sf::Vector2f CameraManager::GetCrosshairPosition() const
{
	return mCursorSprite.getPosition();
}

//------------------------------------------------------------------------------------------------------------------------

sf::View & CameraManager::GetView()
{
	return mView;
}

//------------------------------------------------------------------------------------------------------------------------

sf::Vector2f CameraManager::Lerp(sf::Vector2f start, sf::Vector2f end, float t)
{
	return start + (end - start) * t;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------