#pragma once
#include "GameComponent.h"
class TrackingComponent : public GameComponent
{
public:
	TrackingComponent(GameObject * pOwner, GameObject * pTrackedObject);
	~TrackingComponent();

	virtual void Update(float deltaTime) override;

private:

};

