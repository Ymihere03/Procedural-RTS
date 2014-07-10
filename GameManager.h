#pragma once

#include "stdafx.h"
#include "ActorManager.h"
#include "TerrainManager.h"

class GameManager
{
public:
	ActorManager *aM;
	TerrainManager *tM;

	GameManager(ActorManager *a, TerrainManager *t);
	void update();
	bool isSelectable(Actor *a);
	void toggleActiveTeam(bool teamToggle);
	int getActiveTeam();
	void startTurn();
	~GameManager(void);

private:
	bool teamToggleFlag;
	int activeTeam;
	int maxPlayers;
};

