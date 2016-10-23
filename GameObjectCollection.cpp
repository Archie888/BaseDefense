#include ".\GameObjectcollection.h"
#include "main.h"

GameObjectCollection::GameObjectCollection(void)
{
	background = NULL;
	hgePartMan = new hgeParticleManager(60.0f); //float fps
	preRenderPartMan = new hgeParticleManager(60.0f); //float fps
	player1_cannonSight = NULL;
	player2_cannonSight = NULL;
	player1_MGSight = NULL;
	player2_MGSight = NULL;
	clearTimer = 0.0f;
	projectilesBeginIndex = 0;	
}

GameObjectCollection::~GameObjectCollection(void)
{
	delete hgePartMan; hgePartMan = NULL;
	delete preRenderPartMan; preRenderPartMan = NULL;
}

void GameObjectCollection::render(void)
{
	if (background)
		background->render();

	for (int i = 0; i < NUMBER_OF_RENDER_LISTS; i++)
	{
		//early particles
		if (i == 1)
		{
			preRenderPartMan->Update(timerDelta);
			preRenderPartMan->Render();
		}
		//late particles
		else if (i == NUMBER_OF_RENDER_LISTS - 1)
		{
			hgePartMan->Update(timerDelta);
			hgePartMan->Render();
		}

		list<GameObject *>::iterator current = renderLists[i].begin();
		
 		for (; current != renderLists[i].end(); current++)
		{
			if (!(*current) || (*current)->x == -1.5883997e+038)
				break;
			if ((*current)->visible) //Jos objekti on näkyvissä
				(*current)->render();
			if (game.show_collisionspheres)
				(*current)->renderCollisionSpheres();
			if (game.show_locations &&  render_type_location[(*current)->type])
				(*current)->renderCross();
		}
	}
	
	if (player1_cannonSight && player1_cannonSight->visible)
	{
		if (game.player_1.tank && game.player_1.tank->isAtBattleArea())
		{
			player1_cannonSight->render();
			if (game.show_locations && game.objects.render_type_location[GameObject::SIGHT])
				player1_cannonSight->renderCross();
		}
	}
	else if (player1_MGSight && player1_MGSight->visible)
	{
		if (game.player_1.tank && game.player_1.tank->isAtBattleArea())
		{
			player1_MGSight->render();
			if (game.show_locations && game.objects.render_type_location[GameObject::SIGHT])
				player1_MGSight->renderCross();
		}
	}
	if (game.player_1.tank && game.player_1.pointer.visible)
	{	
		game.player_1.pointer.render();
		Pointer::tankSymbol->RenderEx(game.player_1.pointer.x, game.player_1.pointer.y, game.player_1.tank->direction + HALF_PI);
		Pointer::turretSymbol->RenderEx(game.player_1.pointer.x, game.player_1.pointer.y, game.player_1.tank->turret.direction + HALF_PI);
	}
	if (game.two_player_game)
	{
		if (player2_cannonSight && player2_cannonSight->visible)
		{
			if (game.player_2.tank && game.player_2.tank->isAtBattleArea())
			{
				player2_cannonSight->render();
				if (game.show_locations && game.objects.render_type_location[GameObject::SIGHT])
					player2_cannonSight->renderCross();
			}
		}
		else if (player2_MGSight && player2_MGSight->visible)
		{
			if (game.player_2.tank && game.player_2.tank->isAtBattleArea())
			{				
				player2_MGSight->render();
				if (game.show_locations && game.objects.render_type_location[GameObject::SIGHT])
					player2_MGSight->renderCross();
			}
		}
		if (game.player_2.tank && game.player_2.pointer.visible)
		{
			game.player_2.pointer.render();
			Pointer::tankSymbol->RenderEx(game.player_2.pointer.x, game.player_2.pointer.y, game.player_2.tank->direction + HALF_PI);
			Pointer::turretSymbol->RenderEx(game.player_2.pointer.x, game.player_2.pointer.y, game.player_2.tank->turret.direction + HALF_PI);
		}
	}
	hge->Gfx_SetTransform();
}

void GameObjectCollection::processAI(void)
{
	//Start drawing things into the background. Stopped in processInteraction().
	hge->Gfx_BeginScene(background->target);
	background->image->Render(background->x, background->y);
	list<GameObject *>::iterator current = AIObjects.begin();
	list<GameObject *>::iterator end = AIObjects.end();
}

void GameObjectCollection::processAction(void)
{
	background->processAction();
}


void GameObjectCollection::processInteraction(void)
{
	list<GameObject *>::iterator current;
	list<GameObject *>::iterator other;
	
	for (current = objects.begin(); current != objects.end(); current++)
	{
		//deletoidaan tämä objekti jos se on merkitty tuhottavaksi.
		if (!(*current)->isDestroyed()) //if marked as destroyed
		{
			if ((*current)->computer_controlled)
				(*current)->processAI();

			(*current)->processAction();

			//käydään kaikki listassa tämän objektin jälkeen olevat objektit läpi ja katsotaan törmääkö tämä niihin.
			other = current;
			other++;

			//No need to collide with projectiles, as they have been collided with if necessary. Take notice that the objects created after this object won't be colliisondetected before all projectiles die.

			if ((*current)->type != GameObject::PROJECTILE)
			{
				for (; other != objects.end(); other++)
				{
					if (!(*other)->isDestroyed()) //siis tankkihan tuhoutuessaan on vielä "elossa" eli ei 'destroyed'... voe voe...
					{
						testCollision((*current), (*other));
						(*current)->checkObject((*other));
						(*other)->checkObject((*current));
					}
				}
			}
			//else //Jos näin tekee, projektiileja ei tuhota laisinkaan.
			//	break;
			(*current)->stopCheckingObjects();

			if (!(*current)->isDestroyed()) //if marked as destroyed
			{
				(*current)->processInteraction();
			}	
		}
		else
		{
			deleteGameObject(current);
			continue;
		}
	}

	//Optimization possibility: Could be done every 10th frame or so.
	
	clearTimer += timerDelta;
	if (clearTimer > 0.1f)
	{
		clearTimer = 0.0f;

		for (current = killList.begin(); current != killList.end(); current++)
		{
			if ((*current)->getReferenceCount() <= 0)
			{
				list <GameObject *>::iterator helpi = current;
				current--;
				delete (*helpi); 
				(*helpi) = NULL;
				killList.erase(helpi);
			}
		}
	}

	//Stop drawing things into the background
	hge->Gfx_EndScene();
}

void GameObjectCollection::testCollision(GameObject * object1, GameObject * object2)
{
	if (object2->type != GameObject::PROJECTILE)
	{
		if (object1->type == GameObject::TANK)
			int u = 2;

		int max1 = object1->getCollisionSphereCount();
		int max2 = object2->getCollisionSphereCount();

		//optimization:

		if (max1 == 1 && max2 == 1)
		{
			if (object1->getCollisionSphere().testHit(object2->getCollisionSphere()))
			{
				object1->collide(object2, ArcPoint(object1->getLocation(), object2->getLocation()));
			}
			return;
		}

		//Slower test:
	
		for (int i = 0; i < max1; i++)
		{
			CollisionSphere cs1 = object1->getCollisionSphere(i);
			for (int j = 0; j < max2; j++)
			{
				if (object1->type == GameObject::TANK &&
					object2->type == GameObject::TANK)
					int u = 2;

				CollisionSphere cs2 = object2->getCollisionSphere(j);
				if (cs1.testHit(cs2))
				{
					object1->collide(object2, ArcPoint(object1->getLocation(), object2->getLocation()));
					return;
				}
			}
		}
	}
	else
	{
		Projectile * projectile = static_cast<Projectile *>(object2);
		//No shooting oneself.
		if (projectile->shooter == object1)
			return;
		GameObject * object = object1;
		int max1 = object1->getCollisionSphereCount();

		if (object1->type == GameObject::TANK &&
			projectile->projectileType == Projectile::TANK_CANNON_PROJECTILE)
			int u = 2;

		for (int i = 0; i < max1; i++)
		{
			CollisionSphere targetCollisionSphere = object1->getCollisionSphere(i);

			//is projectile inside object1's collisionsphere?
			if (targetCollisionSphere.testHit(projectile->getCollisionSphere()))
			{
				object1->collide(object2, ArcPoint(object1->getLocation(), object2->getLocation()));
				return;
			}

			//Vaiheluettelo:
			//1. projektiilin suunta selville.
			//2. Johdetaan siitä 90 asteen kulmassa oleva halkaisijajana kohteen törmäysympyrälle.
			//   -1. Laita: x0 = r * cos(projectiledir + HALF_PI), y0 = r * sin(projectiledir + HALF_PI), piste 2: x1 = r * cos(pd - HP), y1 = r * sin(pd - hp)
			//3. Piste 0: halkaisijan vasen laita, piste 1: halkaisijan oikea laita, piste 2: projektiilijanan vasen laita, piste 3: projektiilijanan oikea laita. Järjestyksellä ei liene oikeastaan merkitystä, mutta esim. noin.
			//4. Janojen vektorit selville (x ja y -pituudet).
			//5. Lasketaan leikkausarvo halkaisijajanasta Cramerin säännön mukaan.
			//6. Jos 0 <= leikkausarvo <= 1 ei ole tosi, janat eivät leikkaa, eli törmäystä ei tapahdu.
			//7. Jos törmäys tapahtuu, lasketaan leikkaussijainti ja annetaan se kohdeobjektin törmäysfunktiolle.

			float P0x, P0y, P1x, P1y, P2x, P2y, P3x, P3y;

			//Vaiheet, toteutus:
			//1. projektiilin suunta selville.
			Direction projectileDirection = projectile->direction;		

			//2. Johdetaan siitä 90 asteen kulmassa oleva halkaisijajana kohteen törmäysympyrälle.
			//3. Kohdetörmäysympyrän halkaisijan päätepisteet:
			Distance radius = object1->getCollisionSphereRadius(i);
			float projectileDirectionPlusHALF_PI = projectileDirection + HALF_PI;
			P0x = targetCollisionSphere.location.x + radius * cos(projectileDirectionPlusHALF_PI);
			P0y = targetCollisionSphere.location.y + radius * sin(projectileDirectionPlusHALF_PI);
			float projectileDirectionMinusHALF_PI = projectileDirection - HALF_PI;
			P1x = targetCollisionSphere.location.x + radius * cos(projectileDirectionMinusHALF_PI);
			P1y = targetCollisionSphere.location.y + radius * sin(projectileDirectionMinusHALF_PI);

			//3. Projektiilijanan päätepisteet:
			P2x = projectile->previousLocation.x;
			P2y = projectile->previousLocation.y;
			P3x = projectile->x;
			P3y = projectile->y;

			//4. Janojen vektorit selville (x ja y -pituudet).
			float S1x = P1x - P0x;
			float S1y = P1y - P0y;
			float S2x = P3x - P2x;
			float S2y = P3y - P2y;

			//5. Lasketaan leikkausarvo halkaisijajanasta Cramerin säännön mukaan.
			float value1 = (-S1y * (P0x - P2x) + S1x * (P0y - P2y)) / (-S2x * S1y + S1x * S2y);
			float value2 = (S2x * (P0y - P2y) - S2y * (P0x - P2x)) / (-S2x * S1y + S1x * S2y);

			//6. Jos 0 <= leikkausarvo <= 1 ei ole tosi, janat eivät leikkaa, eli törmäystä ei tapahdu.
			if (value1 >= 0.0f  && value1 <= 1.0f &&
				value2 >= 0.0f  && value2 <= 1.0f )
			{
				//7. Jos törmäys tapahtuu, lasketaan leikkaussijainti ja annetaan se kohdeobjektin törmäysfunktiolle.
				Location collisionLocation;
				collisionLocation.x = P0x + value1 * S1x;
				collisionLocation.y = P0y + value1 * S1y;
				object1->collide(projectile, ArcPoint(object1->getLocation(), collisionLocation));
				return;
			}			
		}
	}
}

void GameObjectCollection::fireParticleSystem(hgeParticleSystem * particleSystem, float x, float y, float direction, bool prerender)
{
	particleSystem->info.fDirection = direction;
	float temp1 = particleSystem->info.fSizeEnd;
	float temp2 = particleSystem->info.fSizeStart;
	particleSystem->info.fSizeEnd *= GameObject::objectSize;
	particleSystem->info.fSizeStart *= GameObject::objectSize;
	if (prerender)
		preRenderPartMan->SpawnPS(&(particleSystem->info), x, y);
	else
		hgePartMan->SpawnPS(&(particleSystem->info), x, y);
	particleSystem->info.fSizeEnd = temp1;
	particleSystem->info.fSizeStart = temp2;
}

void GameObjectCollection::fireParticleSystem(hgeParticleSystemInfo psInfo, float x, float y, float direction, bool prerender)
{
	psInfo.fDirection = direction;
	float temp1 = psInfo.fSizeEnd;
	float temp2 = psInfo.fSizeStart;
	psInfo.fSizeEnd *= GameObject::objectSize;
	psInfo.fSizeStart *= GameObject::objectSize;
	if (prerender)
		preRenderPartMan->SpawnPS(&psInfo, x, y);
	else
		hgePartMan->SpawnPS(&psInfo, x, y);
	psInfo.fSizeEnd = temp1;
	psInfo.fSizeStart = temp2;
}

void GameObjectCollection::addGameObject(GameObject * object, int renderOrder)
{
	//Where to in main objects list?
	if (object->type == GameObject::PROJECTILE ||
		object->type == GameObject::MILITARY_UNIT)
	{
		objects.push_back(object);
	}
	else
	{
		//the object is inserted into the list before the
		//iterator
		list<GameObject *>::iterator it = objects.begin();
		//for this I should use a vector really...
		for (int i = 0; i < projectilesBeginIndex; i++)
			it++;
		this->objects.insert(it, object);
		projectilesBeginIndex++;
	}

	if (isAIObject(object))
	{
		AIObjects.push_back(object);
		object->AIObjectsIterator = --AIObjects.end();
	}
	if (isActionObject(object))
	{
		actionObjects.push_back(object);
		object->actionObjectsIterator = --actionObjects.end();
	}
	if (object->type == GameObject::FOOT_SOLDIER)
	{
		footSoldiers.push_back(object);
		object->combatObjectsIterator = --footSoldiers.end();
	}

	//tank?
	if (object->type == GameObject::TANK)
	{
		tanks.push_back(object);
		object->combatObjectsIterator = --tanks.end();
	}

	int index = 0;
	if (renderOrder != -1)
	{
		index = renderOrder;
		object->setRenderOrder(renderOrder);
	}
	else
		index = object->getRenderOrder();

	if (object->type != GameObject::MILITARY_UNIT)
	{
		renderLists[index].push_back(object);
		object->renderListIterator = --(renderLists[index].end());
	}
}

void GameObjectCollection::deleteGameObject(list<GameObject *>::iterator & current)
{
	//was it a non-projectile?
	if ((*current)->type != GameObject::PROJECTILE &&
		(*current)->type != GameObject::MILITARY_UNIT)
	{
		//projectilesStart--;
		projectilesBeginIndex--;
	}
	//AI (*current)?
	if (isAIObject((*current)))
	{
		AIObjects.erase((*current)->AIObjectsIterator);
	}
	//action (*current)?
	if (isActionObject((*current)))
	{
		actionObjects.erase((*current)->actionObjectsIterator);
	}
	//footsoldier?
	if ((*current)->type == GameObject::FOOT_SOLDIER)
	{
		footSoldiers.erase((*current)->combatObjectsIterator);
	}
	//tank?
	if ((*current)->type == GameObject::TANK)
	{
		tanks.erase((*current)->combatObjectsIterator);
	}

	int index = (*current)->getRenderOrder();

	renderLists[index].erase((*current)->renderListIterator);

	//kill list insertion or straight killing?
	if ((*current)->getReferenceCount() > 0)
	{
		killList.push_back(*current);
	}
	else
	{
		delete (*current);
	}

	list <GameObject *>::iterator helpi = current;
	current--;
	objects.erase(helpi);
}

void GameObjectCollection::clear()
{	
	//This is called when all hell breaks loose. No, seriously, always between levels.
	if (background)
	{
		delete background;
		background = NULL;
	}

	list<GameObject *>::iterator current;

	for (current = objects.begin(); current != objects.end(); current++)
	{
		deleteGameObject(current);
	}

	for (current = killList.begin(); current != killList.end(); current++)
	{
		delete (*current);
		(*current) = NULL;
	}

	illuminators.clear();
	objects.clear(); //all objects except background, sights and player arrows
	killList.clear();
	AIObjects.clear(); //footsoldiers and non-player tanks
	actionObjects.clear(); //moving objects
	footSoldiers.clear();
	tanks.clear();
	for (int i = 0; i < NUMBER_OF_RENDER_LISTS; i++)
	{
		renderLists[i].clear();
	}
	projectilesBeginIndex = 0;
	player1_cannonSight = NULL;
	player2_cannonSight = NULL;
	player1_MGSight = NULL;
	player2_MGSight = NULL;
	hgePartMan->KillAll();
	preRenderPartMan->KillAll();
}

void GameObjectCollection::release()
{
	clear();
}

GameObject * GameObjectCollection::getNearestEnemy(CombatObject * thisObject, GameObject::TYPE enemyType, float minObservingAngle, float maxObservingAngle)
{
	Location thisLocation = thisObject->getLocation();
	bool checkAngles = false;
	if (minObservingAngle != -1.0f && maxObservingAngle != -1.0f)
		checkAngles = true;
	int thisSide = thisObject->side;
	int enemySide;
	if (thisSide ==  CombatObject::DEFENDER_SIDE)
		enemySide = CombatObject::ATTACKER_SIDE;
	else
		enemySide = CombatObject::DEFENDER_SIDE;
	float smallestDistance = 500000.0f;
	GameObject * returnValue = NULL;
	list<GameObject *>::iterator current;
	list<GameObject *>::iterator end;

	if (enemyType == GameObject::FOOT_SOLDIER)
	{
		current = footSoldiers.begin();
		end = footSoldiers.end();
	}

	if (enemyType == GameObject::TANK)
	{
		current = tanks.begin();
		end = tanks.end();
	}

	for (; current != end; current++)
	{
		if ((*current) == thisObject)
			continue;

		CombatObject * enemy = static_cast<CombatObject *>(*current);
		if (enemy->side == thisSide)
			continue;

		if (checkAngles)
		{
			Angle enemyAngle = thisLocation.angle(enemy->getLocation());
			if (enemyAngle < minObservingAngle ||
				enemyAngle > maxObservingAngle)
				continue;
		}
		
		float dist;
		dist = thisLocation.squaredDistance(enemy->getLocation());
		if (dist < smallestDistance)
		{
			smallestDistance = dist;
			returnValue = (*current);
		}	
	}

	return returnValue;
}

void GameObjectCollection::listAll(void)
{
	hudfont->SetScale(0.7f);

	float rendx = 400.0f;
	float y = 10.0f;
	float spacing = 16.0f;
	int a = 0;

	list<GameObject *>::iterator current;

	hge->Gfx_SetTransform(rendx, 10.0f, 0,0,0, 0.7f, 0.7f);
	for (current = objects.begin(); current != objects.end(); current++)
	{
		if ((*current)->type == GameObject::PROJECTILE)
		{
			Projectile * p = static_cast<Projectile *>(*current);
			hudfont->printf(rendx, y += spacing, "%s dist=%.2f", (*current)->dbgString, p->distance);
		}
		else
			hudfont->printf(rendx, y += spacing, "%s %s", (*current)->dbgString, (*current)->isDestroyed()?"DESTROYED":"ALIVE");

		if (a++ == 80)
		{
			rendx += 60.0f;
			a = 0;
			y = 10.0f;
		}
	}
	hge->Gfx_SetTransform();
}

void GameObjectCollection::startIterating(list<GameObject *> & listOf)
{
	it = listOf.begin();
}
bool GameObjectCollection::reachedEnd(void)
{
	return (it == objects.end());
}
GameObject * GameObjectCollection::getNext(void)
{
	list<GameObject *>::iterator ithelp = it;
	it++;
	return (*ithelp);
}

bool GameObjectCollection::isAIObject(GameObject * object)
{
	bool ret = false;
	if ((object->type == GameObject::TANK ||
		object->type == GameObject::FOOT_SOLDIER ||
		object->type == GameObject::BASE) &&
		object->computer_controlled)
	{
		ret = true;
	}
	return ret;
}

bool GameObjectCollection::isActionObject(GameObject * object)
{
	bool ret = false;
	if (object->type == GameObject::TANK ||
		object->type == GameObject::FOOT_SOLDIER ||
		object->type == GameObject::PROJECTILE ||
		object->type == GameObject::TREE ||
		object->type == GameObject::BASE ||
		object->type == GameObject::SEPARATED_BODY_PART ||
		object->type == GameObject::FIRE_SUPPORT)
	{
		ret = true;
	}
	return ret;
}
void GameObjectCollection::setOnTop(GameObject * setUpper, GameObject * setLower)
{
	if (!setUpper || !setLower)
		int u = 2;

	int renderListNumber = setUpper->getRenderOrder();
	int otherListNumber = setLower->getRenderOrder();

	if (renderListNumber != otherListNumber)
		return; //not yet possible

	list<GameObject *>::iterator current = renderLists[renderListNumber].begin();
	list<GameObject *>::iterator end = renderLists[renderListNumber].end();

	list<GameObject *>::iterator top = renderLists[renderListNumber].end();
	list<GameObject *>::iterator bottom = renderLists[renderListNumber].end();

	int counter = 0;
	int bottomNumber = -1;
	int topNumber = -1;
	bool bottom_found = false;
	bool top_found = false;
	
 	for (; current != end; current++)
	{		
		if ((*current) == setUpper)
		{
			top = current;
			top_found = true;
			topNumber = counter;
		}
		if ((*current) == setLower)
		{
			bottom = current;
			bottom_found = true;
			bottomNumber = counter;
		}

		counter++;

		if (top_found && bottom_found)
			break;
	}

	if (top_found && bottom_found)
	{
		//need a switch?
		if (topNumber > bottomNumber)
		{			
			setUpper->renderListIterator = renderLists[renderListNumber].insert(bottom, setLower);
			renderLists[renderListNumber].erase(bottom);			
			setLower->renderListIterator = renderLists[renderListNumber].insert(top, setUpper);								
			renderLists[renderListNumber].erase(top);
		}
	}
}