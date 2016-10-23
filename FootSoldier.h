#pragma once
#include "tank.h"
#include "CombatAIObject.h"
#include "projectile.h"
#include "effect.h"

class BloodMark : public GameObject
{
};

class FootSoldier :
	public CombatAIObject
{
public:

	enum FOOT_SOLDIER_CLASS
	{
		RIFLEMAN,
		MACHINE_GUNNER,
		AT_SOLDIER,
		//SNIPER,
		//RADIST
		MEDIC,
		NUMBER_OF_CLASSES
	}
	footSoldierClass;

	void setClass(FOOT_SOLDIER_CLASS footSoldierClass) { this->footSoldierClass = footSoldierClass; }
	FOOT_SOLDIER_CLASS getClass(void) { return footSoldierClass; }

	//states:

	enum STATE
	{
		//AI states
		STATE_DEFEND,
		STATE_ATTACK,
		STATE_FIGHT,
		STATE_FIRE,
		STATE_RELOAD,
		STATE_THROW_GRENADE,
		STATE_GET_UP,
		STATE_RUN_AWAY,
		//STATE_RUN_AWAY_FROM_TREE_FALL,
		STATE_UNDER_INDIRECT_FIRE,
		STATE_THROWN_BY_FORCE,
		STATE_BURNING,
		STATE_WOUNDED,
		STATE_DYING,
		STATE_MEDIC_ACTION,
		STATE_BEING_CARRIED,
		STATE_SURRENDER,
		STATE_FLEE,
		STATE_CLEAR_OF_ENEMY,
		STATE_DEBUG,
		STATE_MAX,
	}
	state;

	//Different phases of states:

	enum DEFENSE_PHASE
	{
		DEFENSE_PHASE_TURN_TO_MOVING_DIRECTION,
		DEFENSE_PHASE_MOVE_TO_DEFENSE_LINE,
		DEFENSE_PHASE_GOOD_TO_GET_DOWN,
		DEFENSE_PHASE_GET_DOWN, //"ket taun!"
		DEFENSE_PHASE_OBSERVE,
		DEFENSE_PHASE_TURN_TO_ENEMY_TARGET_DIRECTION,
		DEFENSE_PHASE_ADJUST_DIRECTION,
		DEFENSE_PHASE_FIRE_START,
		DEFENSE_PHASE_FIGHT,
		DEFENSE_PHASE_TURN_TO_DEFENSE_DIRECTION
	};	
	enum ATTACK_PHASE
	{
		ATTACK_PHASE_TURN_TO_ATTACK_DIRECTION,
		ATTACK_PHASE_MOVE_FORWARD,
		ATTACK_PHASE_FIRE_RANDOM_SHOTS,
		ATTACK_PHASE_GET_DOWN,
		ATTACK_PHASE_TAKE_COVER,
		ATTACK_PHASE_CRAWL_SIDEWAYS_1,
		ATTACK_PHASE_FIGHT,
		ATTACK_PHASE_CRAWL_FORWARD,
		ATTACK_PHASE_CRAWL_SIDEWAYS_2,
		ATTACK_PHASE_GET_UP,
		ATTACK_PHASE_ADVANCE,
		ATTACK_PHASE_LAST
	};

	enum FIGHTING_PHASE
	{
		FIGHTING_PHASE_FIGHT,
		FIGHTING_PHASE_END
	};

	enum FIRING_PHASE
	{
		FIRING_PHASE_TURN,
		FIRING_PHASE_ADJUST_POSITION,
		FIRING_PHASE_SET_FIRE_MODE,
		FIRING_PHASE_AIM,
		FIRING_PHASE_FIRE,
		FIRING_PHASE_END
	} ;

	enum RELOAD_PHASE
	{
		RELOAD_PHASE_GET_OLD_CLIP,
		RELOAD_PHASE_GHANCE_CLIP,
		RELOAD_PHASE_PUT_NEW_CLIP,
		RELOAD_PHASE_PULL_LOADER,
		RELOAD_PHASE_PULL_LOADER_2,
		RELOAD_PHASE_RELEASE_LOADER,
		RELOAD_PHASE_RELOAD_DONE
	};

	enum GRENADE_THROWING_PHASE
	{
		GRENADE_THROWING_PHASE_TURN,
		GRENADE_THROWING_PHASE_THROW
	};

	enum GET_UP_PHASE
	{
		GET_UP_PHASE_GET_UP,
		GET_UP_PHASE_FINISHED
	};

	enum STATE_RUN_AWAY_PHASE
	{
		STATE_RUN_AWAY_PHASE_GET_UP,
		STATE_RUN_AWAY_PHASE_TURNING,
		STATE_RUN_AWAY_PHASE_RUNNING,
		STATE_RUN_AWAY_PHASE_CLEAR
	};

	enum RUN_AWAY_FROM_TREE_FALL
	{
		RUN_AWAY_FROM_TREE_FALL_PHASE_GET_UP,
		RUN_AWAY_FROM_TREE_FALL_PHASE_TURN,
		RUN_AWAY_FROM_TREE_FALL_PHASE_RUN
	};

	enum UNDER_INDIRECT_FIRE_PHASE
	{
		UNDER_INDIRECT_FIRE_PHASE_GET_DOWN,
		UNDER_INDIRECT_FIRE_PHASE_TAKE_COVER,
		UNDER_INDIRECT_FIRE_PHASE_STAY_DOWN,
		UNDER_INDIRECT_FIRE_PHASE_END
	};

	enum THROWN_BY_FORCE
	{
		THROWN_BY_FORCE_PHASE_FLYING,
		THROWN_BY_FORCE_PHASE_GOT_DOWN
	};

	enum BURNING
	{
		BURNING_PHASE_GET_UP,
		BURNING_PHASE_RUN,
		BURNING_PHASE_FALL
	};

	enum WOUNDED_PHASE
	{
		WOUNDED_PHASE_FALLING,
		WOUNDED_PHASE_CHECK_STATUS,
		WOUNDED_PHASE_CHANGE_POSITION,
		WOUNDED_PHASE_TURNING,
		WOUNDED_PHASE_MOVING,
		WOUNDED_PHASE_LYING,
		WOUNDED_PHASE_IN_MEDICAL_ATTENTION
	};

	enum DYING_PHASE
	{
		DYING_PHASE_FALLING,
		DYING_PHASE_LYING,
		DYING_PHASE_DEATH
	};

	enum BEING_CARRIED_PHASE
	{
		BEING_CARRIED_PHASE_BE_CARRIED
	};

	enum SURRENDER_PHASE
	{
		SURRENDER_PHASE_GET_UP,
		SURRENDER_PHASE_LIFT_HANDS_IN_THE_AIR,
		SURRENDER_PHASE_WALK_TO_CAPTIVITY,
		SURRENDER_PHASE_UNSURRENDER
	};

	enum FLEE_PHASE
	{
		FLEE_PHASE_GET_UP,
		FLEE_PHASE_RUN_TO_SAFETY,
		FLEE_PHASE_SAFE
	};

	enum ACTION
	{
		//actions in standing position
		ACTION_STANDING_FIRST,
		ACTION_STANDING_NOT_AIMING = ACTION_STANDING_FIRST,
		ACTION_WALKING_AIMING,
		ACTION_RUNNING_NOT_AIMING,
		ACTION_STANDING_NOT_AIMING_FIRING,
		ACTION_STANDING_AIMING_FIRING,
		ACTION_STANDING_SURRENDERING,
		ACTION_STANDING_UNSURRENDERING,
		ACTION_WALKING_SURRENDERED,
		ACTION_STANDING_GETTING_PRONE,
		ACTION_STANDING_WOUNDED_FALLING_FORWARD,
		ACTION_STANDING_WOUNDED_FALLING_BACKWARDS, ACTION_STANDING_WOUNDED_FALLING_FIRST = ACTION_STANDING_WOUNDED_FALLING_FORWARD,	ACTION_STANDING_WOUNDED_FALLING_MAX = ACTION_STANDING_WOUNDED_FALLING_BACKWARDS,
		ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD,
		ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS, ACTION_STANDING_WOUNDED_LEG_FALLING_FIRST = ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD,	ACTION_STANDING_WOUNDED_LEG_FALLING_MAX = ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS,
		ACTION_STANDING_WOUNDED_FIRST = ACTION_STANDING_WOUNDED_FALLING_FORWARD, ACTION_STANDING_WOUNDED_MAX = ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS,
		ACTION_STANDING_DEATH_FALLING_FORWARD_1,
		ACTION_STANDING_DEATH_FALLING_FORWARD_2,
		ACTION_STANDING_DEATH_FALLING_BACKWARDS_1,
		ACTION_STANDING_DEATH_FALLING_BACKWARDS_2,
		ACTION_STANDING_DEATH_FALLING_LEFT_1,
		ACTION_STANDING_DEATH_FALLING_LEFT_2,
		ACTION_STANDING_DEATH_FALLING_RIGHT_1,
		ACTION_STANDING_DEATH_FALLING_RIGHT_2,
		ACTION_STANDING_DEATH_FIRST = ACTION_STANDING_DEATH_FALLING_FORWARD_1,
		ACTION_STANDING_DEATH_MAX = ACTION_STANDING_DEATH_FALLING_RIGHT_2,
		ACTION_GETTING_DOWN_FIRST = ACTION_STANDING_GETTING_PRONE,
		ACTION_GETTING_DOWN_MAX = ACTION_STANDING_DEATH_MAX,
		ACTION_STANDING_MAX = ACTION_STANDING_DEATH_FALLING_RIGHT_2,

		//actions in prone position
		ACTION_PRONE_FIRST,
		ACTION_PRONE = ACTION_PRONE_FIRST,
		ACTION_PRONE_FIRING,
		ACTION_PRONE_RELOAD_1,
		ACTION_PRONE_RELOAD_2,
		ACTION_PRONE_RELOAD_FIRST = ACTION_PRONE_RELOAD_1,
		ACTION_PRONE_RELOAD_MAX = ACTION_PRONE_RELOAD_2,
		ACTION_PRONE_GRENADE_THROWING,
		ACTION_PRONE_TAKING_COVER_FROM_INDIRECT_FIRE,
		ACTION_PRONE_CRAWLING,
		ACTION_PRONE_CRAWLING_LEFT,
		ACTION_PRONE_CRAWLING_RIGHT,
		ACTION_PRONE_TURNING_LEFT,
		ACTION_PRONE_TURNING_RIGHT,
		ACTION_PRONE_ADJUSTING_LEFT,
		ACTION_PRONE_ADJUSTING_RIGHT,
		ACTION_PRONE_GETTING_UP,
		ACTION_PRONE_WOUNDED_CRAWLING_1,
		ACTION_PRONE_WOUNDED_CRAWLING_2,
		ACTION_PRONE_WOUNDED_CRAWLING_FIRST = ACTION_PRONE_WOUNDED_CRAWLING_1,
		ACTION_PRONE_WOUNDED_CRAWLING_MAX = ACTION_PRONE_WOUNDED_CRAWLING_2,
		ACTION_PRONE_WOUNDED_TURNING_LEFT,
		ACTION_PRONE_WOUNDED_TURNING_RIGHT,
		ACTION_PRONE_GETTING_WOUNDED_AIMING_1,
		ACTION_PRONE_GETTING_WOUNDED_AIMING_2, ACTION_PRONE_GETTING_WOUNDED_AIMING_FIRST = ACTION_PRONE_GETTING_WOUNDED_AIMING_1, ACTION_PRONE_GETTING_WOUNDED_AIMING_MAX = ACTION_PRONE_GETTING_WOUNDED_AIMING_2,
		ACTION_PRONE_GETTING_WOUNDED_CRAWLING,
		ACTION_PRONE_GETTING_WOUNDED_TAKING_ITEM,
		ACTION_PRONE_GETTING_WOUNDED,
		ACTION_PRONE_GETTING_WOUNDED_FIRST = ACTION_PRONE_GETTING_WOUNDED_AIMING_1, ACTION_PRONE_GETTING_WOUNDED_MAX = ACTION_PRONE_GETTING_WOUNDED,
		ACTION_PRONE_DEATH_AIMING_1,
		ACTION_PRONE_DEATH_AIMING_2,
		ACTION_PRONE_DEATH_AIMING_3,
		ACTION_PRONE_DEATH_AIMING_4, ACTION_PRONE_DEATH_AIMING_FIRST = ACTION_PRONE_DEATH_AIMING_1,	ACTION_PRONE_DEATH_AIMING_MAX = ACTION_PRONE_DEATH_AIMING_4,
		ACTION_PRONE_DEATH_CRAWLING,
		ACTION_PRONE_DEATH_TAKING_ITEM,
		ACTION_PRONE_DEATH_1,
		ACTION_PRONE_DEATH_2,
		ACTION_PRONE_DEATH_3, ACTION_PRONE_DEATH_FIRST = ACTION_PRONE_DEATH_1, ACTION_PRONE_DEATH_MAX = ACTION_PRONE_DEATH_3,
		ACTION_PRONE_THROWN_BY_FORCE_1,
		ACTION_PRONE_THROWN_BY_FORCE_2, ACTION_PRONE_THROWN_BY_FORCE_FIRST = ACTION_PRONE_THROWN_BY_FORCE_1, ACTION_PRONE_THROWN_BY_FORCE_MAX = ACTION_PRONE_THROWN_BY_FORCE_2,
		ACTION_CARRIED_FALLING_1,
		ACTION_CARRIED_FALLING_FIRST = ACTION_CARRIED_FALLING_1,
		ACTION_CARRIED_FALLING_MAX = ACTION_CARRIED_FALLING_1,
		ACTION_PRONE_MAX = ACTION_CARRIED_FALLING_MAX,
		ACTION_MAX,

		REUSED_ANIMATIONS = 1,
		NUMBER_OF_ANIMATIONS = ACTION_MAX - REUSED_ANIMATIONS,
		MAX_FRAMECOUNT = 32
	};
	ACTION currentAction;
	hgeAnimation * actions[ACTION_MAX];

	//physical status information

	enum STATUS 
	{
		STATUS_HEALTHY,
		STATUS_WOUNDED,
		STATUS_SEVERELY_WOUNDED,
		STATUS_DEAD
	}
	status;

	void setStatus(STATUS status) { this->status = status; if (status != STATUS_HEALTHY) setActive(false); }
	STATUS getStatus(void) { return status; }

	enum BODY_POSITION
	{
		BODY_POSITION_STANDING,
		BODY_POSITION_PRONE
	}
	bodyPosition;

	void setBodyPosition(BODY_POSITION bodyPosition) { this->bodyPosition = bodyPosition; }
	BODY_POSITION getBodyPosition(void) { return bodyPosition; }

	enum ADVANCING_MODE
	{
		ADVANCING_MODE_WALKING_FORWARD,
		ADVANCING_MODE_RUNNING_FORWARD,
		ADVANCING_MODE_SNEAKING_FORWARD,
		ADVANCING_MODE_CRAWLING_FORWARD,
		ADVANCING_MODE_CRAWLING_SIDEWAYS_LEFT,
		ADVANCING_MODE_CRAWLING_SIDEWAYS_RIGHT
	}
	advancingMode;

	void setAdvancingMode(ADVANCING_MODE advancingMode) { this->advancingMode = advancingMode; }
	ADVANCING_MODE getAdvancingMode(void) { return advancingMode; }


	//Weapon information:
	static const Time ASSAULT_RIFLE_RATE_OF_FIRE_FULL_AUTO;
	static const int ASSAULT_RIFLE_CLIP_SIZE_FULL;
	static const int ASSAULT_RIFLE_SHOT_COUNT_BURST_MODE;
	static float ASSAULT_RIFLE_AIMING_ACCURACY;

	enum ASSAULT_RIFLE_FIRE_MODE
	{
		ASSAULT_RIFLE_FIRE_MODE_SINGLE_SHOT,
		ASSAULT_RIFLE_FIRE_MODE_BURST,
		ASSAULT_RIFLE_FIRE_MODE_SERIAL_FIRE
	}
	assaultRifleFireMode;

	//Military doctrine/training information:
	static Distance GRENADE_THROWING_DISTANCE;
	static Distance ASSAULT_RIFLE_BURST_FIRE_DISTANCE;
	static Distance COMBAT_DISTANCE; //game specific

	//Behaviour:
	static Time GRENADE_THROWING_INTERVAL;

	//general footsoldier information:
	static int FOOT_SOLDIER_AMOUNT_OF_GRENADES;
	static Distance FOOT_SOLDIER_ENEMY_AUTOMATIC_NOTIFICATION_DISTANCE;
	static Time FOOT_SOLDIER_AVERAGE_REACTION_TIME;
	static Time FOOT_SOLDIER_AVERAGE_REACTION_TIME_VARIATION;
	static Time FOOT_SOLDIER_AVERAGE_AIMING_SPEED_FOR_100_METERS;
	static Time FOOT_SOLDIER_AVERAGE_AIMING_SPEED_VARIATION_FOR_100_METERS;
	static float FOOT_SOLDIER_AVERAGE_AIMING_ACCURACY;
	static float FOOT_SOLDIER_AVERAGE_AIMING_ACCURACY_VARIATION;

	//Personal abilities
	float ability;
	Time reactionTime; //gives a small delay on things
	float aimingSpeed100Meters;
	float aimingAccuracy;

	//Body part information doesn't play a significant role 
	//in this game, but it's here.
	enum BODY_PART
	{
		BODY_PART_HEAD,
		BODY_PART_LEFT_ARM,
		BODY_PART_RIGHT_ARM,
		BODY_PART_LEFT_LEG,
		BODY_PART_RIGHT_LEG,
		BODY_PART_MIDDLE_BODY,
		BODY_PART_MAX,
		ASSAULT_RIFLE = BODY_PART_MAX,
		MACHINE_GUN,
		AT_WEAPON,
		NUMBER_OF_SEPARABLE_BODY_PARTS = 5,
	};
	int woundedBodyParts[BODY_PART_MAX];
	bool separatedBodyParts[BODY_PART_MAX];

	int getNumberOfWounds(BODY_PART bodyPart = (BODY_PART)-1);

	int clipAmmo;
	int grenades;
	Location aimingLocation; //The location where shots will end up from weapon barrel
	bool firing;
	bool throwing_grenade;
	int firingCounter;
	int shots;
	bool first_time_playing_frame;
	int frame;
	bool adjust;

	//sound
	HCHANNEL channel1;

	//death
	static float SCREAM_MIN_TIME;
	static Time HIT_STAY_UP_MAX_TIME;
	Timer screamTimer; //for minimum time between screams
	Timer fallingTimer;	
	Timer deathTimer;

	//flying (from an explosion)
	bool flying;

	//on fire
	bool burning;

	//carrying other foot soldier
	bool carrying_foot_soldier;
	bool carried;

	//medical
	bool medic_target;
	bool in_medical_care;

	//movement blocked by objects
	bool blocked;
	bool passing_from_left;
	bool passing_from_right;
	CollisionSphere blockingObject;
	//field of fire blocked by objects
	bool field_of_fire_blocked;
	bool overlapping_soldier;

	static bool attacker_taking_cover;
	static bool defender_taking_cover;
	static Time INDIRECT_FIRE_END_WAIT_TIME;

	//statics by type:
	static Angle PRONE_ANIMATION_TURNING_ANGLE;
	static Angle PRONE_ANIMATION_ADJUSTING_ANGLE;
	static Distance ENEMY_TANK_MIN_DISTANCE;
	static Distance FRIENDLY_TANK_MIN_DISTANCE;
	static Distance ENEMY_TANK_RUN_DISTANCE;
	static Distance ATTACK_MAX_ADVANCING_DISTANCE;
	static Distance ATTACK_MAX_CRAWLING_FORWARD_DISTANCE;
	static Distance ATTACK_MAX_CRAWLING_SIDEWAYS_DISTANCE;	
	static Distance ATTACK_MIN_ADVANCING_DISTANCE;
	static Distance ATTACK_MIN_CRAWLING_FORWARD_DISTANCE;
	static Distance ATTACK_MIN_CRAWLING_SIDEWAYS_DISTANCE;
	static Height HEIGHT_PRONE;
	static Height HEIGHT_STANDING;
	static Height LOWER_BODY_HEIGHT_STANDING;
	static Height WEAPON_HEIGHT_PRONE;
	static Height WEAPON_HEIGHT_STANDING;
	static Probability PROBABILITY_OF_DEATH_PRONE;	
	static Probability PROBABILITY_OF_DEATH_STANDING;
	static Time MAX_OBSERVATION_TIME_PER_OBJECT;
	static Velocity SPEED_WALKING;
	static Velocity SPEED_RUNNING;
	static Velocity SPEED_RUNNING_CARRYING_FOOT_SOLDIER;
	static Velocity SPEED_SNEAKING;
	static Velocity SPEED_CRAWLING;
	static Velocity SPEED_CRAWLING_SIDEWAYS;
	static Velocity SPEED_CRAWLING_WOUNDED;
	static Velocity TURNING_SPEED_WALKING;
	static Velocity TURNING_SPEED_RUNNING;
	static Velocity TURNING_SPEED_SNEAKING;

	//basic functions:
	static void init(void);
	static void release(void);
	FootSoldier(void);
	virtual ~FootSoldier(void);
	void set(FOOT_SOLDIER_CLASS footSoldierClass, CombatObject::SIDE side, Location startingLocation, Direction startDirection);
	void processAI(void);
	void processAction(void);
	void processInteraction(void);
	void render(void);
	void destroy(void);

	//class-specific state and state phase handling
	int getState(void) { return state; }
	void setStateVariable(int state) { this->state = (STATE)state; }
	void setState(int newState, Time stateChangeTimer = 0.0f, int startStatePhase = 0, bool nextPhaseReturn = true);
	
	//AI state functions
	virtual void attack(void);
	virtual void defend(void);
	virtual void fight(void);
	virtual void processFiring(void);
	virtual void reload(void);
	virtual void fireWeapon(void) {}
	virtual void fireAssaultRifle(void);
	virtual void throwGrenade(void);
	virtual void getUp(void);
	virtual void runAway(void);
	//virtual void runAwayFromTreeFall(void);
	virtual void takeCoverFromIndirectFire(void);
	virtual void thrown(void);
	virtual void burn(void);
	virtual void wounded(void);
	virtual void dying(void);
	virtual void fetchWounded(void) {}
	virtual void beingCarried(void);
	virtual void surrender(void);
	virtual void flee(void); 
	//checks
	virtual void checkTank(Tank * tank);
	virtual void setSurrendering(void);
	virtual void setFleeing(void);

	//action
	virtual void setAction(int nextAction, bool displacement = true);
	virtual void setMoving(ADVANCING_MODE advancingMode, Distance distance = 999999.0f); //"MOVEMENT!" -Marine, Half Life
	virtual void setTurning(Direction turningDirection);
	virtual void setRunningAway(Direction direction, Distance distance);
	virtual bool isActionFinished(void);
	virtual void move(void);
	virtual void turn(void);
	virtual void stop(void);
	virtual void fly(void);

	//interaction
	void collide(GameObject * other, ArcPoint hittingPoint);
	void collide(Projectile * projectile, ArcPoint hittingPoint);
	void collide(Tank * tank, ArcPoint hittingPoint);
	void collide(Effect * effect, ArcPoint hittingPoint);
	void collide(FootSoldier * other, ArcPoint hittingPoint);
	virtual void projectileHit(ArcPoint hittingPoint, Height projectileHeight, Direction projectileDirection, float size);
	virtual void runOverByTank(void);
	virtual void setFlying(Velocity speed, Direction flyingDirection, bool death);
	virtual void setTornApart(float force, ArcPoint pointOfForce); //severity: [0, 100]
	virtual void setBurning(void);
	virtual void setDarkening(float darkening);
	void scream(void);
	//communication
	void soldiersTakeCover(CombatAIObject::SIDE side);
	void startCheckingObjects(void);
	void checkObject(GameObject * otherObject);
	void stopCheckingObjects(void);

	//properties
	int getSubType(void) { return footSoldierClass; }
	void setActive(bool activeSet);
	Time getReactionTime(void);
	Time getAimingTimeFor100Meters(void);
	Height getHeight(void);
	Location getBarrelTipLocation(float addedLength = 0.0f)
	{
		Location l;
		l.x = x + (12.0f + addedLength) * cos(direction + 0.2f);
		l.y = y + (12.0f + addedLength) * sin(direction + 0.2f);
		return l;
	}
	Height getBarrelTipHeight(void);
	Location getGrenadeThrowingLocation(void)
	{
		Location l;
		l.x = x + (12.0f + 5.0f) * cos(direction + 0.2f);
		l.y = y + (12.0f + 5.0f) * sin(direction + 0.2f);
		return l;
	}
	Height getGrenadeThrowingHeight(void) { return HEIGHT_PRONE; }
	bool isNeutralized(void);
	CollisionSphere getCollisionSphere(int index = 0);
	Distance getAutomaticNotificationDistance(void);
	Location getBodyCenter(void);
	Distance getCrawlingDistanceBehindCover(Location coverLocation, Distance coverRadius);

	//static special functions
	static void levelStart(void);

	//technical
	virtual void setWoundedAction(Height height = 0.0f);
	virtual void setDeathAction(void);
	virtual void animationControl(void);
	bool isAnimationLastFramePlaying();

#define AT_ACTION_AMOUNT_OF 9
#define AMOUNT_OF_ALL_ACTIONS ACTION_MAX + AT_ACTION_AMOUNT_OF
	static int animationBodyPositions[AMOUNT_OF_ALL_ACTIONS];
	static Location animationDisplacements[AMOUNT_OF_ALL_ACTIONS];
	static int animationWoundedFrames[AMOUNT_OF_ALL_ACTIONS];
	static int animationDyingFramesMin[AMOUNT_OF_ALL_ACTIONS];
	static int animationDyingFramesMax[AMOUNT_OF_ALL_ACTIONS];
	static bool animationWoundedTwichForward[AMOUNT_OF_ALL_ACTIONS];

	enum ANIMATION_INFO
	{
		ANIMATION_INFO_BASIC = 0,
		ANIMATION_INFO_WOUNDED_SIMPLE = 1,
		ANIMATION_INFO_WOUNDED_COMPLEX = 2,
	};
	static int animationInfos[AMOUNT_OF_ALL_ACTIONS];

	static char classNameStrings[NUMBER_OF_CLASSES][64];

	//statistics
	void setStatistics(STATISTIC_VALUE val);

	//debug:
	char * getClassString(void);
	char * getStateString(int index = -2);
	virtual char * getStatePhaseString(void);
	virtual char * getActionString(void);
	char * getBodyPositionString(void);
	char * getStatusString(void);
	float dtree;
	int nextA;
};

//About different footsoldier classes:
//riflemen are the basic dudes,
//specialists are: machine gunners, AT-soldiers etc.
//riflemen fight in pairs, specialists work alone
//Eri luokat:
//Tehdään hyökkäys-, puolustus- ja taistelurutiinit omiin luokkiinsa.
//Vihollisen tyyppi vaikuttaa paljolti hyökkäyksessä ja puolustuksessa liikkumiseen
//ja taisteluun.

class Rifleman : public FootSoldier
{
public:

	enum RIFLEMAN_ATTACK_PHASE
	{
		RIFLEMAN_ATTACK_PHASE_SUPPORT_PAIR = FootSoldier::ATTACK_PHASE_LAST
	};

	FootSoldier * combatPair;
	bool turn_to_move;

	static void init(void) {}
	static void release(void) {}
	Rifleman() {}
	Rifleman(CombatObject::SIDE side, Location startingLocation, Direction startDirection);
	void set(CombatObject::SIDE side, Location startingLocation, Direction startDirection);
	void processAI(void);
	void render(void);
	void checkObject(GameObject * otherObject);

	//AI
	void fireWeapon(void);

	void setCombatPair(FootSoldier * fs);
	FootSoldier * getCombatPair() { return combatPair; }
};

class MachineGunner : public FootSoldier
{
public:

	Distance distanceFromSupportedTroops;
	int numberOfSupportedTroops;

	static Time MACHINE_GUN_RATE_OF_FIRE_FULL_AUTO;
	static int MACHINE_GUN_AMMO_CONTAINER_SIZE_FULL;
	static int MACHINE_GUN_MIN_SHOTS_PER_TRIGGER_PULL;
	static float MACHINE_GUN_AIMING_ACCURACY;
	static Distance MAX_SUPPORT_DISTANCE; //max distance from supported advancing troops

	static void init(void) {}
	static void release(void) {}
	MachineGunner() {}
	MachineGunner(CombatObject::SIDE side, Location startingLocation, Direction startDirection);
	void set(CombatObject::SIDE side, Location startingLocation, Direction startDirection);
	void render(void);
	void startCheckingObjects(void);
	void checkObject(GameObject * otherObject);
	void stopCheckingObjects(void);
	void attack(void);
	void defend(void);
	void fight(void);
	void processFiring(void);
	void reload(void);
	void fireWeapon(void);
	void fireMachineGun(void);

	Location getBarrelTipLocation(float addedLength = 0.0f);

private:
	Distance cumulativeDistance;
};

class AT_Soldier : public FootSoldier
{
public:

	int ATAmmo;
	bool ATWeaponLoaded;
	bool fire_with_AT_weapon;
	bool tank_at_area;

	static int AT_SOLDIER_INITIAL_AT_AMMO;
	static float AT_WEAPON_ACCURACY;
	static Distance AT_WEAPON_MIN_FIRING_DISTANCE;
#define ANGLE_BETWEEN_BODY_AND_AIMED_AT_WEAPON 0.7853982f //M_PI / 4.0f

	enum AT_ACTION
	{
		AT_ACTION_FIRST = FootSoldier::ACTION_MAX,
		AT_ACTION_TAKE_WEAPON = AT_ACTION_FIRST,
		AT_ACTION_PUT_AWAY_WEAPON,
		AT_ACTION_FIRING_POSITION,
		AT_ACTION_FIRING_AT_WEAPON,
		AT_ACTION_ADJUSTING_LEFT,
		AT_ACTION_ADJUSTING_RIGHT,
		AT_ACTION_RELOADING_AT_WEAPON,
		AT_ACTION_GETTING_HIT_AIMING,
		AT_ACTION_GETTING_HIT_RELOADING,
		AT_ACTION_GETTING_HIT_FIRST = AT_ACTION_GETTING_HIT_AIMING, AT_ACTION_GETTING_HIT_MAX = AT_ACTION_GETTING_HIT_RELOADING,
		AT_ACTION_MAX,
		NUMBER_OF_AT_ACTIONS = AT_ACTION_MAX - AT_ACTION_FIRST
	};

	hgeAnimation *  ATActions[NUMBER_OF_AT_ACTIONS];

	enum AT_FIRING_PHASE
	{
		AT_FIRING_PHASE_TURN,
		AT_FIRING_PHASE_GET_WEAPON,
		AT_FIRING_PHASE_ADJUST,
		AT_FIRING_PHASE_AIM,
		AT_FIRING_PHASE_FIRE,
		AT_FIRING_PHASE_PAUSE,
		AT_FIRING_PHASE_RELOAD,
		AT_FIRING_PHASE_PUT_AWAY_WEAPON,
		AT_FIRING_PHASE_END
	};

	static void init(void);
	static void release(void);
	AT_Soldier() {}	
	AT_Soldier(CombatObject::SIDE side, Location startingLocation, Direction startDirection);
	~AT_Soldier();
	void set(CombatObject::SIDE side, Location startingLocation, Direction startDirection);

	//AI
	void startCheckingObjects(void);
	void checkObject(GameObject * otherObject);
	void fight(void);
	void processFiring(void);
	void fireWeapon(void);
	void fireAntiTankWeapon(void);
	void reloadAntiTankWeapon(void);
	void getUp(void);
	bool isGoodToFire(void);

	//action
	void setAction(int nextAction, bool displacement = true);
	void move(void);
	void turn(void);

	//interaction:
	void setDarkening(float darkening);

	//wpn info
	Direction getATFiringDirection(GameObject * target);
	Location getATWeaponBarrelTipLocation(void);
	Height getATWeaponBarrelTipheight(void);
	Direction getATWeaponAimingDirection(void);
	Location getATWeaponBarrelBackLocation(void);
	Direction getATWeaponBarrelBackDirection(void);
	Location getAimedATWeaponCenter(void);

	//technical
	void setWoundedAction(Height height = 0.0f);
	void setDeathAction(void);
	char * getActionString(void);
	char * getStatePhaseString(void);
};

class Medic : public FootSoldier
{
public:

	FootSoldier * woundedSoldier;
	bool busy;
	bool dropping_soldier;
	float timer;

	enum MEDIC_ACTION
	{
		MEDIC_ACTION_FIRST = FootSoldier::ACTION_MAX,
		MEDIC_ACTION_LIFTING_FOOT_SOLDIER = MEDIC_ACTION_FIRST,
		MEDIC_ACTION_CARRYING_FOOT_SOLDIER_RUNNING,
		MEDIC_ACTION_MAX,
		NUMBER_OF_MEDIC_ACTIONS = MEDIC_ACTION_MAX - MEDIC_ACTION_FIRST
	};

	hgeAnimation *  medicActions[NUMBER_OF_MEDIC_ACTIONS];

	enum MEDIC_ACTION_PHASE
	{
		MEDIC_ACTION_PHASE_NO_WORK_GO_TO_MEDEVAC_ZONE,
		MEDIC_ACTION_PHASE_TURN_TO_BATTLEFIELD,
		MEDIC_ACTION_PHASE_OBSERVE_BATTLEFIELD,
		MEDIC_ACTION_PHASE_GO_TO_WOUNDED_SOLDIER,
		MEDIC_ACTION_PHASE_GET_WOUNDED_SOLDIER,
        MEDIC_ACTION_PHASE_GET_WOUNDED_TO_MEDEVAC_ZONE
	};

	static void init(void);
	static void release(void);
	Medic() {}	
	Medic(CombatObject::SIDE side, Location startingLocation, Direction startDirection);
	~Medic();
	void set(CombatObject::SIDE side, Location startingLocation, Direction startDirection);
	void render(void);

	//AI
	void startCheckingObjects(void);
	void checkObject(GameObject * otherObject);
	void stopCheckingObjects(void);
	void fetchWounded(void);
	void getUp(void);
	void setFleeing(void);
	void setSurrendering(void);

	//action
	void setAction(int nextAction, bool displacement = true);
	void setMoving(ADVANCING_MODE advancingMode, Distance distance = 999999.0f);
	void stop(void);

	//interaction:
	void projectileHit(ArcPoint hittingPoint, Height projectileHeight, Direction projectileDirection, float size);
	void runOverByTank(void);
	void setFlying(Velocity speed, Direction flyingDirection, bool death);
	void setTornApart(float force, ArcPoint pointOfForce); //severity: [0, 100]
	void setBurning(void);
	void setDarkening(float darkening);
	void dropSoldier(void);

	//properties
	FootSoldier * getWoundedSoldier(void) { return woundedSoldier; }
	void setWoundedSoldier(FootSoldier * fs);
	Location getMedEvacZone(void);

	//technical
	void animationControl(void);
	char * getActionString(void);
};

class SeparatedBodyPart : public MovingObject
{
public:

	FootSoldier::BODY_PART bodyPartType;	
	Velocity spinningSpeed;
	bool flying;
	bool blood;
	bool burstStop;
	hgeParticleSystem * bloodBurstPS;
	hgeParticleSystem * bloodSpillPS;
	Timer timer;
	Location centreLoc;
	Location loc;

	static Velocity MIN_START_SPEED;
	static Velocity MAX_START_SPEED;
	static Velocity MIN_SPINNING_SPEED;
	static Velocity MAX_SPINNING_SPEED;

	SeparatedBodyPart() {}
	~SeparatedBodyPart();
	SeparatedBodyPart(Location startLocation, Velocity startSpeed, Direction startDirection, Direction bodyDirection, FootSoldier::BODY_PART bodypart);
	void processAction(void);
	void render(void);

	char * getTypeString(void);
};
class MilitaryUnit : public CombatAIObject
{
public:

	enum INTS
	{
		MAX_SIZE = 30,
		MESSAGE_PULLING_BACK = 0,
		MESSAGE_SURRENDERING,
		NUMBER_OF_MESSAGE_TYPES = 2
	};

	int startSize;
	int numberOfActiveUnits;
	int numberOfNonCombatantUnits;

	CombatAIObject * units[MAX_SIZE];

	static char messages[CombatObject::NUMBER_OF_SIDES][NUMBER_OF_MESSAGE_TYPES][256];

	static void init(void);
	static void release(void);
	MilitaryUnit(CombatObject::SIDE side);
	virtual ~MilitaryUnit(void) {}
	void processInteraction(void);
	virtual void add(CombatAIObject * caio);
	void surrender(void);
	void flee(void);
};