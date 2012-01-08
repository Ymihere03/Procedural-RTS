#include "Bullet.h"


Bullet::Bullet(Vector3 &position, Vector3 &direction)
{
	type = "Bullet";

	setVector(location, position.x+direction.x, position.y+.75, position.z+direction.z);

	//Facing vector with a bit of randomness
	setVector(facing, direction.x+(getRandomAsD(20)-10)/700.0, direction.y, direction.z+(getRandomAsD(20)-10)/700.0);
	normalize(facing);
	
	dead = false;
	velocity = 100;		//Units per second
	downwardVelocity = -1*(velocity-velocity*facing.y+(getRandomAsD(10)-5)/500.0);
	lifeTime = 0;
	totalLifeTime = 20000;
	
}

//
//  FUNCTION: update()
//
//  PURPOSE: Updates the objects location
//
//  COMMENTS: 
//

void Bullet::update(int timeElapsed, double height)
{
	//Convert milliseconds to seconds
	double newTime = timeElapsed/1000.0;

	Vector3 oldLocation;
	setVector(oldLocation, location.x, location.y, location.z);

	//Adjust location based on direction, velocity, and distance by time elapsed
	setLocation(location.x + facing.x*velocity*newTime,
		location.y+(velocity+downwardVelocity)*newTime, location.z + facing.z*velocity*newTime);

	facing.y = location.y-oldLocation.y;

	//Factor in gravity to make a new downward velocity
	downwardVelocity += GRAVITY;

	//Check if the bullet's lifetime is over
	checkLifeTime(timeElapsed);

	if(height > location.y)
	{
		dead = true;
	}
}

void Bullet::draw()
{
	glColor3f(1,1,1);
	glPointSize(4);
	glBegin(GL_POINTS);
		glVertex3f(location.x, location.y, location.z);
	glEnd();
	glLoadIdentity();
}

//Kills the bullet on next update
void Bullet::kill()
{
	dead = true;
}

//Checks to see if the bullet is dead
bool Bullet::isDead()
{
	return dead;
}

Bullet::~Bullet(void)
{
	dead = true;
}
