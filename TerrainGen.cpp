#include "TerrainGen.h"

int dX, dZ;			//Delta numbers used to denote the current generation resolution
const int seed = time(NULL);
double pField, pForest, pDesert;		//Probabilities of each terrain type being generated (Must sum to 100)
double persistence;
int waterHeight;		//Height above the lowest terrain point for water to be generated
int snowHeight;		//Height below the highest terrain point snow to be generated
int sandHeight;		//Height for shoreline to be generated
int featurePoints;	//Detail parameter for the Voronoi Graph, a higher number creates larger and taller mountain structures
double minHeight, maxHeight;

struct tile {
	int type;			//Type of terrain
	double height;		//Height at location
	float gradient;		//Slope
};


double ** cTrack;		//Height values for the camera track
tile ** terrain;		//Data storage for terrain


//
//  FUNCTION: TerrainGen() (Constructor)
//
//  PURPOSE: Creates a terrain map
//
//  COMMENTS:
//		This constructor performs the following jobs to create a fully functional terrain map:
//
//			Allocates the required memory for terrain
//			Initialize terrain heights to zero
//			Initialize some terrain types to random values
//			Fills in the rest of the terrain based on the random values
//			Creates a Perlin Noise height map
//			Uses the height map values to add heights to the terrain
//			Uses an iteration of the height map generation to create the camera track
//			Smooths the terrain
//			Adjusts terrain types to include water and snow if necessary
//

TerrainGen::TerrainGen(void)
{
	//Clear out the log file
	FILE * pFile = fopen("log.txt", "w");
	fclose(pFile);
	
	srand(time(NULL));

	dX = (int)(MAX_WORLD_SIZE/pow(2.0, 3));
	dZ = (int)(MAX_WORLD_SIZE/pow(2.0, 3));

	//Allocate memory space for the height map
	terrain = (tile **) malloc ((MAX_WORLD_SIZE) * sizeof(tile *));
	if(!terrain)
	{
		log("Memory allocation error while making terrain\n");
		exit(1);
	}

	for(int k = 0; k <= MAX_WORLD_SIZE; k++)
	{
		terrain[k] = (tile *) malloc ((MAX_WORLD_SIZE) * sizeof(tile));
		if(!terrain[k])
		{
			log("Memory allocation error while making terrain\n");
			exit(1);
		}
	}

	//Allocate memory for camera Track data
	cTrack = (double **) malloc (9 * sizeof(double *));
	if(!cTrack)
	{
		log("Memory allocation error while making camera Track\n");
		exit(1);
	}

	for(int k = 0; k <= 9; k++)
	{
		cTrack[k] = (double *) malloc (9 * sizeof(double));
		if(!cTrack[k])
		{
			log("Memory allocation error while making camera Track\n");
			exit(1);
		}
	}

	//Allocate memory for the node path list
	nodes = (nodePath **) malloc (MAX_WORLD_SIZE/32 * sizeof(nodePath *));
	if(!cTrack)
	{
		log("Memory allocation error while making camera Track\n");
		exit(1);
	}

	for(int k = 0; k <= MAX_WORLD_SIZE/32; k++)
	{
		nodes[k] = (nodePath *) malloc (MAX_WORLD_SIZE/32 * sizeof(nodePath));
		if(!nodes[k])
		{
			log("Memory allocation error while making camera Track\n");
			exit(1);
		}
	}

	
	switch(0)//getRandomAsI(4))
	{
		//Fields with sparse trees
	case 0:	pField = 75, pForest = 25, pDesert = 0;		//Probabilities of each terrain type being generated (Must sum to 100)
		persistence = .38 + getRandomAsD(7)/100;
		waterHeight = 10;
		snowHeight = 0;
		featurePoints = 100;
		log("Terrain type 0 was picked.\n");
		break;

		//Mountainous with lots of trees
	case 1: pField = 50, pForest = 50, pDesert = 0;
		persistence = .5 + getRandomAsD(5)/100;
		waterHeight = 20;
		snowHeight = 30;
		featurePoints = 40;
		log("Terrain type 1 was picked.\n");
		break;

		//Flat deserts with some brush
	case 2: pField = 25, pForest = 0, pDesert = 75;
		persistence = .30 + getRandomAsD(5)/100;
		waterHeight = 0;
		snowHeight = 0;	
		featurePoints = 100;
		log("Terrain type 2 was picked.\n");
		break;

		//Average mix of fields and trees with extra water
	case 3: pField = 60, pForest = 40, pDesert = 0;
		persistence = .45 + getRandomAsD(10)/100;
		waterHeight = 35;		//Height for water to be generated
		snowHeight = 0;		//Height for snow to be generated
		featurePoints = 40;
		log("Terrain type 3 was picked.\n");
		break;
	}
	sandHeight = waterHeight + 3;
	log("persistence: "+dtos(persistence)+"\n");


	//Initialize the terrain with a few random points and initial height values
	for(int z = 0; z < MAX_WORLD_SIZE; z++)
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			terrain[x][z].height = 0;

			if(x%dX == 0 && z%dX == 0)
				terrain[x][z].type = terrainTypeDistribution();
		}
	
	//Create Terrain data
	generateTerrain(dX, dZ);
	
	//Generate terrain height
	modifyTerrainHeight();

	//Make the track for the camera to move along
	makeCamTrack();

	log(dtos(minHeight) +","+dtos(maxHeight)+"\n");
}

void TerrainGen::modifyTerrainHeight()
{
	//Initialize the Perlin Noise map data
	PerlinNoise pMap = PerlinNoise(persistence);
	for(int octave = 1; octave < 8; octave++)
	{
		pMap.create(octave);
		combineHeightMapWithTerrain(&pMap);
	}

	//Initialize the Voronoi Graph map data
	if(persistence >= .4)
	{
		VoronoiGraph vMap = VoronoiGraph(featurePoints);
		combineHeightMapWithTerrain(&vMap);
	}
	//perturbance(8);
	
	//Smooth the terrain heights
	
	for(int z = 0; z < MAX_WORLD_SIZE; z++)
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			//Smooth terrain
			for(int i = 6; i >= 0; i--)
				smooth(x, z, i);

			//Finds the lowest and the highest point in the terrain
			setTerrainData(x, z);
		}

	//If the slope of a terrain location is over a specified amount, make that type a cliff
	//Make sure snow is generated above the specified height and
	//that water is generated below the specified height
	//Draw shore for spaces just above water
	for(int z = 0; z < MAX_WORLD_SIZE; z++)
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			findSlope(x, z);

			//if(terrain[x][z].gradient > .7)
				//terrain[x][z].type = 2;

			if(terrain[x][z].height < minHeight+waterHeight)
				terrain[x][z].type = 4;

			if(terrain[x][z].height > maxHeight-snowHeight)
				terrain[x][z].type = 3;

			if(minHeight+waterHeight < *getTerrainHeight(x,z) && *getTerrainHeight(x,z) < minHeight+sandHeight)
				terrain[x][z].type = 2;

			if(x%32 == 0 && z%32 == 0)
			{
				setCoord(nodes[x/32][z/32].nodeData, x, z);
				if(terrain[x][z].gradient < .7 && terrain[x][z].type != 4)
					nodes[x/32][z/32].incentive = -1;
				else
					nodes[x/32][z/32].incentive = -2;
				
			}
		}
}

void TerrainGen::generateTerrain(int dX, int dZ)
{
	for(int z = 0; z < MAX_WORLD_SIZE; z += dZ)
		for(int x = 0; x < MAX_WORLD_SIZE; x += dX)
		{
			
			//tile *currentParent = &terrain[x][z];

			tile *child;
			tile *parents[4];
			//Current tile being referenced
			parents[0] = &terrain[x][z];

			double pSum = 0;

			//If there is room in both directions then use four parents
			if(x < MAX_WORLD_SIZE-1 && z < MAX_WORLD_SIZE-1)			
			{
				child = &terrain[x+dX/2][z+dZ/2];		//Find child tile
				parents[1] = &terrain[x+dX][z+dZ];		//Find other parent tile
				parents[2] = &terrain[x+dX][z];	//Find other parent tile
				parents[3] = &terrain[x][z+dZ];	//Find other parent tile

				pSum += getProbabilityByType(parents[0]->type);
				pSum += getProbabilityByType(parents[1]->type);
				pSum += getProbabilityByType(parents[2]->type);
				pSum += getProbabilityByType(parents[3]->type);

				double choice = getRandomAsD(100)/100.0;
				//double choice = getRandomAsD(100);
				if(choice < getProbabilityByType(parents[0]->type)/pSum)
				//if(choice < 25)
					child->type = parents[0]->type;
				else if(choice < (getProbabilityByType(parents[0]->type)+getProbabilityByType(parents[1]->type))/pSum)
					child->type = parents[1]->type;
				else if(choice < (getProbabilityByType(parents[0]->type)+getProbabilityByType(parents[1]->type)+getProbabilityByType(parents[2]->type))/pSum)
					child->type = parents[2]->type;
				else
					child->type = parents[3]->type;
			}

			//Check if there will be a second parent in the Z direction
			if(z < MAX_WORLD_SIZE-1)			
			{
				child = &terrain[x][z+dZ/2];		//Find child tile
				parents[1] = &terrain[x][z+dZ];		//Find other parent tile
				if(x > 0)
					parents[2] = &terrain[x-dX/2][z+dZ/2];	//Find other parent tile

				if(x < MAX_WORLD_SIZE-1)
					parents[3] = &terrain[x+dX/2][z+dZ/2];	//Find other parent tile

				//If #2 spot is null then move #3 to #2
				if(!parents[2])
					parents[2] = parents[3];

				int randBound = 100;
				//If #3 is null then make sure the randomizer doesn't pick the null spot
				if(!parents[3])
					randBound = 75;

				pSum += getProbabilityByType(parents[0]->type);
				pSum += getProbabilityByType(parents[1]->type);
				pSum += getProbabilityByType(parents[2]->type);
				pSum += getProbabilityByType(parents[3]->type);
				
				double choice = getRandomAsD(100)/100.0;
				//double choice = getRandomAsD(100);
				if(choice < getProbabilityByType(parents[0]->type)/pSum)
				//if(choice < 25)
					child->type = parents[0]->type;
				else if(choice < (getProbabilityByType(parents[0]->type)+getProbabilityByType(parents[1]->type))/pSum)
					child->type = parents[1]->type;
				else if(choice < (getProbabilityByType(parents[0]->type)+getProbabilityByType(parents[1]->type)+getProbabilityByType(parents[2]->type))/pSum)
					child->type = parents[2]->type;
				else
					child->type = parents[3]->type;
			}


			pSum = 0;
			//Check if there will be a second parent in the X direction
			if(x < MAX_WORLD_SIZE-1)			
			{
				child = &terrain[x+dX/2][z];		//Find child tile
				parents[1] = &terrain[x+dX][z];		//Find other parent tile
				if(z > 0)
					parents[2] = &terrain[x+dX/2][z-dZ/2];	//Find other parent tile

				if(z < MAX_WORLD_SIZE-1)
					parents[3] = &terrain[x+dX/2][z+dZ/2];	//Find other parent tile

				//If #2 spot is null then move #3 to #2
				if(!parents[2])
					parents[2] = parents[3];

				int randBound = 100;
				//If #3 is null then make sure the randomizer doesn't pick the null spot
				if(!parents[3])
					randBound = 75;

				pSum += getProbabilityByType(parents[0]->type);
				pSum += getProbabilityByType(parents[1]->type);
				pSum += getProbabilityByType(parents[2]->type);
				pSum += getProbabilityByType(parents[3]->type);

				double choice = getRandomAsD(100)/100.0;
				//double choice = getRandomAsD(100);
				if(choice < getProbabilityByType(parents[0]->type)/pSum)
				//if(choice < 25)
					child->type = parents[0]->type;
				else if(choice < (getProbabilityByType(parents[0]->type)+getProbabilityByType(parents[1]->type))/pSum)
					child->type = parents[1]->type;
				else if(choice < (getProbabilityByType(parents[0]->type)+getProbabilityByType(parents[1]->type)+getProbabilityByType(parents[2]->type))/pSum)
					child->type = parents[2]->type;
				else
					child->type = parents[3]->type;
			}


			pSum = 0;
			

			
		}

	int newDX = 0, newDZ = 0;

	if(dX > 1)
		newDX = dX/2;
	if(dZ > 1)
		newDZ = dZ/2;
	if(dX <= 1 && dZ <= 1)
		return;

	generateTerrain(newDX, newDZ);
}

void TerrainGen::perturbance(int variation)
{
	variation *= 100;
	int xMin, xMax = 0, zMin, zMax = 0;
	
	int frequency = (int)pow(2.0, 6);				//Number of points being used on each axis direction
	int delta = MAX_WORLD_SIZE/frequency;					//Distance between the points being used
	double xVal[64], zVal[64], dZ, dX;

	for(int i = 0; i < frequency; i++)
		xVal[i] = (rand() % variation)/100;
	for(int i = 0; i < frequency; i++)
		zVal[i] = (rand() % variation)/100;

	for(int z = 0; z < MAX_WORLD_SIZE; z++)
	{
		if(z%delta == 0)
		{
			zMin = zMax;
			zMax++;
		}
		dZ = cosineInterpolate(zVal[zMin], zVal[zMax], (z%delta)/(double)delta);
		
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			if(x % delta == 0)
			{
				xMin = xMax;
				xMax++;
			}
			dX = cosineInterpolate(xVal[xMin], xVal[xMax], (x%delta)/(double)delta);
			//double dummy = dX;
			*getTerrainHeight(x, z) = *getTerrainHeight(x+(int)dZ, z+(int)dX);
		}
		
		xMax = 0;
	}
}

int TerrainGen::terrainTypeDistribution()
{
	//Chooses a random terrain type based on the given probability distributions
	//of each terrain type
	int choice = rand() % 100;

	if(choice <= pField)
		return 0;	//Field
	
	if(choice <= pField + pForest)
		return 1;	//Forest

	return 2;	//Desert
}

void TerrainGen::combineHeightMapWithTerrain(HeightMap *hMap)
{
	for(int z = 0; z < MAX_WORLD_SIZE; z++)
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			terrain[x][z].height += *hMap->getMap(x, z);
		}
}

void TerrainGen::thermalErosion()
{
	double erodeThreshold, heightDiff;

	//Iterate over the map
	for(int z = 0; z < MAX_WORLD_SIZE; z++)
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			//Iterate over the eight cells surrounding the current cell
			for(int b = -1; b <= 1; b++)
				for(int a = -1; a <= 1; a++)
				{
					if(a == 0 && b == 0)
						continue;

					heightDiff = *getTerrainHeight(x, z) - *getTerrainHeight(x+a, z+b);
					if(abs(heightDiff) > erodeThreshold)
					{
				

					}
				}
		}
}

void TerrainGen::makeCamTrack()
{
	for(int z = 0; z < 5; z++)
		for(int x = 0; x < 5; x++)
		{
			if(getTerrainType(x*256, z*256) == 4)
				cTrack[x][z] = waterHeight+25;
			else
				cTrack[x][z] = terrain[x*256][z*256].height + 10;
		}
}

void TerrainGen::smooth(int x, int z, int spread)
{
	double corners, sides, center;

	if(x <= spread-1 || z <= spread-1 || x >= MAX_WORLD_SIZE-spread || z >= MAX_WORLD_SIZE-spread)
		terrain[x][z].height = *getTerrainHeight(x, z);
	else
	{
		corners = (*getTerrainHeight(x-spread, z-spread) + *getTerrainHeight(x+spread, z+spread) + *getTerrainHeight(x+spread, z-spread) + *getTerrainHeight(x-spread, z+spread))/16;
		sides = (*getTerrainHeight(x-spread, z) + *getTerrainHeight(x+spread, z) + *getTerrainHeight(x, z-spread) + *getTerrainHeight(x, z+spread))/8;
		center = *getTerrainHeight(x, z)/4;
	
		terrain[x][z].height = corners + sides + center;
	}
}

void TerrainGen::findSlope(int x, int z)
{
	double h[4], max;

	
	h[0] = abs(*getTerrainHeight(x, z)-*getTerrainHeight(x-1, z));
	h[1] = abs(*getTerrainHeight(x, z)-*getTerrainHeight(x+1, z));
	h[2] = abs(*getTerrainHeight(x, z)-*getTerrainHeight(x, z-1));
	h[3] = abs(*getTerrainHeight(x, z)-*getTerrainHeight(x, z+1));
	
	max = h[0];
	for(int i = 1; i < 4; i++)
	{
		if(max < h[i])
			max = h[i];
	}

	//log(dtos(max)+"\n");
	terrain[x][z].gradient = max;

}

void TerrainGen::resetNodes()
{
	for(int z = 0; z < MAX_WORLD_SIZE/32; z++)
		for(int x = 0; x < MAX_WORLD_SIZE/32; x++)
		{
			if(nodes[x][z].incentive != -2)
				nodes[x][z].incentive = -1;
		}
}

void TerrainGen::setTerrainData(int x, int z)
{
	if(*getTerrainHeight(x, z) < minHeight)
		minHeight = *getTerrainHeight(x, z);

	else if(*getTerrainHeight(x, z) > maxHeight)
		maxHeight = *getTerrainHeight(x, z);
}

void TerrainGen::getColor(int x, int z)
{
	switch(terrain[x][z].type)
	{
		case 0: glColor3f(0.1f,1,0.1f);		//Fields
			break;
		case 1: glColor3f(0,0.5f,0);		//Forest
			break;
		case 2: glColor3f(0.7f,0.7f,0);		//Desert
			break;
		case 3: glColor3f(0.8f,0.8f,0.8f);	//Snow
			break;
		case 4: glColor3f(0,0,.6f);			//Water
			break;
	}
}

double ** TerrainGen::getCamTrack()
{
	return cTrack;
}

double * TerrainGen::getTerrainHeight(int x, int z)
{
	double val = 0;
	if(x < 0 || z < 0 || x > MAX_WORLD_SIZE-1 || z > MAX_WORLD_SIZE-1)
		return &val;
	return &terrain[x][z].height;
}

int TerrainGen::getTerrainType(int x, int z)
{
	return terrain[x][z].type;
}

double TerrainGen::getTerrainGradient(int x, int z)
{
	return terrain[x][z].gradient;
}

double TerrainGen::getSpecificTerrainHeight(double x, double z)
{
	Vector3 target;
	Vector3 p1, p2, p3, p4, l1, l2;

	setVector(l1, x, -500, z);
	setVector(l2, x, 700, z);

	setVector(p1, (int)x, *getTerrainHeight((int)x,(int)z), (int)z);
	setVector(p2, (int)x+1, *getTerrainHeight((int)x+1,(int)z), (int)z);
	setVector(p3, (int)x+1, *getTerrainHeight((int)x+1,(int)z+1), (int)z+1);
	setVector(p4, (int)x, *getTerrainHeight((int)x,(int)z+1), (int)z+1);

	if(checkLineIntersect(p2, p3, p1, l1, l2, target) || checkLineIntersect(p4, p3, p1, l1, l2, target))
		return target.y;
	else return -1;
}

int TerrainGen::interpolateType(int type1, int type2, double x)
{
	double t = (1-cos(x * PI)) * .5;

	double p = rand() % 100;

	if(p > t*100)
		return type1;
	else
		return type2;
}

double TerrainGen::getProbabilityByType(int terrainType)
{
	switch(terrainType)
	{
	case 0:
		return pField;
	case 1:
		return pForest;
	case 2:
		return pDesert;
	default:
		log("YAY???");
		return -1;
	}
}

TerrainGen::~TerrainGen(void)
{
}
