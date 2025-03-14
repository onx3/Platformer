#include "AstroidsPrivate.h"
#include "TrackingComponent.h"

TrackingComponent::TrackingComponent(GameObject * pOwner, GameObject * pTrackedObject)
	: GameComponent(pOwner)
{

}

//------------------------------------------------------------------------------------------------------------------------

TrackingComponent::~TrackingComponent()
{
}

//------------------------------------------------------------------------------------------------------------------------

void TrackingComponent::Update(float deltaTime)
{

}
