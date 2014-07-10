#include "Scout.h"


Scout::Scout(double x, double y, double z, int t)
{
	//Initialize default variables
	double xSize=.3;
	double ySize=.5;
	double zSize=.3;
	type = "Scout";
	moving = false;
	dead = false;
	damageFlag = false;
	visible = true;
	team = t;
	maxVisionDistance = 30;	//Sight Radius
	velocity = 2.0;		//Base movement speed
	turnSpeed = 1.0;	//Base turning speed
	radianFacing = 0;	//Direction to face in radians
	direction = 1;		//Moving forward
	maxClipSize = 5;	//Number of shots that can be made in a turn
	ammoCount = 0;		//Number of shots left for the turn
	reloadTime = 100;	//Time to ready the gun to fire
	cooldown = 0;		//Current wait time to shoot
	lifeTime = 0;		//Time the object has been alive
	totalLifeTime = -1;	//Length of time to stay alive
	maxHealth = 10;		//Total number of health points
	health = maxHealth;	//Current health points
	totalMovePoints = 10;	//Object can move this distance
	movePointsLeft = totalMovePoints;	//Unconsumed move points
	uphillPenalty = .7;	//Modifier to consumed move units when moving uphill
	downhillBonus = -.9;	//Modifier to consumed move units when moving downhill
	setVector(hitBoxSize, xSize,ySize,zSize);
	overlayRoot = NULL;
	
	setLocation(x, y, z);
	setVector(destination, location.x, location.y, location.z);


	//Turret data
	turretAngle = 0;
	turretMaxAngle = PI/32.0;
	turretMinAngle = -PI/128.0;
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

	char *filepath = "data/models/test.obj";
	model = glmReadOBJ(filepath, 0);
	if(!model)
		log("Tank model loading failed");

	//glmUnitize(model);
	//glmFacetNormals(model);
	//glmVertexNormals(model, 180.0);
	//glmLinearTexture(model);

}


Scout::~Scout(void)
{
}
