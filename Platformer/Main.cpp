#include "AstroidsPrivate.h"
#include <iostream>
#include <cstdlib>

int main()
{
    WindowManager windowManager;
    bool paused = false;
    sf::Clock clock;

    while (windowManager.GetWindow()->isOpen())
    {
        GameManager * pGameManager = new GameManager(windowManager);

        while (windowManager.GetWindow()->isOpen() && !pGameManager->IsGameOver())
        {
            windowManager.PollEvents();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                paused = !paused;
                pGameManager->SetPausedState(paused);
            }

            float deltaTime = clock.restart().asSeconds();
            if (!paused)
            {
                pGameManager->Update(deltaTime);
            }
            else
            {
                pGameManager->DebugUpdate(deltaTime);
                clock.restart();
            }
            pGameManager->Render(deltaTime);
        }
        delete pGameManager;

        bool waitingForRestart = true;
        while (windowManager.GetWindow()->isOpen() && waitingForRestart)
        {
            windowManager.PollEvents();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                waitingForRestart = false;
            }
        }
    }

    return 0;
}
