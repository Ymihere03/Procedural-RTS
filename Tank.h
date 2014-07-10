#pragma once
#include "Actor.h"

class Tank :
	public Actor
{
public:
	//Tank(double x, double y, double z, int team);

	void update(int timeElapsed, double height, tile tileData);
	void setMoveTarget(Vector2 &v);
	void turn(int direction, double elapsedSeconds, double targetRadians);
	void turretAngleAdjust(double adjust);
	bool isReadyToShoot();
	void updateOverlay(int timeElapsed);
	void takeDamage(int damage);
	bool shoot();
	void reloadClip();
	void addOverlayData(Vector3 location, Vector3 color, int fontSet, string *text, int lifeTime);
	OverlayData* getOverlay();

	bool checkHitBox(Vector3 &ray1, Vector3 &ray2);
	void draw();
	bool isDead();
	void kill();
	
	//void addNodePath(Vector3 &nextNode);
	void addNodePath(nodePath &nextNode);
	nodePath * getPathRoot();
	void resetNodePath();
	string getTerrainType();
	
	void init(string path);
	~Tank(void);

protected:
	
	GLMmodel *model;
	int direction, reloadTime, cooldown;
	nodePath *pathRoot;
	nodePath3d *tracerRoot;
	OverlayData *overlayRoot;

	int tracerSpacing;
	double turretAngle, turretFacingRadians, turretLength, turretMaxAngle, turretMinAngle;
	Vector3 turretLocation;

	int ammoCount, maxClipSize;

	double uphillPenalty, downhillBonus;
	double damageFlag;
	
	double getTurretAngle();
	Vector3 getTurretLocation();
	void setHitBox();
	void makeTracer();
	void calcMovesSpent(Vector3 oldLocation, Vector3 newLocation);
};

