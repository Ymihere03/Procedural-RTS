#include "ActorManager.h"


TerrainGen * world;
int nextID = 0, listSize = 0;

struct ActorList
{
	ActorList *next;
	Actor * actor;
	int id;
};

ActorList *root;

ActorManager::ActorManager(){}

ActorManager::ActorManager(TerrainGen &w)
{
	world = &w;

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

	for(int i = 0; i < listSize; i++)
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
			if(checkHitBoxes(id, p1, p2))
			{
				getActorByID(id)->kill();
				target->actor->kill();
			}

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
	int currentIncentive = 0, updateCount = 0;
	bool done = false;

	//Remove any move paths from the target actor
	target->resetNodePath();

	//Inital pass through the node list
	//Find all nodes closest to the final destination
	for(int z = 0; z < MAX_WORLD_SIZE/32; z++)
		for(int x = 0; x < MAX_WORLD_SIZE/32; x++)
		{
			if(world->nodes[x][z].incentive != -2)
				if(abs(world->nodes[x][z].nodeData.x - targetLocation.x) < 32 && abs(world->nodes[x][z].nodeData.y - targetLocation.z) < 32)
					world->nodes[x][z].incentive = currentIncentive;
		}
	currentIncentive++;

	//Add the incentive values for the rest of the grid
	while(!done)
	{
		updateCount = 0;
		//Pass through all nodes in the grid
		for(int z = 0; z < MAX_WORLD_SIZE/32; z++)
			for(int x = 0; x < MAX_WORLD_SIZE/32; x++)
			{
				//Make sure the node we are checking hasn't been used yet
				if(world->nodes[x][z].incentive == -1)
					//Check all nodes around the current node
					for(int a = -1; a <= 1; a++)
						for(int b = -1; b <= 1; b++)
						{
							//Make sure the nearby node we are checking is valid
							if((0 <= x+a && x+a < MAX_WORLD_SIZE/32) && (0 <= z+b && z+b < MAX_WORLD_SIZE/32))
							if(world->nodes[x+a][z+b].incentive != -2 && a != 0 && b != 0)
							{
								//Is this node next to a node we updated on the last passthrough?
								if(world->nodes[x+a][z+b].incentive == currentIncentive-1)
								{
									//Updating current node and immediately exiting the closest for loop
									world->nodes[x][z].incentive = currentIncentive;
									updateCount++;
									a = 2;
									b = 2;
								}
							}
						}
			}
		if(updateCount == 0)
			done = true;
		currentIncentive++;
	}
	

	//All of the nodes have now been identified based on their distance from the end destination
	//Now we want to trace these nodes from the object's start location to the end
	/*done = false;
	nodePath *next = (nodePath *) malloc(sizeof(nodePath));
	currentIncentive = 99999;
	for(int z = 0; z < MAX_WORLD_SIZE/32; z++)
		for(int x = 0; x < MAX_WORLD_SIZE/32; x++)
		{
			//Is the node we are checking is valid?
			if(world->nodes[x][z].incentive != -2)
				//Is the node we are checking next to the object's location??
				if(abs(world->nodes[x][z].nodeData.x - target->getLocation().x) < 32 && abs(world->nodes[x][z].nodeData.y - target->getLocation().z) < 32)
					//Find the correct direction for the move path
					if(world->nodes[x][z].incentive < currentIncentive)
					{
						*next = world->nodes[x][z];
						currentIncentive = world->nodes[x][z].incentive;
					}
		}

	target->addNodePath(*next);
	currentIncentive--;

	//Trace the rest of the path from the first correct path node
	while(!done)
	{
		//Check all nodes around the current node
		for(int a = -1; a <= 1; a++)
			for(int b = -1; b <= 1; b++)
			{
				//Make sure the nearby node we are checking is valid
				if((0 <= next->nodeData.x+a && next->nodeData.x+a < MAX_WORLD_SIZE/32) && (0 <= next->nodeData.y+b && next->nodeData.y+b < MAX_WORLD_SIZE/32))
				if(a != 0 && b != 0)
				{
					//Is this node a possible node to consider?
					if(world->nodes[(int)next->nodeData.x+a][(int)next->nodeData.y+b].incentive == currentIncentive)
					{
						//Updating current node and immediately exiting the closest for loop
						*next = world->nodes[(int)next->nodeData.x+a][(int)next->nodeData.y+b];

						target->addNodePath(*next);
						a = 2;
						b = 2;
					}
				}
			}
		currentIncentive--;
		if(currentIncentive == -1)
			done = true;
	}

	target->addNodePath(targetLocation);		//Final Destination
	*/
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


//Linked List Functions-------------------
void ActorManager::addToList(Actor * actor)
{
	if(listSize == 0)
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
	
	listSize++;
}

void ActorManager::deleteFromList(int id)
{
	ActorList * target = root;
	if(root->id == id)
	{
		root = target->next;
		listSize--;
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
			listSize--;
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
