#pragma once
#include "GameComponent.h"
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

enum class EProjectile
{
    RedLaser,
    GreenLaser
};

class ProjectileComponent : public GameComponent
{
public:
    ProjectileComponent(GameObject * pOwner, GameManager & gameManager);
    ~ProjectileComponent();

    std::string GetCorrectProjectileFile();

    void Shoot();

    virtual void Update(float deltaTime) override;
    virtual void draw(sf::RenderTarget & target, sf::RenderStates states) override;
    virtual void DebugImGuiComponentInfo() override;
    virtual std::string & GetClassName() override;

private:
    struct Projectile
    {
        BD::Handle handle;
        float lifespan;
        int damage;
        sf::Vector2f direction;
    };

    void UpdateProjectiles(float deltaTime);

    std::vector<Projectile> mProjectiles;
    float mSpeed;
    float mCooldown;
    float mTimeSinceLastShot;
    EProjectile mLastUsedProjectile;
    std::string mName;
};
