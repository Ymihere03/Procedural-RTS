#pragma once
#include "Actor.h"
#include "stdafx.h"
#include "TerrainManager.h"
#include "TerrainGen.h"
#include "Tank.h"
#include "BaseTank.h"
#include "Scout.h"
#include "Artillery.h"
#include "Bullet.h"
#include "GroundExplosion.h"
#include "TankExplosion.h"

class ActorManager
{
public:
	struct ActorList
	{
		ActorList *next;
		Actor * actor;
		int id;
	};

	ActorList *root;

	TerrainManager *tM;
	//TerrainGen * world;

	ActorManager(TerrainManager *t);
	void addObject();
	void removeObject(int index);
	void updateObjects(int timeElapsed);
	void drawObjects();
	
	bool checkHitBoxes(int &id, Vector3 &ray1, Vector3 &ray2);
	void updateActorLOS(int activeTeam);
	Actor * getActorByID(int id);
	void shoot(int id);

	//double ** getWorldCamTrack();
	//double getWorldTerrainHeight(int x, int z);
	//double getSpecificTerrainHeight(double x, double z);
	//int getWorldTerrainType(int x, int z);
	//nodePath ** getWorldNodePath();
	
	~ActorManager(void);

private:
	int nextID;
	void checkBlastProximity(int directHitId, Vector3 origin, double blastSize);
	int getNextID();

	//Linked List functions
	void addToList(Actor * actor);
	void deleteFromList(int id);
};

