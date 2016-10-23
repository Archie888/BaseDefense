#pragma once
#include <hgeanim.h>
#include <math.h>
#include <list>
using namespace std;

typedef float Direction;
typedef float Angle;
typedef float Distance;
typedef float SquaredDistance;
typedef float Coordinate;
typedef float Velocity;
typedef float Time;
typedef float Timer;
typedef float Size;
typedef float Height;
typedef float Probability;

class Location
{
public:
	Coordinate x;
	Coordinate y;

	Location() { x = 0.0f; y = 0.0f; }
	Location(Coordinate x, Coordinate y) { this->x = x; this->y = y; }
	~Location() {}
	void set(Coordinate x, Coordinate y) { this->x = x; this->y = y; }
	float distance(Location location)
	{
		float a = location.x - x;
		float b = location.y - y;
		float pow1 = a * a;
		float pow2 = b * b;
		float distance = sqrt(pow1 + pow2);
		return distance;
	}
	float squaredDistance(Location location)
	{
		float a = location.x - x;
		float b = location.y - y;
		float pow1 = a * a;
		float pow2 = b * b;
		float squaredDistance = (pow1 + pow2);
		return squaredDistance;
	}
	float angle(Location location)
	{
		float a = location.x - x;
		float b = location.y - y;
		float tangent = b / a;
		float angle = atan(tangent); 
		if (a < 0) angle += M_PI;
		else if (b < 0) angle += 6.283185f;
		if (a == 0.0f && b == 0.0f) return 0.0f;
		return angle;
	}
};

struct CollisionSphere
{
public:
	Location location;
	Distance radius;
	SquaredDistance squaredRadius;

	CollisionSphere() {set();}
	CollisionSphere(Location location, Distance radius) { set(location, radius); }
	~CollisionSphere() {}
	void set(Location location = Location(), Distance radius = 0.0f)
	{
		this->location = location;
		this->radius = radius;
		this->squaredRadius = radius * radius; 
	}
	bool testHit(CollisionSphere other)
	{
		float rr = radius + other.radius;
		return location.squaredDistance(other.location) < (rr * rr);
	}
	Location getLocation()
	{
		return location;
	}
};

struct ArcPoint
{
public:
    Location baseLocation;
	Distance radius;
	Angle angle;

	ArcPoint() { radius = 0.0f; angle = 0.0f; }
	ArcPoint(Location baseLocation, Location arcPointLocation)
	{
		this->baseLocation = baseLocation;
		radius = baseLocation.distance(arcPointLocation);
		angle = baseLocation.angle(arcPointLocation);
	}
	ArcPoint(Location baseLocation, Distance radius, Angle angle) 
	{ 
		set(baseLocation, radius, angle);
	}
	~ArcPoint() {}
	void set(Location baseLocation, Distance radius, Angle angle) 
	{ 
		this->baseLocation = baseLocation;
		this->radius = radius;
		this->angle = angle;
	}
	Location getLocation()
	{
		Location l;
		l.x = baseLocation.x + radius * cos(angle);
		l.y = baseLocation.y + radius * sin(angle);
		return l;
	}
};

class GameObject
{
private:
	int referenceCount;
	Distance collisionSphereRadius;
public:
	Coordinate x, y;
	Direction direction; //0.0f - 2pi -radiaania
	Size size;
	Height height;
	Distance shadowOutLineRadius;
	static float objectSize;
	int number;
	char * dbgString;
	static int numberOfObjects;
	bool show_location;
	hgeAnimation * image; //Tätä ei saa staattiseksi.
	bool visible;
	bool computer_controlled;
	bool startChecks; //control for object checking
	int collisionSphereCount;
	int renderOrder;
	int spawnCounter;

	enum TYPE
	{
		BACKGROUND,
		TANK,
		TURRET,
		TANKMAGHINEGUN,
		FOOT_SOLDIER,
		SIGHT,
		PROJECTILE,
		EFFECT,
		TANK_CARCASS,
		CORPSE,
		BASE,
		TREE,
		BUSH,
		FIRE_SUPPORT,
		SEPARATED_BODY_PART,
		FLYING_SMOKING_DEBRIS,
		MILITARY_UNIT,
		POINTER,
		NUMBER_OF_TYPES,
		SUBTYPE_MAX = 16
	};
	TYPE type;

	enum DERIVED_CLASS
	{
		BACKGROUND_OBJECT = 1,
		COMBAT_OBJECT = 2,
		COMBAT_AI_OBJECT = 4
	};

	int derivedClass;

	TYPE getType() { return type; }
	void setType(TYPE type) { this->type = type; }

	static char * typeNames[NUMBER_OF_TYPES];
	static int renderOrders[NUMBER_OF_TYPES];
	static Distance SOUND_CLIPPING_DISTANCE;
	static const Direction DIRECTION_FORWARD;
	static const Direction DIRECTION_BACKWARDS;
	static const Direction DIRECTION_LEFT;
	static const Direction DIRECTION_RIGHT;

	int getRenderOrder() { return renderOrder; }
	void setRenderOrder(int order) { renderOrder = order; }
	void changeRenderOrder(int order);
	
	GameObject(void);
	void set(TYPE type, Location location, Direction direction, Size size = 1.0f);
	virtual ~GameObject(void);
	static void init(void);
	virtual void render(void);
	virtual void processAction(void);
	virtual void processAI(void);
	virtual void collide(GameObject * other, ArcPoint hittingPoint = ArcPoint()) {}//; //The arcpoint is needed for projectile penetration (between frames) information
	virtual void processInteraction(void);
	virtual void startCheckingObjects(void);
	virtual void checkObject(GameObject * otherObject);
	virtual void stopCheckingObjects(void);
	virtual void playSound(void);
	HCHANNEL playSoundAtLocation(HEFFECT effect, int volume = 100, float pitch = 1.0f, bool loop = false);
	inline int getLocationPanning();
	inline int getLocationVolume(int volume);
	virtual void playAnimation(void);
	void renderShadows(void);
	static void release(void);
	virtual void destroy(void);

	bool isBackgroundObject(void)
	{
		return ((derivedClass & BACKGROUND_OBJECT) > 0);
	}
	bool isCombatObject(void)
	{
		return ((derivedClass & COMBAT_OBJECT) > 0);
	}
	bool isCombatAIObject(void)
	{
		return ((derivedClass & COMBAT_AI_OBJECT) > 0);
	}

	hgeAnimation * getAnimation(void)
	{
		return image;
	}
	void setAnimation(hgeAnimation * animation)
	{
		this->image = animation;
	}
	virtual Location getLocation(void)
	{
		return Location(x, y);
	}
	virtual void setLocation(Location location)
	{
		this->x = location.x;
		this->y = location.y;
	}
	virtual float getDirection(void)
	{
		return direction;
	}
	virtual void setDirection(float direction)
	{
		if (direction > 6.283185f)
			direction -= 6.283185f;
		if (direction < 0.0f)
			direction += 6.283185f;
		this->direction = direction;
	}
	virtual Size getSize(void)
	{
		return size;
	}
	virtual void setSize(Size size)
	{
		if (size < 0.0f)
			size = 0.0f;
		this->size = size;
	}
	virtual Height getHeight(void)
	{
		return height;
	}
	virtual void setHeight(Height height)
	{
		this->height = height;
	}

	int getDerivedClass(void)
	{
		return derivedClass;
	}
	void setDerivedClass(int dc)
	{
		this->derivedClass = dc;
	}

	void setCollisionSphereRadius(int radius)
	{
		this->collisionSphereRadius = radius * GameObject::objectSize;
	}

	int getCollisionSphereCount()
	{
		return collisionSphereCount;
	}

	virtual Distance getDynamicCollisionSphereRadius(int i) { return collisionSphereRadius; }

	Distance getCollisionSphereRadius(int i = -1)
	{
		if (collisionSphereCount > 1)
			return getDynamicCollisionSphereRadius(i);
		else
			return collisionSphereRadius;
	}

	virtual CollisionSphere getDynamicCollisionSphere(int index) { return CollisionSphere(getLocation(), collisionSphereRadius); }

	virtual CollisionSphere getCollisionSphere(int index = 0)
	{
		if (collisionSphereCount < 2)
		{
			//original gameobject only has 1 collisionsphere, derived may vary.
			CollisionSphere cs(getLocation(), collisionSphereRadius);
			return cs;
		}
		else
		{
			return getDynamicCollisionSphere(index);
		}
	}
	
	//math functions for object properties

	float compareDistance(GameObject * otherObject, float distance)
	{
		float a = otherObject->x - x;
		float b = otherObject->y - y;
		float pow1 = a * a;
		float pow2 = b * b;
		float squaredDistanceToObject = pow1 + pow2;
		float sub = squaredDistanceToObject - (distance * distance);
		return sub;		
	}

	//this function is for comparing squared distances with non-squared ones.
	float compareDistance(float squaredDistance, float distance2)
	{
		float d2 = distance2 * distance2;
		float ret = squaredDistance - d2;
		return ret;
	}

	float objectDistance(GameObject * object2)
	{
		float a = object2->x - x;
		float b = object2->y - y;
		float pow1 = a * a;
		float pow2 = b * b;
		float distance = sqrt(pow1 + pow2);
		return distance;
	}

	float objectDistance(Location location)
	{
		float a = location.x - x;
		float b = location.y - y;
		float pow1 = a * a;
		float pow2 = b * b;
		float distance = sqrt(pow1 + pow2);
		return distance;
	}

	float squaredObjectDistance(GameObject * object2)
	{
		float a = object2->x - x;
		float b = object2->y - y;
		float pow1 = a * a;
		float pow2 = b * b;
		float distance = (pow1 + pow2);
		return distance;
	}

	float squaredObjectDistance(Location location)
	{
		float a = location.x - x;
		float b = location.y - y;
		float pow1 = a * a;
		float pow2 = b * b;
		float distance = (pow1 + pow2);
		return distance;
	}

	float objectAngle(GameObject * object2)
	{
		float a = object2->x - this->x;
		float b = object2->y - this->y;
		float tangent = b / a;
		float angle = atan(tangent); 
		if (a < 0) angle += M_PI;
		else if (b < 0) angle += 6.283185f;//::PI_TIMES_TWO;
		if (a == 0.0f && b == 0.0f) return 0.0f;
		return angle;
	}

	float objectAngle(Location location)
	{
		float a = location.x - this->x;
		float b = location.y - this->y;
		float tangent = b / a;
		float angle = atan(tangent); 
		if (a < 0) angle += M_PI;
		else if (b < 0) angle += 6.283185f;//::PI_TIMES_TWO;
		return angle;
	}
	
	int getVertex(ArcPoint arcPoint);

	//test for random event taking place usually once in a given time.
	bool randomEvent(Time seconds);

	//directions:
	Direction getDirection_FORWARD(void) { return DIRECTION_FORWARD; }
	Direction getDirection_BACKWARDS(void) { return DIRECTION_BACKWARDS; }
	Direction getDirection_LEFT(void) { return DIRECTION_LEFT; }
	Direction getDirection_RIGHT(void) { return DIRECTION_RIGHT; }

	//properties
	virtual int getSubType(void) { return 0; }
	bool isOnScreen(void)
	{
		return x >= 0 && x <= 800 && y >= 0 && y <= 600;
	}
	bool imageIsOnScreen(void)
	{
		float halfTextureWidth = image->GetWidth() / 2.0f;
		float halfTextureHeight = image->GetHeight() / 2.0f;
		return x + halfTextureWidth >= 0 && x - halfTextureWidth <= 800 && y + halfTextureHeight >= 0 && y - halfTextureHeight <= 600;
	}

	bool isAtArea(Distance distance = 0.0f)
	{
		return x >= -distance && x <= 800 + distance && y >= -distance && y <= 600 + distance;
	}

	//dbg functions:
	virtual void renderCollisionSpheres(void);
	virtual void renderCross(void);

	//render lines:
	void rl(); 
	void rl(float x2,float  y2);
	void rl(float x1, float y1, float x2, float y2);
	void rl(ArcPoint ap);
	void rl(Location l);

	//render text:
	void rt(Location l, char * msg);

	//returns the location at previous frame
	virtual Location getPreviousLocation(void) { return Location();}

	void addReference() { referenceCount += 1; }
	void removeReference() { referenceCount -= 1; }
	int getReferenceCount() { return referenceCount; }
	list<GameObject *>::iterator AIObjectsIterator;
	list<GameObject *>::iterator actionObjectsIterator;
	list<GameObject *>::iterator combatObjectsIterator; //tanks or footSoldiers
	list<GameObject *>::iterator renderListIterator;
	
	//technical:
	void checkSpawn(void);
private:
	bool destroyed;
public:
	bool isDestroyed() { return destroyed; }
	virtual void setDestroyed(bool destroyed) { this->destroyed = destroyed; }
};