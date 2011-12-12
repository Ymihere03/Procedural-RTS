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
	void unselect();
	void setSelected(bool target);
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

	virtual void addNodePath(Vector3 &nextNode)
	{return;}

	virtual Vector2 * getNodePath(int index)
	{return NULL;}

	virtual void resetNodePath()
	{return;}

	virtual void update(int timeElapsed, double height)=0;
	virtual void draw()=0;
	virtual bool isDead()=0;
	virtual void kill()=0;

	Vector3 getLocation();
	Vector3 getFacing();
	//int getID();
	double getTargetRads();
	double getFacingRads();

	void init(string path);
	~Actor(void);

private:
	void loadObjModel();

protected:
	Vector3 location, destination, hitBox[8], hitBoxSize, facing;
	double velocity, turnSpeed, radianFacing;
	bool selected, moving, dead;
	double lifeTime;
	
	//int id;
	

	void setLocation(double x, double y, double z);
	void setLocation(Vector3 &v);
	

	virtual void setLifeTime(double lifeTime)=0;
	virtual void setHitBox()
	{return;}
};

