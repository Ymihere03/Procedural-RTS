#include "GroundExplosion.h"

GroundExplosion::GroundExplosion(Vector3 &position, Vector3 &f)
{
	type = "Emitter";
	dead = false;
	nextID = 0;
	velocity = 40;
	lifeTime = 0;
	totalLifeTime = 1500;
	root = NULL;

	setVector(location, position.x, position.y, position.z);
	setVector(facing, f.x, f.y, f.z);
	for(int i = 0; i < 50; i++)
		addQuad();
}

void GroundExplosion::update(int timeElapsed, double height)
{
	double newTime = timeElapsed/1000.0;

	quadList * target = root;

	while(target != NULL)
	{
		target->lifeSpan += timeElapsed;
		if(target->lifeSpan > target->totalLife)
			deleteQuad(target->id);

		setVector(target->quadLocation, target->quadLocation.x + target->quadFacing.x*target->velocity*newTime,
			target->quadLocation.y+target->quadFacing.y*(target->velocity+target->downwardVelocity)*newTime, target->quadLocation.z + target->quadFacing.z*target->velocity*newTime);

		target->downwardVelocity += GRAVITY;

		target = target->next;
	}

	checkLifeTime(timeElapsed);
}

void GroundExplosion::draw()
{
	quadList * target = root;

	glBindTexture( GL_TEXTURE_2D, 7);

	while(target != NULL)
	{
		glColor3f(.7,.4,.2);
		//glRotatef(getRandomAsI(360), 0, 1, 0);
		glPointSize(5);
		glBegin(GL_POINTS);
			glVertex3f(target->quadLocation.x, target->quadLocation.y, target->quadLocation.z);
		glEnd();
		
		target = target->next;
	}
}

void GroundExplosion::kill()
{
	quadList * target = root;
	quadList * next;

	while(target != NULL)
	{
		next = target->next;
		free(target);
		target = next;
	}
	dead = true;
}

bool GroundExplosion::isDead()
{
	return dead;
}

void GroundExplosion::genQuad(quadList *q)
{
	double zStart = getRandomAsD(2)-1, xStart = getRandomAsD(2)-1;
	setVector(q->quadLocation, location.x+xStart, location.y, location.z+zStart);
	q->id = nextID;

	setVector(q->quadFacing, (getRandomAsD(40)-20)/100+facing.x/2, (getRandomAsD(75)+25)/100, (getRandomAsD(40)-20)/100+facing.z/2);
	normalize(q->quadFacing);
	q->lifeSpan = 0;
	q->id = nextID;
	q->downwardVelocity = 0;
	q->velocity = getRandomAsI(velocity);
	q->totalLife = getRandomAsI(1500);

	nextID++;
	q->next = NULL;
}

//Linked List Functions-------------------
void GroundExplosion::addQuad()
{
	if(!root)
	{
		root = (quadList *) malloc(sizeof(quadList));
		genQuad(root);
	}
	else
	{
		quadList * target = root;

		while(target->next != NULL)
			target = target->next;

		target->next = (quadList *) malloc(sizeof(quadList));
		target = target->next;
		genQuad(target);
	}
}

void GroundExplosion::deleteQuad(int id)
{
	quadList * target = root;
	if(root->id == id)
	{
		if(root->next)
			root = root->next;
		else
			root = NULL;
		return;
	}

	while(target->next != NULL)
	{
		if(target->next->id == id)
		{
			if(target->next->next != NULL)
				target->next = target->next->next;
			else
				target->next = NULL;
			return;
		}

		target = target->next;
	}
	log("Attempted deletion of particle effect ID "+itos(id)+" failed");
}
//----------------------

GroundExplosion::~GroundExplosion(void)
{
}
