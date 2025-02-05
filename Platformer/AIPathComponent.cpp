#include "AstroidsPrivate.h"
#include "AIPathComponent.h"
#include "PlayerManager.h"
#include "imgui.h"

AIPathComponent::AIPathComponent(GameObject * pGameObject)
	: GameComponent(pGameObject)
	, mName("AIPathComponent")
{

}

//------------------------------------------------------------------------------------------------------------------------

AIPathComponent::~AIPathComponent()
{

}

//------------------------------------------------------------------------------------------------------------------------

void AIPathComponent::Update(float deltaTime)
{
	auto * pPlayer = GetGameManager().GetManager<PlayerManager>()->GetPlayers()[0];
	if (pPlayer)
	{
		auto playerPos = pPlayer->GetPosition();
	}

	auto myPosition = GetGameObject().GetPosition();


}

//------------------------------------------------------------------------------------------------------------------------

void AIPathComponent::DebugImGuiComponentInfo()
{
	auto gameObjPos = GetGameObject().GetPosition();
	ImGui::Text("Position x,y: %.3f, %.3f", gameObjPos.x, gameObjPos.y);
}

//------------------------------------------------------------------------------------------------------------------------

std::string & AIPathComponent::GetClassName()
{
	return mName;
}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------