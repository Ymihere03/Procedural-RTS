#include "Tank.h"


Tank::Tank(double x, double y, double z, double xSize, double ySize, double zSize)
{
	//Initialize default variables
	type = "Tank";
	selected = false;
	moving = false;
	dead = false;
	velocity = .2;		//Base movement speed
	turnSpeed = .05;	//Base turning speed
	radianFacing = 0;	//Direction to face in radians
	direction = 1;		//Moving forward
	reloadTime = 300;	//Time to ready the gun to fire
	cooldown = 0;		//Current wait time to shoot
	setVector(hitBoxSize, xSize,ySize,zSize);
	
	setLocation(x, y, z);
	setVector(destination, location.x, location.y, location.z);
	
	setVector(facing, cos(radianFacing),0, sin(radianFacing));
	normalize(facing);

	pathRoot = NULL;

	char *filepath = "data/models/test.obj";
	model = glmReadOBJ(filepath, 0);
	if(!model)
		log("Tank model loading failed");

	//glmUnitize(model);
	//glmFacetNormals(model);
	//glmVertexNormals(model, 180.0);
	//glmLinearTexture(model);

}

//
//  FUNCTION: setHitbox()
//
//  PURPOSE: Set the location of the hitbox verticies based on the object's location and size
//
//  COMMENTS: 
//

void Tank::setHitBox()
{
	setVector(hitBox[0], location.x-hitBoxSize.x/2,	location.y,					location.z-hitBoxSize.z/2);
	setVector(hitBox[1], location.x+hitBoxSize.x/2,	location.y,					location.z-hitBoxSize.z/2);
	setVector(hitBox[2], location.x-hitBoxSize.x/2,	location.y+hitBoxSize.y,	location.z-hitBoxSize.z/2);
	setVector(hitBox[3], location.x-hitBoxSize.x/2,	location.y,					location.z+hitBoxSize.z/2);
	setVector(hitBox[4], location.x+hitBoxSize.x/2,	location.y+hitBoxSize.y,	location.z-hitBoxSize.z/2);
	setVector(hitBox[5], location.x-hitBoxSize.x/2,	location.y+hitBoxSize.y,	location.z+hitBoxSize.z/2);
	setVector(hitBox[6], location.x+hitBoxSize.x/2,	location.y+hitBoxSize.y,	location.z+hitBoxSize.z/2);
	setVector(hitBox[7], location.x+hitBoxSize.x/2,	location.y,					location.z+hitBoxSize.z/2);
}

//
//  FUNCTION: setMoveTarget()
//
//  PURPOSE: Gives the object a destination
//
//  COMMENTS: This only sets the destination point and the flag will notify the object
//				that it needs to move
//

void Tank::setMoveTarget(Vector2 &v)
{
	//Set destination
	setVector(destination, v.x, 0, v.y);

	//Flag the object to want to move when updated
	moving = true;
}

/*void Tank::addNodePath(Vector3 &nextNode)
{
	if(!root)
	{
		root = (nodePath *) malloc(sizeof(nodePath));
		root->next = NULL;
		setCoord(root->nodeData, nextNode.x, nextNode.y);
	}
	else
	{
		nodePath * target = (nodePath *) malloc(sizeof(nodePath));
		target = root;

		while(target->next != NULL)
			target = target->next;

		target->next = (nodePath *) malloc(sizeof(nodePath));
		target = target->next;

		setCoord(target->nodeData, nextNode.x, nextNode.y);
		target->next = NULL;
	}
}*/

void Tank::addNodePath(nodePath &nextNode)
{
	if(!pathRoot)
	{
		pathRoot = (nodePath *) malloc(sizeof(nodePath));
		pathRoot->next = NULL;
		pathRoot->incentive = nextNode.incentive;
		setCoord(pathRoot->nodeData, nextNode.nodeData.x, nextNode.nodeData.y);
	}
	else
	{
		nodePath * target = pathRoot;

		while(target->next != NULL)
			target = target->next;

		target->next = (nodePath *) malloc(sizeof(nodePath));
		target = target->next;

		setCoord(target->nodeData, nextNode.nodeData.x, nextNode.nodeData.y);
		target->next = NULL;
	}
}

nodePath * Tank::getPathRoot()
{
	return pathRoot;
}

void Tank::resetNodePath()
{
	if(pathRoot)
	{
		pathRoot->next = NULL;
		pathRoot->incentive = NULL;
		pathRoot->nodeData.x = -1;
		pathRoot->nodeData.y = -1;
		pathRoot = NULL;
	}
}

//
//  FUNCTION: update()
//
//  PURPOSE: Updates the objects location and facing direction
//
//  COMMENTS: This method is part of the main program loop and is called before every frame
//

void Tank::update(int timeElapsed, double height)
{
	//Check if the path list has items queued in it
	if(pathRoot && !moving)
		setMoveTarget(pathRoot->nodeData);

	//Update the cooldown timer
	cooldown -= timeElapsed;

	if(cooldown < 0)
		cooldown = 0;		//Gun is ready to fire

	//Check if we are close enough to the destination
	if(abs(location.x - destination.x) < velocity*5 &&
		abs(location.z - destination.z) < velocity*5)
			moving = false;
	else
		moving = true;

	//If the destination is within a certain distance of the actor, then it will drive in circles
	//around the destination without ever reaching it. This will cause the actor to turn first
	//before trying to move forward.
	if(sqrt(pow(abs(location.x - destination.x),2) + pow(abs(location.z - destination.z),2)) < 20)
		direction = -1;
	

	//Turning - Rotation
	double targetOffset, targetRadians;

	//Offset the atan2 function by the direction the actor is facing
	targetOffset = atan2(facing.z, facing.x);
	
	//Find the angle in radians from the facing vector to the target direction
	targetRadians = atan2(destination.z-location.z, destination.x-location.x) - targetOffset;
	
	//Sanity check to prevent radian values from getting too high or low.
	//Also allows turning across the +pi/2 and -pi/2 bounds of the atan function.
	if(targetRadians > PI)
		targetRadians -= 2*PI;
	else if(targetRadians < -PI)
		targetRadians += 2*PI;

	//Check if we don't need to turn any more to reach the destination
	if(0 < targetRadians && targetRadians < turnSpeed)
	{
		turn(0);			//Don't apply a turn direction
		moving = true;
		direction = 1;
		//Final check after both turn and movement adjustments have been applied.
		//If this is true then the actor has fully reached its destination.
		if(abs(location.x - destination.x) < 7 &&
		abs(location.z - destination.z) < 7)
		{
			//If there is a movement point queued up then set it as the next destination
			if(pathRoot)
			{
				setMoveTarget(pathRoot->nodeData);
				if(pathRoot->next != NULL)
					pathRoot = pathRoot->next;
				else
					moving = false;
			}
		}
	}
	else if(targetRadians > 0)
		turn(1);		//Right turn
	else if(targetRadians < 0)
		turn(-1);		//Left turn

	//Driving - Forward movement
	if(moving)
		setLocation(location.x + cos(radianFacing)*velocity*direction,
				height, location.z + (sin(radianFacing))*velocity*direction);
}

//
//  FUNCTION: turn()
//
//  PURPOSE: Turns the object by exactly one unit of radians around the y-axis. 
//
//  COMMENTS: The amount of radians the object turns by is given by the turn speed 
//				and the direction of the turn (right or left) is given by the direction
//				variable.
//

void Tank::turn(int direction)
{
	//Turn adjustment in radians
	radianFacing = radianFacing + turnSpeed * direction;

	//Sanity check to prevent radian values from getting too high or low.
	//Also allows turning across the +pi/2 and -pi/2 bounds of the atan function.
	if(radianFacing > PI)
		radianFacing -= 2*PI;
	else if(radianFacing < -PI)
		radianFacing += 2*PI;

	//Convert the radian value to a vector (x,z)
	setVector(facing, cos(radianFacing), 0, sin(radianFacing));
}

bool Tank::isReadyToShoot()
{
	if(cooldown == 0)
	{
		cooldown = reloadTime;
		return true;
	}
	else
		return false;
}

//
//  FUNCTION: checkHitBox()
//
//  PURPOSE: Checks to see if the given line segment intersects the object's hitbox
//
//  COMMENTS: The amount of radians the object turns by is given by the turn speed 
//				and the direction of the turn (right or left) is given by the direction
//				variable.
//

bool Tank::checkHitBox(Vector3 &ray1, Vector3 &ray2)
{
	Vector3 target;

	//Check all faces of the object's hitbox for possible intersection
	if(checkLineIntersect(hitBox[1], hitBox[0], hitBox[4], ray1, ray2, target) ||
		checkLineIntersect(hitBox[2], hitBox[4], hitBox[0], ray1, ray2, target) ||
		checkLineIntersect(hitBox[4], hitBox[2], hitBox[6], ray1, ray2, target) ||
		checkLineIntersect(hitBox[5], hitBox[6], hitBox[2], ray1, ray2, target) ||
		checkLineIntersect(hitBox[0], hitBox[3], hitBox[2], ray1, ray2, target) ||
		checkLineIntersect(hitBox[5], hitBox[2], hitBox[3], ray1, ray2, target))
			return true;

	//No intersection was detected
	return false;
}

//
//  FUNCTION: draw()
//
//  PURPOSE: Draws the tank in the OpenGL environment
//
//  COMMENTS: 
//

void Tank::draw()
{
	//Tank Model
	glColor3f(1,0,0);
	glTranslated(location.x, location.y, location.z);
	glRotated(180 - radianFacing*180/PI,0,1,0);
	//glmDraw(model, GLM_SMOOTH|GLM_TEXTURE);
	glBegin(GL_QUADS);
		glNormal3d(1,0,0);
		glVertex3f(hitBoxSize.x/2,0,-hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,1,-hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,1,hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,0,hitBoxSize.z/2);
		
		glNormal3d(-1,0,0);
		glVertex3f(-hitBoxSize.x/2,1,-hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,0,-hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,0,hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,1,hitBoxSize.z/2);

		glNormal3d(0,0,1);
		glVertex3f(hitBoxSize.x/2,1,hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,1,hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,0,hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,0,hitBoxSize.z/2);
		
		glNormal3d(0,0,-1);
		glVertex3f(-hitBoxSize.x/2,0,-hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,1,-hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,1,-hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,0,-hitBoxSize.z/2);

		glNormal3d(0,1,0);
		glVertex3f(-hitBoxSize.x/2,1,-hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,1,hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,1,hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,1,-hitBoxSize.z/2);
	glEnd();
	glLoadIdentity();

	//Facing Vector
	glLineWidth(2);
	glColor3f(1,1,0);
	glBegin(GL_LINES);
		glVertex3f(location.x, location.y+hitBoxSize.y, location.z);
		glVertex3f(location.x+(2*facing.x), location.y+hitBoxSize.y, location.z+(2*facing.z));
	glEnd();

	//Draws the hitbox outline of the object is selected
	if(selected)
	{
		glColor3f(0,1,0);
		glBegin(GL_LINES);
			glVertex3f(hitBox[0].x,hitBox[0].y,hitBox[0].z);
			glVertex3f(hitBox[2].x,hitBox[2].y,hitBox[2].z);
		
			glVertex3f(hitBox[0].x,hitBox[0].y,hitBox[0].z);
			glVertex3f(hitBox[3].x,hitBox[3].y,hitBox[3].z);

			glVertex3f(hitBox[0].x,hitBox[0].y,hitBox[0].z);
			glVertex3f(hitBox[1].x,hitBox[1].y,hitBox[1].z);

			glVertex3f(hitBox[6].x,hitBox[6].y,hitBox[6].z);
			glVertex3f(hitBox[5].x,hitBox[5].y,hitBox[5].z);

			glVertex3f(hitBox[6].x,hitBox[6].y,hitBox[6].z);
			glVertex3f(hitBox[4].x,hitBox[4].y,hitBox[4].z);

			glVertex3f(hitBox[6].x,hitBox[6].y,hitBox[6].z);
			glVertex3f(hitBox[7].x,hitBox[7].y,hitBox[7].z);

			glVertex3f(hitBox[4].x,hitBox[4].y,hitBox[4].z);
			glVertex3f(hitBox[2].x,hitBox[2].y,hitBox[2].z);

			glVertex3f(hitBox[4].x,hitBox[4].y,hitBox[4].z);
			glVertex3f(hitBox[1].x,hitBox[1].y,hitBox[1].z);

			glVertex3f(hitBox[5].x,hitBox[5].y,hitBox[5].z);
			glVertex3f(hitBox[2].x,hitBox[2].y,hitBox[2].z);

			glVertex3f(hitBox[5].x,hitBox[5].y,hitBox[5].z);
			glVertex3f(hitBox[3].x,hitBox[3].y,hitBox[3].z);

			glVertex3f(hitBox[7].x,hitBox[7].y,hitBox[7].z);
			glVertex3f(hitBox[3].x,hitBox[3].y,hitBox[3].z);

			glVertex3f(hitBox[7].x,hitBox[7].y,hitBox[7].z);
			glVertex3f(hitBox[1].x,hitBox[1].y,hitBox[1].z);
		glEnd();
	}


	glLoadIdentity();
}

void Tank::kill()
{
	dead = true;
}

//Is the object dead yet?
bool Tank::isDead()
{
	return dead;
}

//Add to how long the object has been alive
void Tank::setLifeTime(double l)
{
	lifeTime = l;
}

Tank::~Tank(void)
{
}
