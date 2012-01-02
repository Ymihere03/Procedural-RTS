#pragma once
#include "Actor.h"

class Tank :
	public Actor
{
public:
	Tank(double x, double y, double z, double xSize, double ySize, double zSize);

	void update(int timeElapsed, double height);
	void setMoveTarget(Vector2 &v);
	void turn(int direction);
	bool isReadyToShoot();
	
	bool checkHitBox(Vector3 &ray1, Vector3 &ray2);
	void draw();
	bool isDead();
	void kill();
	
	//void addNodePath(Vector3 &nextNode);
	void addNodePath(nodePath &nextNode);
	nodePath * getPathRoot();
	void resetNodePath();
	
	void init(string path);
	~Tank(void);

private:
	
	GLMmodel *model;
	int direction, reloadTime, cooldown;
	nodePath *pathRoot;
	

	void setHitBox();
	void setLifeTime(double lifeTime);
};

