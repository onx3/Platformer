#include "AstroidsPrivate.h"
#include "FollowParentComponent.h"

FollowParentComponent::FollowParentComponent(GameObject * pOwner)
	: GameComponent(pOwner)
	, mName("FollowParentComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

FollowParentComponent::~FollowParentComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void FollowParentComponent::Update(float deltaTime)
{
	auto * pParent = GetGameObject().GetParent();
	if (pParent)
	{
		GetGameObject().SetPosition(pParent->GetPosition());
	}
}

//------------------------------------------------------------------------------------------------------------------------

std::string & FollowParentComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------