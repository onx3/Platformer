#pragma once
#include "GameComponent.h"
class AIPathComponent : public GameComponent
{
public:
	AIPathComponent(GameObject * pGameObject);
	~AIPathComponent();

	virtual void Update(float deltaTime) override;
	virtual void DebugImGuiComponentInfo() override;
	virtual std::string & GetClassName() override;

private:
	std::string mName;
};

