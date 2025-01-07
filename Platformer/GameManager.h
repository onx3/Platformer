#pragma once
#include "AstroidsPrivate.h"
#include <iostream>
#include <vector>
#include "box2d/box2d.h"
#include "EnemyAIManager.h"
#include "GameObject.h"
#include "ScoreManager.h"
#include "BaseManager.h"
#include "WindowManager.h"
#include "CollisionListener.h"
#include "TileEditor.h"

class TileEditor;

struct ParallaxLayer
{
	std::vector<sf::Sprite> mSprites;
	float parallaxSpeed;
};

//------------------------------------------------------------------------------------------------------------------------

class GameManager
{
public:
	GameManager(WindowManager & windowManager);
	~GameManager();

	void EndGame();

	void Update(float deltaTime);
	void UpdateGameObjects(float deltaTime);

	void Render(float deltaTime);

	template <typename T>
	void AddManager();

	template <typename T>
	T * GetManager()
	{
		auto it = mManagers.find(typeid(T));
		if (it != mManagers.end())
		{
			return dynamic_cast<T *>(it->second);
		}
		return nullptr;
	}

	GameObject * CreateNewGameObject(ETeam team, GameObject * pParent);

	GameObject * GetRootGameObject();

	bool IsGameOver() const;

	b2World & GetPhysicsWorld();

	// Window
	WindowManager & mWindowManager;
	sf::RenderWindow * mpWindow;
	sf::Event mEvent;

private:
	void CleanUpDestroyedGameObjects(GameObject * pRoot);

	void RenderImGui();

	void InitWindow();

	void InitImGui();
	
	void GameOver();

	std::vector<std::string> GetCommonResourcePaths();

	void InitializeParallaxLayers();  // Helper to initialize layers
	void UpdateParallaxLayers(float deltaTime, float playerSpeedX);
	void RenderParallaxLayers();

	bool mShowImGuiWindow;
	std::unordered_map<std::type_index, BaseManager *> mManagers;
	sf::Sprite mCursorSprite;
	GameObject * mpRootGameObject;

	// Audio
	sf::SoundBuffer mSoundBuffer;
	sf::Sound mSound;
	bool mSoundPlayed;

	// GameOver
	bool mIsGameOver;
	sf::Text mGameOverText;
	sf::Text mScoreText;
	sf::Font mFont;

	// Box2d
	b2World mPhysicsWorld;
	CollisionListener mCollisionListener;

	std::vector<ParallaxLayer> mParallaxLayers;

	// TileEditor
	TileEditor * mpTileEditor;
};