#include "TerrainManager.h"

TerrainManager::TerrainManager(void)
{
	//seed = 123;
	seed = time(NULL);
	srand(seed);
	
	//world = new TerrainGen();
	root = NULL;
	newChunk(0,0);

	//newChunk(0,1);
	//newChunk(1,0);
	//newChunk(1,1);
	//newChunk(-1,-1);
	//initTerrainVBO(root);
}

void TerrainManager::newChunk(int x, int z)
{
	Vector2 location;
	setCoord(location, x, z);
	TerrainGen *t = new TerrainGen(location);
	addToList(t, location);
	writeTerrainToFile(t, location);

	tile ti;
	if(t->getTileDataFromFile(ti, location, 100, 100))
		log("Tile output from file: " + dtos(ti.y)+"\n");
}

void TerrainManager::writeTerrainToFile(TerrainGen *t, Vector2 location)
{
	string fileName = "terrain/"+itos(location.x)+""+itos(location.y)+".txt";
	const char * f = fileName.c_str();
	FILE * file = fopen(f, "w");

	//Data storage order
		//TerrainType
		//Height
		//Gradient

	for(int z = 0; z < MAX_WORLD_SIZE-1; z++)
		for(int x = 0; x < MAX_WORLD_SIZE-1; x++)
		{
			fprintf(file, "%d %f %f ", t->getTerrainType(x,z), t->getTerrainHeight(x,z), t->getTerrainGradient(x,z));
			//string tData = itos(t->getTerrainType(x,z))+","+dtos(t->getTerrainHeight(x,z))+","+dtos(t->getTerrainGradient(x,z))+",";
			//const char * td = tData.c_str();
			//fputs(td, file);
		}
	fclose(file);
}

void TerrainManager::setMovePath(Actor *target, Vector3 &targetLocation)
{
	//Start with neutral values
	root->world->resetNodes();

	//Actor * target = getActorByID(selectedID);
	int updateCount = 0;
	int i = targetLocation.x/root->world->nodeSpread, j = targetLocation.z/root->world->nodeSpread;						//Index holders for world->nodes
	bool done = false;

	//Remove any move paths from the target actor
	//if(target->getPathRoot())
	target->resetNodePath();

	//If the target distance is close enough then we don't need to do long range pathfinding to find it
	if(findDistance(target->getLocation().x, target->getLocation().z, targetLocation.x, targetLocation.z) <= 
		sqrt((double)(pow(root->world->nodeSpread, 2.0)+pow(root->world->nodeSpread, 2.0))))
	{
		nodePath last;
		setCoord(last.nodeData, targetLocation.x, targetLocation.z);
		target->addNodePath(last);		//Final Destination*/
		return;
	}

	//Inital pass through the node list
	//Find all nodes closest to the final destination
	//And set them to zero
	if(root->world->nodes[i][j].incentive != -2)
		root->world->nodes[i][j].incentive = findDistance(i, j, targetLocation.x/root->world->nodeSpread, targetLocation.z/root->world->nodeSpread);

	if(root->world->nodes[i+1][j].incentive != -2)
		root->world->nodes[i+1][j].incentive = findDistance(i+1, j, targetLocation.x/root->world->nodeSpread, targetLocation.z/root->world->nodeSpread);

	if(root->world->nodes[i][j+1].incentive != -2)
		root->world->nodes[i][j+1].incentive = findDistance(i, j+1, targetLocation.x/root->world->nodeSpread, targetLocation.z/root->world->nodeSpread);

	if(root->world->nodes[i+1][j+1].incentive != -2)
		root->world->nodes[i+1][j+1].incentive = findDistance(i+1, j+1, targetLocation.x/root->world->nodeSpread, targetLocation.z/root->world->nodeSpread);


	//Add the incentive values for the rest of the grid
	while(!done)
	{
		updateCount = 0;
		double newIncentive;
		//Pass through all nodes in the grid
		for(int z = 0; z < MAX_WORLD_SIZE/root->world->nodeSpread; z++)
			for(int x = 0; x < MAX_WORLD_SIZE/root->world->nodeSpread; x++)
			{
				//Make sure the node we are checking hasn't been used yet
				//if(world->nodes[x][z].incentive == -1)
					//Check all nodes around the current node
					for(int a = -1; a <= 1; a++)
						for(int b = -1; b <= 1; b++)
						{
							//Make sure the nearby node we are checking is valid
							if((0 <= x+a && x+a < MAX_WORLD_SIZE/root->world->nodeSpread) && (0 <= z+b && z+b < MAX_WORLD_SIZE/root->world->nodeSpread) && (a != 0 || b != 0))
							{
								//Is this node next to a node we updated on the last passthrough?
								if(root->world->nodes[x+a][z+b].incentive >= 0)
								{
									newIncentive = root->world->nodes[x+a][z+b].incentive+findDistance(x, z, x+a, z+b);
									if(root->world->nodes[x][z].incentive == -1 || root->world->nodes[x][z].incentive > newIncentive)
									{
										root->world->nodes[x][z].incentive = newIncentive;
									
									//Updating current node and immediately exiting the closest for loop
									//world->nodes[x][z].incentive = world->nodes[x+a][z+b].incentive+findDistance(x, z, x+a, z+b);
										updateCount++;
									}
								}
							}
						}
			}
		if(updateCount == 0)
			done = true;
	}
	

	//All of the nodes have now been identified based on their distance from the end destination
	//Now we want to trace these nodes from the object's start location to the end
	done = false;
	nodePath smallest, current;							//Smallest found node and the current node being looked at
	i = target->getLocation().x/root->world->nodeSpread;
	j = target->getLocation().z/root->world->nodeSpread;						//Index holders for world->nodes
	
	//Find the inital node closest to the object
	smallest = root->world->nodes[i][j];
	current = root->world->nodes[i+1][j];

	if(current.incentive < smallest.incentive)
		smallest = current;

	current = root->world->nodes[i][j+1];
	if(current.incentive < smallest.incentive)
		smallest = current;

	current = root->world->nodes[i+1][j+1];
	if(current.incentive < smallest.incentive)
		smallest = current;


	while(!done)
	{
		//Position of the node that is being searched around
		int xPos = smallest.nodeData.x/root->world->nodeSpread;
		int yPos = smallest.nodeData.y/root->world->nodeSpread;

		double smallestIncentive = 99999;

		//Search all the nodes adjacent to the index node
		for(int a = -1; a <= 1; a++)
			for(int b = -1; b <= 1; b++)
			{
				//Make sure the nearby nodes being checked are within a valid range
				if((0 <= xPos+a && xPos+a < MAX_WORLD_SIZE/root->world->nodeSpread) && (0 <= yPos+b && yPos+b < MAX_WORLD_SIZE/root->world->nodeSpread) && (a != 0 || b != 0))
				{
					//Does the node have the incentive value we are looking for?
					//This means that the node is a possible node for the movement path
					if(root->world->nodes[xPos+a][yPos+b].incentive < smallestIncentive && root->world->nodes[xPos+a][yPos+b].incentive > 0)
					{
						smallestIncentive = root->world->nodes[xPos+a][yPos+b].incentive;
						smallest = root->world->nodes[xPos+a][yPos+b];
					}
				}
			}
			target->addNodePath(smallest);
			if(smallest.incentive < 1.45)
				done = true;
	}
	
	nodePath last;
	setCoord(last.nodeData, targetLocation.x, targetLocation.z);
	target->addNodePath(last);		//Final Destination*/
	
}

tile TerrainManager::getTileData(int x, int z)
{
	return root->world->getTileData(x, z);
}

double TerrainManager::getHeight(int x, int z)
{
	return root->world->getTerrainHeight(x, z);
}

int TerrainManager::getType(int x, int z)
{
	return root->world->getTerrainType(x, z);
}

double TerrainManager::getSpecificHeight(double x, double z)
{
	return root->world->getSpecificTerrainHeight(x, z);
}

void TerrainManager::initTerrainVBO(TerrainList *world)
{
	//glGenBuffers(1, world->list);

}

void TerrainManager::drawTerrain()
{
	TerrainList *target = root;

	while(target != NULL)
	{
		//glCallList(target->list);
		drawTerrainChunk(0, target->world, target->locationID);

		target = target->next;
	}
}

void TerrainManager::drawTerrainChunk(GLuint list, TerrainGen * world2, Vector2 &location)
{
	//glNewList(list, GL_COMPILE);
	int offsetX = (MAX_WORLD_SIZE-1)*location.x, offsetZ = (MAX_WORLD_SIZE-1)*location.y;
	//int i = 0;
	//bool restart = true;
	//int z = 0;
	for(int i = 0; i < 5; i++) {
		glBindTexture(GL_TEXTURE_2D, testTex[i]);
		for(int z = 0; z < MAX_WORLD_SIZE-1; z++)
		{
			for(int x = 0; x < MAX_WORLD_SIZE-1; x++)
			{
				if(world2->getTerrainType(x,z) != i)
					continue;

				double h1 = world2->getTerrainHeight(x,z);
				double h2 = world2->getTerrainHeight(x+1,z);
				double h3 = world2->getTerrainHeight(x+1,z+1);
				double h4 = world2->getTerrainHeight(x,z+1);

				Vector3 n1, n2, normal1, normal2;
				//Get normal vector for the first triangle
				setVector(n1, 0, h3-h2, 1);
				setVector(n2, -1, h1-h2, 0);
				normal1 = crossProduct(n2, n1);
				normalize(normal1);

				//Get normal vector for the second triangle
				setVector(n1, 1, h3-h4, 0);
				setVector(n2, 0, h1-h4, -1);
				normal2 = crossProduct(n1, n2);
				normalize(normal2);

				float xTex = .1, zTex = .1, texStep = .5;

				xTex = (float)(x % 2)/2.2;
				zTex = (float)(z % 2)/2.2;

				glBegin(GL_TRIANGLES);
					glNormal3d(normal1.x, normal1.y, normal1.z);
					glTexCoord2f(xTex, zTex);						glVertex3f(x+offsetX,	h1,z+offsetZ);
					glTexCoord2f(xTex + texStep, zTex + texStep);	glVertex3f(x+offsetX+1, h3,z+offsetZ+1);
					glTexCoord2f(xTex + texStep, zTex);				glVertex3f(x+offsetX+1,	h2,z+offsetZ);
					
					glNormal3d(normal2.x, normal2.y, normal2.z);
					
					glTexCoord2f(xTex, zTex);						glVertex3f(x+offsetX,	h1,z+offsetZ);
					glTexCoord2f(xTex, zTex + texStep);				glVertex3f(x+offsetX,	h4,z+offsetZ+1);
					glTexCoord2f(xTex + texStep, zTex + texStep);	glVertex3f(x+offsetX+1, h3,z+offsetZ+1);
				glEnd();
			}
			
		}
		//glEndList();
	}
}



//Linked List Functions-------------------
void TerrainManager::addToList(TerrainGen *world, Vector2 &location)
{
	if(!root)
	{
		root = (TerrainList *) malloc(sizeof(TerrainList));
		root->world = world;

		root->locationID.x = location.x;
		root->locationID.y = location.y;
		
		root->list = glGenLists(1);
		//drawTerrainChunk(root->list, root->world, root->locationID);
		root->next = NULL;
	}
	else
	{
		TerrainList * target = root;

		while(target->next != NULL)
			target = target->next;

		target->next = (TerrainList *) malloc(sizeof(TerrainList));
		target = target->next;
		target->world = world;
		target->locationID = location;

		target->list = glGenLists(1);
		//drawTerrainChunk(target->list, target->world, target->locationID);

		target->next = NULL;
	}
}

void TerrainManager::deleteFromList(Vector2 &locationID)
{
	TerrainList * target = root;
	if(root->locationID.x == locationID.x && root->locationID.y == locationID.y)
	{
		root = target->next;
		return;
	}

	while(target->next != NULL)
	{
		if(target->next->locationID.x == locationID.x && target->next->locationID.y == locationID.y)
		{
			if(target->next->next != NULL)
				target->next = target->next->next;
			else
				target->next = NULL;
			return;
		}

		target = target->next;
	}
	log("Attempted deletion of Terrain ID "+itos(locationID.x) + ", "+itos(locationID.y)+" failed");
}
//----------------------

TerrainManager::~TerrainManager(void)
{
}
