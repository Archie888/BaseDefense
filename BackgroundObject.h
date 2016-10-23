#pragma once
#include "gameobject.h"
#include "projectile.h"
#include "tank.h"
#include "effect.h"

class BackgroundObject :
	public GameObject
{
public:
	static HTEXTURE bushtex;

	enum BACKGROUNDOBJECT_TYPE
	{
		BG_TREE1,
		BG_TREE_MAX,
		BG_BUSH1 = BG_TREE_MAX,
		BG_BUSH2,
		BG_BUSH3,
		BG_BUSH4,
		BG_BUSH_MAX,
		BG_MAX = BG_BUSH_MAX
	};
	static char * names[BG_MAX];


	static void init(void);
	static void release(void);
	BackgroundObject(void);
	void set(GameObject::TYPE type, Location loation, Size size, Direction direction);
	virtual ~BackgroundObject(void);
};

class Bush :
	public BackgroundObject
{
public:

	Bush(void);
	Bush(BackgroundObject::BACKGROUNDOBJECT_TYPE bushtype, Location location, Size size, Direction direction = -1.0f);
	void set(BackgroundObject::BACKGROUNDOBJECT_TYPE bushtype, Location location, Size size, Direction direction = -1.0f);
	virtual ~Bush(void);
	void processInteraction(void);
};

class Tree :
	public BackgroundObject
{
public:

	enum INTS
	{
		TEXTURE_WIDTH = 128,
		TEXTURE_HEIGHT = 64,
	};

	float treeFallSpeed;
	bool falling;
	bool fallen;
	bool set_order;
	bool burning;
	Timer burningTimer;
#define MAX_FIRES 3
	Effect * effects[MAX_FIRES];
	Location burningLocations[MAX_FIRES];
	float timer1;


	static Size TANK_TURRET_BARREL_TURNING_SIZE;
	static Height AVERAGE_TREE_HEIGHT;
	static HTEXTURE treetex;

   	Tree(void);
	Tree(BackgroundObject::BACKGROUNDOBJECT_TYPE treetype, Location location, Size size, Direction direction = -1.0f);
	void set(BackgroundObject::BACKGROUNDOBJECT_TYPE treetype, Location location, Size size, Direction direction = -1.0f);
	virtual ~Tree(void);
	static void init(void);
	static void release(void);
	void processAction(void);
	void processInteraction(void);
	void render(void);
	void fall(Direction direction);
	void collide(GameObject * otherObject, ArcPoint hittingPoint);
	void collide(Tank * tank, ArcPoint hittingPoint);
	void collide(Projectile * projectile, ArcPoint hittingPoint);
	void collide(Effect * effect, ArcPoint hittingPoint);
	Location getLocation(void)
	{
		if (fallen)
		{
			return ArcPoint(Location(x, y), 32.0f, direction).getLocation();
		}
		else return Location(x, y);
	}
private:
	void bulletHit(Projectile * projectile, ArcPoint hittingPoint);
};