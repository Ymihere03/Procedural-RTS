#pragma once
#include "actor.h"
#include "stdafx.h"
class Bullet :
	public Actor
{
public:
	bool dead;

	Bullet(Vector3 &position, Vector3 &facing);

	void update(int timeElapsed, double height);
	void draw();
	bool isDead();
	void kill();
	
	
	~Bullet(void);

private:
	double gravity, downwardVelocity;

	void setLifeTime(double lifeTime);
};

