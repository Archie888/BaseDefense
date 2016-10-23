#pragma once
//#include "GameObject.h"
#include "CombatObject.h"
#include <hgesprite.h>
#include <hgeparticle.h>


class Projectile : public CombatObject
{
public:
	//<dbg>
	static int amount;
	Location firingLocation;
	//</dbg>

	float age;

	enum PROJECTILETYPE
	{
		TANK_CANNON_PROJECTILE,
		AT_ROCKET,
		AIR_TO_GROUND_MISSILE,
		TANK_MG_BULLET,
		ASSAULT_RIFLE_BULLET,
		HAND_GRENADE,
		SMOKE_GRENADE,
		FLARE
	};
	PROJECTILETYPE projectileType;

	GameObject * shooter;
	Height startHeight;
	Distance startDistance;
	Distance distance;
	Distance frameDistance;
	Location previousLocation;
	Location goalLocation;
	bool reachedGoal;
	Timer timer;

	static HTEXTURE tex;
	static hgeAnimation * tankCannonProjectileImage;
	static hgeAnimation * ATProjectileImage;
	static hgeAnimation * bulletImage;
	static hgeAnimation * grenadeImage;

	static const Time MAX_GAME_AGE;
	static const Velocity MISSILE_SPEED;
	static const Velocity TANK_CANNON_PROJECTILE_SPEED;
	static const Velocity AT_ROCKET_SPEED;
	static const Velocity TANK_MG_BULLET_SPEED;
	static const Velocity ASSAULT_RIFLE_BULLET_SPEED;
	static const Velocity HAND_GRENADE_SPEED;	
	static const Distance MISSILE_COLLISIONSPHERE_RADIUS;
	static const Distance TANK_SHELL_COLLISIONSPHERE_RADIUS;
	static const Distance AT_ROCKET_COLLISIONSPHERE_RADIUS;
	static const Distance TANK_MG_BULLET_COLLISIONSPHERE_RADIUS;
	static const Distance ASSAULT_RIFLE_BULLET_COLLISIONSPHERE_RADIUS;
	static const Distance HAND_GRENADE_NOTIFICATION_RADIUS;
	static const Time HAND_GRENADE_TIME;

	Projectile(void);
	Projectile(CombatObject * shooter, PROJECTILETYPE projectileType, Location firingLocation, Height firingHeight, Location destination);
	~Projectile(void);
	void set(CombatObject * shooter, PROJECTILETYPE projectileType, Location firingLocation, Height firingHeight, Location destination);
	static void init(void);
	static void release(void);
	void processAction(void);
	void processInteraction(void);
	void render(void);
	void hit(GameObject::TYPE hitObjectType, Location hittingLocation);
	void hit(void);
	void explode(void);
	void smoke(void);
	bool checkPenetrationCourse(CollisionSphere cs);
	ArcPoint getPenetrationPoint(CollisionSphere cs);
	Height getHeight(void);

	Location getPreviousLocation(void)
	{
		ArcPoint ap(getLocation(), frameDistance, direction + M_PI);
		return ap.getLocation();
	}

	void setDestination(Location location)
	{
		this->goalLocation = location;
	}
	Location getDestination(void)
	{
		return goalLocation;
	}

	Distance getFrameDistance(void)
	{
		return frameDistance;
	}
};

class Grenade : public Projectile
{
public:
	Angle angle;

	Grenade(CombatObject * shooter, PROJECTILETYPE projectileType, Location firingLocation, Height firingHeight, Location destination);
	void processAction();
	void processInteraction(void);
	void render(void);
};