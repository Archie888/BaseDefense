#pragma once
#include "combatobject.h"
#include "projectile.h"
#include "hgeParticle.h"

struct MortarGrenade
{
public:
	//kranaattidata
	Timer flyTimer;
	bool whistle;
	bool flying;
	Location targetLocation;

	static Time WHISTLE_TIME;

	MortarGrenade(void);
	void process(void);
	bool isFlying(void) { return flying; }
	void fire(Location targetLocation, Time flyingTime);
};

class NapalmBomb : public CombatObject
{
public:

	static const float WEIGHT;
	static const Distance IMAGE_DISTANCE_TO_CORNER;
	static const Angle IMAGE_ANGLE_TO_CORNER;

	float height;
	bool dropping;
	float rotation;
	float angle;

	NapalmBomb();
	NapalmBomb(Location location, Direction direction, SIDE side);
	~NapalmBomb();

	void set(Location location, Direction direction, SIDE side);
	void processInteraction(void);
	void render(void);
	void setDropped(Height height);
	void explode(void);
};

class MortarTeam : CombatObject
{
public:

	Timer nextRoundTimer;
	int ammo;
	Time firingInterval;

	enum INTS
	{
		MAX_AMMO = 18,
		NUMBER_OF_SIMULTANEOUS_GRENADES = 4
	};

	MortarGrenade grenades[NUMBER_OF_SIMULTANEOUS_GRENADES];

	//statics
	static Time GRENADE_FLY_TIME_DEFENDER;
	static Time GRENADE_FLY_TIME_ATTACKER;
	static Time NEUTRALIZING_FIRE_INTERVAL;
	static Time MORTAR_FIRE_INTERVAL_VARIATION;

	MortarTeam(void);
	void set(CombatObject::SIDE side);
	void processInteraction(void);
	void processFiring(void);
	void fire(void);
	void processGrenades(void);
	Location getRandomTargetLocation(void);
	Time getGrenadeFlyingTime(void);
	void playFiringSound(void);
	bool finishedFiring(void);
};

class FireSupportUnit : public CombatObject
{
public:

	Timer showTextTimer;

	void set(CombatObject::SIDE side);
	virtual bool isFinished() { return true; }
};

class MortarPlatoon : FireSupportUnit
{
public:

	enum INTS 
	{
		NUMBER_OF_TEAMS = 3
	};
	MortarTeam teams[NUMBER_OF_TEAMS];

	MortarPlatoon() {}
	MortarPlatoon(CombatObject::SIDE side);
	~MortarPlatoon() {}
	void set(CombatObject::SIDE side);
	//void render(void);
	void processInteraction(void);
	bool isFinished(void);
};

class MissileStrike : public FireSupportUnit
{
public:

	int numberOfMissiles;
	Location nextMissileFiringLocation;
	Location nextMissileDestination;
	Timer fighterAppearTime;
	Timer fireMissileTimer;
	bool targets_checked;

	Distance cumulativeDistance;
	int numberOfEnemyTroops;

	MissileStrike();
	~MissileStrike();
	MissileStrike(CombatObject::SIDE side);
	void set(CombatObject::SIDE side);
	void processInteraction(void);
	//void render();
	bool isFinished(void);
	Height getHeight();

	void startCheckingObjects(void);
	void checkObject(GameObject * otherObject);
	void stopCheckingObjects(void);
};

class NapalmStrike : public FireSupportUnit
{
public:

	Timer bomberAppearTimer;
	Timer onLocationTimer;
	Timer checkTimer;
	bool check;
	bool bombs_dropped;
	Location droppingLocation;
	Distance cumulativeDistance;
	int numberOfEnemyTroops;

#define NUMBER_OF_BOMBS 3
	NapalmBomb * bombs[NUMBER_OF_BOMBS];

	NapalmStrike();
	~NapalmStrike();
	NapalmStrike(CombatObject::SIDE side);
	void set(CombatObject::SIDE side);
	void processInteraction(void);
	void render();
	bool isFinished(void);
	Height getHeight();
	void bombsAway();

	void startCheckingObjects(void);
	void checkObject(GameObject * otherObject);
	void stopCheckingObjects(void);
};

class FireSupport : public CombatObject
{
public:

	enum FIRE_SUPPORT_TYPE
	{
		MORTAR_STRIKE,
		MISSILE_STRIKE,
		NAPALM_STRIKE,
		NUMBER_OF_TYPES
	}
	fireSupportType;

	FireSupportUnit * supportingUnit;

	static char messages[CombatObject::NUMBER_OF_SIDES][NUMBER_OF_TYPES][256];
	static char typeNames[NUMBER_OF_TYPES][64];

	static void init(void);
	static void release(void);
	FireSupport(void);
	~FireSupport(void);
	FireSupport(FIRE_SUPPORT_TYPE fireSupportType, CombatObject::SIDE side);
	void set(FIRE_SUPPORT_TYPE fireSupportType, CombatObject::SIDE side);
	void processInteraction(void);
	void render(void);

	void startCheckingObjects(void);
	void checkObject(GameObject * otherObject);
	void stopCheckingObjects(void);
};