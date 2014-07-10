#pragma once
#include "TerrainGen.h"
#include "Actor.h"

class Camera
{
public:
	
	Camera(double ** cTrack);
	void strafe(int forwardDirection, int sideDirection, int timeElapsed);
	void zoom(int zoomDirection);
	void rotate(int rotateDirection, int timeElapsed);
	void followFocus();
	
	void setFocus(Actor &a);
	
	bool hasFocus();
	void adjustHeight(double newHeight);
	
	Vector3 getPosition();
	Vector3 getLookAt();
	~Camera(void);

private:

	void removeFocus();
	void updateLocation();
	void setY();
	void setLA(double x, double y, double z);
};

