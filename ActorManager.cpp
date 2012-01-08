#include "ActorManager.h"


int nextID = 0;

struct ActorList
{
	ActorList *next;
	Actor * actor;
	int id;
};

ActorList *root;

ActorManager::ActorManager(TerrainManager *t)
{
	tM = t;

	for(int i = 0; i < 5; i++)
	{
		int x = getRandomAsI(100)+200;
		int z = getRandomAsI(100)+200;
		Actor * a = new Tank(x, tM->getHeight(x,z), z, 1, 1, 1);
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
		target->actor->update(timeElapsed, tM->getSpecificHeight(target->actor->getLocation().x, target->actor->getLocation().z));
		
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

		root->next = NULL;
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

		target->next = NULL;
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
				target->next = NULL;
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
