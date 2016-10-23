#include ".\base.h"
#include "main.h"

Height Base::WEAPON_HEIGHT = 15.0f;
Velocity Base::MG_TURNING_SPEED = 1.0f;
Distance Base::COLLISIONSPHERE_RADIUS_FRONT_BUNKER = 14.0f;
Distance Base::COLLISIONSPHERE_RADIUS_BARRICADE = 20.0f;
Time Base::MG_RATE_OF_FIRE = 0.1f;
float Base::MG_ACCURACY = 0.2f;
const Distance Base::MG_BARREL_LENGTH = 28.0f;
const Angle Base::MG_MIN_ANGLE = ONE_AND_A_HALF_PI - 1.25f;
const Angle Base::MG_MAX_ANGLE = ONE_AND_A_HALF_PI + 1.25f;

#define DAMAGE_MULTIPLIER_EASY 0.7f
#define DAMAGE_MULTIPLIER_NORMAL 0.8f
#define DAMAGE_MULTIPLIER_HARD 1.0f
#define TANK_MG_DAMAGE 0.1f//0.05f
#define ASSAULT_RIFLE_DAMAGE 0.05f
#define HAND_GRENADE_DAMAGE 5.0f
#define TANK_SHELL_DAMAGE 7.0f
#define AT_ROCKET_DAMAGE 5.0f
#define MORTAR_SHELL_DAMAGE 3.0f
#define MISSILE_DAMAGE 3.0f
#define NAPALM_DAMAGE 0.1f
#define TANK_COLLISION_DAMAGE 1.0f
#define SHIELD_VALUE_CRITICAL 50.0f
#define MG_RATE_OF_FIRE_DEF 0.08f
#define MG_ACCURACY_DEF 0.1f
#define BASE_HEIGHT 15.0f

//smoke:
#define OFF_SMOKE_DISTANCE 200.0f

void Base::init(void)
{

}

void Base::release(void)
{
}

Base::Base(void)
{
	MessageBox(NULL, "Wrong Base::constructor", "i", MB_OK);
}

Base::Base(CombatObject::SIDE side, Location location, Direction direction)
{
	set(side, location, direction);	
}

void Base::set(CombatObject::SIDE side, Location location, Direction direction)
{
	CombatAIObject::set(GameObject::BASE, location, direction, side);
	setHeight(BASE_HEIGHT);
	computer_controlled = true;
	tex = hge->Texture_Load("Base.png");
	image = new hgeAnimation(tex, 2, 0.0f, 0.0f, 0.0f, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	image->SetHotSpot(TEXTURE_WIDTH / 2.0f + 1.0f, TEXTURE_HEIGHT / 2.0f);
	setLocation(location);
	setDirection(direction);
	collisionSphereCount = COLLISIONSPHERECOUNT;
	this->enemyTarget = NULL;
	gunShotIntervalTimer = 0.0f;
	gunner_alive = true;
	this->damagemsg = false;
	this->destroyedmsg = false;

	//AI n' stuff:
	target_near_sight = false;
	target_in_sight = false;
	firing = false;

	//Misc
	whichSound = Game::SOUND_BASE_MG_FIRE_1;

	if (side == CombatObject::DEFENDER_SIDE)
	{
		//setDefensePhase(DEFENSE_PHASE_OBSERVE);
	}
	else
	{
		//Enemy base?
	}
	
	tankMG.image->SetFrame(1);
	tankMG.image->SetHotSpot(0.0f, 4.5f);
	tankMG.setLocation(getLocation());
	tankMG.setDirection(getBattleMainDirection());

	hge->Gfx_BeginScene(game.objects.background->target);
	game.objects.background->modify(image, x, y, direction);
	hge->Gfx_EndScene();

	image->SetFrame(1);

	shield = 100.0f;

	orders = STATE_DEFEND;
	setState((STATE)orders);
}

Base::~Base(void)
{
	if (image)
	{
		delete image;
		image = NULL;
	}
	hge->Texture_Free(tex);
}

void Base::processAI(void)
{
	if (!gunner_alive)
		return;

	setEnemyTargetProperties();
	switch (state)
	{
	case STATE_DEFEND: defend(); break;
	case STATE_FIGHT: fight(); break;
	default: setOriginalState(); break;
	}
}

void Base::processAction(void)
{
	processTurning();
}

void Base::processInteraction(void)
{
	if (firing && isEnemyTarget())
	{
		gunShotIntervalTimer -= timerDelta;
		if (gunShotIntervalTimer < 0.0f)
		{
			gunShotIntervalTimer = MG_RATE_OF_FIRE_DEF;
			playSoundAtLocation(game.sounds[whichSound]);

			if (getEnemyTarget())
				firingTargetLocation = ArcPoint(getEnemyTargetLocation(), randFloat(0.0f, 100.0f), getEnemyTargetDistance()).getLocation();

			Distance aimingLocationDistance = objectDistance(firingTargetLocation);

			Distance radius = randFloat(0.0f, MG_ACCURACY_DEF) * aimingLocationDistance; //TANK_MG_NO_UNACCURACY
			Angle angle = randomDirection();
			Location targetLocation = ArcPoint(firingTargetLocation, radius, angle).getLocation();
			firingTargetLocation = targetLocation;

			if (randFloat(0.0f, 1.0f) > 0.5f)
			{
				for (int i = 0; i < 1; i++)
				{
					Location targetLocation2 = ArcPoint(firingTargetLocation, sqrt(radius * aimingLocationDistance), tankMG.getDirection() + randFloat(-aimingLocationDistance/5000.0f, aimingLocationDistance/5000.0f)).getLocation();
					game.objects.addGameObject(new Projectile(this, Projectile::TANK_MG_BULLET, MGGetBarrelTipLocation(12.0f), MGGetBarrelTipHeight(), targetLocation2));
				}
			}
			else
				game.objects.addGameObject(new Projectile(this, Projectile::TANK_MG_BULLET, MGGetBarrelTipLocation(12.0f), MGGetBarrelTipHeight(), targetLocation));

			Location l = MGGetBarrelTipLocation(2.0f);
			game.objects.fireParticleSystem(Tank::tankFireMGParticleSystem2, l.x, l.y, getDirection());	
			game.objects.fireParticleSystem(Tank::tankFireMGParticleSystem, l.x, l.y, getDirection() + M_PI / 2.0f);

			whichSound++;
			if (whichSound > Game::SOUND_BASE_MG_FIRE_LAST)
			{
				whichSound = Game::SOUND_BASE_MG_FIRE_1;
			}
		}
	}

	if (shield < 20.0f && randomEvent(5.0f))
	{
		gunner_alive = false;
	}

	startChecks = true;
}

void Base::checkObject(GameObject * otherObject)
{
	if (game.currentLevel->smokeTimer > 0.0)
	{
		//Inside smoke?
		if (y < OFF_SMOKE_DISTANCE)
		{
			//Can't see a thing!
			return;
		}

		//Other object inside smoke?
		if (otherObject->y < OFF_SMOKE_DISTANCE)
		{
			//Can't see other object.
			return;
		}
	}

	if (startChecks)
	{
		startCheckingObjects();
		startChecks = false;
	}

	if (engaged)
		return;
	else if (otherObject->isCombatAIObject())
	{
		CombatAIObject * caio = static_cast<CombatAIObject *>(otherObject);

		if (!caio->isAtBattleArea())
			return;

		if (caio->side == this->side)
			return;
		else if (caio->isNeutralized())
		{
				return;
		}
		else if (caio->fleeing || caio->surrendered)
		{
			return;
		}
		if (otherObject->type == GameObject::FOOT_SOLDIER)
		{
			if (static_cast<FootSoldier *>(otherObject)->footSoldierClass == FootSoldier::MEDIC)
				return;

			Distance enemyDistance = squaredObjectDistance(otherObject);
			Direction d = objectAngle(otherObject);
			if (enemyDistance < nearestSuitableEnemyDistance || randFloat(0.0f, 1.0f) < 0.2f)		
			{				
				if (d > MG_MIN_ANGLE && d < MG_MAX_ANGLE)
				{
					nearestSuitableEnemyDistance = enemyDistance;
					suggestedEnemyTarget = otherObject;
				}
			}
		}
	}
}

void Base::collide(GameObject * other, ArcPoint hittingPoint)
{
	if (other->type == GameObject::TANK)
	{
		Tank * t = static_cast<Tank *>(other);
		if (t->isOverLapping(this))
		{
			t->speed = 0.0f;
		}
	}
	if (other->type == GameObject::FOOT_SOLDIER)
	{
		FootSoldier * fs = static_cast<FootSoldier *>(other);
		fs->moving = false;
	}
	else if (other->type == GameObject::PROJECTILE)
	{
		Projectile * pr = static_cast<Projectile *>(other);
		collide(pr, hittingPoint);
	}
	else if (other->type == GameObject::EFFECT)
	{
		Effect * e = static_cast<Effect *>(other);
		collide(e, hittingPoint);
	}
}

void Base::collide(Projectile * projectile, ArcPoint hittingPoint)
{
	float damageMultiplier = 1.0f;
	float probabilityOfGunnerDeath = 1.0f;

	switch (game.skillLevel)
	{
	case Game::SKILL_LEVEL_EASY: damageMultiplier = DAMAGE_MULTIPLIER_EASY; break;
	case Game::SKILL_LEVEL_NORMAL: damageMultiplier = DAMAGE_MULTIPLIER_NORMAL; break;
	case Game::SKILL_LEVEL_HARD: damageMultiplier = DAMAGE_MULTIPLIER_HARD; break;
	}

	damageMultiplier *= randomVariation(0.2f);


	if (projectile->projectileType == Projectile::TANK_MG_BULLET)
	{
		game.objects.addGameObject(new Effect(Effect::TANK_MG_BULLET_HIT_GROUND, hittingPoint.getLocation(), projectile->direction));		
		setDamage(TANK_MG_DAMAGE * damageMultiplier);
		projectile->hit();
	}
	else if (projectile->projectileType == Projectile::ASSAULT_RIFLE_BULLET)		
	{
		game.objects.addGameObject(new Effect(Effect::ASSAULT_RIFLE_BULLET_HIT_GROUND, hittingPoint.getLocation(), projectile->direction));
		setDamage(ASSAULT_RIFLE_DAMAGE * damageMultiplier);
		projectile->hit();
	}
	else if (projectile->projectileType == Projectile::TANK_CANNON_PROJECTILE ||
		projectile->projectileType == Projectile::AIR_TO_GROUND_MISSILE ||
		projectile->projectileType == Projectile::AT_ROCKET)
	{
		if (projectile->projectileType == Projectile::AIR_TO_GROUND_MISSILE)
		{
			if (randFloat(0.0f, 1.0f) < 7.0f)
				return;
		}

		float hsx, hsy;
		image->GetHotSpot(&hsx, &hsy);
		int imageX0 = x - hsx;
		int imageY0 = y - hsy;
		int hitX = projectile->x - imageX0;
		int hitY = projectile->y - imageY0;
		setHole(hitX, hitY);
		game.objects.addGameObject(new Effect(Effect::BASE_EXPLOSION, hittingPoint.getLocation(), 0.0f)); 

		switch (projectile->projectileType) {
		case Projectile::TANK_CANNON_PROJECTILE: setDamage(TANK_SHELL_DAMAGE * damageMultiplier); probabilityOfGunnerDeath = 0.5f; break;
		case Projectile::AIR_TO_GROUND_MISSILE: setDamage(MISSILE_DAMAGE * damageMultiplier); probabilityOfGunnerDeath = 0.3f; break;
		case Projectile::AT_ROCKET: setDamage(AT_ROCKET_DAMAGE * damageMultiplier); probabilityOfGunnerDeath = 0.5f; break;
		}
		
		if (shield < 50.0f && randFloat(0.0f, 1.0f) < probabilityOfGunnerDeath)
		{
			gunner_alive = false;
		}
		projectile->hit();
	}
	else if (projectile->projectileType == Projectile::HAND_GRENADE)		
	{
		if (hittingPoint.radius < 10.0f)
			projectile->visible = false;		
	}
}

void Base::collide(Effect * effect, ArcPoint hittingPoint)
{
	float damageMultiplier = 1.0f;
	float probabilityOfGunnerDeath = 1.0f;

	switch (game.skillLevel)
	{
	case Game::SKILL_LEVEL_EASY: damageMultiplier = DAMAGE_MULTIPLIER_EASY; break;
	case Game::SKILL_LEVEL_NORMAL: damageMultiplier = DAMAGE_MULTIPLIER_NORMAL; break;
	case Game::SKILL_LEVEL_HARD: damageMultiplier = DAMAGE_MULTIPLIER_HARD; break;
	}

	damageMultiplier *= randomVariation(0.2f);

	if (effect->collisionTimer <= 0.0f)
		return;

	if (effect->effectType == Effect::NAPALM_EXPLOSION)
	{
		Distance dist = squaredObjectDistance(effect);
		if (compareDistance(dist, effect->counter1) < 0.0f)
		{
			setDamage(NAPALM_DAMAGE * damageMultiplier);
		}

		probabilityOfGunnerDeath = 0.8f;

		if (shield < 50.0f && randFloat(0.0f, 1.0f) < probabilityOfGunnerDeath)
		{
			gunner_alive = false;
		}
	}
	if (effect->effectType == Effect::MORTARSHELL_EXPLOSION)
	{
		if (hittingPoint.radius < 100.0f)
		{
			setDamage(MORTAR_SHELL_DAMAGE * damageMultiplier);
		}

		probabilityOfGunnerDeath = 0.2f;

		if (shield < 50.0f && randFloat(0.0f, 1.0f) < probabilityOfGunnerDeath)
		{
			gunner_alive = false;
		}
	}
	if (effect->effectType == Effect::HAND_GRENADE_EXPLOSION)
	{
		game.objects.addGameObject(new Effect(Effect::BASE_EXPLOSION, hittingPoint.getLocation(), 0.0f, NULL, 0.5f)); 
		setDamage(HAND_GRENADE_DAMAGE * damageMultiplier);

		probabilityOfGunnerDeath = 0.8f;

		//Tossing in a nade will prolly deal with tha gunner.
		if (randFloat(0.0f, 1.0f) < probabilityOfGunnerDeath)
		{
			gunner_alive = false;
		}
	}
}

void Base::render(void)
{
	if (!game.currentLevel->night)
	{
		renderShadows();
	}

	tankMG.render();
	float renderSize = size * GameObject::objectSize;
	if (image && visible)
	{
		if (game.currentLevel->night)
		{
			DWORD col = image->GetColor();
			image->SetColor(colorAdd(game.currentLevel->getObjectColor(), -50));
			image->RenderEx(x, y, direction, renderSize, renderSize);
			image->SetColor(col);
		}
		else
			image->RenderEx(x, y, direction, renderSize, renderSize);
	}
}

void Base::renderShadows(void)
{
	list<Effect *>::iterator it = game.objects.illuminators.begin();

	int i = 0;
	for (; it != game.objects.illuminators.end() && i < game.maxShadows; it++, i++)
	{

		float lightRadiusUnit = 64.0f; //'Effective' radius of light object in light.png
		float illuminatorRadius = (*it)->illuminationSize * lightRadiusUnit * GameObject::objectSize;		
		Location lightLocation = (*it)->getLocation();
		Distance lightDistance = objectDistance(lightLocation);
		if (lightDistance < 10.0f) return;
		if (lightDistance < illuminatorRadius)
		{
			//Get shadow length:
			Distance shadowLength = 0.0f;
			Height objectHeight = getHeight();
			Height lightHeight = (*it)->getHeight();
			if (objectHeight > lightHeight)
			{
				//Set shadow length to the length of the light:
				shadowLength = illuminatorRadius;
			}
			else
			{
				float d = objectHeight;
				float a = lightHeight - objectHeight;
				if (a == 0.0f)
					a = 0.00001;
				float ratio = d / a;
				shadowLength = lightDistance * ratio;
			}

			float shadowRenderTimes = shadowLength;

			if (shadowRenderTimes > 50.0f)
				shadowRenderTimes = 50.0f;

			//Get shadow top center:
			Location stc = ArcPoint(GameObject::getLocation(), shadowLength, GameObject::objectAngle(lightLocation) + M_PI).getLocation();

			if (lightDistance == 0.0f) lightDistance = 0.00001f;
			float shadowSize = shadowLength / lightDistance;
			float shadowdiff = shadowSize;
			float dshadow = shadowdiff / shadowRenderTimes;
			float shadowCurSize = 1.0f;

			float xdiff = stc.x - x;
			float ydiff = stc.y - y;
			float xx = x, dx = (xdiff) / shadowRenderTimes;
			float yy = y, dy = (ydiff) / shadowRenderTimes;

			DWORD col = image->GetColor();
			if (game.currentLevel->night)
				image->SetColor(0x55000000);
			else
				image->SetColor(0x03000000);

			xdiff = fabs(xdiff);
			ydiff = fabs(ydiff);
			float dx2 = fabs(dx);
			float dy2 = fabs(dy);
			float rs = 0.0f;
			for (int i = 0; !(xdiff <= 0.0f && ydiff <= 0.0f); i++)
			{
				//Shadow position
				xx += dx, yy += dy;
				xdiff -= dx2, ydiff -= dy2;

				//Shadow size
				shadowCurSize += dshadow;
				rs = shadowCurSize * size * GameObject::objectSize;

				image->RenderEx(xx, yy, direction, rs, rs);

				if (i > shadowRenderTimes)
					break;
			}
			
			image->SetColor(col);
		}
	}
}




CollisionSphere Base::getDynamicCollisionSphere(int index)
{
	//collisionsphere shifts for base barricade
	//This shift is not considering other kinds of directions than 0.0 - which is for the defender base pointing upwards texture.
	float cssbx = 16.0f, cssby1 = 8.0f, cssby2 = 15.0f; 
	switch (index)
	{
		//front middle bunker
	case 0: return CollisionSphere(Location(x, y - 25.0f), COLLISIONSPHERE_RADIUS_FRONT_BUNKER * GameObject::objectSize); break;
		//front left
	case 1: return CollisionSphere(Location(x - cssbx, y - cssby1), COLLISIONSPHERE_RADIUS_BARRICADE * GameObject::objectSize); break;
		//front right
	case 2: return CollisionSphere(Location(x + cssbx, y - cssby1), COLLISIONSPHERE_RADIUS_BARRICADE * GameObject::objectSize); break;
		//back right
	case 3: return CollisionSphere(Location(x + cssbx, y + cssby2), COLLISIONSPHERE_RADIUS_BARRICADE * GameObject::objectSize); break;
		//back left
	case 4: return CollisionSphere(Location(x - cssbx, y + cssby2), COLLISIONSPHERE_RADIUS_BARRICADE * GameObject::objectSize); break;
		//middle
	default:  return CollisionSphere(Location(x, y), 34.0f * GameObject::objectSize); break;
	}
}

Distance Base::getDynamicCollisionSphereRadius(int index)
{
	if (index == 0)
		return COLLISIONSPHERE_RADIUS_FRONT_BUNKER * GameObject::objectSize;
	else
		return COLLISIONSPHERE_RADIUS_BARRICADE * GameObject::objectSize;
}

Location Base::getLocation(void)
{
	if (this->side == CombatObject::DEFENDER_SIDE)
		return Location(x, y - 22.0f);
	else
		return Location(x, y);
}

GameObject * Base::getNearestEnemy(void)
{
	return game.objects.getNearestEnemy(this, GameObject::FOOT_SOLDIER, MG_MIN_ANGLE, MG_MAX_ANGLE);
}


Location Base::MGGetBarrelTipLocation(float addedLength)
{
	Location l = getLocation();
	l.x = l.x + (24.0f + addedLength) * cos(getDirection());
	l.y = l.y + (24.0f + addedLength) * sin(getDirection());
	return l;
}

Height Base::MGGetBarrelTipHeight(void)
{
	return WEAPON_HEIGHT;
}

void Base::processTurning(void)
{
	if (turning_left)
		turnLeft();
	if (turning_right)
		turnRight();
}
void Base::turnLeft(void)
{
	float amount = MG_TURNING_SPEED * timerDelta;
	if ((tankMG.getDirection() - amount) > MG_MIN_ANGLE)
	{
		tankMG.setDirection(tankMG.getDirection() - amount);
		angleToTurn -= amount;
		if (angleToTurn < 0.0f)
			turning_left = false;
	}
	else
		turning_left = false;
}

void Base::turnRight(void)
{
	float amount = MG_TURNING_SPEED * timerDelta;
	if ((tankMG.getDirection() + amount) < MG_MAX_ANGLE)
	{
		tankMG.setDirection(tankMG.getDirection() + amount);
		angleToTurn -= amount;
		if (angleToTurn < 0.0f)
			turning_right = false;
	}
	else
		turning_right = false;
}

float Base::objectAngle(GameObject * object2)
{
	float a = object2->x - this->tankMG.x;
	float b = object2->y - this->tankMG.y;
	float tangent = b / a;
	float angle = atan(tangent); 
	if (a < 0) angle += M_PI;
	else if (b < 0) angle += 6.283185f;//::PI_TIMES_TWO;
	return angle;
}
Direction Base::getDirection(void)
{
	return tankMG.direction;
}

void Base::setDamage(float damage)
{
	if (shield > 0.0f)
	{
		shield -= damage;

		if (shield < SHIELD_VALUE_CRITICAL && !damagemsg)
		{
			hge->System_SetState(HGE_INIFILE, "Settings.ini");
			game.showMessage(hge->Ini_GetString("MESSAGES", "MESSAGE_BASE_CRITICAL_DAMAGE", ""), Game::COLOR_TEXT_MESSAGE_ATTACKER_SIDE);
			damagemsg = true;	
		}

		if (shield < 0.0f && !destroyedmsg)
		{
			shield = 0;
			hge->System_SetState(HGE_INIFILE, "Settings.ini");
			game.showMessage(hge->Ini_GetString("MESSAGES", "MESSAGE_BASE_DESTROYED", ""), Game::COLOR_TEXT_MESSAGE_ATTACKER_SIDE);
			destroyedmsg = true;	
		}
	}
}

void Base::renderStats(void)
{
	hudfont->SetScale(0.6f);
	if (shield > SHIELD_VALUE_CRITICAL)
		hudfont->SetColor(game.colors[Game::COLOR_HUD_TEXT_NORMAL]);
	else
		hudfont->SetColor(game.colors[Game::COLOR_HUD_TEXT_CRITICAL]);

	hudfont->printf(340, 550, "Base shield: %i%%", (int)shield);
}

void Base::setHole(int holeX, int holeY)
{
	int HOLE_TEXTURE_DIMENSION = 48;
	int HALF_HOLE_TEXTURE_DIMENSION = HOLE_TEXTURE_DIMENSION / 2;

	//Range check
	if (holeX < HALF_HOLE_TEXTURE_DIMENSION) holeX = HALF_HOLE_TEXTURE_DIMENSION;
	if (holeX > TEXTURE_WIDTH - HALF_HOLE_TEXTURE_DIMENSION) holeX = TEXTURE_WIDTH - HALF_HOLE_TEXTURE_DIMENSION;
	if (holeY < HALF_HOLE_TEXTURE_DIMENSION) holeY = HALF_HOLE_TEXTURE_DIMENSION;
	if (holeY > TEXTURE_WIDTH - HALF_HOLE_TEXTURE_DIMENSION) holeY = TEXTURE_WIDTH - HALF_HOLE_TEXTURE_DIMENSION;

	int target_image_position_x = 128 + holeX - HALF_HOLE_TEXTURE_DIMENSION;
	int target_image_position_y = 0 + holeY - HALF_HOLE_TEXTURE_DIMENSION;
	int hole_image_position_x = 168;
	int hole_image_position_y = 168;
	int w = hge->Texture_GetWidth(tex);

	DWORD * colArray = hge->Texture_Lock(tex, false);
	for (int i = 0; i < HOLE_TEXTURE_DIMENSION; i++)
	{
		for (int j = 0; j < HOLE_TEXTURE_DIMENSION; j++)
		{
			int target_image_x = (j + target_image_position_x);
			int target_image_y = (i + target_image_position_y);
			int hole_image_x = (j + hole_image_position_x);
			int hole_image_y = (i + hole_image_position_y);

			//if there is black in the hole image pixel, render transparent to target image pixel.
			if (colArray[hole_image_y * w + hole_image_x] == 0xff000000)
			{
				colArray[target_image_y * w + target_image_x] = 0x00000000;
			}
		}
	}
	hge->Texture_Unlock(tex);
}

void Base::defend(void)
{
	switch (statePhase)
	{
	case DEFENSE_PHASE_OBSERVE:
		{
			if (hasEnemyTargetCrossedFireStartLevel() || game.currentLevel->levelCombatStarted)
			{
				nextStatePhase();
			}

			break;
		}
	case DEFENSE_PHASE_FIRE_START:
		{
			nextStatePhase();
			break;
		}
	case DEFENSE_PHASE_FIGHT:
		{
			if (getEnemyTarget())
			{
				setState(STATE_FIGHT, 0.0f, 0, false);
			}
			else
			{
				setStatePhase(DEFENSE_PHASE_OBSERVE);
			}
			break;
		}
	default: setStatePhase(0); break;
	}
}

void Base::fight(void)
{
	if (getEnemyTarget())
	{
		CombatAIObject * caio = static_cast<CombatAIObject *>(getEnemyTarget());
		if (caio->isNeutralized() ||
			(getEnemyTargetDirection() < MG_MIN_ANGLE  || getEnemyTargetDirection() > MG_MAX_ANGLE))
		{
			setEnemyTarget(NULL);
			previousState();
		}
	}
	else
		previousState();

	switch (statePhase)
	{
	case FIRING_PHASE_TURN:
		{
			if (first_time_in_state_phase)
			{
				setTurningDirection(getEnemyTargetDirection());
				first_time_in_state_phase = false;
			}

			if (!turning_left && !turning_right)
			{
				nextStatePhase();
			}

			break;
		}
	case FIRING_PHASE_AIM:
		{
			if (first_time_in_state_phase)
			{
				Direction d = getEnemyTargetDirection();
				if (smallestAngleBetween(getDirection(), d) > 0.2f)
				{
					setStatePhase(FIRING_PHASE_TURN);
					break;
				}
				
				statePhaseTimer = 0.000003 * squaredObjectDistance(getEnemyTarget()) + randFloat(0.0f, 1.0f);				

				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;
			if (statePhaseTimer < 0.0f)
			{
				nextStatePhase();
			}

			break;
		}
	case FIRING_PHASE_FIRE:
		{
			if (first_time_in_state_phase)
			{
				int rand = randInt(1, 10);
				switch (rand)
				{
				case 1: statePhaseTimer = 0.2f; break;
				case 2: statePhaseTimer = 0.3f; break;
				case 3: statePhaseTimer = 0.3f; break;
				case 4: statePhaseTimer = 0.3f; break;
				case 5: statePhaseTimer = 0.3f; break;
				case 6: statePhaseTimer = randFloat(0.3f, 0.6f); break;
				case 7: statePhaseTimer = randFloat(0.3f, 0.6f); break;
				case 8: statePhaseTimer = randFloat(0.6f, 1.0f); break;
				case 9: statePhaseTimer = randFloat(0.6f, 1.0f); break;
				case 10: statePhaseTimer = randFloat(1.0f, 5.0f); break;
				default: break;
				}
				firing = true;
				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;
			if (statePhaseTimer < 0.0f)
			{
				firing = false;
				setEnemyTarget(NULL);
				setStatePhase(FIRING_PHASE_AIM);
			}

			break;
		}
	//case FIRING_PHASE_RELOAD:
	}
}