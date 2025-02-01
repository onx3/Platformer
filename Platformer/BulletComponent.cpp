#include "AstroidsPrivate.h"
#include "BulletComponent.h"
#include "CollisionComponent.h"

BulletComponent::BulletComponent(GameObject * pOwner, float speed, float lifetime)
	: GameComponent(pOwner)
	, mSpeed(speed)
	, mLifetime(lifetime)
	, mName("Bulletcompnent")
{

}

//------------------------------------------------------------------------------------------------------------------------

void BulletComponent::Update(float deltaTime)
{
    if (!mpOwner->IsActive()) return;

    mLifetime -= deltaTime;
    if (mLifetime <= 0)
    {
        mpOwner->Deactivate();
        return;
    }

    auto pCollisionComp = mpOwner->GetComponent<CollisionComponent>().lock();
    if (pCollisionComp)
    {
        auto newPos = mpOwner->GetPosition() + (mVelocity * mSpeed * deltaTime);
        mpOwner->SetPosition(newPos);
    }
}

//------------------------------------------------------------------------------------------------------------------------

void BulletComponent::draw(sf::RenderTarget & target, sf::RenderStates states)
{
	
}

//------------------------------------------------------------------------------------------------------------------------

void BulletComponent::DebugImGuiComponentInfo()
{

}

//------------------------------------------------------------------------------------------------------------------------

std::string & BulletComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------

void BulletComponent::SetVelocity(sf::Vector2f dir)
{
	mVelocity = dir;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------