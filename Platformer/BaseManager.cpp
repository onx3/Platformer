#include "AstroidsPrivate.h"
#include "BaseManager.h"

BaseManager::BaseManager(GameManager * pGameManager)
	: mpGameManager(pGameManager)
{
	
}

//------------------------------------------------------------------------------------------------------------------------

BaseManager::~BaseManager()
{
	
}

//------------------------------------------------------------------------------------------------------------------------

void BaseManager::Update(float deltaTime)
{

}

//------------------------------------------------------------------------------------------------------------------------

void BaseManager::OnGameEnd()
{

}

//------------------------------------------------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------------------------------------------------