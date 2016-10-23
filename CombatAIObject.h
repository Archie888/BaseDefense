#pragma once
#include "combatobject.h"

typedef bool AIFunction;

class CombatAIObject :
	public CombatObject
{
public:
	bool active;
	int orders;
	GameObject * suggestedEnemyTarget;
	GameObject * enemyTarget;
	GameObject * nearestEnemy;
	bool engaged;
	bool fleeing;
	bool surrendered;
	bool captured;
	Location firingTargetLocation;
	Location targetLocation; //The location this unit is trying to seize and/or move to.
	Distance nearestSuitableEnemyDistance;
	Distance nearestEnemyDistance;
	bool supporting_unit;
	bool danger; //The enemy has unit(s) capable of incapasitating this unit at area.
	int numberOfDangerousEnemyUnits;

	Distance enemyTargetDistance;
	Direction enemyTargetDirection;

	//states:
#define STATE_MEMORY_SIZE 10
	int statePhase;
	int previousStates[STATE_MEMORY_SIZE][2];
	Timer statePhaseTimer;
	bool first_time_in_state_phase;
	int nextState;
	Timer stateChangeTimer;

	//Observation
	Time observingTime;

	static float BATTLE_MAIN_DIRECTION_DEFENDER;
	static float BATTLE_MAIN_DIRECTION_ATTACKER;

	CombatAIObject(void);
	void set(GameObject::TYPE type, Location location, Direction direction, SIDE side);
	virtual ~CombatAIObject(void);
	virtual void processAction(void);
	virtual void processAI(void);
	void startCheckingObjects(void);
	void checkObject(GameObject * otherObject);
	void stopCheckingObjects(void);

	//AI functions
	virtual void attack();
	virtual void defend();
	//virtual void retreat();
	virtual void fight(void);
	virtual void fire(void);
	virtual void closeCombat(void);
	virtual void surrender(void) {}
	virtual void flee(void) {}
	virtual void setSurrendering(void) {}
	virtual void setFleeing(void) {}

	//states
	virtual int getState(void) { return -1; } //should be implemented
	virtual void setStateVariable(int state) {} //should be implemented
	virtual void setState(int newState, Time stateChangeTimer = 0.0f, int startStatePhase = 0, bool nextPhaseReturn = true);
	virtual void setStatePhase(int statePhase);
	virtual void nextStatePhase(void);
	virtual void previousState(void);
	virtual void setOriginalState(void);

	//Combat
	Direction getBattleMainDirection(void);
	Direction getSafeDirection(void) { return getBattleMainDirection() + M_PI; }
	virtual Distance getCloseCombatRange(void) { return 0.0f; }
	virtual GameObject * getNearestEnemy(void) { return NULL; }
	void setEnemyTarget(GameObject * object);
	GameObject * getEnemyTarget(void) { return enemyTarget; }
	bool isEnemyTarget(void) { return getEnemyTarget() != NULL; }
	Location getEnemyTargetLocation(void) { if (isEnemyTarget()) return getEnemyTarget()->getLocation(); return Location(); }
	Direction getEnemyTargetDirection(void) { return enemyTargetDirection; }
	Distance getEnemyTargetDistance(void) { return enemyTargetDistance; }
	GameObject::TYPE getEnemyTargetType(void) { if (isEnemyTarget()) return getEnemyTarget()->type; }
	Distance getNearestEnemyDistance(void) { if (nearestEnemy) return squaredObjectDistance(nearestEnemy); else return 3.402823466e+38F; } //FLT_MAX }
	bool hasEnemyTargetCrossedFireStartLevel(void);
	Location getTargetLocation(void) { return targetLocation; }
	void setTargetLocation(Location location) { this->targetLocation = location; }
	virtual bool isNeutralized(void) { return isDestroyed(); }
	virtual bool isActive(void) { return active; }
	virtual void setActive(bool active) { this->active = active; }
	virtual Distance getAutomaticNotificationDistance(void) { return 0.0f; } //Game::BIG_DISTANCE; }
	virtual bool isGoodToFire(void) { return true; }
	virtual bool isAtBattleArea(void);

	//Communication
	virtual void sendMessage(int msg, CombatAIObject * receiver) { receiver->receiveMessage(msg, this); }
	virtual void receiveMessage(int msg, CombatAIObject * sender) {}

	//properties:
	void setEnemyTargetProperties(void) 
	{ 
		if (getEnemyTarget()) 
		{ 
			enemyTargetDistance = squaredObjectDistance(getEnemyTarget()); 
			enemyTargetDirection = objectAngle(getEnemyTarget()); 
		}
		else
		{
			enemyTargetDistance = 3.402823466e+38F;
		}
	}

	//debug:
	float eDist;
};