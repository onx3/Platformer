#pragma once

class GameManager;

class BaseManager
{
public:
	explicit BaseManager(GameManager * pGameManager);
	~BaseManager();

	virtual void Update(float deltaTime);
	virtual void Render(sf::RenderWindow & window);

	virtual void OnGameEnd();

protected:
	GameManager * mpGameManager;
};

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------