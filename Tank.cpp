#include ".\tank.h"
#include "main.h"
#include "projectile.h"

HTEXTURE Turret::playerTurrettex = NULL;
HTEXTURE Turret::enemyTurrettex = NULL;
HTEXTURE Turret::carcassTurrettex;
hgeAnimation * Turret::playerTurretImage = NULL;
hgeAnimation * Turret::enemyTurretImage = NULL;
hgeAnimation * Turret::carcassTurretImage;

const float Turret::TURRET_ROTATION_SPEED = 0.5f;
const float Turret::BARREL_LENGTH = DEF_TANK_TURRET_BARREL_LENGTH;
const float Turret::MAX_ANGLE = 1.0f;
const float Turret::TURRET_ANGLE_ADJUSTMENT_SPEED = 0.1f;

#define TURRET_AMMUNITION_FULL 20

Turret::Turret(void)
:playing_firing_animation(false)
,ready_to_fire(false)
{
	GameObject::set(GameObject::TURRET, Location(0.0f, 0.0f), 0.0f);
	angle = 0.15f;
	aimingLocation = ArcPoint(getLocation(), 100.0f, 0.0f).getLocation();
	reload();
}

Turret::~Turret(void)
{
}

void Turret::init(void)
{
	game.renderLoadScreen("LOADING TURRET CLASS");
	playerTurrettex = hge->Texture_Load("player1_tank.png");
	enemyTurrettex = hge->Texture_Load("enemy_tank.png");
	carcassTurrettex = hge->Texture_Load("tank_carcass_test1.png");

	//player turret
	
	playerTurretImage = new hgeAnimation(playerTurrettex, 6, 40.0f, 0.0f, 159.0f, 114.0f, 187.0f-159.0f + 1);//152.0f, 114.0f, 194.0f - 152.0f);
	playerTurretImage->SetHotSpot(14.0f, 14.5f); //49, 15 png-kuvassa.
	playerTurretImage->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
	playerTurretImage->SetFrame(5);

	//enemy turret
	enemyTurretImage = new hgeAnimation(enemyTurrettex, 6, 40.0f, 0.0f, 159.0f, 114.0f, 187.0f-159.0f + 1);//152.0f, 114.0f, 194.0f - 152.0f);
	enemyTurretImage->SetHotSpot(14.0f, 14.5f); //49, 15 png-kuvassa.
	enemyTurretImage->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
	enemyTurretImage->SetFrame(5);

	//carcass turret
	carcassTurretImage = new hgeAnimation(carcassTurrettex, 6, 40.0f, 0.0f, 159.0f, 114.0f, 187.0f-159.0f + 1);//152.0f, 114.0f, 194.0f - 152.0f);
	carcassTurretImage->SetHotSpot(14.0f, 14.5f); //49, 15 png-kuvassa.
	carcassTurretImage->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
	carcassTurretImage->SetFrame(5);
}

void Turret::release(void)
{
	hge->Texture_Free(playerTurrettex);
	hge->Texture_Free(enemyTurrettex);
	hge->Texture_Free(carcassTurrettex);

	delete playerTurretImage; playerTurretImage = NULL;
	delete enemyTurretImage; enemyTurretImage = NULL;
	delete carcassTurretImage; carcassTurretImage = NULL;
}

bool Turret::isReadyToFire(void)
{
	return ready_to_fire;
}

void Turret::reload(void)
{
	ammunition = TURRET_AMMUNITION_FULL;
}

void Turret::playAnimation(bool fire)
{
	
	if (fire)
	{
		image->Play();
		image->SetFrame(1);
	}
	image->Update(timerDelta);
}

/*		
//Muistiinpanoja:
//x = 7, y = 3, d (hypotenuusa) = sqrt(7^2 + 3^2)= 7.61577 on siis kk:n etäisyys tornin keskipisteestä.
//kk:n kulma suhteessa tornin keskipisteeseen on arkuskosinin kanssa selvitettynä: 3.5464836
//a = x + d * cos(alpha + beta) + c * cos(alpha + beta + gamma)
//a = piipunsuun x-koordinaatti, x = tornin keskustan x-koordinaatti,
//d = kk:n etäisyys tornin keskuksesta (7.61577)
//c = kk:n piipun pituus (20)
//kulma alpha = kk:n sijainnin kulma suhteessa torniin (3.5464836)
//kulma beta = tornin pyörähdyskulma
//kk:n pyörähdyskulma
*/

HTEXTURE TankMachineGun::tankmgtex = NULL;
const float TankMachineGun::TANKMG_ROTATION_ACCELERATION = 0.01f;
const float TankMachineGun::TANKMG_ROTATION_MAX_SPEED = 0.25f;
const float TankMachineGun::MG_POSITION_ANGLE = 2.7367f;
const float TankMachineGun::MG_POSITION_DISTANCE_FROM_CENTER = 7.61577f;
const float TankMachineGun::MG_BARREL_LENGTH = DEF_TANK_MG_BARREL_LENGTH;
const float TankMachineGun::MAX_ANGLE = 1.0f;
const float TankMachineGun::MG_ANGLE_ADJUSTMENT_SPEED = 100.4f;

#define TANK_MG_AMMUNITION_FULL 200

TankMachineGun::TankMachineGun(void)
{
	GameObject::set(GameObject::TANKMAGHINEGUN, Location(0.0f ,0.0f), 0.0f);
	image = new hgeAnimation(tankmgtex, 2, 0.0f, 0.0f, 136.0f, 32.0f, 10.0f);
	image->SetHotSpot(5.0f, 4.5f);
	angle = 0.0f;
	aimingLocation = ArcPoint(getLocation(), 100.0f, 0.0f).getLocation();
	ready_to_fire = false;
	reload();
}

TankMachineGun::~TankMachineGun(void)
{
	if (image) { delete image; image = NULL; }
}

void TankMachineGun::init(void)
{
	game.renderLoadScreen("LOADING TANK MAGHINE GUN CLASS");
	tankmgtex = hge->Texture_Load("tank.png");
}

void TankMachineGun::release()
{
	hge->Texture_Free(tankmgtex);
}

void TankMachineGun::reload(void)
{
	ammunition = TANK_MG_AMMUNITION_FULL;
}

void TankMachineGun::setPosition(float x, float y, float direction, float mgTurn)
{
	float ax = MG_POSITION_DISTANCE_FROM_CENTER * cos(MG_POSITION_ANGLE + direction);
	this->x = x + ax;
	float b = sin(MG_POSITION_ANGLE + direction);
	float a = MG_POSITION_DISTANCE_FROM_CENTER * b;
	this->y = y + a;
	this->direction += mgTurn;

	if (this->direction > 2 * M_PI)
		this->direction = direction - 2 * M_PI;
	if (this->direction < 0.0f)
		this->direction = 2 * M_PI - (direction - 2 * M_PI);
}

Location TankMachineGun::getBarrelTipLocation(float addedLength)
{
	Location l;
	l.x = x + (15.0f + addedLength) * cos(direction);
	l.y = y + (15.0f + addedLength) * sin(direction);
	return l;
}


void TankMachineGun::setAimingLocation(void)
{
	float d = angle * 800.0f + 100.0f;
	aimingLocation.x = x + d * cos(direction);
	float s = sin(direction);
	aimingLocation.y = y + d * sin(direction);
}

bool TankMachineGun::isReadyToFire(void)
{
	return ready_to_fire;
}

void TankObject::renderTankShadows(GameObject * object)
{
	list<Effect *>::iterator it = game.objects.illuminators.begin();

	int i = 0;
	for (; it != game.objects.illuminators.end() && i < game.maxShadows; it++, i++)
	{
		float lightRadiusUnit = 64.0f; //'Effective' radius of light object in light.png
		float illuminatorRadius = (*it)->illuminationSize * lightRadiusUnit * GameObject::objectSize;		
		Location lightLocation = (*it)->getLocation();
		Distance lightDistance = object->objectDistance(lightLocation);
		if (lightDistance < 10.0f) return;
		if (lightDistance < illuminatorRadius)
		{
			//Get shadow length:
			Distance shadowLength = 0.0f;
			Height objectHeight = object->getHeight();
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
			Location stc = ArcPoint(object->getLocation(), shadowLength, object->objectAngle(lightLocation) + M_PI).getLocation();

			if (lightDistance == 0.0f) lightDistance = 0.00001f;
			float shadowSize = shadowLength / lightDistance;
			float shadowdiff = shadowSize;
			float dshadow = shadowdiff / shadowRenderTimes;
			float shadowCurSize = 1.0f;

			float xdiff = stc.x - object->x;
			float ydiff = stc.y - object->y;
			float xx = object->x, dx = (xdiff) / shadowRenderTimes;
			float yy = object->y, dy = (ydiff) / shadowRenderTimes;

			DWORD col = object->image->GetColor();
			DWORD col2 = turret.image->GetColor();

			if (game.currentLevel->night)
			{
				object->image->SetColor(0x11000000);
				turret.image->SetColor(0x25000000);
			}
			else
			{
				object->image->SetColor(0x04000000);
				turret.image->SetColor(0x04000000);
			}

			xdiff = fabs(xdiff);
			ydiff = fabs(ydiff);
			float dx2 = fabs(dx);
			float dy2 = fabs(dy);
			float rs = 0.0f;
			for (int i = 0; !(xdiff <= 0.0f && ydiff <= 0.0f); i++)
			{
				//shadow position
				xx += dx, yy += dy;
				xdiff -= dx2, ydiff -= dy2;

				//shadow size
				shadowCurSize += dshadow;
				rs = shadowCurSize * object->size * GameObject::objectSize;

				object->image->RenderEx(xx, yy, object->direction, rs, rs);				

				if (i > shadowRenderTimes)
					break;
			}
			turret.image->RenderEx(xx, yy, turret.direction, rs, rs);
			
			object->image->SetColor(col);
			turret.image->SetColor(col2);
		}
	}
}

int Tank::tanks = 0;

const float Tank::d = sqrt(pow(61.0f / 2.0f, 2.0f) + pow(34.0f / 2.0f, 2.0f));
const float Tank::alpha = asin(34.0f / 2.0f / d);

const Direction Tank::TANK_DIRECTION_FORWARD = 0.0f;
const Direction Tank::TANK_DIRECTION_BACKWARDS = M_PI;
const float Tank::TANK_ACCELERATION = 50.0f;
const float Tank::TANK_MAX_VELOCITY =  75.0f;
const float Tank::TANK_ROTATION_SPEED = 1.0f;
const float Tank::TANK_CANNON_LOAD_TIME = 5.0f;
const float Tank::MG_RATE_OF_FIRE = 0.0857f; //RPS = 60 / 700 RPM
const float Tank::MG_MAX_HEAT = 1.0f;
const float Tank::MG_HEAT_PER_SHOT = 0.05f;
const float Tank::MG_COOLING_PER_SECOND = 0.2f;
const float Tank::TANK_TRACKMARK_DISTANCE = 2.5f;
const float Tank::TANK_ACCELERATE_SOUND_DURATION = 6.046f;
const float Tank::TANK_MOVE_SOUND_DURATION = 2.02014f;
const float Tank::TANK_DEACCELERATE_SOUND_DURATION = 2.248750f;
const float Tank::TANK_CANNON_SIGHT_SPEED = 1.0f;
const float Tank::TANK_COLLISIONSPHERE_RADIUS = 18.0f;
const float Tank::FRICTION_AND_MASS_COLLISION_FACTOR = 0.8f;
const float Tank::BURNING_TIME = 3.0f;
const float Tank::AITANK_MAX_AIM_TIME = 1.0f;
const float Tank::MOVING_DUST_TIME = 0.1f;
float Tank::TANK_CANNON_ACCURACY = 1.0f;
float Tank::TANKMG_ACCURACY = 0.05f; //the smaller the better
const Height Tank::TANK_HEIGHT = 2.19f * Game::METER;
int Tank::NUMBER_OF_COLLISIONSPHERES = 2;

#define MG_RATE_OF_FIRE_DEF 0.12f
#define TANKMG_ACCURACY_DEF 0.11f

HTEXTURE Tank::player1Tanktex = NULL;
HTEXTURE Tank::player2Tanktex = NULL;
HTEXTURE Tank::enemyTanktex = NULL;
hgeAnimation * Tank::player1TankImage = NULL;
hgeAnimation * Tank::player2TankImage = NULL;
hgeAnimation * Tank::enemyTankImage = NULL;
HEFFECT Tank::tank_idle_sound = NULL;
HEFFECT Tank::tank_accelerate_sound = NULL;
HEFFECT Tank::tank_move_sound = NULL;
HEFFECT Tank::tank_deaccelerate_sound = NULL;
HEFFECT Tank::turret_move_sound = NULL;
HEFFECT Tank::turret_clunk_sound = NULL;
HEFFECT Tank::tank_fire_cannon_sound = NULL;
HEFFECT Tank::tank_fire_mg_sound = NULL;
HEFFECT Tank::tank_crash_other_tank_sound = NULL;
hgeParticleSystem * Tank::tankFireMGParticleSystem = NULL;
hgeParticleSystem * Tank::tankFireMGParticleSystem2 = NULL;
hgeParticleSystem * Tank::tankMoveParticleSystem = NULL;

float Tank::possibilityOfTankDeathFromATRocket[];
float Tank::possibilityOfTankDeathFromTankAmmo[];

#define AIM_TIME_EASY_MIN 5.0f
#define AIM_TIME_EASY_MAX 10.0f
#define AIM_TIME_NORMAL_MIN 3.0f
#define AIM_TIME_NORMAL_MAX 7.0f
#define AIM_TIME_HARD_MIN 1.0f
#define AIM_TIME_HARD_MAX 3.0f

//smoke:
#define OFF_SMOKE_DISTANCE 200.0f

Tank::Tank(void)
{
	tanks++;
}

Tank::Tank(int tankType, CombatObject::SIDE side, Location startingLocation, Direction startDirection)
:
tank_turn_left(false),
tank_turn_right(false),
tank_forward(false),
tank_backwards(false),
tank_turret_turn_left(false),
tank_turret_turn_right(false),
tank_machine_gun_turn_left(false),
tank_machine_gun_turn_right(false),
tank_fire_cannon(false),
tank_toggle_firemode(false),
tank_turret_angle_up(false),
tank_turret_angle_down(false)
{
	set(tankType, side, startingLocation, startDirection);
}

Tank::~Tank(void)
{
	delete trackMark;
	trackMark = NULL;
}

void Tank::set(int tankType, CombatObject::SIDE side, Location startingLocation, Direction startDirection)
{
	CombatAIObject::set(GameObject::TANK, startingLocation, startDirection, side);

	//commands
	tank_turn_left = false;
	tank_turn_right = false;
	tank_forward = false;
	tank_backwards = false;
	tank_turret_turn_left = false;
	tank_turret_turn_right = false;
	tank_machine_gun_turn_left = false;
	tank_machine_gun_turn_right = false;
	tank_fire_cannon = false;
	tank_fire_mg = false;
	tank_toggle_firemode = false;
	tank_turret_angle_up = false;
	tank_turret_angle_down = false;
	tank_mg_angle_up = false;
	tank_mg_angle_down = false;

	setHeight(TANK_HEIGHT);

	//AI
	turretAImsg_halt = false;

	//dbg
	counter = 0;

	this->tankType = (TANKTYPE)tankType;

	//orders
	if (tankType == ENEMY_TANK)
	{
		dbgName = "ENEMY_TANK";
		this->image = enemyTankImage;
		turret.image = Turret::enemyTurretImage;
		turret.turningSpeed = Turret::TURRET_ROTATION_SPEED;
		cannonSight.visible  = true;
		MGSight.visible  = false;
		computer_controlled = true;
		orders = STATE_ATTACK;
		setTargetLocation(Location(x, randFloat(200.0f, 300.0f)));
		distance_to_drive = 50.0f + (float)rand() / RAND_MAX * 200.0f;
		distance_driven = 0.0f;
		target_tank_in_sight = false;
		targetTank = NULL;
		horizontal_angle_right = false;
		vertical_angle_right = false;
	}
	else if (tankType == PLAYER_1_TANK || tankType == PLAYER_2_TANK)
	{
		if (tankType == PLAYER_1_TANK)
		{
			dbgName = "PLAYER_1_TANK";
			this->image = player1TankImage;
		}
		else
		{
			dbgName = "PLAYER_2_TANK";
			this->image = player2TankImage;
		}

		turret.image = Turret::playerTurretImage;
		turret.turningSpeed = 3.0f;
		cannonSight.visible  = true;
		MGSight.visible  = false;
		computer_controlled = false;
	}

	this->image->SetColor(0xffffffff);

	this->speed = 0.0f;
	this->turningSpeed = TANK_ROTATION_SPEED;
	dx = speed * cos(direction);
	dy = -speed * sin(direction);
	this->rotation = 0.0f;

	tankMG.x = turret.x = this->x, tankMG.y = turret.y = this->y;
	turret.direction = direction;
	tankMG.direction = direction;

	this->accelerating = false;
	this->moving = false;
	this->deaccelerating = false;
	this->turning = false;
	this->turret_adjusting = false;
	this->firing_cannon = false;
	this->firing_mg = false;
	this->mg_overheated = false;

	this->tank_idle_playing = false;
	this->tank_accelerate_playing = false;
	this->tank_move_playing = false;
	this->tank_deaccelerate_playing = false;
	this->turret_move_playing = false;	

	this->tank_accelerate_sound_timer = -10.0f;
	this->tank_move_sound_timer = -10.0f;
	this->tank_deaccelerate_sound_timer = -10.0f;

	//cannon
	this->cannontimer = 0.0f;
	this->cannon_loaded = true;

	//mg
	this->mgHeat = 0.0f;

	//track marks
	trackMark = new hgeSprite(player1Tanktex, 0.0f, 273.0f, 1.0f, 304.0f - 273.0f);
	trackMark->SetHotSpot(0.0f, (304.0f - 273.0f) / 2.0f); 
	this->trackMarkCounter = TANK_TRACKMARK_DISTANCE;

	//sights
	this->tank_firemode = TANK_FIREMODE_CANNON;
	this->cannonSight.setType(Sight::SIGHT_TYPE_CANNON);
	this->MGSight.setType(Sight::SIGHT_TYPE_MG);

	cannonProj = NULL;

	this->unable_to_turn = false;
	this->colliding = false;
	this->newCollision = false;
	this->collisions = 0;
	this->lastCollisions = 0;
	this->collisionSphereCount = 2; 
	setCollisionSphereRadius(TANK_COLLISIONSPHERE_RADIUS);

	engineSound = (ENGINE_SOUND)-1;

	//dbg:
	othertank = NULL;

	cant_turn_left = false;
	cant_turn_right = false;
	collisioncalls = 0;
	processactioncalls = 0;
	collisionToFront = false;
	collisionToBack = false;
	collisionToSide = false;

	moveCommand = true;
	rotation_multiplicator = 0.0f;

	burning = false;

	movingDustTimer = 0.0f;

	this->statisticValue = CombatObject::STATISTIC_VALUE_OTHER;
}

void Tank::init(void) //class init
{
	game.renderLoadScreen("LOADING TANK CLASS");

	//tank body
	player1Tanktex = hge->Texture_Load("player1_tank.png");
	player1TankImage = new hgeAnimation(player1Tanktex, 1, 0.0f, 0.0f, 0.0f, 61.0f, 34.0f);
	player1TankImage->SetHotSpot(/*25.0f*/61.0f/2.0f, 34.0f/2.0f);

	player2Tanktex = hge->Texture_Load("player2_tank.png");
	player2TankImage = new hgeAnimation(player2Tanktex, 1, 0.0f, 0.0f, 0.0f, 61.0f, 34.0f);
	player2TankImage->SetHotSpot(/*25.0f*/61.0f/2.0f, 34.0f/2.0f);

	enemyTanktex = hge->Texture_Load("enemy_tank.png");
	enemyTankImage = new hgeAnimation(enemyTanktex, 1, 0.0f, 0.0f, 0.0f, 61.0f, 34.0f);
	enemyTankImage->SetHotSpot(/*25.0f*/61.0f/2.0f, 34.0f/2.0f);

	//sounds
	tank_idle_sound = hge->Effect_Load("tank_idle.wav");
	tank_accelerate_sound = hge->Effect_Load("tank_accelerate.wav");
	tank_move_sound = hge->Effect_Load("tank_move.wav");
	tank_deaccelerate_sound = hge->Effect_Load("tank_deaccelerate.wav");
	turret_move_sound = hge->Effect_Load("tank_turret_move.wav");
	turret_clunk_sound = hge->Effect_Load("tank_turret_clunk.wav");
	tank_fire_cannon_sound = hge->Effect_Load("w_t72fire_long_echo.wav");
	tank_fire_mg_sound = hge->Effect_Load("w_l96a_ss.wav");
	tank_crash_other_tank_sound = hge->Effect_Load("143578_SOUNDDOGS_CR tank crash.wav");

	//mg firing particles
	tankFireMGParticleSystem = new hgeParticleSystem("mgparticle1.psi", game.smokeParticleSprite3, 50.0f);
	tankFireMGParticleSystem->info.bRelative = false;
	tankFireMGParticleSystem->info.nEmission *= (float)game.amountOfParticles / 100.0f;
	tankFireMGParticleSystem2 = new hgeParticleSystem("mgparticle2.psi", game.smokeParticleSprite2, 50.0f);
	tankFireMGParticleSystem2->info.nEmission *= (float)game.amountOfParticles / 100.0f;
	
	//cannon firing particles
	//tankFireBaseParticleSystem = new hgeParticleSystem("dust_rise.psi", game.smokeParticleSprite);
	//tankFireBaseParticleSystem->info.fParticleLifeMax = 1.5f; //?
	//tankFireBaseParticleSystem->info.nEmission;
	//tankFireBaseParticleSystem->info.nEmission *= (float)game.amountOfParticles / 100.0f;

	//tank moving ps
	tankMoveParticleSystem	= new hgeParticleSystem("tank_move_dust.psi", game.smokeParticleSprite, 50.0f);
	tankMoveParticleSystem->info.nEmission = 400.0f;
	tankMoveParticleSystem->info.nEmission *= (float)game.amountOfParticles / 100.0f;
	int r = 220;
	int g = 170;
	int b = 70;
	tankMoveParticleSystem->info.colColorStart = getColor(100, r, g, b);
	tankMoveParticleSystem->info.colColorEnd = getColor(0, r, g, b);
	tankMoveParticleSystem->info.fSizeEnd = 1.0f;
	tankMoveParticleSystem->info.fSizeStart;
	tankMoveParticleSystem->info.sprite = game.animations[Game::ANIMATION_PARTICLE_SPRITE_CLOUD_2];

	//Skill level tables:
	possibilityOfTankDeathFromATRocket[Game::SKILL_LEVEL_EASY] = 0.2f;
	possibilityOfTankDeathFromATRocket[Game::SKILL_LEVEL_NORMAL] = 0.4f;
	possibilityOfTankDeathFromATRocket[Game::SKILL_LEVEL_HARD] = 0.5f;
	possibilityOfTankDeathFromTankAmmo[Game::SKILL_LEVEL_EASY] = 0.5f;
	possibilityOfTankDeathFromTankAmmo[Game::SKILL_LEVEL_NORMAL] = 0.8f;
	possibilityOfTankDeathFromTankAmmo[Game::SKILL_LEVEL_HARD] = 0.9f;
}

void Tank::release(void)
{
	//Free tank bod.
	hge->Texture_Free(player1Tanktex);
	hge->Texture_Free(player2Tanktex);
	hge->Texture_Free(enemyTanktex);

	//Free Willy? Nah, free sounds.
	hge->Effect_Free(tank_idle_sound);
	hge->Effect_Free(tank_accelerate_sound);
	hge->Effect_Free(tank_move_sound);
	hge->Effect_Free(tank_deaccelerate_sound);
	hge->Effect_Free(turret_move_sound);
	hge->Effect_Free(turret_clunk_sound);
	hge->Effect_Free(tank_fire_cannon_sound);
	hge->Effect_Free(tank_fire_mg_sound);
	hge->Effect_Free(tank_crash_other_tank_sound);

	delete tankFireMGParticleSystem; tankFireMGParticleSystem = NULL;
	delete tankFireMGParticleSystem2; tankFireMGParticleSystem2 = NULL;
	delete tankMoveParticleSystem; tankMoveParticleSystem = NULL;
}
void Tank::processAction(void)
{
	processOrders();

	//pre-collision detection actions:
	lastCollisions = collisions;
}

void Tank::processOrders(void)
{
	//***MOVING***

	//Tank body - body moving
	moveCommand = false;
	accelerating = false;
	//moving = false;
	turning = false;
	turning_left = false;
	turning_right = false;
	deaccelerating = false;
	float mgTurn = 0.0f;
	rotation_multiplicator = 0.0f;

	if (tank_turn_left && !cant_turn_left && !burning)
	{
		rotation_multiplicator = -1.0f;
		turning = true;	
		turning_left = true;
	}

	if (tank_turn_right && !cant_turn_right && !burning)
	{
		rotation_multiplicator = 1.0f;
		turning = true;
		turning_right = true;
	}

	//boundaries for player controlled tank - orders to stay at the field
	if (!computer_controlled)
	{
		bool fwd_stop = false;
		bool bwd_stop = false;

		if (x < 50.0f)
		{
			if (direction > HALF_PI && direction < ONE_AND_A_HALF_PI)
				fwd_stop = true;
			else
				bwd_stop = true;
		}
		if (x > 750.0f)
		{
			if (direction > ONE_AND_A_HALF_PI && direction < HALF_PI)
				bwd_stop = true;
			else
				fwd_stop = true;
		}
		if (y < 300.0f)
		{
			if (direction > M_PI && direction < PI_TIMES_TWO)
				fwd_stop = true;
			else
				bwd_stop = true;
		}
		if (y > 650.0f)
		{
			if (direction > 0.0f && direction < M_PI)
				fwd_stop = true;
			else
				bwd_stop = true;
		}

		if (fwd_stop)
		{
			tank_forward = false;

		}
		if (bwd_stop)
		{
			tank_backwards = false;
		}		
	}

	if (tank_forward && !burning)
	{
		//normal acceleration
		if (speed >= 0 && speed < maxSpeed)
		{
			speed += TANK_ACCELERATION * timerDelta;
			accelerating = true;
		}
		//slowing down from backward movement
		if (speed <= 0)
		{
			speed += TANK_ACCELERATION * 4 * timerDelta;
			deaccelerating = true;
		}
		moveCommand = true;
	}

	if (tank_backwards && !burning)
	{
		//normal acceleration
		if (speed <= 0 && speed > -maxSpeed)
		{
			speed -= TANK_ACCELERATION * timerDelta;
			accelerating = true;
		}
		//slowing down from forward movement
		if (speed >= 0)
		{
			speed -= TANK_ACCELERATION * 4 * timerDelta;;
			deaccelerating = true;
		}
		moveCommand = true;
	}

	if (turning && !burning)
	{
		rotation = turningSpeed * rotation_multiplicator * timerDelta;
		direction += rotation;
		turret.direction += rotation;
		mgTurn += rotation;

		if (computer_controlled)
		{
			angleToTurn -= fabs(rotation);

			if (angleToTurn <= 0.0f)
			{
				tank_turn_left = false;
				tank_turn_right = false;
				turning_left = false;
				turning_right = false;
				turning = false;
			}
		}
	}

	//automatic deacceleration
	if (((moveCommand == false && !turning) || burning) && speed != 0.0f)
	{
		deaccelerating = true;

		//deaccelerating from forward movement
		if (speed > 0.0f)
		{
			speed -= TANK_ACCELERATION * 2.0f * timerDelta;
			
			if (speed < 0.0f)
				speed = 0.0f;		
		}

		//deaccelerating from backwards movement
		if (speed < 0.0f)
		{
			speed += TANK_ACCELERATION * 2.0f * timerDelta;

			if (speed > 0.0f)
				speed = 0.0f;			
		}

		//stopped, not moving
		if (speed == 0.0f)
		{
			deaccelerating = false;
			moving = false;
		}
	}

	//move object
	dx = speed * cos(direction);
	dy = speed * sin(direction);

	//movement
	if (speed != 0)
	{
		moving = true;
		x += dx * timerDelta, y += dy * timerDelta;
		turret.x = this->x;
		turret.y = this->y;

		if (computer_controlled)
		{
			movingDistance -= speed * timerDelta;

			if (movingDistance < 0.0f)
			{
				tank_forward = false;
				tank_backwards = false;
			}
		}
	}

	if (direction > 2 * M_PI)
		direction = 0.0f;
	if (direction < 0.0f)
		direction = 2 * M_PI;

	if (burning)
		return;

	//Fire mode
	if (tank_toggle_firemode)
	{
		if (tank_firemode == TANK_FIREMODE_CANNON)
			tank_firemode = TANK_FIREMODE_MG;
		else if (tank_firemode == TANK_FIREMODE_MG)
			tank_firemode = TANK_FIREMODE_CANNON;

		cannonSight.visible = !cannonSight.visible;
		MGSight.visible = !MGSight.visible;

		tank_toggle_firemode = false;
	}

	//mouse movement for sight
	if (this == game.player_1.tank)
	{

		//Horizontal movement
		GameObject temp = GameObject();
		hge->Input_GetMousePos(&(temp.x), &(temp.y));

		float err = 0.01f;

		if (tank_firemode == TANK_FIREMODE_CANNON)
		{
			float angleOfLineFromTankToCursor = objectAngle(&temp);


			if (directionalSmallestAngleBetween(turret.direction, angleOfLineFromTankToCursor) > err)
				tank_turret_turn_right = true;
			else if (directionalSmallestAngleBetween(turret.direction, angleOfLineFromTankToCursor) < -err)
				tank_turret_turn_left = true;

			//Vertical adjustment for turret
			float distToCursor = objectDistance(&temp);
			float distToSight = objectDistance(&cannonSight);
			if (distToSight < distToCursor - 0.2f)
				tank_turret_angle_up = true; 
			else if (distToSight > distToCursor + 0.2f)
				tank_turret_angle_down = true;
		}
	}

	//Turret adjusting, cannon firing
	turret_adjusting = false;
	if (tank_firemode == TANK_FIREMODE_CANNON || computer_controlled)
	{	
		if (tank_turret_turn_left)
		{
			turret.direction -= turret.turningSpeed * timerDelta;
			mgTurn -= turret.turningSpeed * timerDelta;

			turret_adjusting = true;
		}
		if (tank_turret_turn_right)
		{
			turret.direction += turret.turningSpeed * timerDelta;
			mgTurn += turret.turningSpeed * timerDelta;

			turret_adjusting = true;
		}
		if (tank_turret_angle_up)
		{
			if (turret.angle <= turret.MAX_ANGLE)
				turret.angle += turret.TURRET_ANGLE_ADJUSTMENT_SPEED * timerDelta ;
			turret_adjusting = true;
		}
		if (tank_turret_angle_down)
		{
			if (turret.angle > 0.0f)
				turret.angle -= turret.TURRET_ANGLE_ADJUSTMENT_SPEED * timerDelta ;
			else
				turret.angle = 0.0f;
			turret_adjusting = true;
		}

		//***FIRING CANNON*** -With cannon you can.

		if (tank_fire_cannon && cannon_loaded && turret.ammunition > 0 && isAtBattleArea())
		{
			//counter++;
			firing_cannon = true;
			cannontimer = hge->Timer_GetTime();
			cannon_loaded = false;
			cannonLoadTimer = hge->Timer_GetTime();

			tank_fire_cannon = false;
		}
	}
	if (turret.direction > 2 * M_PI)
		turret.direction = 0.0f;
	if (turret.direction < 0.0f)
		turret.direction = 2 * M_PI;

	cannonSight.setPosition(turret.x, turret.y, turret.direction, turret.angle);
	turret.setAimingLocation();

	tankMG.setPosition(turret.x, turret.y, turret.direction, mgTurn);

	//Maghine gun
	if (tank_firemode == TANK_FIREMODE_MG || computer_controlled)
	{
		float additionalSpeed = 0.0f;
		float additionalSpeed2 = 0.0f;

		if (!computer_controlled)
		{
			additionalSpeed = 10.0f;
			additionalSpeed2 = 0.4f;
		}

		if (tank_machine_gun_turn_left || tankMG.turning_left)
		{
			if (tankMG.turningSpeed > 0.0f)
				tankMG.turningSpeed = 0;
			if (tankMG.turningSpeed > -TankMachineGun::TANKMG_ROTATION_MAX_SPEED)
				tankMG.turningSpeed -= TankMachineGun::TANKMG_ROTATION_ACCELERATION + additionalSpeed;			
			tankMG.direction += tankMG.turningSpeed * timerDelta;
		}
		else if (tank_machine_gun_turn_right || tankMG.turning_right)
		{
			if (tankMG.turningSpeed < 0.0f)
				tankMG.turningSpeed = 0;
			if (tankMG.turningSpeed < TankMachineGun::TANKMG_ROTATION_MAX_SPEED)
				tankMG.turningSpeed += TankMachineGun::TANKMG_ROTATION_ACCELERATION + additionalSpeed;			
			tankMG.direction += tankMG.turningSpeed * timerDelta;
		}
		else
		{
			tankMG.turningSpeed = 0.0f;
		}
		if (tank_mg_angle_up)
		{
			if (tankMG.angle <= tankMG.MAX_ANGLE)
				tankMG.angle += (tankMG.MG_ANGLE_ADJUSTMENT_SPEED + additionalSpeed2) * timerDelta ;
		}

		if (tank_mg_angle_down)
		{
			if (tankMG.angle > 0.0f)
				tankMG.angle -= (tankMG.MG_ANGLE_ADJUSTMENT_SPEED + additionalSpeed2) * timerDelta ;
			else
				tankMG.angle = 0.0f;
		}

		mgDelay -= timerDelta;
		if (tank_fire_mg && mgDelay <= 0.0f && tankMG.ammunition > 0 &&!mg_overheated && isAtBattleArea())
		{
			firing_mg = true;
			mgDelay = MG_RATE_OF_FIRE_DEF;
		}
	}

	if (tankMG.direction > 2 * M_PI)
		tankMG.direction = 0.0f;
	if (tankMG.direction < 0.0f)
		tankMG.direction = 2 * M_PI;

	if (computer_controlled || this != game.player_1.tank)
	{
		tankMG.setAimingLocation();		
	}
	else
	{
		GameObject temp = GameObject();
		hge->Input_GetMousePos(&(temp.x), &(temp.y));
		float ang = objectAngle(temp.getLocation());
		float xShaking = randFloat(0.0f, 2.0f) * cos(ang) * speed / TANK_MAX_VELOCITY * 10.0f;
		float yShaking = randFloat(0.0f, 2.0f) * sin(ang) * speed / TANK_MAX_VELOCITY * 10.0f;  
 
		temp.x = temp.x + xShaking;  
		temp.y = temp.y + yShaking;  
		tankMG.direction = ang; 
		tankMG.angle = objectDistance(&temp);
		tankMG.aimingLocation = temp.getLocation(); 
	}
	MGSight.setLocation(tankMG.aimingLocation);

	//after set variables/flags:
	cant_turn_left = false;
	cant_turn_right = false;

	processactioncalls++;
}

void Tank::processAI(void)//attack(void)
{
	if (computer_controlled)
	{
		turret.setEnemyTargetProperties();
		tankMG.setEnemyTargetProperties();
	}

	if (side == ATTACKER_SIDE)
		int u = 2;

	switch (state)
	{
	case STATE_ATTACK: attack(); break;
	case STATE_DEFEND: defend(); break;
	case STATE_FIGHT: fight(); break;
	default: setState(orders);
	}

	if (stateChangeTimer > 0.0f)
	{
		stateChangeTimer -= timerDelta;

		if (stateChangeTimer <= 0.0f)
			setState(nextState);
	}
}

void Tank::collide(GameObject * otherObject, ArcPoint hittingPoint)
{
	if (otherObject->type == GameObject::TANK)
	{
		Tank * tank = static_cast<Tank *>(otherObject);

		if (!getCollisionSphere1(25.0f).testHit(tank->getCollisionSphere1(25.0f)) &&
			!getCollisionSphere1(25.0f).testHit(tank->getCollisionSphere2(25.0f)) &&
			!getCollisionSphere2(25.0f).testHit(tank->getCollisionSphere1(25.0f)) &&
			!getCollisionSphere2(25.0f).testHit(tank->getCollisionSphere2(25.0f)))
				return;

		speed = -speed;
		tank->speed = -tank->speed;

		if (tank_turn_left)
			cant_turn_left = true;
		if (tank_turn_right)
			cant_turn_right = true;

		setLocation(ArcPoint(getLocation(), 0.5f, objectAngle(otherObject) + M_PI).getLocation());

		if (speed < -10.0f || speed > 10.0f ||
			tank->speed < -10.0f || tank->speed > 10.0f)
			collisions++;
	}

	else if (otherObject->type == GameObject::TANK_CARCASS)
	{
		Tank_Carcass * tankCarcass = static_cast<Tank_Carcass *>(otherObject);

		if (!getCollisionSphere1(25.0f).testHit(tankCarcass->getCollisionSphere1(25.0f)) &&
			!getCollisionSphere1(25.0f).testHit(tankCarcass->getCollisionSphere2(25.0f)) &&
			!getCollisionSphere2(25.0f).testHit(tankCarcass->getCollisionSphere1(25.0f)) &&
			!getCollisionSphere2(25.0f).testHit(tankCarcass->getCollisionSphere2(25.0f)))
				return;
		
		//move away when spawning
		checkSpawn();

		speed = -speed;

		if (tank_turn_left && !cant_turn_right)
		{
			cant_turn_left = true;
		}
		if (tank_turn_right && !cant_turn_left)
		{
			cant_turn_right = true;
		}

		setLocation(ArcPoint(getLocation(), 0.5f, objectAngle(otherObject) + M_PI).getLocation());

		if (speed < -10.0f || speed > 10.0f)
			collisions++;
	}
	else if (otherObject->type == GameObject::FOOT_SOLDIER)
	{
		otherObject->collide(this, hittingPoint);
	}
	else if (otherObject->type == GameObject::PROJECTILE)
	{
		if (game.godMode &&
			(tankType == TANKTYPE::PLAYER_1_TANK ||
			tankType == TANKTYPE::PLAYER_2_TANK)) //debuggod
			return;

		if (!isOverLapping(otherObject))
			return;

		Projectile * other = static_cast<Projectile *>(otherObject);
		
		if (other->shooter == this)
			return;

		if (other->getHeight() > getHeight() && other->projectileType != Projectile::AIR_TO_GROUND_MISSILE)
			return;

		bool destroy = false;

		if (other->projectileType == Projectile::TANK_CANNON_PROJECTILE)
		{	
			if (1.0f / timerDelta < 30.0f)
				destroy = true;

			float probability   = 1.0f;
			if(this->side == CombatObject::DEFENDER_SIDE)
			{
				probability  = possibilityOfTankDeathFromTankAmmo[game.skillLevel];
			}
			else
			{
				probability  = possibilityOfTankDeathFromTankAmmo[Game::SKILL_LEVEL_MAX - game.skillLevel];
			}

			float rand = randFloat(0.0f, 1.0f);

			if (rand < probability)
			{
				destroy = true;

				if (other->shooter != NULL)
				{
					if (other->shooter == game.player_1.tank)
					{
						game.player_1.addScore(this);
					}
					else if (game.two_player_game && other->shooter == game.player_2.tank)
					{
						game.player_1.addScore(this);
					}
				}
			}
			else
			{
				game.objects.addGameObject(new Effect(Effect::TANKSHELL_EXPLOSION, hittingPoint.getLocation(), direction, NULL, 0.1f));
				other->hit();
				return;
			}
		}
		else if (other->projectileType == Projectile::AT_ROCKET)
		{
			float probability   = 1.0f;
			if(this->side == CombatObject::DEFENDER_SIDE)
			{
				probability  = possibilityOfTankDeathFromATRocket[game.skillLevel];
			}
			else
			{
				probability  = possibilityOfTankDeathFromATRocket[Game::SKILL_LEVEL_MAX - game.skillLevel];
			}

			if (randFloat(0.0f, 1.0f) < probability  )
				destroy = true;
			else
			{
				game.objects.addGameObject(new Effect(Effect::TANKSHELL_EXPLOSION, hittingPoint.getLocation(), direction, NULL, 0.1f));
				other->hit();
				return;
			}
		}
		else if (other->projectileType == Projectile::AIR_TO_GROUND_MISSILE)
		{
			if (!burning)
				destroy = true;
		}
		else
		{
			game.objects.addGameObject(new Effect(Effect::PROJECTILE_HIT_TANK, hittingPoint.getLocation(), other->direction));
		}
		
		if (destroy)
		{
			game.objects.addGameObject(new Effect(Effect::TANK_EXPLOSION, hittingPoint.getLocation(), other->direction, this));
			active = false;
			burning = true;
			burningTimer = BURNING_TIME;
		}

		other->hit();
	}

	else if (otherObject->type == GameObject::EFFECT)
	{
		if (game.godMode &&
			(tankType == TANKTYPE::PLAYER_1_TANK ||
			tankType == TANKTYPE::PLAYER_2_TANK)) //debuggod
			return;

		if (burning)
			return;

		Effect * e = static_cast<Effect *>(otherObject);

		if (e->collisionTimer <= 0.0f)
			return;

		bool destroy = false;
		
		
		if (e->effectType == Effect::NAPALM_EXPLOSION)
		{
			Distance dist = squaredObjectDistance(otherObject);
			if (compareDistance(dist, e->counter1) < 0.0f)
			{
				destroy = true;
			}
		}
		else if (e->effectType == Effect::DEBRIS_FALL_SOUND_EMITTER)
		{
			if (isOverLapping(otherObject))
			{
				DebrisFallEffectData * dfed = static_cast<DebrisFallEffectData *>(e->ed);
				e->playSoundAtLocation(dfed->metalHitSounds[randInt(0, dfed->nMetalHitSounds - 1)], 70, randFloat(1.0f, 1.2f));
			}
		}

		if (destroy)
		{
			game.objects.addGameObject(new Effect(Effect::TANK_EXPLOSION, getLocation(), otherObject->direction, this));
			active = false;
			burning = true;
			burningTimer = BURNING_TIME;
		}
	}
}

//orders to play sounds, animations, fire particle systems etc.
void Tank::processInteraction(void) 
{

	if (burning)
	{
		if (burningTimer > 0.0f)
			burningTimer -= timerDelta;
		else
		{
			destroy();
			Tank_Carcass * carcass = new Tank_Carcass(getLocation(), direction, turret.direction, tankMG.direction);
			game.objects.addGameObject(carcass);		
		}
	}

	//Interaction:
	newCollision = false;
	if (collisions > 0)
	{

		//unable_to_turn = true;
		colliding = true;

		if (lastCollisions < collisions)
			newCollision = true;
	}
	else
	{
		//unable_to_turn = false;
		colliding = false;
	}
	collisions = 0;

	//Multimedia:

	playSound();
	playAnimation();

	if (firing_cannon)
	{		
		fireTurret();
		firing_cannon = false;
	}

	if (!cannon_loaded)
	{
		float t = hge->Timer_GetTime();
		float t2 = hge->Timer_GetTime() - cannonLoadTimer;
		if (hge->Timer_GetTime() - cannonLoadTimer > TANK_CANNON_LOAD_TIME)
			cannon_loaded = true;
	}

	//mg
	if (mgHeat > MG_MAX_HEAT)
	{
		mg_overheated = true;
	}

	if (mgHeat > 0.0f)
	{
		mgHeat -= timerDelta * 0.3f;
	}

	if (mgHeat <= 0.0f)
	{
		mg_overheated = false;
	}

	//kk:lla ampuminen
	if (firing_mg && !mg_overheated)
	{
		fireTankMG();
		mgHeat += 0.05f; //MG_HEAT_PER_SHOT;
	}

	firing_mg = false;

	//track marks
	if ((moving || turning) && trackMarkCounter > 0.0f)
	{
		float frontDist = sqrt(pow(dx * timerDelta,2.0f)+pow(dy * timerDelta,2.0f));
		trackMarkCounter -= frontDist;
	}
	if ((moving || turning) && trackMarkCounter < 0.0f)
	{
		game.objects.background->modify(trackMark, x, y, direction);
		trackMarkCounter = TANK_TRACKMARK_DISTANCE;
	}

	//moving dust rise particle effect
	if (speed > 20.0f || speed < -20.0f)
	{
		movingDustTimer += timerDelta;

		if (movingDustTimer > MOVING_DUST_TIME)
		{
			movingDustTimer = 0.0f;

			tankMoveParticleSystem->info.colColorStart = game.currentLevel->dustColor; //getColor(100, 255, 200, 100);
			tankMoveParticleSystem->info.colColorEnd = modifyColor(game.currentLevel->dustColor, 0, -1, -1, -1); //getColor(0, 255, 200, 100);

			if (game.currentLevel->night)
			{
				tankMoveParticleSystem->info.colColorStart = colorMul(tankMoveParticleSystem->info.colColorStart.GetHWColor(), game.currentLevel->getObjectColor());
				tankMoveParticleSystem->info.colColorEnd = colorMul(tankMoveParticleSystem->info.colColorEnd.GetHWColor(), game.currentLevel->getObjectColor());
			}

			game.objects.fireParticleSystem(tankMoveParticleSystem, x, y, direction, true);
		}
	}
	
	float treeFallDist = 100.0f;
	if ((x < 0 - treeFallDist || 
		x > Game::SCREENWIDTH + treeFallDist || 
		y < 0 - treeFallDist || 
		y > Game::SCREENHEIGHT + treeFallDist) 
		&& speed > 20.0f)
	{
		if (randomEvent(5.0f))
			playSoundAtLocation(game.sounds[randInt(Game::SOUND_TREE_FALL_1, Game::SOUND_TREE_FALL_MAX)]);
	}


	turretBarrelTipLastFrameLocation = turret.getBarrelTipLocation()

	if (computer_controlled)
	{
		tank_turret_turn_left = false;
		tank_turret_turn_right = false;
		tank_machine_gun_turn_left = false;
		tank_machine_gun_turn_right = false;
		tank_turret_angle_up = false;
		tank_turret_angle_down = false;
		tank_mg_angle_up = false;
		tank_mg_angle_down = false;
	}

	if (spawnCounter > 0)
		spawnCounter--;

	startChecks = true;
}

void Tank::render(void)
{
	bool render_shadows = true;

	if (game.currentLevel->night)
	{
		render_shadows = (!burning);
	}

	if (render_shadows)
	{
		renderTankShadows(this);
	}

	GameObject::render();

	//render turret
	turret.render();

	//render tank maghine gun
	tankMG.render();

	if (computer_controlled && game.show_debug_text)
	{
		float yp = 0.0f;
		fnt->printf(x + 50.0f, y + (yp += 11), "state: %s", getStateString());
		fnt->printf(x + 50.0f, y + (yp += 11), "movement statePhase: %s", getStatePhaseString(1));
		fnt->printf(x + 50.0f, y + (yp += 11), "turret statePhase: %s", getStatePhaseString(2));
		fnt->printf(x + 50.0f, y + (yp += 11), "mg statePhase: %s", getStatePhaseString(3));	
		fnt->printf(x + 50.0f, y + (yp += 11), "turret turn left %i", tank_turret_turn_left);	
		fnt->printf(x + 50.0f, y + (yp += 11), "turret turn right %i", tank_turret_turn_right);	
	}
}

void Tank::destroy(void)
{
	setStatistics(STATISTIC_VALUE_VEHICLE_DESTROYED);

	if (tankType != ENEMY_TANK)
	{
		if (tankType == PLAYER_1_TANK)
			game.player_1.die();
		if (tankType == PLAYER_2_TANK)
			game.player_2.die();
	}
	else

	hge->Channel_Stop(engine_sound_channel_1);
	hge->Channel_Stop(engine_sound_channel_2);
	hge->Channel_Stop(engine_sound_channel_3);
	hge->Channel_Stop(engine_sound_channel_4);
	hge->Channel_Stop(idle_sound_channel);
	hge->Channel_Stop(move_sound_channel);
	hge->Channel_Stop(turret_sound_channel);
	hge->Channel_Stop(weapon_sound_channel);

	setDestroyed(true);
}

void Tank::playSound(void)
{
	//Silloin kun mitään ei paineta, soitetaan idleä loopissa.
	//Silloin kun painetaan jotain liikkumispainiketta, soitetaan movea loopissa.
	//setting the pan

	//Tank volume level
	int volume = 30;

	if (!active || burning)
	{
		hge->Channel_Stop(idle_sound_channel);
		hge->Channel_Stop(move_sound_channel);
		hge->Channel_Stop(engine_sound_channel_1);
		hge->Channel_Stop(engine_sound_channel_2);
		hge->Channel_Stop(engine_sound_channel_3);
		return;
	}

	//Tank engine sounds:
	{		
		tank_accelerate_sound_timer -= timerDelta;
		tank_deaccelerate_sound_timer -= timerDelta;

		//moving
		if (moving)
		{
			//if playing idle, stop it
			if (hge->Channel_IsPlaying(engine_sound_channel_1))
			{
				hge->Channel_Stop(engine_sound_channel_1);
			}

			//acceleration
			if (accelerating)
			{
				//reset deacceleration sound timer
				tank_deaccelerate_sound_timer = -10.0f;

				//if playing deacceleration, stop it
				if (hge->Channel_IsPlaying(engine_sound_channel_3))
				{
					hge->Channel_Stop(engine_sound_channel_3);
				}

				//if not playing acceleration sound, play it once
				//use timer to prevent rapid replaying from being blocked 
				if (tank_accelerate_sound_timer < 0.0f)
				{
					//if not playing full speed already, play
					if (!hge->Channel_IsPlaying(engine_sound_channel_4))
					{
						engine_sound_channel_2 = playSoundAtLocation(tank_accelerate_sound, volume);
						tank_accelerate_sound_timer = 6.0f; //TANK_ACCELERATE_SOUND_DURATION;
					}
				}				
			}
			//deacceleration
			else if (deaccelerating)
			{
				//reset acceleration sound timer to prevent silent moments
				tank_accelerate_sound_timer = -10.0f;

				//if playing acceleration, stop it
				if (hge->Channel_IsPlaying(engine_sound_channel_2))
				{
					hge->Channel_Stop(engine_sound_channel_2);
				}

				//if playing full speed, stop it
				if (hge->Channel_IsPlaying(engine_sound_channel_4))
				{
					hge->Channel_Stop(engine_sound_channel_4);
				}

				//if not playing deacceleration sound, play it once
				if (tank_deaccelerate_sound_timer < 0.0f)
				{
					engine_sound_channel_3 = playSoundAtLocation(tank_deaccelerate_sound, volume);
					tank_deaccelerate_sound_timer = 5.95f;
				}				
			}
			//normal movement, full speed
			else
			{
				//if not playing full speed sound, play it in a loop
				if (!hge->Channel_IsPlaying(engine_sound_channel_4))
				{
					//play full speed sound only if acceleration sound is no longer playing
					if (tank_accelerate_sound_timer < 0.0f)
					{
						engine_sound_channel_4 = playSoundAtLocation(tank_move_sound, volume, 1.0f, true);
					}
				}	
			}
		}
		//not moving
		else
		{
			//turning
			if (turning)
			{
				//if playing idle, stop it
				if (hge->Channel_IsPlaying(engine_sound_channel_1))
				{
					hge->Channel_Stop(engine_sound_channel_1);
				}

				//if playing acceleration, stop it
				if (hge->Channel_IsPlaying(engine_sound_channel_2))
				{
					hge->Channel_Stop(engine_sound_channel_2);
				}

				//if playing deacceleration, stop it
				if (hge->Channel_IsPlaying(engine_sound_channel_3))
				{
					hge->Channel_Stop(engine_sound_channel_3);
				}

				//if not playing full speed sound, play it in a loop
				if (!hge->Channel_IsPlaying(engine_sound_channel_4))
				{
					//play full speed sound only if acceleration sound is no longer playing
					engine_sound_channel_4 = playSoundAtLocation(tank_move_sound, volume, 1.0f, true);
				}
			}
			//not turning
			//if not playing idle, play it on a loop
			else if (!hge->Channel_IsPlaying(engine_sound_channel_1))
			{
				//if playing full speed, stop it
				if (hge->Channel_IsPlaying(engine_sound_channel_4))
				{
					hge->Channel_Stop(engine_sound_channel_4);
				}

				//wait till deacceleration sound has played
				if (tank_deaccelerate_sound_timer < 0.0f || !hge->Channel_IsPlaying(engine_sound_channel_3))
				{
					engine_sound_channel_1 = playSoundAtLocation(tank_idle_sound, volume, 1.0f, true);
				}
			}
		}
	}

	if (turret_adjusting && !turret_move_playing)
	{
		turret_sound_channel = playSoundAtLocation(turret_move_sound, volume, 1.0f, true);
		turret_move_playing = true;
	}
	if (!turret_adjusting && turret_move_playing)
	{
		hge->Channel_Stop(turret_sound_channel);	
		turret_move_playing = false;
	}

	if (firing_cannon)
	{
		weapon_sound_channel = playSoundAtLocation(tank_fire_cannon_sound, volume);
	}

	if (firing_mg)
	{
		weapon_sound_channel = playSoundAtLocation(tank_fire_mg_sound, 0);
	}

	if (newCollision)
	{
		playSoundAtLocation(tank_crash_other_tank_sound);
	}

	if (game.play_sounds)
	{
		int pan = getLocationPanning();
		volume = getLocationVolume(volume);
		hge->Channel_SetPanning(idle_sound_channel, pan);
		hge->Channel_SetPanning(move_sound_channel, pan);
		hge->Channel_SetPanning(turret_sound_channel, pan);
		hge->Channel_SetPanning(weapon_sound_channel, pan);

		hge->Channel_SetVolume(idle_sound_channel, volume);
		hge->Channel_SetVolume(move_sound_channel, volume);
		hge->Channel_SetVolume(turret_sound_channel, volume);
		hge->Channel_SetVolume(weapon_sound_channel, volume);

		hge->Channel_SetVolume(engine_sound_channel_1, volume);
		hge->Channel_SetVolume(engine_sound_channel_2, volume);
		hge->Channel_SetVolume(engine_sound_channel_3, volume);
		hge->Channel_SetVolume(engine_sound_channel_4, volume);
	}
}

void Tank::playAnimation(void)
{
	turret.playAnimation(firing_cannon);
}

//action
void Tank::setTurning(Direction direction2)
{
	direction2 = directionCheck(direction2);
	turningDirection = direction2;
	angleToTurn = smallestAngleBetween(direction, direction2);

	if (direction > M_PI && direction < PI_TIMES_TWO &&
		direction2 > 0.0f && direction2 < M_PI)
	{
		if (direction - M_PI > direction2)
		{
			tank_turn_right = true;
		}
		else
		{
			tank_turn_left = true;
		}
	}
	else if (direction2 > M_PI && direction2 < PI_TIMES_TWO &&
		direction > 0.0f && direction < M_PI)
	{
		if (direction2 - M_PI > direction)
		{
			tank_turn_left = true;
		}
		else
		{
			tank_turn_right = true;
		}
	}
	else
	{
		if (direction < direction2)
			tank_turn_right = true;
		else
			tank_turn_left = true;
	}
}

void Tank::setMoving(Location location, bool forward)
{
	setMovingDistance(objectDistance(location));
	if (forward)
	{
		tank_forward = true;
		tank_backwards = false;
	}
	else
	{
		tank_forward = false;
		tank_backwards = true;
	}
	setTurning(objectAngle(location));
}

void Tank::stop(void)
{
	movingDistance = 0.0f;
	angleToTurn = 0.0f;
	turning = false;
}

void Tank::aimTurret(void)
{
	if (turret.getEnemyTarget())
	{
		Direction direction1 = turret.direction;
		Direction direction2 = turret.getEnemyTargetDirection();

		//turn barrel
		{

			bool turn_left = false, turn_right = false;

			if (direction1 > M_PI && direction1 < PI_TIMES_TWO &&
				direction2 > 0.0f && direction2 < M_PI)
			{
				if (direction1 - M_PI > direction2)
				{
					turn_right = true;
				}
				else
				{
					turn_left = true;
				}
			}
			else if (direction2 > M_PI && direction2 < PI_TIMES_TWO &&
				direction1 > 0.0f && direction1 < M_PI)
			{
				if (direction2 - M_PI > direction1)
				{
					turn_left = true;
				}
				else
				{
					turn_right = true;
				}
			}
			else
			{
				if (direction1 < direction2)
					turn_right = true;
				else
					turn_left = true;
			}

			if (turn_left)
			{
				tank_turret_turn_left = true;
				tank_turret_turn_right = false;
			}
			else
			{
				tank_turret_turn_left = false;
				tank_turret_turn_right = true;
			}
		}

		float dist = squaredObjectDistance(turret.aimingLocation) - turret.getEnemyTargetDistance();
		
		//adjust barrel height
		{
			if (dist < 0.0f)
			{
				tank_turret_angle_up = true;
				tank_turret_angle_down = false;	
			}
			else
			{
				tank_turret_angle_up = false;
				tank_turret_angle_down = true;	
			}
		}

		bool dirRight = fabs(direction1 - direction2) < 0.1f; //1.0f; //0.1f;

		if (dirRight)
		{
				tank_turret_turn_left = false;
				tank_turret_turn_right = false;
		}

		bool altRight = dist > -1000.0f && dist < 10000.0f;

		if (dirRight && altRight)
		{
			turret.ready_to_fire = true;
		}
		else
			turret.ready_to_fire = false;
	}
}

void Tank::fireTurret(void)
{
	float accuracy = getRelativeFigure(speed, 0.0f, TANK_MAX_VELOCITY, 0.0f, 0.1f);
	Location destination = ArcPoint(cannonSight.getLocation(), objectDistance(&cannonSight) * accuracy, randomDirection()).getLocation(); 
	cannonProj = new Projectile(this, Projectile::TANK_CANNON_PROJECTILE, turret.getBarrelTipLocation(), turret.getBarrelTipHeight(), destination);
	game.objects.addGameObject(cannonProj);
	game.objects.addGameObject(new Effect(Effect::TANK_CANNON_MUZZLE_FLASH, turret.getBarrelTipLocation(5.0f), turret.getDirection()));

	//dust rises from the ground right away when the tank cannon is fired
	hgeParticleSystem ps(&(game.particleSystems[Game::PARTICLE_SYSTEM_DUST_RISE]->info), 50.0f);
	ps.info.colColorStart = modifyColor(game.currentLevel->dustColor, 5, -1, -1, -1);
	ps.info.colColorEnd = modifyColor(game.currentLevel->dustColor, 0, -1, -1, -1);
	ps.info.fSizeEnd = 1.5f;
	ps.info.fSizeStart;
	ps.info.sprite = game.animations[Game::ANIMATION_PARTICLE_SPRITE_CLOUD_2];
	ps.info.fLifetime = 0.5f;
	ps.info.nEmission = 500;
	ps.info.fParticleLifeMax = 6.0f;
	ps.info.fParticleLifeMin = 3.0f;
	ps.info.fSpeedMin = 0.01f;
	ps.info.fSpeedMax = 0.3f;
	ps.info.fTangentialAccelMax = 0.0f;
	ps.info.fTangentialAccelMin = 0.0f;
	ps.info.fRadialAccelMax = 0.0f;
	ps.info.fRadialAccelMin = 0.0f;
	ps.info.fGravityMax = 0.0f;
	ps.info.fGravityMin = 0.0f;
	for (int i = 0; i < 100; i++)
		ps.Update(0.01f);
	ps.info.fLifetime = 0.1f;
	ps.info.nEmission = 1000;
	Location l1 = ArcPoint(getLocation(), 10.0f, direction + HALF_PI).getLocation();
	Location l2 = ArcPoint(getLocation(), 10.0f, direction - HALF_PI).getLocation();
	Location l3 = turret.getBarrelTipLocation(); //ArcPoint(turret.getBarrelTipLocation(), 20.0f, turret.direction + M_PI).getLocation();
	game.objects.fireParticleSystem(&ps, l1.x, l1.y, direction, true);
	game.objects.fireParticleSystem(&ps, l2.x, l2.y, direction, true);
	game.objects.fireParticleSystem(&ps, l3.x, l3.y, direction, true);
	turret.ready_to_fire = false;
}

void Tank::aimTankMG(void)
{
	tankMG.ready_to_fire = false;
	if (tankMG.getEnemyTarget())
	{
		Direction direction1 = tankMG.direction;
		Direction direction2 = tankMG.getEnemyTargetDirection();
		
		//turn barrel
		{

			bool turn_left = false, turn_right = false;

			if (direction1 > M_PI && direction1 < PI_TIMES_TWO &&
				direction2 > 0.0f && direction2 < M_PI)
			{
				if (direction1 - M_PI > direction2)
				{
					turn_right = true;
				}
				else
				{
					turn_left = true;
				}
			}
			else if (direction2 > M_PI && direction2 < PI_TIMES_TWO &&
				direction1 > 0.0f && direction1 < M_PI)
			{
				if (direction2 - M_PI > direction1)
				{
					turn_left = true;
				}
				else
				{
					turn_right = true;
				}
			}
			else
			{
				if (direction1 < direction2)
					turn_right = true;
				else
					turn_left = true;
			}

			if (turn_left)
			{
				tank_machine_gun_turn_left = true;
				tank_machine_gun_turn_right = false;
			}
			else
			{
				tank_machine_gun_turn_left = false;
				tank_machine_gun_turn_right = true;
			}
		}

		float dist = squaredObjectDistance(tankMG.aimingLocation) - tankMG.getEnemyTargetDistance();
		
		//adjust barrel height
		{
			if (dist < 0.0f)
			{
				tank_mg_angle_up = true;
				tank_mg_angle_down = false;	
			}
			else
			{
				tank_mg_angle_up = false;
				tank_mg_angle_down = true;	
			}
		}

		//orders: suppress!
		if (fabs(direction1 - direction2) < 0.4f && dist > -1000.0f && dist < 1000.0f)
			tankMG.ready_to_fire = true;
	}
}

void Tank::fireTankMG(void)
{
	//"Let one fly, we don't die we multiply" -Cypress Hill
	//-Make a bullet to fly to a randomly selected spot near mg sight

	Distance aimingLocationDistance = objectDistance(tankMG.aimingLocation);
	Distance radius = randFloat(0.0f, TANKMG_ACCURACY_DEF) * aimingLocationDistance; //TANK_MG_NO_UNACCURACY
	Angle angle = randomDirection();
	Location targetLocation = ArcPoint(tankMG.aimingLocation, radius, angle).getLocation();

	if (randFloat(0.0f, 1.0f) > 0.5f)
	{
		for (int i = 0; i < 1; i++)
		{
			Location targetLocation2 = ArcPoint(tankMG.aimingLocation, sqrt(radius * aimingLocationDistance), tankMG.getDirection() + randFloat(-aimingLocationDistance/5000.0f, aimingLocationDistance/5000.0f)).getLocation();
			game.objects.addGameObject(new Projectile(this, Projectile::TANK_MG_BULLET, tankMG.getBarrelTipLocation(20.0f), getTankMGBarrelTipHeight(), targetLocation2));
		}
	}
	else
		game.objects.addGameObject(new Projectile(this, Projectile::TANK_MG_BULLET, tankMG.getBarrelTipLocation(20.0f), getTankMGBarrelTipHeight(), targetLocation));

	//bullet shell:
	game.objects.addGameObject(new Effect(Effect::BULLET_SHELL_FLY, tankMG.getLocation(), tankMG.direction + HALF_PI));

	//make particle effects

	Location tankMGBarrelTipLocation = tankMG.getBarrelTipLocation();
	game.objects.fireParticleSystem(tankFireMGParticleSystem2, tankMGBarrelTipLocation.x, tankMGBarrelTipLocation.y, tankMG.direction);	
	game.objects.fireParticleSystem(tankFireMGParticleSystem, tankMGBarrelTipLocation.x, tankMGBarrelTipLocation.y, tankMG.direction + M_PI / 2.0f);
}


void Tank::setState(int newState, Time stateChangeTimer, int startStatePhase, bool nextPhaseReturn)
{
	turret.statePhase = 0;
	tankMG.statePhase = 0;
	CombatAIObject::setState(newState, stateChangeTimer, startStatePhase, nextPhaseReturn);
}

void Tank::attack(void)
{
	switch (statePhase)
	{
	case ATTACK_PHASE_MOVE_TO_POSITION:
		{
			if (first_time_in_state_phase)
			{
				setMoving(getTargetLocation(), true);
				first_time_in_state_phase = false;
			}

			if (isEngaged() && isOnScreen())
			{
				nextStatePhase();
			}
			break;
		}
	case ATTACK_PHASE_FIGHT:
		{
			if (isEngaged())
			{
				setState(STATE_FIGHT, randFloat(0.0f, 0.5f), 0, true);
			}
			else
			{
				setStatePhase(ATTACK_PHASE_MOVE_TO_POSITION);
			}

			break;
		}
	default: setStatePhase(0); break;
	}
}

void Tank::defend(void)
{
	switch (statePhase)
	{
	default: setStatePhase(0); break;
	}
}

void Tank::fight(void)
{
	movementAI_fight();
	turretAI_fight();
	tankMGAI_fight();
}

void Tank::movementAI_fight(void)
{
	switch (statePhase)
	{
	case FIGHTING_PHASE_MOVE_AROUND:
		{
			if (first_time_in_state_phase)
			{
				Location movingLocation;
				bool forward;
				movingLocation = Location(randFloat(0.0f, (float)Game::SCREENWIDTH), randFloat(0.0f, (float)Game::SCREENHEIGHT) * 0.7f); //ArcPoint(getLocation(), randFloat(50.0f, 100.0f), getDirection() + randFloat(-2.0f, 2.0f)).getLocation();
				forward = true;
				setMoving(movingLocation, forward);
				first_time_in_state_phase = false;
			}

			if (closestBlockingObjectDistance < 10000.0f)
			{
				Location movingLocation(-100.0f, -100.0f);
				while (movingLocation.x > 0.0f && 
					movingLocation.x < Game::SCREENWIDTH &&
					movingLocation.y > 0.0f &&
					movingLocation.y < Game::SCREENHEIGHT * 0.65f)
				{
					movingLocation = ArcPoint(getLocation(), randFloat(0.0f, 100.0f), (getDirection() + M_PI) + randFloat(-M_PI, M_PI)).getLocation(); //Location(randFloat(0.0f, (float)Game::SCREENWIDTH), randFloat(0.0f, (float)Game::SCREENHEIGHT) * 0.7f); //ArcPoint(getLocation(), randFloat(50.0f, 100.0f), getDirection() + randFloat(-2.0f, 2.0f)).getLocation();
				}

				setMoving(movingLocation, true);
			}

			if (randomEvent(5.0f))
			{
				setStatePhase(FIGHTING_PHASE_MOVE_AROUND);
			}

			if (turretAImsg_halt)
			{
				nextStatePhase();
			}
			break;
		}
	case FIGHTING_PHASE_HALT:
		{
			if (first_time_in_state_phase)
			{
				first_time_in_state_phase = false;
			}			

			if (!turretAImsg_halt)
			{
				setStatePhase(FIGHTING_PHASE_MOVE_AROUND);
			}
			else
			{
				stop();
			}

			break;
		}
	default: setStatePhase(0); break;
	}
}

void Tank::turretAI_fight(void)
{	
	if (turret.getEnemyTarget())
	{
		if (static_cast<CombatAIObject *>(turret.getEnemyTarget())->isNeutralized())
		{
			turret.setEnemyTarget(NULL);
		}
	}

	if (turret.getEnemyTarget())
	{
		switch(turret.statePhase)
		{
		case TURRET_AI_FIGHTING_PHASE_AIM:
			{
				aimTurret();

				if (turret.direction > turret.getEnemyTargetDirection() - 1.0f && 
					turret.direction < turret.getEnemyTargetDirection() + 1.0f)
				{
					if (isOnScreen())
						turret.nextStatePhase();
				}

				break;
			}
		case TURRET_AI_FIGHTING_PHASE_HALT:
			{
				if (turret.first_time_in_state_phase)
				{
					turretAImsg_halt = true;

					switch (game.skillLevel)
					{
					case Game::SKILL_LEVEL_EASY: turret.statePhaseTimer = randFloat(AIM_TIME_EASY_MIN, AIM_TIME_EASY_MAX); break;
					case Game::SKILL_LEVEL_NORMAL: turret.statePhaseTimer = randFloat(AIM_TIME_NORMAL_MIN, AIM_TIME_NORMAL_MAX); break;
					case Game::SKILL_LEVEL_HARD: turret.statePhaseTimer = randFloat(AIM_TIME_HARD_MIN, AIM_TIME_HARD_MAX); break;
					}
					
					turret.first_time_in_state_phase = false;
				}

				turret.statePhaseTimer -= timerDelta;

				aimTurret();

				if (turret.isReadyToFire() && turret.statePhaseTimer < 0.0f)
				{
					turret.nextStatePhase();
				}
				break;
			}
		case TURRET_AI_FIGHTING_PHASE_FIRE:
			{
				if (isOnScreen())
				{
					tank_fire_cannon = true;
					turret.nextStatePhase();
					break;
				}
				else
				{
					turretAImsg_halt = false;
					turret.setStatePhase(TURRET_AI_FIGHTING_PHASE_AIM);
					break;
				}
				
				break;
			}
		case TURRET_AI_FIGHTING_PHASE_RELOAD:
			{
				if (turret.first_time_in_state_phase)
				{
					turretAImsg_halt = false;
					turret.statePhaseTimer = TANK_CANNON_LOAD_TIME;
					turret.first_time_in_state_phase = false;
				}

				aimTurret();

				turret.statePhaseTimer -= timerDelta;

				if (turret.statePhaseTimer < 0.0f)
				{
					turret.setStatePhase(0);
				}

				break;
			}
		default: turret.setStatePhase(0); break;;
		}
	}
}

void Tank::tankMGAI_fight(void)
{
	if (tankMG.getEnemyTarget())
	{
		if (static_cast<CombatAIObject *>(tankMG.getEnemyTarget())->isNeutralized() && randomEvent(0.3f))
			tankMG.setEnemyTarget(NULL);
	}

	if (tankMG.getEnemyTarget())
	{
		switch(tankMG.statePhase)
		{
		case MACHINE_GUN_AI_FIGHTING_PHASE_AIM:
			{
				if (tankMG.first_time_in_state_phase)
				{
					tankMG.ready_to_fire = false;
					tank_fire_mg = false;
					switch (game.skillLevel)
					{
					case Game::SKILL_LEVEL_EASY: tankMG.statePhaseTimer = randFloat(2.0f, 5.0f); break;//randFloat(1.0f, 2.0f); break;
					case Game::SKILL_LEVEL_NORMAL: tankMG.statePhaseTimer = randFloat(1.0f, 4.5f); break;//randFloat(0.0f, 0.5f); break;
					case Game::SKILL_LEVEL_HARD: tankMG.statePhaseTimer = randFloat(0.0f, 3.5f); break;//0.0f; break;
					}

					tankMG.first_time_in_state_phase = false;
				}

				aimTankMG();

				tankMG.statePhaseTimer -= timerDelta;

				if (tankMG.statePhaseTimer < 0.0f &&												
					tankMG.isReadyToFire())
				{
					tankMG.nextStatePhase();
				}

				break;
			}
		case MACHINE_GUN_AI_FIGHTING_PHASE_FIRE:
			{
				if (tankMG.first_time_in_state_phase)
				{
					tankMG.statePhaseTimer = randFloat(1.0f, 2.0f);
					tankMG.first_time_in_state_phase = false;
				}
				
				tankMG.statePhaseTimer -= timerDelta;

				aimTankMG();

				if (isOnScreen())
				{
					tank_fire_mg = true;
				}

				if (tankMG.statePhaseTimer < 0.0f)
				{
					tank_fire_mg = false;
					tankMG.setStatePhase(0);
				}
				break;
			}
		default: tankMG.setStatePhase(0); break;
		}
	}
	else
		tank_fire_mg = false;
}

void Tank::startCheckingObjects(void)
{
	numberOfDangerousEnemyUnits = 0;
	turret.suggestedEnemyTarget = NULL;
	turret.nearestSuitableEnemyDistance = FLT_MAX;
	tankMG.suggestedEnemyTarget = NULL;
	tankMG.nearestSuitableEnemyDistance = FLT_MAX;
	closestBlockingObjectDistance = FLT_MAX;
	maxSpeed = TANK_MAX_VELOCITY;
}

void Tank::checkObject(GameObject * otherObject)
{
	if (game.currentLevel->smokeTimer > 0.0)
	{
		//inside smoke?
		if (y < OFF_SMOKE_DISTANCE)
		{
			//can't see a thing!
			return;
		}

		//other object inside smoke?
		if (otherObject->y < OFF_SMOKE_DISTANCE)
		{
			//can't see other object.
			return;
		}
	}

	if (startChecks)
	{
		startCheckingObjects();
		startChecks = false;
	}

	CombatAIObject::checkObject(otherObject);

	if (!computer_controlled)
		return;

	if (otherObject->isCombatAIObject())
	{
		CombatAIObject * caio = static_cast<CombatAIObject *>(otherObject);

		if (!caio->isNeutralized() && !caio->fleeing && !caio->surrendered)
		{
			Distance dist = squaredObjectDistance(caio);
			bool setTurretTarget = false;
			bool setMGTarget = false;
	
			if (caio->type == GameObject::TANK && caio->side != side)
			{	
				if (game.currentLevel->smokeTimer > 0.0f)
					return;

				//this is a dangerous unit!
				numberOfDangerousEnemyUnits++;
				//check possible turret target

				if (turret.suggestedEnemyTarget)
				{
					if (turret.suggestedEnemyTarget->type == GameObject::BASE)
					{
						setTurretTarget = true;
					}
					else if (dist < turret.nearestSuitableEnemyDistance) 
					{
						setTurretTarget = true;
					}
				}
				else		
				{
					setTurretTarget = true;
				}
			}
			else if (caio->type == GameObject::FOOT_SOLDIER ||
					(caio->type == GameObject::TANK && caio->side == side))
			{
				if (caio->side != side)
				{
					if (game.currentLevel->smokeTimer > 0.0f)
						return;
					if (static_cast<FootSoldier *>(otherObject)->footSoldierClass == FootSoldier::MEDIC)
						return;
					if (!tankMG.engaged)
					{
						FootSoldier * fs = static_cast<FootSoldier *>(caio);
						//At hard level, AT soldiers will be shot first by tanks.
						if (game.skillLevel == Game::SKILL_LEVEL_HARD &&
							fs->footSoldierClass == FootSoldier::AT_SOLDIER)
						{
							//this is a dangerous unit!
							numberOfDangerousEnemyUnits++;
							if (tankMG.suggestedEnemyTarget)
							{
								if (tankMG.suggestedEnemyTarget->type == GameObject::FOOT_SOLDIER)
								{
									if (static_cast<FootSoldier *>(tankMG.suggestedEnemyTarget)->footSoldierClass != FootSoldier::AT_SOLDIER)
									{
										setMGTarget = true;
									}
									else if (dist < tankMG.nearestSuitableEnemyDistance) 
									{
										setMGTarget = true;
									}		
								}
							}
							else
							{
								setMGTarget = true;
							}
						}
						else
						{
							if (tankMG.suggestedEnemyTarget == NULL && dist < tankMG.nearestSuitableEnemyDistance) 
							{
								setMGTarget = true;
							}
						}
					}
				}
				else
				{
					float alpha = M_PI * 1.2f;
					l1 = ArcPoint(getLocation(), 50.0f, direction + alpha).getLocation();
					l2 = ArcPoint(getLocation(), 50.0f, direction - alpha).getLocation();
					Angle a1 = l1.angle(caio->getLocation());
					Angle a2 = l2.angle(caio->getLocation());
                    if (a1 > direction && a2 < direction)
					{
						float dist = squaredObjectDistance(caio);
						closestBlockingObjectDistance = min(dist, closestBlockingObjectDistance);
					}
				}
			}
			else if (caio->type == GameObject::BASE && caio->side != side)
			{
				if (game.currentLevel->smokeTimer > 0.0f)
					return;
				if (!turret.engaged && turret.suggestedEnemyTarget == NULL)
				{
					setTurretTarget = true;
				}
			}

			if (setTurretTarget)
			{
				turret.suggestedEnemyTarget = caio;
				turret.nearestSuitableEnemyDistance = dist;
			}
			if (setMGTarget)
			{
				tankMG.suggestedEnemyTarget = caio;
				tankMG.nearestSuitableEnemyDistance;
			}
		}
	}
	else if (otherObject->type == GameObject::TANK_CARCASS)
	{
		float alpha = M_PI * 1.2f;
		l1 = ArcPoint(getLocation(), 50.0f, direction + alpha).getLocation();
		l2 = ArcPoint(getLocation(), 50.0f, direction - alpha).getLocation();
		Angle a1 = l1.angle(otherObject->getLocation());
		Angle a2 = l2.angle(otherObject->getLocation());
        if (a1 > direction && a2 < direction)
		{
			float dist = squaredObjectDistance(otherObject);
			closestBlockingObjectDistance = min(dist, closestBlockingObjectDistance);
		}
	}
}

void Tank::stopCheckingObjects(void)
{
	if (turret.suggestedEnemyTarget)
	{
		if (!turret.engaged)
			turret.setEnemyTarget(turret.suggestedEnemyTarget);
		else if (turret.getEnemyTarget()->type == GameObject::BASE)
			turret.setEnemyTarget(turret.suggestedEnemyTarget);
	}
	if (!tankMG.engaged)
		tankMG.setEnemyTarget(tankMG.suggestedEnemyTarget);
	float d = 150.0f;
	float a = d * d;
	if (closestBlockingObjectDistance < a && computer_controlled)
	{
		maxSpeed = TANK_MAX_VELOCITY * closestBlockingObjectDistance / a - 20.0f;
		if (maxSpeed < 0.0f)
			maxSpeed = 0.0f;
	}
	
	if (turret.getEnemyTarget() ||
		tankMG.getEnemyTarget())
	{
		engaged = true;
	}
}

bool Tank::isNeutralized(void)
{
	return !active;
}

bool Tank::isOverLapping(GameObject * object)
{
	for (int i = 0; i < object->getCollisionSphereCount(); i++)
	{
		if (getCollisionSphere1().testHit(object->getCollisionSphere(i)) ||
			getCollisionSphere2().testHit(object->getCollisionSphere(i)))
			return true;
	}

	return false;
}

Direction Tank::getMovingDirection(void)
{
	if (tank_forward)
		return TANK_DIRECTION_FORWARD;

	return TANK_DIRECTION_FORWARD;
}

void Tank::setLocation(Location location)
{
	this->x = location.x;
	this->y = location.y;

	turret.x = this->x;
	turret.y = this->y;
	cannonSight.setPosition(turret.x, turret.y, turret.direction, turret.angle);
	turret.setAimingLocation();

	tankMG.setPosition(turret.x, turret.y, turret.direction, 0.0f);
	tankMG.setAimingLocation();
	MGSight.setLocation(tankMG.aimingLocation);
}

char * Tank::getStateString(int index)
{
	if (index == -2)
		index = state;

	switch (index)
	{
		case STATE_ATTACK: return "STATE_ATTACK";
		case STATE_DEFEND: return "STATE_DEFEND";
		case STATE_FIGHT: return "STATE_FIGHT";
	}
	return "STATE_UNKNOWN";
}

char * Tank::getStatePhaseString(int movement1_turret2_mg3)
{
	switch (state)
	{
		case STATE_ATTACK: 
			{
				switch (statePhase)
				{
				case ATTACK_PHASE_MOVE_TO_POSITION: return "ATTACK_PHASE_MOVE_TO_POSITION";
				case ATTACK_PHASE_FIGHT: return "ATTACK_PHASE_FIGHT";
				}
				break;
			}
		case STATE_DEFEND:
			{
				switch (statePhase)
				{
				case DEFENSE_PHASE_MOVE_TO_DEFENSE_POSITION: return "DEFENSE_PHASE_MOVE_TO_DEFENSE_POSITION";
				case DEFENSE_PHASE_OBSERVE: return "DEFENSE_PHASE_OBSERVE";
				case DEFENSE_PHASE_FIGHT: return "DEFENSE_PHASE_FIGHT";
				}
				break;
			}
		case STATE_FIGHT:
			{
				switch (movement1_turret2_mg3)
				{
				case 1:
					{
						switch (statePhase)
						{
						case FIGHTING_PHASE_MOVE_AROUND: return "FIGHTING_PHASE_MOVE_AROUND"; //set random target location to move to after moved to the previous
						case FIGHTING_PHASE_HALT: return "FIGHTING_PHASE_HALT";
						}
						break;
					}
				case 2:
					{
						switch (statePhase)
						{
						case TURRET_AI_FIGHTING_PHASE_AIM: return "TURRET_AI_FIGHTING_PHASE_AIM";
						case TURRET_AI_FIGHTING_PHASE_HALT: return "TURRET_AI_FIGHTING_PHASE_HALT";
						case TURRET_AI_FIGHTING_PHASE_FIRE: return "TURRET_AI_FIGHTING_PHASE_FIRE";
						case TURRET_AI_FIGHTING_PHASE_RELOAD: return "TURRET_AI_FIGHTING_PHASE_RELOAD";
						}
						break;
					}
				case 3:
					{
						switch (statePhase)
						{
						case MACHINE_GUN_AI_FIGHTING_PHASE_AIM: return "MACHINE_GUN_AI_FIGHTING_PHASE_AIM";
						case MACHINE_GUN_AI_FIGHTING_PHASE_FIRE: return "MACHINE_GUN_AI_FIGHTING_PHASE_FIRE";
						}
						break;
					}
				}
				break;
			}
	}
	return "STATE_PHASE_UNKNOWN";
}

void Tank::setStatistics(CombatObject::STATISTIC_VALUE val)
{
	game.currentLevel->numberOfTanksLeft[side]--;
	game.currentLevel->numberOfTanksAtArea[side]--;
	CombatObject::setStatistics(val);
}

HTEXTURE Tank_Carcass::tex = NULL;
hgeAnimation * Tank_Carcass::carcassImage = NULL;

Tank_Carcass::Tank_Carcass(void)
{
}

Tank_Carcass::Tank_Carcass(Location location, float tankDirection, float turretDirection, float MGDirection)
{
	set(location, tankDirection, turretDirection, MGDirection);	
}

Tank_Carcass::~Tank_Carcass(void)
{
}

void Tank_Carcass::set(Location location, float tankDirection, float turretDirection, float MGDirection)
{
	GameObject::set(GameObject::TANK_CARCASS, location, tankDirection);

	this->image = carcassImage;
	turret.image = Turret::carcassTurretImage;
	tankMG.x = turret.x = this->x, tankMG.y = turret.y = this->y;
	turret.direction = turretDirection;
	tankMG.direction = MGDirection;
	tankMG.setPosition(x, y, turret.direction, 0.0f);
	collisionSphereCount = 2;
	setCollisionSphereRadius(Tank::TANK_COLLISIONSPHERE_RADIUS);
	setHeight(Tank::TANK_HEIGHT);
}

void Tank_Carcass::init(void)
{
	game.renderLoadScreen("LOADING TANK CARCASS CLASS");
	tex = hge->Texture_Load("tank_carcass_test1.png");
	carcassImage = new hgeAnimation(tex, 1, 0.0f, 0.0f, 0.0f, 61.0f, 34.0f);
	carcassImage->SetHotSpot(/*25.0f*/61.0f/2.0f, 34.0f/2.0f);
}

void Tank_Carcass::release(void)
{
	hge->Texture_Free(tex);
	delete carcassImage; carcassImage = NULL;
}

void Tank_Carcass::processAction(void)
{
//	tankMG.setPosition(x, y, turret.direction, 0.0f);
}

void Tank_Carcass::render(void)
{
	bool render_shadows = true;

	if (render_shadows)
	{
		renderTankShadows(this);
	}

	GameObject::render();

	//render turret
	turret.render();

	//render tank maghine gun
	tankMG.render();
}

void Tank_Carcass::collide(GameObject * otherObject, ArcPoint hittingPoint)
{
	if (otherObject->type == GameObject::TANK)
	{
		otherObject->collide(this);
	}
	else if (otherObject->type == GameObject::FOOT_SOLDIER)
	{
		otherObject->collide(this);
	}
	else if (otherObject->type == GameObject::PROJECTILE)
	{
		Projectile * pr = static_cast<Projectile *>(otherObject);
		if (pr->projectileType == Projectile::TANK_CANNON_PROJECTILE)
		{
			if (randFloat(0.0f, 1.0f) < 0.5f)
				return;
		}
		if (pr->projectileType == Projectile::AT_ROCKET)
		{
			if (squaredObjectDistance(pr->shooter) < 10000.0f)
				return;
			else if (randFloat(0.0f, 1.0f) < 0.3f)
				return;
		}
		if (pr->projectileType == Projectile::AIR_TO_GROUND_MISSILE)
		{
			if (randFloat(0.0f, 1.0f) > 0.3f)
				return;
		}
		if (pr->projectileType == Projectile::ASSAULT_RIFLE_BULLET)
		{
			if (randFloat(0.0f, 1.0f) < 0.8f)
				return;
		}
		if (pr->projectileType == Projectile::TANK_MG_BULLET)
		{
			if (randFloat(0.0f, 1.0f) < 0.8f)
				return;
		}

		Projectile * projectile = static_cast<Projectile *>(otherObject);
		projectile->hit(GameObject::TANK_CARCASS, otherObject->getLocation());
	}
}