#pragma once
#include "GameComponent.h"

class BulletComponent : public GameComponent
{
public:
    BulletComponent(GameObject * pOwner, float speed, float lifetime);

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

    void SetVelocity(sf::Vector2f dir);

    void Deactivate();
    void Activate();

protected:
    sf::Vector2f mVelocity;
    float mSpeed;
    float mLifetime;
    std::string mName;
};

