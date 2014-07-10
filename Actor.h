#pragma once
#include "stdafx.h"
#include <math.h>
#include "glm.h"

class Actor
{
public:
	string type;

	//Actor(double x, double y, double z, double xSize, double ySize, double zSize);
	//Actor(char *path);
	//Actor(void);
	//void init(double x, double y, double z, double xSize, double ySize, double zSize, int id);
	
	//void unselect();
	//void setSelected(bool target);
	bool isSelected();

	virtual bool checkHitBox(Vector3 &ray1, Vector3 &ray2)
	{return false;}

	virtual void setMoveTarget(Vector3 &v)
	{return;}
	
	virtual void turn(int direction)
	{return;}

	virtual bool isReadyToShoot()
	{return false;}

	virtual void addNodePath(nodePath &nextNode)
	{return;}

	virtual nodePath * getPathRoot()
	{return NULL;}

	virtual void resetNodePath()
	{return;}

	virtual double getTurretAngle()
	{return NULL;}

	virtual void turretAngleAdjust(double adjust)
	{return;}

	virtual Vector3 getTurretLocation()
	{return getLocation();}

	virtual string getTerrainType()
	{return NULL;}

	virtual double getExplosionRadius()
	{return 0;}

	virtual void takeDamage(int damage)
	{return;}

	virtual bool shoot()
	{return false;}

	virtual void reloadClip()
	{return;}

	virtual void updateOverlay(int timeElapsed)
	{return;}

	virtual void addOverlayData(Vector3 location, Vector3 color, int fontSet, string text, int lifeTime)
	{return;}
	
	virtual OverlayData* getOverlay()
	{return NULL;}

	virtual void update(int timeElapsed, double height, tile tileData)=0;
	virtual void draw()=0;
	virtual bool isDead()=0;
	virtual void kill()=0;

	Vector3 getLocation();
	Vector3 getFacing();
	int getID();
	void setID(int i);
	double getTargetRads();
	double getFacingRads();
	double getVelocity();
	double getMovePointsLeft();
	void resetMovePoints();
	int getHealth();
	int getTeam();
	int getMaxVisionDistance();
	void setMaxVisionDistance(int target);
	bool isVisible();
	void setVisible(bool target);



	void init(string path);
	~Actor(void);

private:
	void loadObjModel();

protected:
	Vector3 location, destination, hitBox[8], hitBoxSize, facing;
	double velocity, turnSpeed, radianFacing, explosionRadius;
	bool moving, dead, visible;
	int id, lifeTime, totalLifeTime, health, maxHealth, team, maxVisionDistance;
	double totalMovePoints, movePointsLeft;
	tile currentTile;
	

	void setLocation(double x, double y, double z);
	void setLocation(Vector3 &v);
	

	virtual void checkLifeTime(int time);
	virtual void setHitBox()
	{return;}
};

