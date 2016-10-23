#pragma once
//#include "GameObject.h"
#include "CombatAIObject.h"
#include "projectile.h"
#include <hge.h>
#include <hgeparticle.h>
#include "sight.h"

class Turret :
	public CombatAIObject
{
public:
	static HTEXTURE playerTurrettex;
	static HTEXTURE enemyTurrettex;
	static HTEXTURE carcassTurrettex;
	static hgeAnimation * playerTurretImage;
	static hgeAnimation * enemyTurretImage;
	static hgeAnimation * carcassTurretImage;
	static const float TURRET_ROTATION_SPEED;
#define DEF_TANK_TURRET_BARREL_LENGTH 65.0f
	static const float BARREL_LENGTH;
	static const float MAX_ANGLE;
	static const float TURRET_ANGLE_ADJUSTMENT_SPEED;
	float angle;
	Location aimingLocation;
	float firingTargetHeight;
	bool playing_firing_animation;
	int firingAnimCounter;
	bool ready_to_fire;
	int ammunition;

	Turret(void);
	~Turret(void);


	static void init(void);
	static void release();
	void playAnimation(bool);

	bool isReadyToFire(void);
	void reload(void);

	Location getBarrelTipLocation(float addedLength = 0.0f)
	{
		Location l;
		l.x = x + (DEF_TANK_TURRET_BARREL_LENGTH + addedLength) * cos(direction);
		l.y = y + (DEF_TANK_TURRET_BARREL_LENGTH + addedLength) * sin(direction);
		return l;
	}

	Height getBarrelTipHeight(void)
	{
		return 20.0f;
	}

	void setAimingLocation(void)
	{
		float d = angle * angle * 10000.0f + 100.0f;
		aimingLocation.x = x + d * cos(direction);
		aimingLocation.y = y + d * sin(direction);
	}
};

//optimointimahdollisuuksia:
//sini ja kosini laskettaisiin vain kerran / freimi.

class TankMachineGun : public CombatAIObject
{
public:
	static HTEXTURE tankmgtex;
	static const float TANKMG_ROTATION_ACCELERATION;
	static const float TANKMG_ROTATION_MAX_SPEED;
	static const float MG_POSITION_ANGLE;
	static const float MG_POSITION_DISTANCE_FROM_CENTER;
#define DEF_TANK_MG_BARREL_LENGTH 17.0f
	static const float MG_BARREL_LENGTH;
	static const float MAX_ANGLE;
	static const float MG_ANGLE_ADJUSTMENT_SPEED;
	float angle;
	Location aimingLocation;
	bool ready_to_fire;
	int ammunition;

	TankMachineGun(void);
	~TankMachineGun(void);
	static void init(void);
	static void release();

	void reload(void);
	void setPosition(float, float, float, float);

	float getPositionX();
	float getPositionY();

	Location getBarrelTipLocation(float addedLength = 0.0f);

	bool isReadyToFire(void);
	void setAimingLocation(void);
};

class TankObject
{
public: 
	//tank parts:
	Turret turret;
	TankMachineGun tankMG;

	//To avoid multiple inheritance problems, a pointer is used
	//for the functions of this class. No inheritance here.
	void renderTankShadows(GameObject * object);
};

class Tank : 
	public CombatAIObject, 
	public TankObject
{
public:

	//type
	enum TANKTYPE
	{
		PLAYER_1_TANK,
		PLAYER_2_TANK,
		ENEMY_TANK
	} 
	tankType;

	//state
	enum STATE
	{
		STATE_ATTACK,
		STATE_DEFEND,
		STATE_FIGHT
	}
	state;

	//state phases
	enum ATTACK_PHASE
	{
		ATTACK_PHASE_MOVE_TO_POSITION,
		ATTACK_PHASE_FIGHT
	};

	enum DEFENSE_PHASE
	{
		DEFENSE_PHASE_MOVE_TO_DEFENSE_POSITION,
		DEFENSE_PHASE_OBSERVE,
		DEFENSE_PHASE_FIGHT
	};

	enum TURRET_AI_FIGHT
	{
		TURRET_AI_FIGHTING_PHASE_AIM,
		TURRET_AI_FIGHTING_PHASE_HALT,
		TURRET_AI_FIGHTING_PHASE_FIRE,
		TURRET_AI_FIGHTING_PHASE_RELOAD
	};

	enum MACHINE_GUN_AI_FIGHT
	{
		MACHINE_GUN_AI_FIGHTING_PHASE_AIM,
		MACHINE_GUN_AI_FIGHTING_PHASE_FIRE
	};

	enum MOVEMENT_AI_FIGHT
	{
		FIGHTING_PHASE_MOVE_AROUND, //set random target location to move to after moved to the previous
		FIGHTING_PHASE_HALT
	};

	enum MOVEMENT_AI_MOVING_PHASE
	{
		MOVING_PHASE_SELECT_DIRECTION, //select closest direction to target location that is not blocked
		MOVING_PHASE_SELECT_SPEED, //set speed according to how close there are own troops ahead
		MOVING_PHASE_MOVE //move
	};

	enum ENGINE_SOUND
	{
		ENGINE_SOUND_IDLE,
		ENGINE_SOUND_ACCELERATING,
		ENGINE_SOUND_MOVING,
		ENGINE_SOUND_DEACCELERATING
	}
	engineSound;

	//data
	Velocity maxSpeed; //The maximum speed this tank can drive due to own troops ahead etc. This will be altered in checkObject and used for maximum speed in aciton processing, mo9ve() -function.

	//AI:
	//bool move;
	bool turretAImsg_halt; //flag when turret needds tank to halt

	float distance_to_drive;
	float distance_driven;
	bool target_tank_in_sight;
	bool horizontal_angle_right;
	bool vertical_angle_right;
	Tank * targetTank;
	float directionForTurretRotation;
	float dist;
	float sightDist;
	float aimWait;

	static int tanks;

	//Skill level variables
	static float possibilityOfTankDeathFromATRocket[3];
	static float possibilityOfTankDeathFromTankAmmo[3];

	//Commands
	bool tank_turn_left;
	bool tank_turn_right;
	bool tank_forward;
	bool tank_backwards;
	bool tank_turret_turn_left;
	bool tank_turret_turn_right;
	bool tank_machine_gun_turn_left;
	bool tank_machine_gun_turn_right;
	bool tank_fire_cannon;
	bool tank_fire_mg;
	bool tank_toggle_firemode;
	bool tank_turret_angle_up;
	bool tank_turret_angle_down;
	bool tank_mg_angle_up;
	bool tank_mg_angle_down;

	//Movement:
	bool turret_adjusting;
	float rotation;

	//situation
	bool colliding;
	bool newCollision;
	bool unable_to_turn;
	bool cant_turn_left;
	bool cant_turn_right;
	int collisions;
	int lastCollisions;
	//dbg:
	Tank * othertank;

	//Battle:
	enum TANK_FIREMODE
	{
		TANK_FIREMODE_CANNON,
		TANK_FIREMODE_MG
	};
	int tank_firemode;
	//cannon:
	bool firing_cannon;
	bool cannon_loaded;
	float cannonLoadTimer;
	float cannontimer;
	static float TANK_CANNON_ACCURACY;
	//mg:
	bool firing_mg;
	bool mg_overheated;
	float mgHeat;
	float mgDelay;
	static float TANKMG_ACCURACY;

	//tank part info:
	Location turretBarrelTipLastFrameLocation;
	Sight cannonSight;
	Sight MGSight;
	//dbg:
	Projectile * cannonProj;

	//destroying
	bool burning;
	float burningTimer;

	//object checking
	Distance closestBlockingObjectDistance;

	//Tank resources
	static HTEXTURE player1Tanktex;
	static HTEXTURE player2Tanktex;
	static HTEXTURE enemyTanktex;
	static hgeAnimation * player1TankImage;
	static hgeAnimation * player2TankImage;
	static hgeAnimation * enemyTankImage;

	//Tank constants
	static const Direction TANK_DIRECTION_FORWARD;
	static const Direction TANK_DIRECTION_BACKWARDS;
	static const float TANK_ACCELERATION;
	static const float TANK_MAX_VELOCITY;
	static const float TANK_ROTATION_SPEED;
	static const float TANK_CANNON_SIGHT_SPEED;
	static const float TANK_CANNON_LOAD_TIME;
	static const float MG_RATE_OF_FIRE;
	static const float MG_MAX_HEAT;
	static const float MG_HEAT_PER_SHOT;
	static const float MG_COOLING_PER_SECOND;
	static const float TANK_COLLISIONSPHERE_RADIUS;
	static const float FRICTION_AND_MASS_COLLISION_FACTOR;
	static const float BURNING_TIME;
	static const float AITANK_MAX_AIM_TIME;
	static const Height TANK_HEIGHT;
	static int NUMBER_OF_COLLISIONSPHERES;

	//Track marks
	static const float  TANK_TRACKMARK_DISTANCE;
	hgeSprite * trackMark;
	float trackMarkCounter;

	//moving dust
	static const float MOVING_DUST_TIME;
	float movingDustTimer;

	//sounds
	static const float TANK_ACCELERATE_SOUND_DURATION;
	static const float TANK_MOVE_SOUND_DURATION;
	static const float TANK_DEACCELERATE_SOUND_DURATION;
	static HEFFECT tank_idle_sound;
	static HEFFECT tank_accelerate_sound;
	static HEFFECT tank_move_sound;
	static HEFFECT tank_deaccelerate_sound;
	static HEFFECT turret_move_sound;
	static HEFFECT turret_clunk_sound;
	static HEFFECT tank_fire_cannon_sound;
	static HEFFECT tank_fire_mg_sound;
	static HEFFECT tank_crash_other_tank_sound;
	static hgeParticleSystem * tankFireMGParticleSystem;
	static hgeParticleSystem * tankFireMGParticleSystem2;
	static hgeParticleSystem * tankMoveParticleSystem;
	HCHANNEL idle_sound_channel;
	HCHANNEL move_sound_channel;
	HCHANNEL turret_sound_channel;
	HCHANNEL weapon_sound_channel;
	HCHANNEL engine_sound_channel_1;
	HCHANNEL engine_sound_channel_2;
	HCHANNEL engine_sound_channel_3;
	HCHANNEL engine_sound_channel_4;
	bool tank_idle_playing;
	bool tank_accelerate_playing;
	bool tank_move_playing;
	bool tank_deaccelerate_playing;
	bool turret_move_playing;
	float tank_accelerate_sound_timer;
	float tank_move_sound_timer;
	float tank_deaccelerate_sound_timer; bool deaccelaration_sound_help_flag;

	//basic functions
	Tank(void);
	Tank(int tankType, CombatObject::SIDE side, Location startingLocation, Direction startDirection);//, int);
	virtual ~Tank(void);
	void set(int tankType, CombatObject::SIDE side, Location startingLocation, Direction startDirection);//, int);
	static void init(void);
	void processAction(void);
	void processAI(void);
	void processOrders(void);
	void collide(GameObject * other, ArcPoint hittingPoint = ArcPoint());
	void processInteraction(void);
	void render(void);
	void destroy(void);
	void playSound(void);
	void playAnimation(void);
	static void release(void);

	//action
	void setTurning(Direction direction);
	void setMoving(Location location, bool forward);
	void stop(void); //voidaaan laittaa movingObjectiin, jos jalizulle ei tule siitä mitään häikkää...
	void aimTurret(void);
	void fireTurret(void);
	void aimTankMG(void);
	void fireTankMG(void);

	//custom AI state handling
	void setStateVariable(int state) { this->state = (Tank::STATE)state; }
	void setState(int newState, Time stateChangeTimer = 0.0f, int startStatePhase = 0, bool nextPhaseReturn = true);

	//AI
	void attack(void);
	void defend(void);
	void fight(void);
	void movementAI_fight(void);
	void turretAI_fight(void);
	void tankMGAI_fight(void);

	//interaction
	void startCheckingObjects(void);
	void checkObject(GameObject * otherObject);
	void stopCheckingObjects(void);

	//properties
	Height getHeight(void) { return TANK_HEIGHT; }
	void setLocation(Location location);
	bool isNeutralized(void);
	bool isOverLapping(GameObject * object);
	Direction getMovingDirection(void);
	bool isEngaged(void) { return turret.engaged || tankMG.engaged; }

	//collision detection helper functions:
	CollisionSphere getCollisionSphere1(float radius = TANK_COLLISIONSPHERE_RADIUS)
	{
		radius *= GameObject::objectSize;
		CollisionSphere pos;
		pos.location.x = x + 15.25f * cos(direction);
		pos.location.y = y + 15.25f * sin(direction);
		pos.radius = radius;
		pos.squaredRadius = radius * radius;
		return pos;
	}
	CollisionSphere getCollisionSphere2(float radius = TANK_COLLISIONSPHERE_RADIUS)
	{
		radius *= GameObject::objectSize;
		CollisionSphere pos;
		pos.location.x = x + 15.25f * cos(direction + M_PI);
		pos.location.y = y + 15.25f * sin(direction + M_PI);
		pos.radius = radius;
		pos.squaredRadius = radius * radius;
		return pos;
	}
	CollisionSphere getDynamicCollisionSphere(int index)
	{
		if (index == 1)
			return getCollisionSphere2();

		return getCollisionSphere1();
	}

	Height getTankMGBarrelTipHeight(void)
	{
		return 22.0f;
	}

	int counter;

	//techno
	char * getStateString(int index = -2);
	virtual char * getStatePhaseString(int movement1_turret2_mg3);

	//statistics
	void setStatistics(CombatObject::STATISTIC_VALUE val);

	//dbg:
	int collisioncalls;
	int processactioncalls;
	bool collisionToFront;
	bool collisionToBack;
	bool collisionToSide;
	float rotation_multiplicator;
	bool moveCommand;
	Angle angleToCurrentBarrelTipLocation;
	Angle angleToLastFrameBarrelTipLocation;
	Angle angleToObject;
	bool b1;
	bool b2;
	Location l1, l2;
	char * dbgName;

private:
	static const float d;
	static const float alpha;
};

class Tank_Carcass :
	public GameObject,
	public TankObject
{
public:

	static HTEXTURE tex;
	static hgeAnimation * carcassImage;

	Tank_Carcass(void);
	Tank_Carcass(Location location, float tankDirection, float turretDirection, float MGDirection);
	~Tank_Carcass(void);
	void set(Location location, float tankDirection, float turretDirection, float MGDirection);
	static void init(void);
	static void release(void);
	void processAction(void);
	void render(void);
	void collide(GameObject * otherObject, ArcPoint hittingPoint);

	CollisionSphere getCollisionSphere1(float radius = Tank::TANK_COLLISIONSPHERE_RADIUS)
	{
		radius *= GameObject::objectSize;
		CollisionSphere pos;
		pos.location.x = x + 15.25f * cos(direction);
		pos.location.y = y + 15.25f * sin(direction);
		pos.radius = radius;
		pos.squaredRadius = radius * radius;
		return pos;
	}
	CollisionSphere getCollisionSphere2(float radius = Tank::TANK_COLLISIONSPHERE_RADIUS)
	{
		radius *= GameObject::objectSize;
		CollisionSphere pos;
		pos.location.x = x + 15.25f * cos(direction + M_PI);
		pos.location.y = y + 15.25f * sin(direction + M_PI);
		pos.radius = radius;
		pos.squaredRadius = radius * radius;
		return pos;
	}
	CollisionSphere getDynamicCollisionSphere(int index)
	{
		if (index == 1)
			return getCollisionSphere2();

		return getCollisionSphere1();
	}
};