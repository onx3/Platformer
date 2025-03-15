#pragma once
#include "GameComponent.h"
class FollowParentComponent : public GameComponent
{
public:
    FollowParentComponent(GameObject * pOwner, GameManager & gameManager);
    ~FollowParentComponent();

    virtual void Update(float deltaTime) override;
    virtual std::string & GetClassName() override;
private:

    std::string mName;
};

