#pragma once
#include "Actor.h"
#include "stdafx.h"
#include "TerrainGen.h"
#include "Tank.h"
#include "Bullet.h"

class ActorManager
{
public:
	

	ActorManager();
	ActorManager(TerrainGen &w);
	void addObject();
	void removeObject(int index);
	void updateObjects(int timeElapsed);
	void drawObjects();
	void setMovePath(int selectedID, Vector3 &targetLocation);
	bool checkHitBoxes(int &id, Vector3 &ray1, Vector3 &ray2);
	Actor * getActorByID(int id);
	void shoot(int id);

	

	~ActorManager(void);

private:
	int getNextID();

	//Linked List functions
	void addToList(Actor * actor);
	void deleteFromList(int id);
};

