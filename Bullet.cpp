#include "Bullet.h"


Bullet::Bullet(Vector3 &position, Vector3 &direction)
{
	type = "Bullet";
	gravity = -.098;
	downwardVelocity = (getRandomAsD(10)-5)/500.0;
	dead = false;
	velocity = 50;		//Units per second
	setVector(location, position.x+direction.x, position.y+.75, position.z+direction.x);

	//Facing vector with a bit of randomness
	setVector(facing, direction.x+(getRandomAsD(10)-5)/700.0, 0, direction.z+(getRandomAsD(10)-5)/700.0);
	normalize(facing);
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

	//Adjust location based on direction, velocity, and distance by time elapsed
	setLocation(location.x + facing.x*velocity*newTime,
		location.y+downwardVelocity*newTime, location.z + facing.z*velocity*newTime);

	//Factor in gravity to make a new downward velocity
	downwardVelocity += gravity;

	//Check if the bullet's lifetime is over
	if(height > location.y || lifeTime > 20000)
		dead = true;
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

//Updates the lifespan counter
void Bullet::setLifeTime(double l)
{
	lifeTime = l;
}

Bullet::~Bullet(void)
{
	dead = true;
}
