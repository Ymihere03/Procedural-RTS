#pragma once
#include "actor.h"
#include "stdafx.h"
class GroundExplosion :
	public Actor
{
public:
	GroundExplosion(Vector3 &position, Vector3 &facing, tile tileData);
	void updateOverlay(int timeElapsed);

	void update(int timeElapsed, double height, tile tileData);
	void draw();
	bool isDead();
	void kill();

	
	
	~GroundExplosion(void);

private:
	struct quadList
	{
		quadList *next;
		Vector3 quadLocation,
			quadDirection;
		Vector3 color;
		double velocity;
		int totalLife, lifeSpan;
		int id;
	};

	quadList *root;

	int reloadTime, coolDown;
	int nextID;
	int team;

	void genQuad(quadList *q);

	void addQuad();
	void deleteQuad(int id);
};

