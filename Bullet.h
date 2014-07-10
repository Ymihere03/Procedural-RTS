#pragma once
#include "actor.h"
#include "stdafx.h"
class Bullet :
	public Actor
{
public:
	bool dead;

	Bullet(Vector3 &position, Vector3 &facing);
	void updateOverlay(int timeElapsed);

	void update(int timeElapsed, double height, tile tileData);
	void draw();
	bool isDead();
	void kill();

	double getExplosionRadius();
	
	~Bullet(void);

private:
	Vector3 direction, initialLocation, initialFacing;
};

