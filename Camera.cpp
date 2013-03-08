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
double moveSpeed, zoomSpeed, rotateSpeed;


Camera::Camera(double ** cTrack)
{
	camTrack = cTrack;
	theta = -PI*3/4;
	phi = PI*7/16;
	radius = 10;

	moveSpeed = 20;
	rotateSpeed = 1;
	zoomSpeed = 300;

	setLA(10, 0, 10);
	setY();

	updateLocation();
}

void Camera::strafe(int forwardDirection, int sideDirection, int timeElapsed)
{
	removeFocus();

	double xMove=0, zMove=0;

	if(forwardDirection != 0)
	{
		xMove = forwardDirection * moveSpeed * facing.x * (timeElapsed/1000.0);
		zMove = forwardDirection * moveSpeed * facing.z * (timeElapsed/1000.0);
	}
	else
	{
		xMove = sideDirection * moveSpeed * facing.z * (timeElapsed/1000.0);
		zMove = sideDirection * moveSpeed * -facing.x * (timeElapsed/1000.0);
	}
	setLA(lookAt.x + xMove, 0, lookAt.z + zMove);
	setY();

	updateLocation();
}

void Camera::zoom(int zoomDirection, int timeElapsed)
{ 
	radius += zoomDirection * zoomSpeed * (timeElapsed/1000.0);
	if(radius < 1)
		radius = 1;

	updateLocation();
}

void Camera::rotate(int rotateDirection, int timeElapsed)
{
	theta += rotateDirection * rotateSpeed * (timeElapsed/1000.0);
	updateLocation();
}

void Camera::setFocus(Actor &a)
{
	focusTarget = &a;
	focus = true;
	if(radius < 10)
		radius = 10;
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
	int lowX = (MAX_WORLD_SIZE-1)-(MAX_WORLD_SIZE-1)/32, lowXi=31, highXi;
	int lowZ = (MAX_WORLD_SIZE-1)-(MAX_WORLD_SIZE-1)/32, lowZi=31, highZi;

	while(lowX > lookAt.x)
	{
		lowX -= (MAX_WORLD_SIZE-1)/32;
		lowXi -= 1;
	}
	highXi = lowXi+1;

	while(lowZ > lookAt.z)
	{
		lowZ -= (MAX_WORLD_SIZE-1)/32;
		lowZi -= 1;
	}
	highZi = lowZi+1;


	//int low
	//if(lowX == 0)

	//int i1 = MAX_WORLD_SIZE/

	double h1 = cosineInterpolate(camTrack[lowXi][lowZi], camTrack[highXi][lowZi], (lookAt.x-lowX)/((MAX_WORLD_SIZE-1)/32.0));
	double h2 = cosineInterpolate(camTrack[lowXi][highZi], camTrack[highXi][highZi], (lookAt.x-lowX)/((MAX_WORLD_SIZE-1)/32.0));

	double h3 = cosineInterpolate(camTrack[lowXi][lowZi], camTrack[lowXi][highZi], (lookAt.z-lowZ)/((MAX_WORLD_SIZE-1)/32.0));
	double h4 = cosineInterpolate(camTrack[highXi][lowZi], camTrack[highXi][highZi], (lookAt.z-lowZ)/((MAX_WORLD_SIZE-1)/32.0));

	double h5 = cosineInterpolate(h1, h2, (lookAt.z-lowZ)/((MAX_WORLD_SIZE-1)/32.0));
	double h6 = cosineInterpolate(h3, h4, (lookAt.x-lowX)/((MAX_WORLD_SIZE-1)/32.0));

	lookAt.y = (h5+h6)/2;
}

//Prevent the camera from colliding with terrain
void Camera::adjustHeight(double newHeight)
{
	position.y = newHeight;
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
