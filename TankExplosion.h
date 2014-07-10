#pragma once
#include "actor.h"
#include "stdafx.h"
class TankExplosion :
	public Actor
{
public:
	TankExplosion(Vector3 &position, Vector3 &facing, int team);

	void update(int timeElapsed, double height, tile tileData);
	void draw();
	bool isDead();
	void kill();
	~TankExplosion(void);

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
	int group, team;

	void genQuad(int angle, quadList *q);

	void addQuad(int angle);
	void deleteQuad(int id);
};

