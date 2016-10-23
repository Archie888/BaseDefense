#pragma once
#include "CombatAIObject.h"
#include "tank.h"
#include "effect.h"

class Base :
	public CombatAIObject
{
public:
		
	TankMachineGun tankMG;

	//shield:
	float shield;

	//combat:
	Timer aimingTimer;
	Timer firingTimer;
	Timer gunShotIntervalTimer;
	bool firing;
	bool gunner_alive;

	//AI information flags:
	bool target_near_sight;
	bool target_in_sight;

	//message flags
	bool damagemsg;
	bool destroyedmsg;

	enum CONST_INTS
	{
		TEXTURE_WIDTH = 128,
		TEXTURE_HEIGHT = 128,
		COLLISIONSPHERECOUNT = 5
	};

	enum STATE
	{
		STATE_DEFEND,
		STATE_FIGHT
	} state;

	enum DEFENSE_PHASE
	{
		DEFENSE_PHASE_OBSERVE,
		DEFENSE_PHASE_FIRE_START,
		DEFENSE_PHASE_FIGHT
	};

	enum FIGHTING_PHASE
	{
		FIRING_PHASE_TURN,
		FIRING_PHASE_AIM,
		FIRING_PHASE_FIRE,
		//FIRING_PHASE_RELOAD
	};

	HTEXTURE tex;

	static Height WEAPON_HEIGHT;
	static Velocity MG_TURNING_SPEED;	
	static Distance COLLISIONSPHERE_RADIUS_FRONT_BUNKER;
	static Distance COLLISIONSPHERE_RADIUS_BARRICADE;
	static Time MG_RATE_OF_FIRE;
	static float MG_ACCURACY;
	static const Distance MG_BARREL_LENGTH;
	static const Angle MG_MIN_ANGLE;
	static const Angle MG_MAX_ANGLE;

	Base(void);
	Base(CombatObject::SIDE side, Location location, Direction direction);
	~Base(void);
	void set(CombatObject::SIDE side, Location location, Direction direction);
	static void init(void);
	static void release(void);
	void processAI(void);
	void processAction(void);
	void processInteraction(void);
	void render(void);
	void renderShadows(void);
	void collide(GameObject * other, ArcPoint hittingPoint);
	void collide(Projectile * projectile, ArcPoint hittingPoint);
	void collide(Effect * effect, ArcPoint hittingPoint);

	//helpers
	Location MGGetBarrelTipLocation(float addedLength = 0.0f);
	Height MGGetBarrelTipHeight(void);

	//AI state functions
	int getState(void) { return (int)state; }
	void setStateVariable(int state) { this->state = (STATE)state; }

	//AI
	void checkObject(GameObject * otherObject);
	void defend(void);
	void fight(void);

	//processAction methods
	inline void processTurning(void);

	//processTurning:
	inline void turnLeft(void);
	inline void turnRight(void);

	//properties
	Distance getDynamicCollisionSphereRadius(int index = -1);
	CollisionSphere getDynamicCollisionSphere(int index);
	Location getLocation(void);
	GameObject * getNearestEnemy(void);
	float objectAngle(GameObject * other);
	Direction getDirection(void);
	void setDamage(float damage);

	//misc
	void renderStats(void);
	void setHole(int holeX, int holeY);
private:
	int whichSound;
};
