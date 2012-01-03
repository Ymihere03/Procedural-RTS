#pragma once
#include "actor.h"
#include "stdafx.h"
class GroundExplosion :
	public Actor
{
public:
	GroundExplosion(Vector3 &position, Vector3 &facing);

	void update(int timeElapsed, double height);
	void draw();
	bool isDead();
	void kill();

	
	
	~GroundExplosion(void);

private:
	struct quadList
	{
		quadList *next;
		Vector3 quadLocation;
		Vector3 quadFacing;
		double downwardVelocity, velocity;
		int totalLife, lifeSpan;
		int id;
	};

	quadList *root;

	int reloadTime, coolDown;
	int nextID;

	void genQuad(quadList *q);

	void addQuad();
	void deleteQuad(int id);
};

