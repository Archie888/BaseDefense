#include ".\CombatObject.h"
#include "main.h"

MovingObject::MovingObject(void)
:
 moving(false)
,turning(false)
,speed(0.0f)
,turningSpeed(0.0f)
,acceleration(0.0f)
,deacceleration(0.0f)
,dx(0.0f)
,dy(0.0f)
,movingDirection(0.0f)
,movingDistance(0.0f)
,angleToTurn(0.0f)
,turning_left(false)
,turning_right(false)
{
}

void MovingObject::set(GameObject::TYPE type, Location location, Direction direction)
{
	GameObject::set(type, location, direction);
}

void MovingObject::processAction(void)
{
	if (turning)
		turn();
	if (moving)
		move();
}

void MovingObject::turn(void)
{
	float amount = getTurningSpeed() * timerDelta;
	if (angleToTurn < amount)
	{
		amount = angleToTurn;
		angleToTurn = 0.0f;
		stopTurning();
	}
	else
	{
		angleToTurn -= amount;
	}

	if (turning_left)
		direction -= amount;					
	else if (turning_right)
		direction += amount;
}

void MovingObject::move()
{
	float amount = (speed * timerDelta);
	if (movingDistance < amount)
	{
		amount = movingDistance;
		movingDistance = 0.0f;
		stopMoving();
	}
	else
	{
		movingDistance -= amount;
	}

	dx = amount * cos(direction + movingDirection);
	dy = amount * sin(direction + movingDirection);
	x += dx;
	y += dy;
}

void MovingObject::pixelMove(float pixels)
{
	x += pixels * cos(direction + movingDirection);
	y += pixels * sin(direction + movingDirection);
	movingDistance -= pixels;

	if (movingDistance <= 0.0f)
	{
		stopMoving();
	}
}

void MovingObject::setTurningDirection(Direction direction2)
{
	turningDirection = direction2;

	if (direction2 > PI_TIMES_TWO)
		direction2 -= PI_TIMES_TWO;
	if (direction2 < 0.0f)
		direction2 += PI_TIMES_TWO;

	angleToTurn = smallestAngleBetween(getDirection(), direction2);

	if (getDirection() < M_PI)
	{
		if (direction2 > getDirection() && direction2 < (getDirection() + M_PI))
			turning_right = true;
		else
			turning_left = true;
	}
	else
	{
		if (direction2 < getDirection() && direction2 > (getDirection() - M_PI))
			turning_left = true;
		else
			turning_right = true;
	}
	turning = true;
}


Distance CombatObject::MAX_UNIT_DISTANCE_FROM_AREA = 100.0f;
char CombatObject::sideNames[NUMBER_OF_SIDES][64];
int CombatObject::colorIndexes[NUMBER_OF_SIDES];
void  CombatObject::init()
{
	sprintf(sideNames[DEFENDER_SIDE], "DEFENDER");
	sprintf(sideNames[ATTACKER_SIDE], "ATTACKER");
	colorIndexes[DEFENDER_SIDE] = Game::COLOR_TEXT_MESSAGE_DEFENDER_SIDE;
	colorIndexes[ATTACKER_SIDE] = Game::COLOR_TEXT_MESSAGE_ATTACKER_SIDE;
}

void  CombatObject::release()
{

}

CombatObject::CombatObject(void)
//:
// side() //no default side
{
}

void CombatObject::set(GameObject::TYPE type, Location location, Direction direction, SIDE side)
{
	setDerivedClass(getDerivedClass() | (int)COMBAT_OBJECT);
	GameObject::set(type, location, direction);
	setSide(side);
	statisticValue = STATISTIC_VALUE_OTHER;
	is_at_area = false;
}

CombatObject::~CombatObject(void)
{
}

void CombatObject::setStatistics(STATISTIC_VALUE val)
{
	if (this->statisticValue > val)
	{
		game.currentLevel->setStatistics(this, val);
	}
}
