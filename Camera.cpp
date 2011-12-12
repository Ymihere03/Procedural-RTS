#include "StdAfx.h"
#include "Camera.h"
//#include "OpenGL.h"

//double posX, posY, posZ;				//Camera Position
//double lookAtX, lookAtY, lookAtZ;		//Camera LookAt position
//double distModX, distModY, distModZ;	//Distance Modifier camera position relative to LookAt
Vector3 position, lookAt, facing;
Actor *focusTarget;
bool focus;
double theta, phi, radius;
double ** camTrack;


Camera::Camera(double ** cTrack)
{
	camTrack = cTrack;
	theta = -PI*3/4;
	phi = PI*7/16;
	radius = 20;

	setLA(200, 0, 200);
	setY();

	updateLocation();
}

void Camera::strafe(double forwardSpeed, double sideSpeed)
{
	removeFocus();

	double xMove=0, zMove=0;

	if(forwardSpeed != 0)
	{
		xMove = forwardSpeed * facing.x;
		zMove = forwardSpeed * facing.z;
	}
	else
	{
		xMove = sideSpeed * facing.z;
		zMove = sideSpeed * -facing.x;
	}
	setLA(lookAt.x + xMove, 0, lookAt.z + zMove);
	setY();

	updateLocation();
}

void Camera::zoom(double zoomSpeed)
{ 
	radius += zoomSpeed;
	if(radius < 1)
		radius = 1;

	updateLocation();
}

void Camera::rotate(double rotate)
{
	theta += rotate;
	updateLocation();
}

void Camera::setFocus(Actor &a)
{
	focusTarget = &a;
	focus = true;
	if(radius < 20)
		radius = 20;
	followFocus();
}

void Camera::followFocus()
{
	setLA(focusTarget->getLocation().x, focusTarget->getLocation().y, focusTarget->getLocation().z);
	updateLocation();
}

void Camera::updateLocation()
{
	double x, y, z, xMod, yMod, zMod;

	if(hasFocus())
	{
		xMod = focusTarget->getLocation().x;
		yMod = focusTarget->getLocation().y;
		zMod = focusTarget->getLocation().z;
	}
	else
	{
		xMod = lookAt.x;
		yMod = lookAt.y;
		zMod = lookAt.z;
	}

	x = radius*cos(theta)*sin(phi)+xMod;
	z = radius*sin(theta)*sin(phi)+zMod;
	y = radius*cos(phi)+yMod;

	setVector(position, x, y, z);

	setVector(facing, lookAt.x - position.x, lookAt.y - position.y, lookAt.z - position.z);
	normalize(facing);
}

void Camera::removeFocus()
{
	focus = false;
}

bool Camera::hasFocus()
{
	return focus;
}

void Camera::setLA(double x, double y, double z)
{
	setVector(lookAt, x, y, z);
	
	if(lookAt.x < 0)
		lookAt.x = 0;

	if(lookAt.x >= MAX_WORLD_SIZE)
		lookAt.x = MAX_WORLD_SIZE-1;

	if(lookAt.z < 0)
		lookAt.z = 0;

	if(lookAt.z >= MAX_WORLD_SIZE)
		lookAt.z = MAX_WORLD_SIZE-1;
}

void Camera::setY()
{
	int lowX = 768, lowXi=3, highXi;
	int lowZ = 768, lowZi=3, highZi;

	while(lowX > lookAt.x)
	{
		lowX -= 256;
		lowXi -= 1;
	}
	highXi = lowXi+1;

	while(lowZ > lookAt.z)
	{
		lowZ -= 256;
		lowZi -= 1;
	}
	highZi = lowZi+1;


	//int low
	//if(lowX == 0)

	//int i1 = MAX_WORLD_SIZE/

	double h1 = cosineInterpolate(camTrack[lowXi][lowZi], camTrack[highXi][lowZi], (lookAt.x-lowX)/256);
	double h2 = cosineInterpolate(camTrack[lowXi][highZi], camTrack[highXi][highZi], (lookAt.x-lowX)/256);

	double h3 = cosineInterpolate(camTrack[lowXi][lowZi], camTrack[lowXi][highZi], (lookAt.z-lowZ)/256);
	double h4 = cosineInterpolate(camTrack[highXi][lowZi], camTrack[highXi][highZi], (lookAt.z-lowZ)/256);

	double h5 = cosineInterpolate(h1, h2, (lookAt.z-lowZ)/256);
	double h6 = cosineInterpolate(h3, h4, (lookAt.x-lowX)/256);

	lookAt.y = (h5+h6)/2;
}

//Prevent the camera from colliding with terrain
void Camera::adjustHeight(double newHeight)
{
	position.y = newHeight;
	//lookAt.y = position.y-distanceMod.y;
}

Vector3 Camera::getPosition()
{
	return position;
}

Vector3 Camera::getLookAt()
{
	return lookAt;
}

Camera::~Camera(void)
{
}
