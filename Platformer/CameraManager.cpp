#include "AstroidsPrivate.h"
#include "CameraManager.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "PlayerManager.h"

CameraManager::CameraManager(GameManager * pGameManager)
	: BaseManager(pGameManager)
	, mCursorSprite()
	, mPreviousViewCenter()
{
	ResourceId resourceId("Art/Crosshair.png");
	auto pTexture = GetGameManager().GetManager<ResourceManager>()->GetTexture(resourceId);
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
	auto pPlayerManager = GetGameManager().GetManager<PlayerManager>();
	auto & players = pPlayerManager->GetPlayers();
	if (players.empty())
	{
		return;
	}

	auto * pPlayer = players[0];
	if (!pPlayer)
	{
		return;
	}

	sf::Vector2f previousCenter = mView.getCenter();
	sf::Vector2f targetPos = pPlayer->GetPosition();
	mView.setCenter(Lerp(mView.getCenter(), targetPos, 0.1f));

	sf::Vector2f cameraDelta = mView.getCenter() - previousCenter;

	auto & gameManager = GetGameManager();
	gameManager.GetWindow().setView(mView);

	sf::Vector2f cursorWorldPos = gameManager.GetWindow().mapPixelToCoords(
		sf::Mouse::getPosition(gameManager.GetWindow()), mView
	);

	mCursorSprite.setPosition(cursorWorldPos);
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