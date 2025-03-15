#include "AstroidsPrivate.h"
#include "TrackingComponent.h"

TrackingComponent::TrackingComponent(GameObject * pOwner, GameManager & gameManager, BD::Handle trackedHandle)
	: GameComponent(pOwner, gameManager)
	, mTracker(trackedHandle)
    , mName("TrackingComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

TrackingComponent::~TrackingComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void TrackingComponent::Update(float deltaTime)
{
    GameManager & gameManager = GetGameManager();

    // Get the owner and tracked object
    GameObject * pOwner = gameManager.GetGameObject(mOwnerHandle);
    GameObject * pTrackedGameObject = gameManager.GetGameObject(mTracker);

    if (!pOwner || !pTrackedGameObject)
    {
        return;
    }

    // Get positions
    sf::Vector2f ownerPosition = pOwner->GetPosition();
    sf::Vector2f trackedPosition = pTrackedGameObject->GetPosition();

    // Calculate direction vector
    sf::Vector2f direction = trackedPosition - ownerPosition;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0)
    {
        direction /= length; // Normalize direction
    }

    // Calculate angle in degrees
    float angleDegrees = std::atan2(direction.y, direction.x) * (180.f / 3.14159265f);

    // Apply rotation (adjusted to match sprite alignment)
    pOwner->SetRotation(angleDegrees + 90.f);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & TrackingComponent::GetClassName()
{
    return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------