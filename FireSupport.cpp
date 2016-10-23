#include ".\firesupport.h"
#include "main.h"

//info text
#define INFO_TEXT_SIZE 0.7f

#define ENEMY_MORTAR_STRIKE_SHOW_TEXT_DELAY 3.0f
#define ENEMY_MISSILE_STRIKE_SHOW_TEXT_DELAY 7.0f
#define ENEMY_NAPALM_STRIKE_SHOW_TEXT_DELAY 8.0f
#define ENEMY_FIRE_SUPPORT_SHOW_TEXT_DELAY_VARIATION 0.2f

Time MortarGrenade::WHISTLE_TIME = 3.0f;
Time MortarTeam::GRENADE_FLY_TIME_DEFENDER = 5.0f;
Time MortarTeam::GRENADE_FLY_TIME_ATTACKER = 7.0f;
Time MortarTeam::NEUTRALIZING_FIRE_INTERVAL = 2.0f;
Time MortarTeam::MORTAR_FIRE_INTERVAL_VARIATION = 0.5f;

#define MORTAR_FIRING_SOUND_VOLUME 25

MortarGrenade::MortarGrenade(void)
{
	flying = false;
}

void MortarGrenade::process(void)
{
	if (flying)
	{
		flyTimer -= timerDelta;
		if (flyTimer < WHISTLE_TIME && !whistle)
		{
			//soitetaan kranaatin vihellysääni
			game.playSound(game.sounds[randInt(Game::SOUND_MORTAR_WHISTLE_1, Game::SOUND_MORTAR_WHISTLE_LAST)]); 
			whistle = true;
		}
		if (flyTimer < 0.0f)
		{
			//lisätään/luodaan kranaatin räjähdysäänet ja efekti, räjähdys jne...			
			game.objects.addGameObject(new Effect(Effect::EFFECTTYPE::MORTARSHELL_EXPLOSION, targetLocation, randomDirection()));
			flying = false;
		}
	}
}

void MortarGrenade::fire(Location targetLocation, Time flyingTime)
{
	this->targetLocation = targetLocation;
	flyTimer = flyingTime;
	flying = true;
	whistle = false;
}

const float NapalmBomb::WEIGHT = 340.0f;
const Distance NapalmBomb::IMAGE_DISTANCE_TO_CORNER = sqrt(16.0f * 16.0f + 24.0f * 24.0f);
const Angle NapalmBomb::IMAGE_ANGLE_TO_CORNER = atan(16.0f / 24.0f);
#define ROTATION_SPEED 10.0f
#define ROTATION_RADIUS 5.0f

NapalmBomb::NapalmBomb()
{

}

NapalmBomb::NapalmBomb(Location location, Direction direction, SIDE side)
:height(FLT_MAX)
,dropping(false)
{
	set(location, direction, side);	
}

void NapalmBomb::set(Location location, Direction direction, SIDE side)
{
	CombatObject::set(GameObject::FIRE_SUPPORT, location, direction, side);

	image = new hgeAnimation(game.textures[Game::TEXTURE_EFFECTS], 6, 12.0f, 0.0f, 320.0f, 48.0f, 32.0f); 
	image->SetHotSpot(48.0f / 2.0f, 32.0f / 2.0f); 
	image->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);

	rotation = 0.0f;
	angle = 0.0f;
	height = FLT_MAX;
	speed = 0.0f;
}

NapalmBomb::~NapalmBomb()
{
	if (image)
	{
		delete image;
		image = NULL;
	}
}

void NapalmBomb::processInteraction(void)
{
	if (isDestroyed())
		return;

	//falling:
	if (height > 0.0f)
	{
		speed += 9.8f * Game::METER * timerDelta;
		height -= speed * timerDelta;
	}
	else
	{
		explode();
	}

	rotation += ROTATION_SPEED * timerDelta;
	angle = direction + 0.2f * sin(rotation);
}


void NapalmBomb::render(void)
{
	if (image)
	{
		DWORD col = image->GetColor();
		if (game.currentLevel->night)
		{			
			image->SetColor(game.currentLevel->getObjectColor());				
		}
		image->Update(timerDelta);

		//set corners
		float scale = (1000.0f) / (200.0f * Game::METER - height);
		float lenghtA = scale * IMAGE_DISTANCE_TO_CORNER + 2.0f * sin(rotation);
		float lengthB = scale * IMAGE_DISTANCE_TO_CORNER + 2.0f * cos(rotation);
		float direction2 = direction + angle;
		float x0 = x + lenghtA * cos(direction2 + M_PI + IMAGE_ANGLE_TO_CORNER);
		float y0 = y + lenghtA * sin(direction2 + M_PI + IMAGE_ANGLE_TO_CORNER);
		float x1 = x + lengthB * cos(direction2 - IMAGE_ANGLE_TO_CORNER);
		float y1 = y + lengthB * sin(direction2 - IMAGE_ANGLE_TO_CORNER);
		float x2 = x + lenghtA * cos(direction2 + IMAGE_ANGLE_TO_CORNER);
		float y2 = y + lenghtA * sin(direction2 + IMAGE_ANGLE_TO_CORNER);
		float x3 = x + lengthB * cos(direction2 + M_PI - IMAGE_ANGLE_TO_CORNER);
		float y3 = y + lengthB * sin(direction2 + M_PI - IMAGE_ANGLE_TO_CORNER);
		image->Render4V(x0, y0, x1, y1, x2, y2, x3, y3);
		image->SetColor(col);
	}
}

void NapalmBomb::setDropped(Height height)
{
	this->height = height;
	dropping = true;
	image->Play();
}

void NapalmBomb::explode(void)
{
	image->Stop();
	game.objects.addGameObject(new Effect(Effect::NAPALM_EXPLOSION, getLocation(), getDirection()));
	destroy();
}


#define MAX_AMMO_MORTAR_DEF 10

MortarTeam::MortarTeam(void)
{
	firingInterval = NEUTRALIZING_FIRE_INTERVAL;
	nextRoundTimer = randFloat(0.0f, 2.0f);
	ammo = randInt(MAX_AMMO_MORTAR_DEF - 3, MAX_AMMO_MORTAR_DEF);
}

void MortarTeam::set(CombatObject::SIDE side)
{
	this->side = side;
}

void MortarTeam::processInteraction(void)
{
	processFiring();
	processGrenades();
}  

void MortarTeam::processFiring(void)
{
	nextRoundTimer -= timerDelta;

	if (nextRoundTimer < 0)
	{		
		fire();
		nextRoundTimer = randFloat(firingInterval - MORTAR_FIRE_INTERVAL_VARIATION, firingInterval + MORTAR_FIRE_INTERVAL_VARIATION);
	}
}

void MortarTeam::fire(void)
{

	for (int i = 0; i < NUMBER_OF_SIMULTANEOUS_GRENADES; i++)
	{
		if (ammo > 0)
		{
			if (!grenades[i].isFlying())
			{
				grenades[i].fire(getRandomTargetLocation(), getGrenadeFlyingTime());
				playFiringSound();
				ammo--;
				game.currentLevel->statisticsForSides[side][GameObject::FIRE_SUPPORT][FireSupport::MORTAR_STRIKE][0]++;
				break;
			}
		}
		else
			break;
	}
}

void MortarTeam::processGrenades(void)
{
	for (int i = 0; i < NUMBER_OF_SIMULTANEOUS_GRENADES; i++)
	{
		grenades[i].process();
	}
}

Location MortarTeam::getRandomTargetLocation(void)
{
	if (side == CombatObject::DEFENDER_SIDE)
		return Location(randFloat(-100.0f, 900.0f), randFloat(-100.0f, 300.0f));
	else
		return Location(randFloat(-100.0f, 900.0f), randFloat(300.0f, 700.0f));
}

Time MortarTeam::getGrenadeFlyingTime(void)
{
	if (side == CombatObject::DEFENDER_SIDE)
		return GRENADE_FLY_TIME_DEFENDER;
	else
		return GRENADE_FLY_TIME_ATTACKER;
}

void MortarTeam::playFiringSound(void)
{
	if (side == CombatObject::ATTACKER_SIDE)
		game.playSound(game.sounds[Game::SOUND_MORTAR_FIRE_LAST], MORTAR_FIRING_SOUND_VOLUME);
	else
		game.playSound(game.sounds[randInt(Game::SOUND_MORTAR_FIRE_1, Game::SOUND_MORTAR_FIRE_2)], MORTAR_FIRING_SOUND_VOLUME);
}

bool MortarTeam::finishedFiring(void)
{
	bool returnValue = true;
	for (int i = 0; i < NUMBER_OF_SIMULTANEOUS_GRENADES; i++)
	{
		if (grenades[i].flying)
		{
			returnValue = false;
			break;
		}
	}
	if (ammo > 0)
		returnValue = false;	

	return returnValue;
}

void FireSupportUnit::set(CombatObject::SIDE side)
{
	CombatObject::set(GameObject::FIRE_SUPPORT, Location(FLT_MAX, FLT_MAX), 0.0f, side);
}

MortarPlatoon::MortarPlatoon(CombatObject::SIDE side)
{
	set(side);
}
void MortarPlatoon::set(CombatObject::SIDE side)
{
	FireSupportUnit::set(side);
	for (int i = 0; i < NUMBER_OF_TEAMS; i++)
		teams[i].set(side);

	showTextTimer = 0.0f;
	if (side == CombatObject::ATTACKER_SIDE)
		showTextTimer += (ENEMY_MORTAR_STRIKE_SHOW_TEXT_DELAY * randomVariation(ENEMY_FIRE_SUPPORT_SHOW_TEXT_DELAY_VARIATION));

	game.currentLevel->statisticsForSides[side][GameObject::FIRE_SUPPORT][FireSupport::NUMBER_OF_TYPES][0]++;
}

void MortarPlatoon::processInteraction(void)
{
	for (int i = 0; i < NUMBER_OF_TEAMS; i++)
		teams[i].processInteraction();
}

bool MortarPlatoon::isFinished(void)
{
	for (int i = 0; i < NUMBER_OF_TEAMS; i++)
		if (!teams[i].finishedFiring())
			return false;

	return true;
}


#define FIGHTER_PLANE_APPEARANCE_TIME 0.0f
#define FIGHTER_PLANE_APPEARANCE_DISTANCE_FROM_SCREEN 13000.0f
#define DEFENDER_FIGHTER_PLANE_APPEARANCE_X -FIGHTER_PLANE_APPEARANCE_DISTANCE_FROM_SCREEN
#define DEFENDER_FIGHTER_PLANE_APPEARANCE_Y 300.0f
#define ATTACKER_FIGHTER_PLANE_APPEARANCE_X 800 + FIGHTER_PLANE_APPEARANCE_DISTANCE_FROM_SCREEN
#define ATTACKER_FIGHTER_PLANE_APPEARANCE_Y 300.0f
#define FIGHTER_PLANE_SPEED 800.0f
#define DEFENDER_FIGHTER_PLANE_DIRECTION 0.0f
#define ATTACKER_FIGHTER_PLANE_DIRECTION M_PI
#define MAX_MISSILES 20
#define MAX_ANGLE_BETWEEN_MISSILE_DESTINATIONS 0.5f
#define MIN_DISTANCE_BETWEEN_MISSILE_DESTINATIONS 50.0f
#define MAX_DISTANCE_BETWEEN_MISSILE_DESTINATIONS 50.0f
#define MISSILES_START_COMING_TIME 5.0f
#define MIN_TIME_BETWEEN_MISSILES 0.2f
#define MAX_TIME_BETWEEN_MISSILES 0.3f

MissileStrike::MissileStrike()
:targets_checked(false)
{
}

MissileStrike::~MissileStrike()
{
}

MissileStrike::MissileStrike(CombatObject::SIDE side)
:targets_checked(false)
{
	set(side);
}

void MissileStrike::set(CombatObject::SIDE side)
{
	FireSupportUnit::set(side);
	numberOfMissiles = randInt(MAX_MISSILES - 3, MAX_MISSILES);

	fighterAppearTime = FIGHTER_PLANE_APPEARANCE_TIME;
	fireMissileTimer = FLT_MAX;
	showTextTimer = 0.0f;
	if (side == CombatObject::ATTACKER_SIDE)
		showTextTimer += (ENEMY_MISSILE_STRIKE_SHOW_TEXT_DELAY * randomVariation(ENEMY_FIRE_SUPPORT_SHOW_TEXT_DELAY_VARIATION));

	game.currentLevel->statisticsForSides[side][GameObject::FIRE_SUPPORT][FireSupport::NUMBER_OF_TYPES][0]++;
}

void MissileStrike::processInteraction(void)
{
	if (fighterAppearTime > 0.0f)
	{
		fighterAppearTime -= timerDelta;

		if (fighterAppearTime > FIGHTER_PLANE_APPEARANCE_TIME)
		{
			nextMissileFiringLocation.x = nextMissileFiringLocation.x + FIGHTER_PLANE_SPEED * cos(direction) * timerDelta;
			nextMissileFiringLocation.y = nextMissileFiringLocation.y + FIGHTER_PLANE_SPEED * sin(direction) * timerDelta;
		}
	}
	else
	{
		game.playSound(game.sounds[Game::SOUND_FIGHTER_PLANE_FLY_BY]); 
		fighterAppearTime = FLT_MAX;
		fireMissileTimer = MISSILES_START_COMING_TIME;
	}

	if (fireMissileTimer > 0.0f)
	{
		fireMissileTimer -= timerDelta;
	}
	else
	{
		if (numberOfMissiles > 0)
		{
			game.objects.addGameObject(new Projectile(this, Projectile::AIR_TO_GROUND_MISSILE, nextMissileFiringLocation, getHeight(), nextMissileDestination), GameObjectCollection::LAST_RENDER_LIST_INDEX);
			fireMissileTimer = randFloat(MIN_TIME_BETWEEN_MISSILES, MAX_TIME_BETWEEN_MISSILES);

			float dir = direction + (MAX_ANGLE_BETWEEN_MISSILE_DESTINATIONS * randFloat(-1.0f, 1.0f));
			dir = directionCheck(dir);
			float dist = randFloat(MIN_DISTANCE_BETWEEN_MISSILE_DESTINATIONS, MAX_DISTANCE_BETWEEN_MISSILE_DESTINATIONS);
			nextMissileDestination = ArcPoint(nextMissileDestination, dist, dir).getLocation();

			if (side == CombatObject::ATTACKER_SIDE)
			{
				nextMissileDestination.y = randFloat(Game::SCREENHEIGHT / 2.0f, Game::SCREENHEIGHT);
			}
			else 
			{
				nextMissileDestination.y = randFloat(0.0f, Game::SCREENHEIGHT / 2.0f);
			}

			game.currentLevel->statisticsForSides[side][GameObject::FIRE_SUPPORT][FireSupport::MISSILE_STRIKE][0]++;
			numberOfMissiles--;
		}
	}
	startChecks = true;
}

bool MissileStrike::isFinished(void)
{
	return numberOfMissiles <= 0;
}

Height MissileStrike::getHeight(void)
{
	return 1000.0f;
}

void MissileStrike::startCheckingObjects(void)
{
	cumulativeDistance = 0.0f;
	numberOfEnemyTroops = 0;
}

void MissileStrike::checkObject(GameObject * otherObject)
{
	if (startChecks)
	{
		startCheckingObjects();
		startChecks = false;
	}

	if (!targets_checked)
	{
		if (otherObject->isCombatAIObject())
		{
			CombatAIObject * caio = static_cast<CombatAIObject *>(otherObject);

			if (caio->side != this->side && caio->type != GameObject::MILITARY_UNIT)
			{
				numberOfEnemyTroops++;
				cumulativeDistance += caio->getLocation().y;
			}
		}
	}
}

void MissileStrike::stopCheckingObjects(void)
{
	if (!targets_checked)
	{
		float starty = cumulativeDistance / numberOfEnemyTroops;

		switch (side)
		{
		case DEFENDER_SIDE: 
			{
				if (numberOfEnemyTroops <= 0)
					starty = 150.0f;
				setDirection(DEFENDER_FIGHTER_PLANE_DIRECTION);
				nextMissileFiringLocation = Location(DEFENDER_FIGHTER_PLANE_APPEARANCE_X, DEFENDER_FIGHTER_PLANE_APPEARANCE_Y);
				float startx = randFloat(-100.0f, 100.0f);
				nextMissileDestination = Location(startx, starty); 
				break;
			}
		case ATTACKER_SIDE: 
			{
				if (numberOfEnemyTroops <= 0)
					starty = 450.0f;
				setDirection(ATTACKER_FIGHTER_PLANE_DIRECTION);
				nextMissileFiringLocation = Location(ATTACKER_FIGHTER_PLANE_APPEARANCE_X, ATTACKER_FIGHTER_PLANE_APPEARANCE_Y);
				float startx = randFloat(700.0f, 900.0f);
				nextMissileDestination = Location(startx, starty); 
				break;
			}
		}
		targets_checked = true;
	}
}


#define BOMBER_APPEAR_TIME 0.0f
#define ON_LOCATION_TIME 6.0f
#define CHECKING_TIME 1.0f
NapalmStrike::NapalmStrike()
:check(false)
,bombs_dropped(false)
{
	for (int i = 0; i < NUMBER_OF_BOMBS; i++)
	{
		bombs[i] = NULL;
	}
}

NapalmStrike::~NapalmStrike()
{
	for (int i = 0; i < NUMBER_OF_BOMBS; i++)
	{
		if (bombs[i])
		{
			delete bombs[i];
			bombs[i] = NULL;
		}
	}
}

NapalmStrike::NapalmStrike(CombatObject::SIDE side)
:check(false)
,bombs_dropped(false)
{
	for (int i = 0; i < NUMBER_OF_BOMBS; i++)
	{
		bombs[i] = NULL;
	}
	set(side);
}

void NapalmStrike::set(CombatObject::SIDE side)
{
	FireSupportUnit::set(side);
	bomberAppearTimer = BOMBER_APPEAR_TIME;
	onLocationTimer = FLT_MAX;
	checkTimer = FLT_MAX;
	check = false;
	bombs_dropped = false;
	showTextTimer = 0.0f;
	if (side == CombatObject::ATTACKER_SIDE)
		showTextTimer += (ENEMY_NAPALM_STRIKE_SHOW_TEXT_DELAY * randomVariation(ENEMY_FIRE_SUPPORT_SHOW_TEXT_DELAY_VARIATION));

	game.currentLevel->statisticsForSides[side][GameObject::FIRE_SUPPORT][FireSupport::NUMBER_OF_TYPES][0]++;
}

void NapalmStrike::processInteraction(void)
{
	bomberAppearTimer -= timerDelta;

	if (bomberAppearTimer < 0.0f)
	{
		game.playSound(game.sounds[Game::SOUND_FIGHTER_PLANE_FLY_BY]); 
		onLocationTimer = ON_LOCATION_TIME;
		bomberAppearTimer = FLT_MAX;
	}

	onLocationTimer -= timerDelta;

	if (onLocationTimer < 0.0f)
	{
		check = true;
		onLocationTimer = FLT_MAX;
	}

	checkTimer -= timerDelta;

	if (checkTimer < 0.0f)
	{
		bombsAway();
		checkTimer = FLT_MAX;
	}

	for (int i = 0; i < NUMBER_OF_BOMBS; i++)
	{
		if (bombs[i])
		{
			bombs[i]->processInteraction();
		}
	}

	startChecks = true;
}

void NapalmStrike::render()
{
	for (int i = 0; i < NUMBER_OF_BOMBS; i++)
	{
		if (bombs[i])
		{
			bombs[i]->render();
		}
	}
}

bool NapalmStrike::isFinished(void)
{
	bool ret = false;

	if (bombs_dropped)
	{
		ret = true;

		for (int i = 0; i < NUMBER_OF_BOMBS; i++)
		{
			if (bombs[i])
			{
				if (!bombs[i]->isDestroyed())
				{
					ret = false;
					break;
				}
			}
		}
	}
	return ret;
}

Height NapalmStrike::getHeight()
{
	return 200.0f * Game::METER;
}

void NapalmStrike::bombsAway()
{
	Angle a = randFloat(-0.3, 0.3f);
	a = directionCheck(a);
	//first bomb distance from dropping location:
	Distance d = 800.0f / (NUMBER_OF_BOMBS + 1);
	//first bomb dropping location
	float halfBombs = (NUMBER_OF_BOMBS - 1) / 2.0f;
	Distance distFromCenter = d * halfBombs;
	Location l = ArcPoint(droppingLocation, distFromCenter, a + M_PI).getLocation();

	for (int i = 0; i < NUMBER_OF_BOMBS; i++)
	{
		bombs[i] = new NapalmBomb(l, getDirection(), getSide());
		bombs[i]->setDropped(getHeight());

		l = ArcPoint(l, d, a).getLocation();
	}

	bombs_dropped = true;

	game.currentLevel->statisticsForSides[side][GameObject::FIRE_SUPPORT][FireSupport::NAPALM_STRIKE][0] += NUMBER_OF_BOMBS;
}

void NapalmStrike::startCheckingObjects(void)
{
	cumulativeDistance = 0.0f;
	numberOfEnemyTroops = 0;
}

void NapalmStrike::checkObject(GameObject * otherObject)
{
	if (startChecks)
	{
		startCheckingObjects();
		startChecks = false;
	}

	if (check)
	{
		if (otherObject->isCombatAIObject())
		{
			CombatAIObject * caio = static_cast<CombatAIObject *>(otherObject);

			if (caio->side != this->side && 
				caio->type != GameObject::MILITARY_UNIT &&
				caio->isOnScreen())
			{
				numberOfEnemyTroops++;
				cumulativeDistance += caio->getLocation().y;
			}
		}
	}
}

void NapalmStrike::stopCheckingObjects(void)
{
	if (check)
	{
		float starty;
		if (numberOfEnemyTroops > 0)
			starty = cumulativeDistance / numberOfEnemyTroops;
		else 
		{
			if (side == DEFENDER_SIDE)
			{
				starty = 150.0f;
			}
			else if (side == ATTACKER_SIDE)
			{
				starty = 450.0f;
			}
		}
		droppingLocation.set(400.0f, starty);
		checkTimer = CHECKING_TIME;
		check = false;
	}
}
char FireSupport::messages[CombatObject::NUMBER_OF_SIDES][NUMBER_OF_TYPES][256];
char FireSupport::typeNames[NUMBER_OF_TYPES][64];

void FireSupport::init(void)
{
	char * sideNames[2] = {"DEFENDER", "ATTACKER"};
	sprintf(typeNames[MORTAR_STRIKE], "MORTAR");
	sprintf(typeNames[MISSILE_STRIKE], "MISSILE");
	sprintf(typeNames[NAPALM_STRIKE], "NAPALM");

	hge->System_SetState(HGE_INIFILE, "Settings.ini");

	char text[256];
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < NUMBER_OF_TYPES; j++)
		{
			sprintf(text, "MESSAGE_%s_%s_STRIKE", sideNames[i], typeNames[j]);
			sprintf(messages[i][j], hge->Ini_GetString("MESSAGES", text, ""));
		}
	}
}

void FireSupport::release(void)
{
}

FireSupport::FireSupport(void)
:supportingUnit(NULL)
{
}

FireSupport::~FireSupport(void)
{
	if (supportingUnit)
	{
		delete supportingUnit; supportingUnit = NULL;
	}
}

FireSupport::FireSupport(FIRE_SUPPORT_TYPE fireSupportType, CombatObject::SIDE side)
:supportingUnit(NULL)
{
	set(fireSupportType, side);
}

void FireSupport::set(FIRE_SUPPORT_TYPE fireSupportType, CombatObject::SIDE side)
{
	CombatObject::set(GameObject::FIRE_SUPPORT, Location(FLT_MAX, FLT_MAX), 0.0f, side);
	this->fireSupportType = fireSupportType;
	switch (fireSupportType)
	{
	case MORTAR_STRIKE: supportingUnit = (FireSupportUnit *)(new MortarPlatoon(side)); break;
	case MISSILE_STRIKE: supportingUnit = (FireSupportUnit *)(new MissileStrike(side)); break;
	case NAPALM_STRIKE: supportingUnit = (FireSupportUnit *)(new NapalmStrike(side)); break;
	}
}

void FireSupport::processInteraction(void)
{
	supportingUnit->processInteraction();

	if (supportingUnit->isFinished())
		destroy();

	startChecks = true;
}

void FireSupport::render(void)
{
	supportingUnit->render();

	supportingUnit->showTextTimer -= timerDelta;

	if (supportingUnit->showTextTimer < 0.0f)
	{
		game.showMessage(messages[side][fireSupportType], CombatObject::colorIndexes[side]);
		supportingUnit->showTextTimer = FLT_MAX ;
	}
}

void FireSupport::startCheckingObjects(void)
{
	supportingUnit->startCheckingObjects();
}

void FireSupport::checkObject(GameObject * otherObject)
{
	if (startChecks)
	{
		startCheckingObjects();
		startChecks = false;
	}

	supportingUnit->checkObject(otherObject);
}

void FireSupport::stopCheckingObjects(void)
{
	supportingUnit->stopCheckingObjects();
}
