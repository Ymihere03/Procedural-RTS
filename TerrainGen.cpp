#include "TerrainGen.h"

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

TerrainGen::TerrainGen(Vector2 l)
{
	location = l;
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
	int delta = 32;
	cTrack = (double **) malloc (delta * sizeof(double *));
	if(!cTrack)
	{
		log("Memory allocation error while making camera Track\n");
		exit(1);
	}

	for(int k = 0; k <= delta; k++)
	{
		cTrack[k] = (double *) malloc (delta * sizeof(double));
		if(!cTrack[k])
		{
			log("Memory allocation error while making camera Track\n");
			exit(1);
		}
	}

	nodeSpread = 32;
	//Allocate memory for the node path list
	nodes = (nodePath **) malloc (MAX_WORLD_SIZE/nodeSpread * sizeof(nodePath *));
	if(!cTrack)
	{
		log("Memory allocation error while making camera Track\n");
		exit(1);
	}

	for(int k = 0; k <= MAX_WORLD_SIZE/nodeSpread; k++)
	{
		nodes[k] = (nodePath *) malloc (MAX_WORLD_SIZE/nodeSpread * sizeof(nodePath));
		if(!nodes[k])
		{
			log("Memory allocation error while making camera Track\n");
			exit(1);
		}
	}

	waterHeight = 3;		//Default height for water to be generated
	deepWater = waterHeight-3;
	snowHeight = 20;		//Height for snow to be generated

	biome = LAKES;//getRandomAsI(BIOME_COUNT);
	switch(biome)
	{
		//Fields with sparse trees
	case PLAINS:	pField = 65, pForest = 35, pDesert = 0;		//Probabilities of each terrain type being generated (Must sum to 100)
		persistence = .5 + getRandomAsD(8)/100;
		//avgHeight = 20+getRandomAsD(20);
		waterHeight = 0;
		//snowHeight = 0;
		featurePoints = 100;
		log("Terrain type 0 was picked.\n");
		break;

		//Mountainous with lots of trees
	case MOUNTAINS: pField = 55, pForest = 45, pDesert = 0;
		persistence = .9;// + getRandomAsD(5)/100;
		waterHeight = -100;
		//avgHeight = 100+getRandomAsD(100);
		featurePoints = 50;
		log("Terrain type 1 was picked.\n");
		break;

		//Flat deserts with some brush
	case DESERT: pField = 20, pForest = 0, pDesert = 80;
		persistence = .30 + getRandomAsD(5)/100;
		waterHeight = 0;
		//avgHeight = 20+getRandomAsD(20);
		featurePoints = 60;
		log("Terrain type 2 was picked.\n");
		break;

		//Average mix of fields and trees with extra water
	case FOREST_MIX: pField = 40, pForest = 60, pDesert = 0;
		persistence = .52 + getRandomAsD(10)/100;
		featurePoints = 60;
		log("Terrain type 3 was picked.\n");
		break;

	case LAKES:	pField = 85, pForest = 15, pDesert = 0;		//Probabilities of each terrain type being generated (Must sum to 100)
		persistence = .4 + getRandomAsD(8)/100;
		//avgHeight = 20+getRandomAsD(20);
		waterHeight = 1;
		//snowHeight = 0;
		featurePoints = 100;
		log("Terrain type 4 was picked.\n");
		break;

	default:
		log("Invalid biome "+itos(biome)+"value picked.\n");
	}
	sandHeight = waterHeight + 1;
	log("persistence: "+dtos(persistence)+"\n");


	//Initialize the terrain with a few random points and initial height values
	for(int z = 0; z < MAX_WORLD_SIZE; z++)
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			terrain[x][z].x = x;
			terrain[x][z].y = 0;
			terrain[x][z].z = z;

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

//
//  FUNCTION: modifyTerrainHeight()
//
//  PURPOSE: Creates the terrain by combining all the terrain generating algorithms
//
//  COMMENTS: 
//

void TerrainGen::modifyTerrainHeight()
{
	//Initialize the Perlin Noise map data
	HeightMap * pMap = new PerlinNoise(persistence, avgHeight, biome);
	for(int i = 0; i <= 6; i++)
	{
		pMap->create();
		combineHeightMapWithTerrain(pMap, i);
	}

	if(biome == DESERT) {
		HeightMap * vMap = new VoronoiGraph(featurePoints);
		vMap->create();
		combineHeightMapWithTerrain(vMap, 10);
	}
	
	//Smooth the terrain heights
	for(int z = 0; z < MAX_WORLD_SIZE; z++)
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			//Smooth terrain
			for(int i = 5; i >= 0; i--)
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
			//findSlope(x, z);

			//Calculate sand before water
			if(getTerrainHeight(x,z) < sandHeight)
				terrain[x][z].type = SAND;

			//Water will override the sand if it's low enough
			if(terrain[x][z].y < waterHeight) {
				//if(!(terrain[x][z].y < deepWater))
				//	terrain[x][z].y = waterHeight;
				terrain[x][z].type = WATER;
			}

			//Snow is above a certain height
			//int heightDiff = terrain[x][z].y-snowHeight;
			if(terrain[x][z].y > snowHeight) {
				terrain[x][z].type = SNOW;
			}
			else if(terrain[x][z].y > snowHeight-2) {
				if(choose(getDecimalRandom(terrain[x][z].y-snowHeight+3)-1))
					terrain[x][z].type = SNOW;
			}

			//Initialize the nodes for pathfinding
			if(x%nodeSpread == 0 && z%nodeSpread == 0)
			{
				setCoord(nodes[x/nodeSpread][z/nodeSpread].nodeData, x, z);
				if(terrain[x][z].gradient < .7 && terrain[x][z].type != 4)
					nodes[x/nodeSpread][z/nodeSpread].incentive = -1;
				else
					nodes[x/nodeSpread][z/nodeSpread].incentive = -2;
				
			}

			Vector3 n1, n2;
			//Get normal vector for the first triangle
			setVector(n1, 0, terrain[x+1][z+1].y-terrain[x+1][z].y, 1);
			setVector(n2, -1, terrain[x][z].y-terrain[x+1][z].y, 0);
			terrain[x][z].normal1 = crossProduct(n2, n1);
			normalize(terrain[x][z].normal1);

			//Get normal vector for the second triangle
			setVector(n1, 1, terrain[x+1][z+1].y-terrain[x][z+1].y, 0);
			setVector(n2, 0, terrain[x][z].y-terrain[x][z+1].y, -1);
			terrain[x][z].normal2 = crossProduct(n1, n2);
			normalize(terrain[x][z].normal2);
		}
}

//
//  FUNCTION: generateTerrain()
//
//  PURPOSE: Uses the Diamond-Square algorithm to generate the base terrain type structure
//
//  COMMENTS: This randomizes the placement of terrain types like forests, plains
//

void TerrainGen::generateTerrain(int dX, int dZ)
{
	for(int z = 0; z < MAX_WORLD_SIZE; z += dZ)
		for(int x = 0; x < MAX_WORLD_SIZE; x += dX)
		{
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
				else
				{
					Vector2 adjLoc;
					setCoord(adjLoc, location.x-1, location.y);
					getTileDataFromFile(*parents[2], adjLoc, MAX_WORLD_SIZE+(x-dX/2), z+dZ/2);
					parents[2]->y = 0;
				}

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

//
//  FUNCTION: perturbance()
//
//  PURPOSE: Terrain modifying algorithm that skews the terrain formations with perlin noise
//
//  COMMENTS: 
//

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
			//getTerrainHeight(x, z) = getTerrainHeight(x+(int)dZ, z+(int)dX);
		}
		
		xMax = 0;
	}
}

//
//  FUNCTION: terrainTypeDistribution()
//
//  PURPOSE: Picks a random number based on the types of terrain used in the generator and the 
//				probability for each type to appear
//
//  COMMENTS: 
//

int TerrainGen::terrainTypeDistribution()
{
	//Chooses a random terrain type based on the given probability distributions
	//of each terrain type
	int choice = rand() % 100;

	if(choice <= pField)
		return FIELD;	//Field
	
	if(choice <= pField + pForest)
		return FOREST;	//Forest

	return SAND;	//Desert
}

//
//  FUNCTION: combineHeightMapWithTerrain()
//
//  PURPOSE: Combines a height map onto the current terrain
//
//  COMMENTS: 
//

void TerrainGen::combineHeightMapWithTerrain(HeightMap *hMap, int iter)
{
	for(int z = 0; z < MAX_WORLD_SIZE; z++)
		for(int x = 0; x < MAX_WORLD_SIZE; x++)
		{
			//Combine only the high height values of the large features
			if(biome == MOUNTAINS && iter < 3)
				terrain[x][z].y += *hMap->getMap(x, z);
			//Combine only the low values of the small features
			if(biome == MOUNTAINS && iter == 5)
				terrain[x][z].y += *hMap->getMap(x, z)/2;

			if(biome != MOUNTAINS)
				terrain[x][z].y += *hMap->getMap(x, z);
		}
}

//
//  FUNCTION: thermalErosion()
//
//  PURPOSE: Uses thermal erosion algorithms to modify the terrain
//
//  COMMENTS: Moves terrain around based on nearby slopes
//				Currently not a complete algorithm
//

void TerrainGen::thermalErosion()
{
	/*double erodeThreshold, heightDiff;

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

					heightDiff = getTerrainHeight(x, z) - getTerrainHeight(x+a, z+b);
					if(abs(heightDiff) > erodeThreshold)
					{
				

					}
				}
		}*/
}

//
//  FUNCTION: makeCamTrack()
//
//  PURPOSE: Sets the height values for the camera so it floats above the terrain
//
//  COMMENTS: 
//

void TerrainGen::makeCamTrack()
{
	double delta = (MAX_WORLD_SIZE-1)/32.0;	//Distance between the inital points of the track
	for(int z = 0; z <= MAX_WORLD_SIZE/4; z++)
		for(int x = 0; x <= MAX_WORLD_SIZE/4; x++)
		{
			if(getTerrainType(x*delta, z*delta) == WATER)
				cTrack[x][z] = waterHeight+2;
			else
				cTrack[x][z] = getTerrainHeight(x*delta, z*delta) + 2;
		}
}

//
//  FUNCTION: smooth()
//
//  PURPOSE: Smooths the terrain height values by averaging the height of a tile with it's neighbor tiles
//
//  COMMENTS: 
//

void TerrainGen::smooth(int x, int z, int spread)
{
	double corners, sides, center;

	if(x <= spread-1 || z <= spread-1 || x >= MAX_WORLD_SIZE-spread || z >= MAX_WORLD_SIZE-spread){}
	else
	{
		corners = (getTerrainHeight(x-spread, z-spread) + getTerrainHeight(x+spread, z+spread) + getTerrainHeight(x+spread, z-spread) + getTerrainHeight(x-spread, z+spread))/16;
		sides = (getTerrainHeight(x-spread, z) + getTerrainHeight(x+spread, z) + getTerrainHeight(x, z-spread) + getTerrainHeight(x, z+spread))/8;
		center = getTerrainHeight(x, z)/4;
	
		terrain[x][z].y = corners + sides + center;
	}
}

void TerrainGen::findSlope(int x, int z)
{
	double h[4], max;

	
	h[0] = abs(getTerrainHeight(x, z)-getTerrainHeight(x-1, z));
	h[1] = abs(getTerrainHeight(x, z)-getTerrainHeight(x+1, z));
	h[2] = abs(getTerrainHeight(x, z)-getTerrainHeight(x, z-1));
	h[3] = abs(getTerrainHeight(x, z)-getTerrainHeight(x, z+1));
	
	max = h[0];
	for(int i = 1; i < 4; i++)
	{
		if(max < h[i])
			max = h[i];
	}

	//log(dtos(max)+"\n");
	terrain[x][z].gradient = max;

}

//
//  FUNCTION: resetNodes()
//
//  PURPOSE: Clears the node path overlay of all values and returns them to their default values
//
//  COMMENTS: 
//

void TerrainGen::resetNodes()
{
	for(int z = 0; z < MAX_WORLD_SIZE/nodeSpread; z++)
		for(int x = 0; x < MAX_WORLD_SIZE/nodeSpread; x++)
		{
			if(nodes[x][z].incentive != -2)
				nodes[x][z].incentive = -1;
		}
}

//
//  FUNCTION: setTerrainData()
//
//  PURPOSE: Saves stat values of the terrain after the generation is complete
//
//  COMMENTS: 
//

void TerrainGen::setTerrainData(int x, int z)
{
	if(getTerrainHeight(x, z) < minHeight)
		minHeight = getTerrainHeight(x, z);

	else if(getTerrainHeight(x, z) > maxHeight)
		maxHeight = getTerrainHeight(x, z);
}

void TerrainGen::getColor(int x, int z)
{
	switch(terrain[x][z].type)
	{
		case FIELD: glColor3f(0.1f,1,0.1f);		//Fields
			break;
		case FOREST: glColor3f(0,0.5f,0);		//Forest
			break;
		case SAND: glColor3f(0.7f,0.7f,0);		//Desert
			break;
		case SNOW: glColor3f(0.8f,0.8f,0.8f);	//Snow
			break;
		case WATER: glColor3f(0,0,.6f);			//Water
			break;
	}
}

double ** TerrainGen::getCamTrack()
{
	return cTrack;
}

double TerrainGen::getTerrainHeight(int x, int z)
{
	//double val = 0;
	if(x < 0 || z < 0 || x > MAX_WORLD_SIZE-1 || z > MAX_WORLD_SIZE-1)
		return 0;
	else
		return terrain[x][z].y;
}

int TerrainGen::getTerrainType(int x, int z)
{
	return terrain[x][z].type;
}

double TerrainGen::getTerrainGradient(int x, int z)
{
	return terrain[x][z].gradient;
}

Vector3 TerrainGen::getN1(int x, int z)
{
	return terrain[x][z].normal1;
}

Vector3 TerrainGen::getN2(int x, int z)
{
	return terrain[x][z].normal2;
}

//
//  FUNCTION: getSpecificTerainHeight()
//
//  PURPOSE: Returns the height of the terrain between the tile data points by checking for the interestion of the plane
//
//  COMMENTS: 
//

double TerrainGen::getSpecificTerrainHeight(double x, double z)
{
	Vector3 target;
	Vector3 p1, p2, p3, p4, l1, l2;

	setVector(l1, x, -500, z);
	setVector(l2, x, 700, z);

	setVector(p1, (int)x, getTerrainHeight((int)x,(int)z), (int)z);
	setVector(p2, (int)x+1, getTerrainHeight((int)x+1,(int)z), (int)z);
	setVector(p3, (int)x+1, getTerrainHeight((int)x+1,(int)z+1), (int)z+1);
	setVector(p4, (int)x, getTerrainHeight((int)x,(int)z+1), (int)z+1);

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
		return -1;
	}
}

bool TerrainGen::getTileDataFromFile(tile &t, Vector2 chunkLocation, int tileX, int tileZ)
{
	string fileName = "terrain/"+itos(chunkLocation.x)+""+itos(chunkLocation.y)+".txt";
	string test = "terrain/00.txt";
	if(fileName != test)
		return false;
	//string fileName = "00.txt";
	//string fileName2 = "00.txt";
	const char * f = fileName.c_str();
	FILE * file = fopen(f, "r");
	if(file == NULL){
		log("Couldn't open file "+fileName);
		return false;
	}


	int n = 0;
	float f1=0, f2=0;
	for(int i = 0; i < tileX*(MAX_WORLD_SIZE-1)+tileZ; i++) {
		fscanf(file, "%d", &n);
		fscanf(file, "%E", &f1);
		fscanf(file, "%E", &f2);
	}



	t.type = n;
	t.y = f1;
	t.gradient = f2;

	return true;
}

TerrainGen::~TerrainGen(void)
{
}
