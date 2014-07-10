#include "Artillery.h"


Artillery::Artillery(double x, double y, double z, int t)
{
	//Initialize default variables
	double xSize=.5;
	double ySize=.2;
	double zSize=.2;
	type = "Artillery";
	moving = false;
	dead = false;
	damageFlag = false;
	visible = true;
	team = t;
	maxVisionDistance = 5;	//Sight Radius
	velocity = .7;		//Base movement speed
	turnSpeed = .5;		//Base turning speed
	radianFacing = 0;	//Direction to face in radians
	direction = 1;		//Moving forward
	maxClipSize = 3;	//Number of shots that can be made in a turn
	ammoCount = 0;		//Number of shots left for the turn
	reloadTime = 500;	//Time to ready the gun to fire
	cooldown = 0;		//Current wait time to shoot
	lifeTime = 0;		//Time the object has been alive
	totalLifeTime = -1;	//Length of time to stay alive
	maxHealth = 5;		//Total number of health points
	health = maxHealth;	//Current health points
	totalMovePoints = 3;	//Object can move this distance
	movePointsLeft = totalMovePoints;	//Unconsumed move points
	uphillPenalty = 1.3;	//Modifier to consumed move units when moving uphill
	downhillBonus = -.2;	//Modifier to consumed move units when moving downhill
	setVector(hitBoxSize, xSize,ySize,zSize);
	overlayRoot = NULL;
	
	setLocation(x, y, z);
	setVector(destination, location.x, location.y, location.z);


	//Turret data
	turretAngle = PI/16.0;
	turretMaxAngle = PI/2.0;
	turretMinAngle = PI/16.0;
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
	for(int i = 0; i < 50; i++)
	{
		current->next = (nodePath3d *) malloc(sizeof(nodePath3d));
		current = current->next;
		current->nodeData = turretLocation;
	}
	current->next = NULL;

	//char *filepath = "data/models/test.obj";
	//model = glmReadOBJ(filepath, 0);
	//if(!model)
	//	log("Tank model loading failed");
}


Artillery::~Artillery(void)
{
}
