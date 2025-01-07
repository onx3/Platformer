#include "AstroidsPrivate.h"
#include <cassert>
#include <imgui.h>
#include <stack>
#include <imgui-SFML.h>
#include "SpriteComponent.h"
#include "ControlledMovementComponent.h"
#include "ProjectileComponent.h"
#include "BDConfig.h"
#include "CollisionComponent.h"
#include "HealthComponent.h"
#include "PlayerManager.h"
#include "DropManager.h"
#include "ResourceManager.h"
#include "TileEditor.h"

GameManager::GameManager(WindowManager & windowManager)
    : mWindowManager(windowManager)
    , mpWindow(windowManager.GetWindow())
    , mEvent(windowManager.GetEvent())
    , mShowImGuiWindow(false)
    , mpRootGameObject(nullptr)
    , mManagers()
    , mCursorSprite()
    , mSoundPlayed(false)
    , mIsGameOver(false)
    , mPhysicsWorld(b2Vec2(0.0f, 0.f))
    , mCollisionListener(this)
    , mpTileEditor(nullptr)
{
    // Keep this first
    {
        AddManager<ResourceManager>();
    }

    InitWindow();
    InitImGui();
    mpRootGameObject = new GameObject(this, ETeam::Neutral);

    AddManager<PlayerManager>();
    AddManager<EnemyAIManager>();
    AddManager<ScoreManager>();
    AddManager<DropManager>();

    InitializeParallaxLayers();


    // Game Audio
    /*{
        assert(mSoundBuffer.loadFromFile("Audio/ThroughSpace.ogg"));
        mSound.setBuffer(mSoundBuffer);
        mSound.setVolume(25.f);
        mSound.setLoop(true);
    }*/

    // End Game
    {
        assert(mFont.loadFromFile("Art/font.ttf"));

        mGameOverText.setFont(mFont);
        mGameOverText.setString("GAME OVER");
        mGameOverText.setCharacterSize(64);
        mGameOverText.setFillColor(sf::Color::Green);
        mGameOverText.setPosition(700, 400);

        // Setup Score text
        mScoreText.setFont(mFont);
        mScoreText.setCharacterSize(32);
        mScoreText.setFillColor(sf::Color::Green);
        mScoreText.setPosition(700, 500);
    }

    // Box2d
    {
        mPhysicsWorld.SetContactListener(&mCollisionListener);
        mPhysicsWorld.Step(0.f, 0, 0);
    }

    // ResourceManager
    {
        auto * pResourceManager = GetManager<ResourceManager>();
        auto resourcesToLoad = GetCommonResourcePaths();
        pResourceManager->PreloadResources(resourcesToLoad);
    }

    // TileEditorManager
    {
        mpTileEditor = new TileEditor(20, 15, 32);
        mpTileEditor->Initalize();
    }
}

//------------------------------------------------------------------------------------------------------------------------

GameManager::~GameManager()
{
    // Clean up managers
    for (auto & manager : mManagers)
    {
        if (manager.second)
        {
            delete manager.second;
            manager.second = nullptr;
        }
    }
    mManagers.clear();

    if (mpTileEditor)
    {
        delete mpTileEditor;
        mpTileEditor = nullptr;
    }

    ImGui::SFML::Shutdown();
    if (ImGui::GetCurrentContext() != nullptr)
    {
        ImGui::DestroyContext();
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::EndGame()
{
    mIsGameOver = true;

    // Notify all managers that the game is ending
    for (auto & manager : mManagers)
    {
        if (manager.second)
        {
            manager.second->OnGameEnd();
        }
    }

    // Cleanup all game objects starting from the root
    if (mpRootGameObject)
    {
        CleanUpDestroyedGameObjects(mpRootGameObject);
        delete mpRootGameObject;
        mpRootGameObject = nullptr;
    }

    mPhysicsWorld.ClearForces();
    GameOver();
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::Update(float deltaTime)
{
    // Game Audio
    if (!mSoundPlayed)
    {
        mSound.play();
        mSoundPlayed = true;
    }

    // Physics
    {
        float timeStep = 1.0f / 60.0f; // 60 Hz
        int velocityIterations = 8;
        int positionIterations = 3;
        mPhysicsWorld.Step(timeStep, velocityIterations, positionIterations);
    }

    UpdateParallaxLayers(deltaTime, 100.f);
    UpdateGameObjects(deltaTime);

    for (auto & manager : mManagers)
    {
        if (manager.second)
        {
            manager.second->Update(deltaTime);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::UpdateGameObjects(float deltaTime)
{
    if (mpRootGameObject)
    {
        mpRootGameObject->Update(deltaTime);
        CleanUpDestroyedGameObjects(mpRootGameObject);
        if (!mpRootGameObject)
        {
            EndGame();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::CleanUpDestroyedGameObjects(GameObject * pRoot)
{
    if (!pRoot)
        return;

    auto & children = pRoot->GetChildren();
    std::vector<GameObject *> objectsToDelete;

    for (int i = static_cast<int>(children.size()) - 1; i >= 0; --i)
    {
        GameObject * pChild = children[i];

        CleanUpDestroyedGameObjects(pChild);

        // Collect destroyed objects
        if (pChild && pChild->IsDestroyed())
        {
            objectsToDelete.push_back(pChild);
        }
    }

    // Notify and delete in bulk
    for (GameObject * pObject : objectsToDelete)
    {
        pObject->NotifyParentOfDeletion(); // Notify parent
        delete pObject;                    // Delete object
    }
}


//------------------------------------------------------------------------------------------------------------------------

void GameManager::RenderImGui()
{
#if IMGUI_ENABLED()
    static GameObject * pSelectedGameObject = nullptr;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
    {
        mShowImGuiWindow = true;
    }

    if (mShowImGuiWindow && mpRootGameObject)
    {
        ImGui::Begin("Game Objects", &mShowImGuiWindow, ImGuiWindowFlags_NoCollapse);

        ImGui::Columns(2, "GameObjectsColumns", true);

        // LEFT SIDE: GameObject Tree
        ImGui::Text("GameObject Tree");
        ImGui::Separator();

        std::stack<std::pair<GameObject *, int>> stack; // GameObject* + Depth
        stack.push({ mpRootGameObject, 0 });

        while (!stack.empty())
        {
            auto [pGameObject, depth] = stack.top();
            stack.pop();

            if (!pGameObject || pGameObject->IsDestroyed())
                continue;

            ImGui::Indent(depth * 10.0f);

            std::string label = "GameObject " + std::to_string(reinterpret_cast<std::uintptr_t>(pGameObject));
            if (ImGui::Selectable(label.c_str(), pSelectedGameObject == pGameObject))
            {
                pSelectedGameObject = pGameObject;
            }

            for (auto * child : pGameObject->GetChildren())
            {
                stack.push({ child, depth + 1 });
            }

            ImGui::Unindent(depth * 10.0f);
        }

        ImGui::NextColumn();

        // RIGHT SIDE: Components of the Selected GameObject
        ImGui::Text("Components");
        ImGui::Separator();

        if (pSelectedGameObject && !pSelectedGameObject->IsDestroyed())
        {
            ImGui::Text("GameObject %p", pSelectedGameObject);

            for (auto * pComponent : pSelectedGameObject->GetAllComponents())
            {
                std::string componentLabel = "" + pComponent->GetClassName();

                if (ImGui::CollapsingHeader(componentLabel.c_str()))
                {
                    pComponent->DebugImGuiComponentInfo();
                }
            }
        }
        else
        {
            ImGui::Text("No GameObject selected or it has been deleted.");
            pSelectedGameObject = nullptr;
        }

        ImGui::Columns(1);

        ImGui::End();
    }
#endif
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::Render(float deltaTime)
{
    mpWindow->clear();

    RenderParallaxLayers();

    if (mIsGameOver)
    {
        mpWindow->draw(mGameOverText);
        mpWindow->draw(mScoreText);
    }
    else
    {
        mpWindow->setMouseCursorVisible(mShowImGuiWindow);

        if (mpRootGameObject)
        {
            mpWindow->draw(*mpRootGameObject);
        }

        auto * pScoreManager = GetManager<ScoreManager>();
        if (pScoreManager)
        {
            mpWindow->draw(pScoreManager->GetScoreText());
            for (auto & life : pScoreManager->GetSpriteLives())
            {
                mpWindow->draw(life);
            }
        }

        sf::Vector2i mousePosition = sf::Mouse::getPosition(*mpWindow);
        mCursorSprite.setPosition(float(mousePosition.x), float(mousePosition.y));
        mpWindow->draw(mCursorSprite);
    }

    // Start ImGui frame
    int imGuiTime = std::max(deltaTime, 0.0001f);
    ImGui::SFML::Update(*mpWindow, sf::milliseconds(1));

    // Render ImGui windows
    if (mpTileEditor)
    {
        mpTileEditor->RenderEditor();
    }
    RenderImGui(); // Render additional ImGui windows if needed

    // Render ImGui draw data
    ImGui::SFML::Render(*mpWindow);

    mpWindow->display();
}

//------------------------------------------------------------------------------------------------------------------------

template <typename T>
void GameManager::AddManager()
{
    static_assert(std::is_base_of<BaseManager, T>::value, "T must inherit from BaseManager");
    if (mManagers.find(typeid(T)) == mManagers.end())
    {
        mManagers[typeid(T)] = new T(this);
    }
}

//------------------------------------------------------------------------------------------------------------------------

GameObject * GameManager::CreateNewGameObject(ETeam team, GameObject * pParent)
{
    GameObject * pGameObj = new GameObject(this, team, pParent);
    return pGameObj;
}

//------------------------------------------------------------------------------------------------------------------------

GameObject * GameManager::GetRootGameObject()
{
    return mpRootGameObject;
}

//------------------------------------------------------------------------------------------------------------------------

bool GameManager::IsGameOver() const
{
    return mIsGameOver;
}

//------------------------------------------------------------------------------------------------------------------------

b2World & GameManager::GetPhysicsWorld()
{
    return mPhysicsWorld;
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::InitWindow()
{
    ResourceId resourceId("Art/Crosshair.png");
    auto pTexture = GetManager<ResourceManager>()->GetTexture(resourceId);
    mCursorSprite.setTexture(*pTexture);
    mCursorSprite.setScale(.25f, .25f);

    sf::FloatRect localBounds = mCursorSprite.getLocalBounds();
    mCursorSprite.setOrigin(
        localBounds.width / 2.0f,
        localBounds.height / 2.0f
    );

    mpWindow->setMouseCursorVisible(false);
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::InitImGui()
{
    if (ImGui::GetCurrentContext() == nullptr)
    {
        ImGui::CreateContext();
    }

    ImGuiIO & io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    ImGui::SFML::Init(*mpWindow);
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::GameOver()
{
    mIsGameOver = true;

    auto * pScoreManager = GetManager<ScoreManager>();
    if (pScoreManager)
    {
        mScoreText.setString("Score: " + std::to_string(pScoreManager->GetScore()) + "\n" + "Press ENTER to Play Again!");
    }
}

//------------------------------------------------------------------------------------------------------------------------

std::vector<std::string> GameManager::GetCommonResourcePaths()
{
    return {
        "Art/Astroid.png",
        "Art/Nuke.png",
        "Art/life.png",
        "Art/laserGreen.png",
        "Art/laserRed.png",
        "Art/player.png",
        "Art/playerLeft.png",
        "Art/playerRight.png",
        "Art/playerDamaged.png",
        "Art/Explosion.png",
        "Art/Crosshair.png",
        "Art/Background/backgroundFar.png",
        "Art/Background/backgroundReallyFar.png"
    };
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::InitializeParallaxLayers()
{
    auto * pResourceManager = GetManager<ResourceManager>();
    if (!pResourceManager)
    {
        return;
    }

    std::vector<std::pair<std::string, float>> layerConfigs = {
        {"Art/Background/backgroundFar.png", 0.7f},
        {"Art/Background/backgroundReallyFar.png", .9f},
    };

    for (const auto & [texturePath, parallaxSpeed] : layerConfigs)
    {
        ResourceId resource(texturePath);
        auto pTexture = pResourceManager->GetTexture(resource);
        if (!pTexture)
        {
            throw std::runtime_error("Failed to load background layer: " + texturePath);
        }

        // Create three sprites per layer for seamless scrolling
        ParallaxLayer layer;
        layer.parallaxSpeed = parallaxSpeed;

        for (int i = 0; i < 3; ++i)
        {
            sf::Sprite sprite(*pTexture);
            sprite.setScale(
                float(mpWindow->getSize().x) / sprite.getTexture()->getSize().x,
                float(mpWindow->getSize().y) / sprite.getTexture()->getSize().y
            );
            sprite.setPosition(i * sprite.getGlobalBounds().width, 0);
            layer.mSprites.push_back(sprite);
        }

        mParallaxLayers.push_back(layer);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::UpdateParallaxLayers(float deltaTime, float playerSpeedX)
{
    for (auto & layer : mParallaxLayers)
    {
        float offset = -playerSpeedX * layer.parallaxSpeed * deltaTime;

        for (auto & sprite : layer.mSprites)
        {
            sprite.move(offset, 0.0f);

            // If the sprite goes off-screen to the left, reposition it to the right
            if (sprite.getPosition().x + sprite.getGlobalBounds().width < 0)
            {
                float maxRight = 0;
                for (const auto & s : layer.mSprites)
                {
                    maxRight = std::max(maxRight, s.getPosition().x);
                }
                sprite.setPosition(maxRight + sprite.getGlobalBounds().width, sprite.getPosition().y);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------

void GameManager::RenderParallaxLayers()
{
    for (const auto & layer : mParallaxLayers)
    {
        for (const auto & sprite : layer.mSprites)
        {
            mpWindow->draw(sprite);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------