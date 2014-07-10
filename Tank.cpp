#include "Tank.h"

/*
Tank::Tank(double x, double y, double z, int t)
{
	//Initialize default variables
	double xSize=.4;
	double ySize=.2;
	double zSize=.3;
	type = "Tank";
	moving = false;
	dead = false;
	damageFlag = false;
	visible = true;
	team = t;
	maxVisionDistance = 15;	//Sight Radius
	velocity = 1.0;		//Base movement speed
	turnSpeed = .7;		//Base turning speed
	radianFacing = 0;	//Direction to face in radians
	direction = 1;		//Moving forward
	maxClipSize = 3;	//Number of shots that can be made in a turn
	ammoCount = 0;		//Number of shots left for the turn
	reloadTime = 300;	//Time to ready the gun to fire
	cooldown = 0;		//Current wait time to shoot
	lifeTime = 0;		//Time the object has been alive
	totalLifeTime = -1;	//Length of time to stay alive
	maxHealth = 15;		//Total number of health points
	health = maxHealth;	//Current health points
	totalMovePoints = 5;	//Object can move this distance
	movePointsLeft = totalMovePoints;	//Unconsumed move points
	uphillPenalty = 1;	//Modifier to consumed move units when moving uphill
	downhillBonus = -.5;	//Modifier to consumed move units when moving downhill
	setVector(hitBoxSize, xSize,ySize,zSize);
	overlayRoot = NULL;
	
	setLocation(x, y, z);
	setVector(destination, location.x, location.y, location.z);


	//Turret data
	turretAngle = 0;
	turretMaxAngle = PI/16.0;
	turretMinAngle = -PI/64.0;
	turretLength = .5;
	turretFacingRadians = radianFacing;
	setVector(turretLocation, location.x+turretLength*cos(turretAngle)*cos(turretFacingRadians), 
		location.y+hitBoxSize.y+turretLength*sin(turretAngle), location.z+turretLength*cos(turretAngle)*sin(turretFacingRadians));

	setVector(facing, cos(radianFacing), sin(turretAngle), sin(radianFacing));
	normalize(facing);

	pathRoot = NULL;
	//Allocate memory for tracer points
	tracerRoot = (nodePath3d *) malloc(sizeof(nodePath3d));
	tracerRoot->nodeData = turretLocation;
	nodePath3d *current = tracerRoot;
	for(int i = 0; i < 10; i++)
	{
		current->next = (nodePath3d *) malloc(sizeof(nodePath3d));
		current = current->next;
		current->nodeData = turretLocation;
	}
	current->next = NULL;

	char *filepath = "data/models/test.obj";
	model = glmReadOBJ(filepath, 0);
	if(!model)
		log("Tank model loading failed");

	//glmUnitize(model);
	//glmFacetNormals(model);
	//glmVertexNormals(model, 180.0);
	//glmLinearTexture(model);

}
*/
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

void Tank::addNodePath(nodePath &nextNode)
{
	bool IDontWantWayPoints = true;
	if(IDontWantWayPoints) {
		setMoveTarget(nextNode.nodeData);
		return;
	}

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

void Tank::update(int timeElapsed, double height, tile tileData)
{
	//Record the data of the terrain the unit is on
	currentTile = tileData;

	//Update the cooldown timer
	cooldown -= timeElapsed;

	double elapsedSeconds = timeElapsed/1000.0;

	//Check if the gun is ready to fire
	if(cooldown < 0)
		cooldown = 0;

	//Check if the path list has items queued in it
	if(pathRoot && !moving)
		setMoveTarget(pathRoot->nodeData);

	if(moving)
	{
		//If the destination is within a certain distance of the actor, then it will drive in circles
		//around the destination without ever reaching it. This will cause the actor to turn first
		//before trying to move forward.
		if(sqrt(pow(abs(location.x - destination.x),2) + pow(abs(location.z - destination.z),2)) < 10)
			direction = 0;
	

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
		if(-.01 < targetRadians && targetRadians < .01)
		{
			turn(0, elapsedSeconds, targetRadians);			//Don't apply a turn direction
			moving = true;
			direction = 1;
			//Final check after both turn and movement adjustments have been applied.
			//If this is true then the actor has fully reached its destination.
			if(abs(location.x - destination.x) < .1 &&
			abs(location.z - destination.z) < .1) {
				//If there is a movement point queued up then set it as the next destination
				if(pathRoot) {
					setMoveTarget(pathRoot->nodeData);
					if(pathRoot->next != NULL)
						pathRoot = pathRoot->next;
					else
						moving = false;
				}
				else
					moving = false;
			}
		}
		else if(targetRadians > 0)
			turn(1, elapsedSeconds, targetRadians);		//Right turn
		else if(targetRadians < 0)
			turn(-1, elapsedSeconds, targetRadians);		//Left turn
		setLocation(location.x, height, location.z);

		//Driving - Forward movement
		if(moving && movePointsLeft > 0) {
			Vector3 oldLocation = location;
			setLocation(location.x + cos(radianFacing)*velocity*direction*elapsedSeconds,
					height, location.z + (sin(radianFacing))*velocity*direction*elapsedSeconds);

			calcMovesSpent(oldLocation, location);
		}

	}

	setVector(turretLocation, location.x+turretLength*cos(turretAngle)*cos(turretFacingRadians), 
		location.y+hitBoxSize.y+turretLength*sin(turretAngle), location.z+turretLength*cos(turretAngle)*sin(turretFacingRadians));
	checkLifeTime(timeElapsed);
	makeTracer();
	updateOverlay(timeElapsed);
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

void Tank::turn(int direction, double elapsedSeconds, double targetRadians)
{
	double adjustment = turnSpeed * direction * elapsedSeconds;

	//Check if the full movement is more than needed
	if(abs(adjustment) > abs(targetRadians) || direction == 0) {
		//Turn only the needed amount
		radianFacing += targetRadians;
		turretFacingRadians += targetRadians;
	}
	else {
		//Turn full adjustment in radians
		radianFacing += adjustment;
		turretFacingRadians += adjustment;
	}

	//Sanity check to prevent radian values from getting too high or low.
	//Also allows turning across the +pi/2 and -pi/2 bounds of the atan function.
	if(radianFacing > PI)
		radianFacing -= 2*PI;
	else if(radianFacing < -PI)
		radianFacing += 2*PI;

	if(turretFacingRadians > PI)
		turretFacingRadians -= 2*PI;
	else if(turretFacingRadians < -PI)
		turretFacingRadians += 2*PI;

	//Convert the radian value to a vector (x,z)
	setVector(facing, cos(radianFacing), sin(turretAngle), sin(radianFacing));
	normalize(facing);
}

void Tank::turretAngleAdjust(double adjust)
{
	turretAngle += adjust;
	if(turretAngle > turretMaxAngle)
		turretAngle = turretMaxAngle;
	if(turretAngle < turretMinAngle)
		turretAngle = turretMinAngle;

	//Convert the radian value to a vector (x,z)
	setVector(facing, cos(radianFacing), sin(turretAngle), sin(radianFacing));
	normalize(facing);
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
//  FUNCTION: makeTracer()
//
//  PURPOSE: Calculates a series of points in 3d as a projection of where a bullet would travel
//					if shot by this object
//
//  COMMENTS: 
//

void Tank::makeTracer() 
{
	//Reset the list
	tracerRoot->nodeData = getTurretLocation();
	nodePath3d *current = tracerRoot;
	Vector3 direction, initialLocation;

	double time = 0;
	setVector(initialLocation, turretLocation.x, turretLocation.y, turretLocation.z);
	setVector(direction, facing.x*50, facing.y*50, facing.z*50);
	for(int i = 0; i < 50; i++)
	{
		setVector(current->next->nodeData, initialLocation.x+direction.x*time, initialLocation.y+direction.y*time + GRAVITY*pow(time, 2)*.5, initialLocation.z+direction.z*time);
		
		//Factor in gravity to make a new downward velocity
		setVector(direction, direction.x, direction.y, direction.z);

		nodePath3d intersect;
		//Check if the tracer segment has crossed the ground
		/*if(!checkLineIntersect(,,,,,intersect)	//TEMP needs data for function
		{
			//Code for when the tracer crosses the ground
			break;
		}*/
		time += .03;
		current = current->next;
	}
}

//
//  FUNCTION: calcMovesSpent()
//
//  PURPOSE: Calculate how many movement points the tank has spent
//
//  COMMENTS: 
//

void Tank::calcMovesSpent(Vector3 oldLocation, Vector3 newLocation)
{
	Vector3 movePoints;
	double modifier = 0;

	setVector(movePoints, newLocation.x-oldLocation.x,newLocation.y-oldLocation.y,newLocation.z-oldLocation.z);
	if(movePoints.y > .3)
		modifier = uphillPenalty;
	else if(movePoints.y < -.3)
		modifier = downhillBonus;

	movePoints.x *= 1+modifier;
	movePoints.y *= 1+modifier;
	movePoints.z *= 1+modifier;

	movePointsLeft -= find3dDistance(movePoints);
	if(movePointsLeft < 0)
		movePointsLeft = 0;
}

void Tank::takeDamage(int damage)
{
	health -= damage;
	if(health <= 0)
		kill();
	damageFlag = 10;
	string *text = new string;
	text->assign("-"+itos(damage));
	
	Vector3 start;
	setVector(start, location.x+getDecimalRandom(1)-.5, location.y+.2, location.z+getDecimalRandom(1)-.5);

	Vector3 color;
	setVector(color, 1,1,1);
	addOverlayData(start, color, 1, text, 4000);
}

bool Tank::shoot()
{
	ammoCount -= 1;
	if(ammoCount < 0)
	{
		ammoCount = 0;
		return false;		//Not enough ammo to fire
	}
	return true;			//Fire attempt was successful
}

void Tank::reloadClip()
{
	ammoCount = maxClipSize;
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
	if(team == 1)
		glColor3f(1,damageFlag/10.0,damageFlag/10.0);
	else if(team == 2)
		glColor3f(damageFlag/10.0,damageFlag/10.0,1);
	damageFlag--;
	if(damageFlag < 0)
		damageFlag = 0;
	glTranslated(location.x, location.y, location.z);
	glRotated(180 - radianFacing*180/PI,0,1,0);
	//glmDraw(model, GLM_SMOOTH|GLM_TEXTURE);
	glBegin(GL_QUADS);
		glNormal3d(1,0,0);
		glVertex3f(hitBoxSize.x/2,0,-hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,hitBoxSize.y,-hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,hitBoxSize.y,hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,0,hitBoxSize.z/2);
		
		glNormal3d(-1,0,0);
		glVertex3f(-hitBoxSize.x/2,hitBoxSize.y,-hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,0,-hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,0,hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,hitBoxSize.y,hitBoxSize.z/2);

		glNormal3d(0,0,1);
		glVertex3f(hitBoxSize.x/2,hitBoxSize.y,hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,hitBoxSize.y,hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,0,hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,0,hitBoxSize.z/2);
		
		glNormal3d(0,0,-1);
		glVertex3f(-hitBoxSize.x/2,0,-hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,hitBoxSize.y,-hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,hitBoxSize.y,-hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,0,-hitBoxSize.z/2);

		glNormal3d(0,1,0);
		glVertex3f(-hitBoxSize.x/2,hitBoxSize.y,-hitBoxSize.z/2);
		glVertex3f(-hitBoxSize.x/2,hitBoxSize.y,hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,hitBoxSize.y,hitBoxSize.z/2);
		glVertex3f(hitBoxSize.x/2,hitBoxSize.y,-hitBoxSize.z/2);
	glEnd();
	glLoadIdentity();

	//Facing Vector
	glLineWidth(2);
	glColor3f(1,1,0);
	glBegin(GL_LINES);
		glVertex3f(location.x, location.y+hitBoxSize.y, location.z);
		glVertex3f(getTurretLocation().x, getTurretLocation().y, getTurretLocation().z);
	glEnd();

	//Draws the hitbox outline of the object is selected
	if(isSelected())
	{
		/*
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
		*/
		//Draw the tracer
		glColor3f(1,1,1);
			nodePath3d *target = tracerRoot;
			bool drawFlag = false;
			while(target->next != NULL ) {
				if(drawFlag) {
					glBegin(GL_LINES);
						glVertex3f(target->nodeData.x,target->nodeData.y,target->nodeData.z);
						glVertex3f(target->next->nodeData.x,target->next->nodeData.y,target->next->nodeData.z);
					glEnd();
					drawFlag = false;
				}
				else
					drawFlag = true;
				target = target->next;
			}
	}


	glLoadIdentity();
}

void Tank::updateOverlay(int timeElapsed)
{
	OverlayData *target = overlayRoot;
	if(overlayRoot)
		if(overlayRoot->lifeTime == 0) {
			target = overlayRoot->next;
			overlayRoot = target;
		}
	while(target != NULL)
	{
		if(target->lifeTime != -1)
		{
			if(target->lifeTime - timeElapsed <= 0)
				target->lifeTime = 0;
			else
				target->lifeTime -= timeElapsed;
			target->location.y += .01;
		}
		target = target->next;
	}
}

void Tank::addOverlayData(Vector3 location, Vector3 color, int fontSet, string *text, int lifeTime)
{
	if(!overlayRoot)
	{
		overlayRoot = (OverlayData *) malloc(sizeof(OverlayData));
		overlayRoot->location = location;
		overlayRoot->color = color;
		overlayRoot->fontSet = fontSet;
		overlayRoot->text = text;
		overlayRoot->lifeTime = lifeTime;

		overlayRoot->next = NULL;
	}
	else
	{
		OverlayData * target = overlayRoot;

		while(target->next != NULL)
			target = target->next;

		target->next = (OverlayData *) malloc(sizeof(OverlayData));
		target = target->next;
		target->location = location;
		target->color = color;
		target->fontSet = fontSet;
		target->text = text;
		target->lifeTime = lifeTime;

		target->next = NULL;
	}
}

OverlayData* Tank::getOverlay()
{
	if(isVisible())
		return overlayRoot;
	return NULL;
}

Vector3 Tank::getTurretLocation()
{
	return turretLocation;
}

double Tank::getTurretAngle()
{
	return turretAngle;
}

string Tank::getTerrainType()
{
	switch(currentTile.type)
	{
	case FIELD: return "Field";
	case FOREST: return "Forest";
	case SAND: return "Sand";
	case WATER: return "Water";
	case SNOW: return "Snow";
	}
}

void Tank::kill()
{
	dead = true;
}

//Is the object dead?
bool Tank::isDead()
{
	return dead;
}

Tank::~Tank(void)
{
}
