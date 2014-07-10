#include "ActorManager.h"

ActorManager::ActorManager(TerrainManager *t)
{
	root = NULL;
	nextID = 0;
	tM = t;

	//Team 1
	Actor * a = new Scout(10, tM->getHeight(10,8), 8, 1);
	addToList(a);
	a = new Artillery(10, tM->getHeight(10,12), 12, 1);
	addToList(a);
	a = new BaseTank(12, tM->getHeight(12,10), 10, 1);
	addToList(a);
	a = new BaseTank(8, tM->getHeight(8,10), 10, 1);
	addToList(a);

	//Team 2
	a = new Scout(24, tM->getHeight(24,22), 22, 2);
	addToList(a);
	a = new Artillery(24, tM->getHeight(24,26), 26, 2);
	addToList(a);
	a = new BaseTank(26, tM->getHeight(26,24), 24, 2);
	addToList(a);
	a = new BaseTank(22, tM->getHeight(22,24), 24, 2);
	addToList(a);

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

void ActorManager::checkBlastProximity(int directHitId, Vector3 origin, double blastRadius)
{
	ActorList * target = root;
	while(target != NULL)
	{
		if(target->id == directHitId) {
			target = target->next;
			continue;
		}

		double dist = find3dDistance(target->actor->getLocation(), origin);
		
		if(dist <= blastRadius/2)
			target->actor->takeDamage(2);
		else if(dist <= blastRadius)
			target->actor->takeDamage(1);

		target = target->next;
	}
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
		
		//User control to adjust turret angle
		if(target->id == selectedID) {
			if(keys['R']) {
				target->actor->turretAngleAdjust(.005);
			}
			if(keys['F']) {
				target->actor->turretAngleAdjust(-.005);
			}
		}
		
				//Trying to fix a bug
				double testVar = tM->getSpecificHeight(target->actor->getLocation().x, target->actor->getLocation().z);
				if (testVar <= 0) {
					tM->getSpecificHeight(target->actor->getLocation().x, target->actor->getLocation().z);
				}

		//Update the object
		target->actor->update(timeElapsed, tM->getSpecificHeight(target->actor->getLocation().x, target->actor->getLocation().z),
			tM->getTileData(target->actor->getLocation().x, target->actor->getLocation().z));

		if(keys[VK_SPACE] && target->id == selectedID)
			shoot(target->id);
		
		//Save updated location
		setVector(p2, target->actor->getLocation().x, target->actor->getLocation().y, target->actor->getLocation().z);

		

		if(target->actor->type == "Bullet")
		{
			int directHitId = -1;
			if(checkHitBoxes(id, p1, p2))
			{
				getActorByID(id)->takeDamage(3);
				target->actor->kill();
				directHitId = id;
			}
			if(target->actor->isDead())
			{
				log("bullet asploded at "+dtos(target->actor->getLocation().x)+", "+dtos(target->actor->getLocation().z)+"\n");
				Actor *e = new GroundExplosion(target->actor->getLocation(), target->actor->getFacing(), tM->getTileData(target->actor->getLocation().x, target->actor->getLocation().z));
				addToList(e);
				checkBlastProximity(directHitId, target->actor->getLocation(), target->actor->getExplosionRadius());
			}
		}

		if(target->actor->type == "Tank" && target->actor->isDead())
		{
			Actor *e = new TankExplosion(target->actor->getLocation(), target->actor->getFacing(), target->actor->getTeam());
			addToList(e);
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
		if(target->actor->isVisible())
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
				if(target->actor->shoot())
				{
					Actor * b = new Bullet(target->actor->getTurretLocation(), target->actor->getFacing());
					addToList(b);
				}
			}
		}
		target = target->next;
	}
}

void ActorManager::updateActorLOS(int activeTeam)
{
	//Check Line of Sight between actors to determine which actors can be displayed
	//Line of Sight can only be drawn from actors that have Line of Sight capabilities

	//The outer loop iterates through all the actors and determines if they
	//need to be displayed using LOS rules
	ActorList * target = root;
	while(target != NULL)
	{
		//Any object not belonging to the active player may be hidden
		if(target->actor->getTeam() != activeTeam) {
			
			//Start point for LOS
			Vector3 start = target->actor->getLocation();

			//Attempt to find an object that can see the target
			ActorList * losToTarget = root;
			while(losToTarget != NULL) {
				
				//End point for LOS
				Vector3 end = losToTarget->actor->getLocation();

				//If the object can see the target then set the target to visible and stop checking
				if(find3dDistance(start, end) <= losToTarget->actor->getMaxVisionDistance() && losToTarget->actor->getTeam() == activeTeam) {
					target->actor->setVisible(true);
					break;
				}
				else
					target->actor->setVisible(false);

				losToTarget = losToTarget->next;
			}
		}
		else
			target->actor->setVisible(true);

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
	if(!root)
	{
		root = (ActorList *) malloc(sizeof(ActorList));
		root->actor = actor;
		root->id = getNextID();
		root->actor->setID(root->id);

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
		target->actor->setID(target->id);

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
