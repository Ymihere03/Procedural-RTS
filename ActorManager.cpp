#include "ActorManager.h"


int nextID = 0;

struct ActorList
{
	ActorList *next;
	Actor * actor;
	int id;
};

ActorList *root;

ActorManager::ActorManager(TerrainGen * w)
{
	world = w;

	for(int i = 0; i < 5; i++)
	{
		int x = getRandomAsI(100)+200;
		int z = getRandomAsI(100)+200;
		Actor * a = new Tank(x, *world->getTerrainHeight(x,z), z, 1, 1, 1);
		addToList(a);
	}

}

bool ActorManager::checkHitBoxes(int &id, Vector3 &ray1, Vector3 &ray2)
{
	ActorList * target = root;

	while(target != NULL)
	{
		if(target->actor->checkHitBox(ray1, ray2))
		{
			id = target->id;
			return true;
		}
		target = target->next;
	}
	return false;
}

void ActorManager::updateObjects(int timeElapsed)
{
	ActorList * target = root;
	Vector3 p1, p2;
	int id;
	while(target != NULL)
	{
		//Save beginning location
		setVector(p1, target->actor->getLocation().x, target->actor->getLocation().y, target->actor->getLocation().z);

		//Update the object
		target->actor->update(timeElapsed, world->getSpecificTerrainHeight(target->actor->getLocation().x, target->actor->getLocation().z));
		
		//Save updated location
		setVector(p2, target->actor->getLocation().x, target->actor->getLocation().y, target->actor->getLocation().z);

		if(target->actor->type == "Bullet")
		{
			if(checkHitBoxes(id, p1, p2))
			{
				getActorByID(id)->kill();
				target->actor->kill();
			}
			if(target->actor->isDead())
			{
				Actor *e = new GroundExplosion(target->actor->getLocation(), target->actor->getFacing());
				addToList(e);
			}
		}

		//Check if the actor is dead
		if(target->actor->isDead())
			deleteFromList(target->id);
		
		target = target->next;
	}
	/*for(actorIT = actorList.begin(); actorIT != actorList.end(); actorIT++)
		(*actorIT)->move(1, world->getSpecificTerrainHeight((*actorIT)->getLocation().x, (*actorIT)->getLocation().z));
	*/
}

void ActorManager::drawObjects()
{
	ActorList * target = root;
	while(target != NULL)
	{
		target->actor->draw();
		target = target->next;
	}
	/*for(actorIT = actorList.begin(); actorIT != actorList.end(); actorIT++)
		(*actorIT)->draw();*/
}

Actor * ActorManager::getActorByID(int id)
{
	ActorList * target = root;
	while(target != NULL)
	{
		if(target->id == id)
			return target->actor;

		target = target->next;
	}
	log("Attempted retrival of Actor ID "+itos(id)+" failed");
	return NULL;
}

void ActorManager::setMovePath(int selectedID, Vector3 &targetLocation)
{
	//Start with neutral values
	world->resetNodes();

	Actor * target = getActorByID(selectedID);
	int updateCount = 0;
	int i = targetLocation.x/32, j = targetLocation.z/32;						//Index holders for world->nodes
	bool done = false;

	//Remove any move paths from the target actor
	//if(target->getPathRoot())
	target->resetNodePath();

	//Inital pass through the node list
	//Find all nodes closest to the final destination
	//And set them to zero
	if(world->nodes[i][j].incentive != -2)
		world->nodes[i][j].incentive = findDistance(i, j, targetLocation.x/32, targetLocation.z/32);

	if(world->nodes[i+1][j].incentive != -2)
		world->nodes[i+1][j].incentive = findDistance(i+1, j, targetLocation.x/32, targetLocation.z/32);

	if(world->nodes[i][j+1].incentive != -2)
		world->nodes[i][j+1].incentive = findDistance(i, j+1, targetLocation.x/32, targetLocation.z/32);

	if(world->nodes[i+1][j+1].incentive != -2)
		world->nodes[i+1][j+1].incentive = findDistance(i+1, j+1, targetLocation.x/32, targetLocation.z/32);


	//Add the incentive values for the rest of the grid
	while(!done)
	{
		updateCount = 0;
		double newIncentive;
		//Pass through all nodes in the grid
		for(int z = 0; z < MAX_WORLD_SIZE/32; z++)
			for(int x = 0; x < MAX_WORLD_SIZE/32; x++)
			{
				//Make sure the node we are checking hasn't been used yet
				//if(world->nodes[x][z].incentive == -1)
					//Check all nodes around the current node
					for(int a = -1; a <= 1; a++)
						for(int b = -1; b <= 1; b++)
						{
							//Make sure the nearby node we are checking is valid
							if((0 <= x+a && x+a < MAX_WORLD_SIZE/32) && (0 <= z+b && z+b < MAX_WORLD_SIZE/32) && (a != 0 || b != 0))
							{
								//Is this node next to a node we updated on the last passthrough?
								if(world->nodes[x+a][z+b].incentive >= 0)
								{
									newIncentive = world->nodes[x+a][z+b].incentive+findDistance(x, z, x+a, z+b);
									if(world->nodes[x][z].incentive == -1 || world->nodes[x][z].incentive > newIncentive)
									{
										world->nodes[x][z].incentive = newIncentive;
									
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
	i = target->getLocation().x/32;
	j = target->getLocation().z/32;						//Index holders for world->nodes
	
	//Find the inital node closest to the object
	smallest = world->nodes[i][j];
	current = world->nodes[i+1][j];

	if(current.incentive < smallest.incentive)
		smallest = current;

	current = world->nodes[i][j+1];
	if(current.incentive < smallest.incentive)
		smallest = current;

	current = world->nodes[i+1][j+1];
	if(current.incentive < smallest.incentive)
		smallest = current;


	while(!done)
	{
		//Position of the node that is being searched around
		int xPos = smallest.nodeData.x/32;
		int yPos = smallest.nodeData.y/32;

		double smallestIncentive = 99999;

		//Search all the nodes adjacent to the index node
		for(int a = -1; a <= 1; a++)
			for(int b = -1; b <= 1; b++)
			{
				//Make sure the nearby nodes being checked are within a valid range
				if((0 <= xPos+a && xPos+a < MAX_WORLD_SIZE/32) && (0 <= yPos+b && yPos+b < MAX_WORLD_SIZE/32) && (a != 0 || b != 0))
				{
					//Does the node have the incentive value we are looking for?
					//This means that the node is a possible node for the movement path
					if(world->nodes[xPos+a][yPos+b].incentive < smallestIncentive && world->nodes[xPos+a][yPos+b].incentive > 0)
					{
						smallestIncentive = world->nodes[xPos+a][yPos+b].incentive;
						smallest = world->nodes[xPos+a][yPos+b];
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

void ActorManager::shoot(int id)
{
	ActorList * target = root;
	while(target != NULL)
	{
		if(target->id == id)
		{
			if(target->actor->isReadyToShoot())
			{
				Actor * b = new Bullet(target->actor->getLocation(), target->actor->getFacing());
				addToList(b);
			}
		}
		target = target->next;
	}
}

int ActorManager::getNextID()
{
	return nextID++;
}

//Get Terrain Data functions
/*double ** ActorManager::getWorldCamTrack()
{
	return world->getCamTrack();
}

double ActorManager::getWorldTerrainHeight(int x, int z)
{
	return *world->getTerrainHeight(x, z);
}

double ActorManager::getSpecificTerrainHeight(double x, double z)
{
	return world->getSpecificTerrainHeight(x,z);
}

int ActorManager::getWorldTerrainType(int x, int z)
{
	return world->getTerrainType(x, z);
}*/



//Linked List Functions-------------------
void ActorManager::addToList(Actor * actor)
{
	if(!root)
	{
		root = (ActorList *) malloc(sizeof(ActorList));
		root->actor = actor;
		root->id = getNextID();
		//root->prev = NULL;
		root->next = NULL;
		//last = root;
	}
	else
	{
		ActorList * target = root;

		while(target->next != NULL)
			target = target->next;

		target->next = (ActorList *) malloc(sizeof(ActorList));
		target = target->next;
		target->actor = actor;
		target->id = getNextID();
		//root->prev = NULL;
		target->next = NULL;

		//last = target;
	}
}

void ActorManager::deleteFromList(int id)
{
	ActorList * target = root;
	if(root->id == id)
	{
		root = target->next;
		return;
	}

	while(target->next != NULL)
	{
		if(target->next->id == id)
		{
			if(target->next->next != NULL)
				target->next = target->next->next;
			else
			{
				target->next = NULL;
				//last = target;
			}
			return;
		}

		target = target->next;
	}
	log("Attempted deletion of Actor ID "+itos(id)+" failed");
}
//----------------------

ActorManager::~ActorManager(void)
{
}
