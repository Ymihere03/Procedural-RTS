#include "TankExplosion.h"


TankExplosion::TankExplosion(Vector3 &position, Vector3 &f, int t)
{
	type = "Emitter";
	dead = false;
	visible = true;
	nextID = 0;
	velocity = 3;
	lifeTime = 0;
	totalLifeTime = 4000;
	team = t;
	root = NULL;
	maxVisionDistance = 0;
	
	setVector(location, position.x, position.y, position.z);
	setVector(facing, f.x, f.y, f.z);
	
	group = 0;
	for(int i = 0; i < 90; i++)
		addQuad(i);
	group = 1;
	for(int i = 0; i < 90; i++)
		addQuad(i);
}

void TankExplosion::update(int timeElapsed, double height, tile tileData)
{
	currentTile = tileData;
	double newTime = timeElapsed/1000.0;

	quadList * target = root;

	while(target != NULL)
	{
		target->lifeSpan += timeElapsed;
		double lifeSpanMilli = target->lifeSpan/1000.0;
		if(target->lifeSpan > target->totalLife)
			deleteQuad(target->id);

		setVector(target->quadLocation,
			target->quadLocation.x + target->quadDirection.x*newTime,
			target->quadLocation.y + target->quadDirection.y*newTime, 
			target->quadLocation.z + target->quadDirection.z*newTime);

		setVector(target->quadDirection, target->quadDirection.x, target->quadDirection.y, target->quadDirection.z);

		target = target->next;
	}

	checkLifeTime(timeElapsed);
}

void TankExplosion::draw()
{
	quadList * target = root;

	while(target != NULL)
	{
		glColor4f(target->color.x,target->color.y,target->color.z,1-((double)lifeTime)/((double)totalLifeTime));
		glPointSize(6);
		glBegin(GL_POINTS);
			glVertex3f(target->quadLocation.x, target->quadLocation.y, target->quadLocation.z);
		glEnd();
		
		target = target->next;
	}
}

void TankExplosion::kill()
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

bool TankExplosion::isDead()
{
	return dead;
}

void TankExplosion::genQuad(int angle, quadList *q)
{
	Vector3 f;

	setVector(f, sin((angle*4*PI)/180), getDecimalRandom(4), cos((angle*4*PI)/180));
  	normalize(f);

	setVector(q->quadLocation, location.x, location.y, location.z);
	q->id = nextID;
	
	q->lifeSpan = 0;
	q->id = nextID;
	q->velocity = velocity;//*cos(angle*4*PI/180)*.5;//+sin(angle*4*PI/180)))*velocity;
	q->totalLife = totalLifeTime;
	if(team == 1)
		setVector(q->color,getDecimalRandom(1)+.6,.05,.05);
	else if(team == 2)
		setVector(q->color,.05,.05,getDecimalRandom(1)+.6);


	switch(group) {
	case 0: setVector(q->quadDirection, 
		f.x*q->velocity*.7, 
		f.y*q->velocity, 
		f.z*q->velocity);
		break;
	case 1: setVector(q->quadDirection, 
		f.x*q->velocity, 
		f.y*q->velocity, 
		f.z*q->velocity*.7);
	}
	//setVector(q->quadFacing, (getRandomAsD(100)-50)/100+facing.x, -facing.y+getRandomAsD(40)/100, (getRandomAsD(100)-50)/100+facing.z);
	//normalize(q->quadFacing);

	nextID++;
	q->next = NULL;
}

//Linked List Functions-------------------
void TankExplosion::addQuad(int angle)
{
	if(!root)
	{
		root = (quadList *) malloc(sizeof(quadList));
		genQuad(angle, root);
	}
	else
	{
		quadList * target = root;

		while(target->next != NULL)
			target = target->next;

		target->next = (quadList *) malloc(sizeof(quadList));
		target = target->next;
		genQuad(angle, target);
	}
}

void TankExplosion::deleteQuad(int id)
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


TankExplosion::~TankExplosion(void)
{
}
