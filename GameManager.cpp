#include "GameManager.h"


GameManager::GameManager(ActorManager *a, TerrainManager *t)
{
	aM = a;
	tM = t;
	activeTeam = 0;
	maxPlayers = 2;
	teamToggleFlag = false;
	toggleActiveTeam(true);		//Start first turn
}

void GameManager::update()
{
	toggleActiveTeam(keys['T']);

	aM->updateActorLOS(activeTeam);
}

void GameManager::toggleActiveTeam(bool teamToggle)
{
	if(teamToggle && !teamToggleFlag)
	{
		teamToggleFlag = true;
		activeTeam++;
		if(activeTeam > maxPlayers)
			activeTeam = 1;
		
		clearCurrentSelection();
		startTurn();
	}
	if(!teamToggle)
		teamToggleFlag = false;
}

int GameManager::getActiveTeam()
{
	return activeTeam;
}

void GameManager::startTurn()
{
	//Reload all the tanks for the team that is now active
	ActorManager::ActorList * target = aM->root;
	while(target != NULL)
	{
		Actor *a = target->actor;
		if(a->getTeam() == activeTeam)
		{
			a->reloadClip();
			a->resetMovePoints();
		}

		target = target->next;
	}
}

bool GameManager::isSelectable(Actor *a)
{
	if(a->isVisible() && a->getTeam() == activeTeam)
		return true;
	else
		return false;
}

GameManager::~GameManager(void)
{
}
