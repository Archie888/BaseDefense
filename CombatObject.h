#pragma once
#include "gameobject.h"

class MovingObject : public GameObject
{
public:

	bool moving;
	bool turning;
	bool accelerating;
	bool deaccelerating;

	Velocity speed;
	Velocity turningSpeed;
	Velocity acceleration;
	Velocity deacceleration;
	Distance dx;
	Distance dy;	
	Direction movingDirection; //2D moving direction relative to object's facing direction
	Distance movingDistance;
	Direction turningDirection;
	Angle angleToTurn;

	//turning
	bool turning_left;
	bool turning_right;

	MovingObject(void);
	void set(GameObject::TYPE type, Location location, Direction direction);
	void processAction(void);
	virtual void move(void);
	void pixelMove(float pixels = 1.0f);
	void stopMoving() { setSpeed(0.0f); moving = false; }
	virtual void turn(void); //turning is moving
	void stopTurning() { setTurningSpeed(0.0f); turning_left = turning_right = turning = false; }
	virtual void stop(void) { stopMoving(); stopTurning(); }
	bool isMoving(void) { return moving; }
	bool isTurning(void) { return turning; }

	void setSpeed(Velocity speed) { this->speed = speed; }
	Velocity getSpeed(void) { return speed; }

	void setMovingDirection(Direction dir) { this->movingDirection = dir; }
	Direction getMovingDirection(void) { return movingDirection; }

	void setMovingDistance(Distance dist) { this->movingDistance = dist; }
	Distance getMovingDistance(void) { return movingDistance; }

	void setTurningDirection(Direction direction2);
	Direction getTurningDirection(void) { return turningDirection; }

	void setTurningSpeed(Velocity turningSpeed) { this->turningSpeed = turningSpeed; }
	Velocity getTurningSpeed(){ return turningSpeed; }
};

class CombatObject : public MovingObject
{
public:

	enum SIDE
	{
		DEFENDER_SIDE,
		ATTACKER_SIDE,
		NUMBER_OF_SIDES,
		IMPARTIAL
	};
	SIDE side;

	SIDE getSide() { return side; }
	SIDE getOppositeSide() { return (side == DEFENDER_SIDE)? DEFENDER_SIDE:ATTACKER_SIDE; }
	static SIDE getOppositeSide(SIDE side) { return (side == DEFENDER_SIDE)? DEFENDER_SIDE:ATTACKER_SIDE; }
	void setSide(SIDE s) { side = s; }

	enum STATISTIC_VALUE
	{
		STATISTIC_VALUE_FIRST, 
		STATISTIC_VALUE_PERSON_FIRST = STATISTIC_VALUE_FIRST,
		STATISTIC_VALUE_PERSON_KILLED = STATISTIC_VALUE_PERSON_FIRST,
		STATISTIC_VALUE_PERSON_CAPTURED,
		STATISTIC_VALUE_PERSON_WOUNDED,
		STATISTIC_VALUE_PERSON_FLED,
		//STATISTIC_VALUE_PERSON_ACTIVE,
		//STATISTIC_VALUE_PERSON_AT_AREA,
		//STATISTIC_VALUE_PERSON_INVOLVED,
		STATISTIC_VALUE_PERSON_LAST = STATISTIC_VALUE_PERSON_FLED,
		STATISTIC_VALUE_VEHICLE_DESTROYED,
		STATISTIC_VALUE_VEHICLE_FLED,
		//STATISTIC_VALUE_VEHICLE_ACTIVE,
		//STATISTIC_VALUE_VEHICLE_AT_AREA,
		//STATISTIC_VALUE_VEHICLE_INVOLVED,
		STATISTIC_VALUE_OTHER,
		STATISTIC_VALUE_LAST = STATISTIC_VALUE_OTHER,
		NUMBER_OF_STATISTIC_VALUES
	} statisticValue;

	//scores
	bool scored;

	static Distance MAX_UNIT_DISTANCE_FROM_AREA;
	bool is_at_area;

	static char sideNames[NUMBER_OF_SIDES][64];
	static int colorIndexes[NUMBER_OF_SIDES];

	static void  init();
	static void  release();
	CombatObject();
	void set(GameObject::TYPE type, Location location, Direction direction, SIDE side);
	virtual ~CombatObject(void);
	virtual void setStatistics(STATISTIC_VALUE val);
};