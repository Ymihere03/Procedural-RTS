#pragma once
#include "Actor.h"
#include "stdafx.h"
#include "TerrainGen.h"
#include "Tank.h"
#include "Bullet.h"
#include "GroundExplosion.h"
#include "Camera.h"

class ActorManager
{
public:
	TerrainGen * world;

	ActorManager(TerrainGen *w);
	void addObject();
	void removeObject(int index);
	void updateObjects(int timeElapsed);
	void drawObjects();
	void setMovePath(int selectedID, Vector3 &targetLocation);
	bool checkHitBoxes(int &id, Vector3 &ray1, Vector3 &ray2);
	Actor * getActorByID(int id);
	void shoot(int id);

	//double ** getWorldCamTrack();
	//double getWorldTerrainHeight(int x, int z);
	//double getSpecificTerrainHeight(double x, double z);
	//int getWorldTerrainType(int x, int z);
	//nodePath ** getWorldNodePath();
	
	~ActorManager(void);

private:
	int getNextID();

	//Linked List functions
	void addToList(Actor * actor);
	void deleteFromList(int id);
};

