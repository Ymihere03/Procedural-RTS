#include "Bullet.h"


Bullet::Bullet(Vector3 &position, Vector3 &direction2)
{
	type = "Bullet";
   	setVector(location, position.x, position.y, position.z);
	setVector(initialLocation, position.x, position.y, position.z);
	log("Bullet shot at x:"+dtos(location.x)+" y:"+dtos(location.y)+" z:"+dtos(location.z)+"\n");
	dead = false;
	visible = true;
	maxVisionDistance = 0;
	velocity = 50;		//Units per second

	//Facing vector with a bit of randomness
	setVector(facing, direction2.x+(getRandomAsD(20)-10)/700.0, direction2.y+(getRandomAsD(20)-10)/550.0, direction2.z+(getRandomAsD(20)-10)/700.0);
	normalize(facing);

	setVector(initialFacing, facing.x*velocity, facing.y*velocity, facing.z*velocity);

	lifeTime = 0;
	totalLifeTime = 8000;
	explosionRadius = 2;
	
}

//
//  FUNCTION: update()
//
//  PURPOSE: Updates the objects location
//
//  COMMENTS: 
//

void Bullet::update(int timeElapsed, double height, tile tileData)
{
	//Check if the bullet's lifetime is over
	checkLifeTime(timeElapsed);

	//Convert milliseconds to seconds
	double newTime = (double)(lifeTime)/1000.0;
	Vector3 oldLocation = location;
	

	//Adjust location based on direction, velocity, and distance by time elapsed
	setLocation(initialLocation.x + initialFacing.x*newTime,
		initialLocation.y + initialFacing.y*newTime + GRAVITY*pow(newTime, 2)*.5, initialLocation.z + initialFacing.z*newTime);

	//Recalculate facing vector
	setVector(facing, location.x-oldLocation.x, location.y-oldLocation.y, location.z-oldLocation.z);
	normalize(facing);
	
	//If below ground, then explode
	if(height > location.y) {
		dead = true;
	}
}

double Bullet::getExplosionRadius()
{
	return explosionRadius;
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

void Bullet::updateOverlay(int timeElapsed)
{

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
