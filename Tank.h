#pragma once
#include "Actor.h"

class Tank :
	public Actor
{
public:
	Tank(double x, double y, double z);

	void update(int timeElapsed, double height);
	void setMoveTarget(Vector2 &v);
	void turn(int direction, double elapsedSeconds, double targetRadians);
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
	nodePath3d *tracerRoot;

	int tracerSpacing;
	double turretAngle, turretFacingRadians, turretLength;
	Vector3 turretLocation;

	double uphillPenalty, downhillBonus;
	
	double getTurretAngle();
	Vector3 getTurretLocation();
	void setHitBox();
	void makeTracer();
	void calcMovesSpent(Vector3 oldLocation, Vector3 newLocation);
};

