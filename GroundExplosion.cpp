#include "GroundExplosion.h"

GroundExplosion::GroundExplosion(Vector3 &position, Vector3 &f, tile tileData)
{
	type = "Emitter";
	dead = false;
	visible = true;
	nextID = 0;
	velocity = 3;
	lifeTime = 0;
	totalLifeTime = 1500;
	root = NULL;
	currentTile = tileData;
	maxVisionDistance = 0;

	setVector(location, position.x, position.y, position.z);
	setVector(facing, f.x, f.y, f.z);
	

	for(int i = 0; i < 40; i++)
		addQuad();
}

void GroundExplosion::update(int timeElapsed, double height, tile tileData)
{
	visible = true;
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

		setVector(target->quadDirection, target->quadDirection.x, target->quadDirection.y+GRAVITY*pow(lifeSpanMilli, 2)*.1, target->quadDirection.z);

		target = target->next;
	}

	checkLifeTime(timeElapsed);
}

void GroundExplosion::draw()
{
	quadList * target = root;

	while(target != NULL)
	{
		glColor4f(target->color.x,target->color.y,target->color.z,1-((double)lifeTime)/((double)totalLifeTime));
		glPointSize(6);
		glBegin(GL_POINTS);
			glVertex3f(target->quadLocation.x, target->quadLocation.y, target->quadLocation.z);
		glEnd();

		/*glColor3f(1,1,1);
		glBegin(GL_TRIANGLES);
			glTexCoord2f(0,0);	glVertex3f(location.x-(1-facing.x), location.y, location.z-(1-facing.z));
			glTexCoord2f(1,0);	glVertex3f(location.x+(1-facing.x), location.y, location.z+(1-facing.z));
			glTexCoord2f(.5,1);	glVertex3f(target->quadLocation.x, target->quadLocation.y, target->quadLocation.z);

			glTexCoord2f(1,0);	glVertex3f(location.x+(1-facing.x), location.y, location.z+(1-facing.z));
			glTexCoord2f(0,0);	glVertex3f(location.x-(1-facing.x), location.y, location.z-(1-facing.z));
			glTexCoord2f(.5,1);	glVertex3f(target->quadLocation.x, target->quadLocation.y, target->quadLocation.z);
		glEnd();*/
		
		target = target->next;
	}
}

void GroundExplosion::updateOverlay(int timeElapsed)
{

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
	Vector3 f;

	//Random Facing vector
	setVector(f, (getRandomAsD(50)-25)/100, 0, (getRandomAsD(50)-25)/100);
  	normalize(f);

	double zStart = (getRandomAsD(50)-25)/100, xStart = (getRandomAsD(50)-25)/100;
	setVector(q->quadLocation, location.x+(xStart*f.x), location.y, location.z+(zStart*f.z));
	q->id = nextID;

	double angle = (getRandomAsD(85)+5)*PI/180;

	
	
	q->lifeSpan = 0;
	q->id = nextID;
	q->velocity = getDecimalRandom(velocity);
	q->totalLife = getRandomAsI(totalLifeTime);
	switch(currentTile.type) {
		case FOREST: 
			if (choose(.8))
				setVector(q->color,.05,.4,.05);	//Green
			else
				setVector(q->color,.4,.25,.05); //Brown
			break;

		case FIELD: 
			if(choose(.8))
				setVector(q->color,.15,.5,.05); //Green
			else
				setVector(q->color,.5,.4,.05);	//Brown
			break;

		case SAND: 
			setVector(q->color,1,.87,.42);
			break;

		case WATER: 
			setVector(q->color,0,.3,1);
			break;

		case SNOW: 
			setVector(q->color,1,1,1);
			break;

		default: 
			setVector(q->color,.5,.15,.05);
		}

	setVector(q->quadDirection, 
		f.x*q->velocity, 
		q->velocity*sin(angle)*3, 
		f.z*q->velocity);

	//setVector(q->quadFacing, (getRandomAsD(100)-50)/100+facing.x, -facing.y+getRandomAsD(40)/100, (getRandomAsD(100)-50)/100+facing.z);
	//normalize(q->quadFacing);

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
