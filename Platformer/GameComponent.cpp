#include "AstroidsPrivate.h"
#include "GameComponent.h"
#include "BDConfig.h"

GameComponent::GameComponent(GameObject * pOwner)
    : mpOwner(pOwner)
    , mName("GameComponent")
{
}

//------------------------------------------------------------------------------------------------------------------------

void GameComponent::SetOwner(GameObject * pOwner)
{
    mpOwner = pOwner;
}

//------------------------------------------------------------------------------------------------------------------------

GameObject & GameComponent::GetGameObject() const
{
    assert(mpOwner && "mpOwner is nullptr!");
    return *mpOwner;
}

//------------------------------------------------------------------------------------------------------------------------

GameManager & GameComponent::GetGameManager() const
{
    assert(mpOwner && "mpOwner is nullptr!");
    return mpOwner->GetGameManager();
}

//------------------------------------------------------------------------------------------------------------------------

void GameComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{

}

//------------------------------------------------------------------------------------------------------------------------

void GameComponent::DebugImGuiComponentInfo()
{
#if IMGUI_ENABLED()

#endif
}

//------------------------------------------------------------------------------------------------------------------------

std::string & GameComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------