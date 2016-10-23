#include ".\combataiobject.h"
#include "main.h"

/*
Adding a CombatAIObject to the game:
-If object is "abstract", set start location to Location(FLT_MAX, FLT_MAX) to avoid taking object into consideration.
*/

Direction CombatAIObject::BATTLE_MAIN_DIRECTION_DEFENDER = ONE_AND_A_HALF_PI + 0.00001f;
Direction CombatAIObject::BATTLE_MAIN_DIRECTION_ATTACKER = HALF_PI - 0.00001f;

//smoke:
#define OFF_SMOKE_DISTANCE 200.0f

CombatAIObject::CombatAIObject(void)
:
 active(true)
,suggestedEnemyTarget(NULL)
,enemyTarget(NULL)
,nearestEnemy(NULL)
,firingTargetLocation(0.0f, 0.0f)
,orders(0)
,nearestSuitableEnemyDistance(0.0f)
,nearestEnemyDistance(0.0f)
,engaged(false)
,fleeing(false)
,captured(false)
,observingTime(0.0f)
,supporting_unit(false)
,danger(false)
,numberOfDangerousEnemyUnits(0)
{
}

void CombatAIObject::set(GameObject::TYPE type, Location location, Direction direction, SIDE side)
{
	setDerivedClass(getDerivedClass() | (int)COMBAT_AI_OBJECT);
	CombatObject::set(type, location, direction, side);
	enemyTarget = NULL;
	observingTime = 0.0f;
	engaged = false;
	fleeing = false;
	surrendered = false;
	captured = false;

	//set state memory array
	{
		setStateVariable(-1);
		statePhase = -1;
		for (int i = 0; i < STATE_MEMORY_SIZE; i++)
		{
			previousStates[i][0] = -1;
			previousStates[i][1] = -1;
		}
		statePhaseTimer = 0.0f;
		first_time_in_state_phase = true;
	}
}

CombatAIObject::~CombatAIObject(void)
{
}

void CombatAIObject::processAction(void)
{

}

void CombatAIObject::processAI(void)
{
	if (stateChangeTimer > 0.0f)
	{
		stateChangeTimer -= timerDelta;

		if (stateChangeTimer <= 0.0f)
			setState(nextState);
	}
}

Direction CombatAIObject::getBattleMainDirection(void)
{
	if (side == DEFENDER_SIDE)
		return BATTLE_MAIN_DIRECTION_DEFENDER;
	else
		return BATTLE_MAIN_DIRECTION_ATTACKER;
}

void CombatAIObject::attack(void)
{
}

void CombatAIObject::defend(void)
{
}

void CombatAIObject::fight(void)
{
}
void CombatAIObject::fire(void)
{
}

void CombatAIObject::closeCombat(void)
{
}

void CombatAIObject::setState(int newState, Time stateChangeTime, int startStatePhase, bool nextPhaseReturn)
{
	if (stateChangeTime > 0.0f)
	{
		stateChangeTimer = stateChangeTime;
		nextState = newState;
		return;
	}
	else
		nextState = -1;

	first_time_in_state_phase = true;

	if (newState == getState())
	{
		setStatePhase(startStatePhase);
		return;
	}
	else if (getState() == -1)
	{
		setStateVariable(newState);
		statePhase = startStatePhase;
		return;
	}

	int i = 0;
	for (i = 0; i < STATE_MEMORY_SIZE; i++)
	{
		if (previousStates[i][0] == -1)
		{
			previousStates[i][0] = getState();
			previousStates[i][1] = statePhase + ((nextPhaseReturn)?1:0);

			setStateVariable(newState);
			statePhase = startStatePhase;
			return;
		}
	}

	for (i = 0; i < STATE_MEMORY_SIZE - 1; i++)
	{
		previousStates[i][0] = previousStates[i + 1][0];
		previousStates[i][1] = previousStates[i + 1][1];
	}
	previousStates[i][0] = getState();
	previousStates[i][1] = statePhase;

	setStateVariable(newState);
	statePhase = startStatePhase;
}

void CombatAIObject::setStatePhase(int statePhase)
{
	this->statePhase = statePhase;
	first_time_in_state_phase = true;
}

void CombatAIObject::nextStatePhase(void)
{
	statePhase++;
	first_time_in_state_phase = true;
}

void CombatAIObject::previousState(void)
{
	for (int i = STATE_MEMORY_SIZE - 1; i >= 0; i--)
	{
		if (previousStates[i][0] != -1)
		{
			setStateVariable(previousStates[i][0]);
			statePhase = previousStates[i][1];
			first_time_in_state_phase = true;
			previousStates[i][0] = -1;
			previousStates[i][1] = -1;
			return;
		}
	}
	//If no more states in memory, do as ordered.
	setState(orders);
}

void CombatAIObject::setOriginalState(void)
{
	for (int i = STATE_MEMORY_SIZE - 1; i >= 0; i--)
	{
		previousStates[i][0] = -1;
		previousStates[i][1] = -1;
	}
	setState(orders);
}

bool CombatAIObject::hasEnemyTargetCrossedFireStartLevel(void)
{
	if (isEnemyTarget()) 
		return (getEnemyTargetLocation().y 
			> 
			game.currentLevel->defenderFireStartY); 
	else return false; 
}

bool CombatAIObject::isAtBattleArea(void)
{
	if (side == DEFENDER_SIDE)
	{
		return x >= -50.0f && x <= 850.0f && y >= -50.0f && y <= 650.0f;		
	}
	else
		return x >= 0.0f && x <= 800.0f && y >= 20.0f && y <= 600.0f;
}

void CombatAIObject::setEnemyTarget(GameObject * object)
{
	if (enemyTarget)
	{
		//enemy target moved to killList, or other enemy closer
		enemyTarget->removeReference();
	}
	enemyTarget = object;
	if (object)
	{
		object->addReference();
		engaged = true;
	}
	else
		engaged = false;
}

void CombatAIObject::startCheckingObjects(void)
{
	suggestedEnemyTarget = NULL;
	nearestSuitableEnemyDistance = FLT_MAX;
	nearestEnemyDistance = FLT_MAX;
}

void CombatAIObject::checkObject(GameObject * otherObject)
{

	if (!game.currentLevel->levelCombatStarted)
	{
		if (otherObject->isCombatAIObject())
		{
			CombatAIObject * caio = static_cast<CombatAIObject *>(otherObject);

			if (caio->side != side)
			{
 				eDist = compareDistance(caio, getAutomaticNotificationDistance());
				if (compareDistance(caio, getAutomaticNotificationDistance()) < 0.0f)
				{
					game.currentLevel->levelCombatStarted = true;
				}
			}
		}
	}
	if (otherObject->isCombatAIObject())
	{
		if (static_cast<CombatObject *>(otherObject)->side != side)
		{
			if (otherObject->type == GameObject::MILITARY_UNIT)
				int u = 2;

			Distance sqDist = squaredObjectDistance(otherObject);
			if (sqDist < nearestEnemyDistance)
			{
				nearestEnemy = otherObject;
				nearestEnemyDistance = sqDist;
			}
		}
	}
}

void CombatAIObject::stopCheckingObjects(void)
{
	if (!engaged)
		setEnemyTarget(suggestedEnemyTarget);
}