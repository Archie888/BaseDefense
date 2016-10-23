#pragma once

#include "GameObjectcollection.h"
#include <list>
using namespace std;

class AppearanceInfo
{
public:

	int type;
	float time;
	float x;

	bool operator < (const AppearanceInfo & ai)
	{
		return (this->time < ai.time);
	}
};

struct LevelSound
{
	HEFFECT sound;
	HCHANNEL channel;
	bool played;
	float time;
	int volume;
	float pitch;
	bool loop;
	bool random;
	float intervalMin;
	float intervalMax;
};

class Level
{
public:

	enum INTS
	{
		MAX_NUMBER_OF_FOOTSOLDIER_UNITS = 256 //a foot soldier unit is a combat pair of rifleman or a specialist
	};

	//dbg
	Tank * firstAttackerTank;
	FootSoldier * firstAttackerFootSoldier;
	FootSoldier * firstDefenderFootSoldier;
	Tree * firstTree;
	Base * defenderBase;
	int levelNumber;
	char levelName[64];

	//environment
	float wind; //range: 0 - pi/2
	float windDirection;
	bool raining;
	bool night;
	DWORD dustColor;
	float smokeTimer;

	//events
	Timer flareTimer1;
	Timer flareTimer2;
	FootSoldier * defenderFlareShooter;
	FootSoldier * attackerFlareShooter;

	int numberOfTrees;
	//int numberOfBush1s;

	float defenderDefenseLineY;
	float defenderFireStartY;
	bool levelCombatStarted;

	bool randomness;

	//Statistics
#define STAT_0 CombatObject::NUMBER_OF_SIDES
#define STAT_1 GameObject::NUMBER_OF_TYPES
#define STAT_2 GameObject::SUBTYPE_MAX
#define STAT_3 CombatObject::NUMBER_OF_STATISTIC_VALUES
	int statisticsForSides[STAT_0][STAT_1][STAT_2][STAT_3];

	//Tactical information
	//tanks
	int numberOfTanks[CombatObject::NUMBER_OF_SIDES]; //level overall
	int numberOfTanksLeft[CombatObject::NUMBER_OF_SIDES]; //level left
	int numberOfTanksAtArea[CombatObject::NUMBER_OF_SIDES]; //in level now
	//infantry
	int numberOfFootSoldiers[CombatObject::NUMBER_OF_SIDES];
	int numberOfFootSoldiersLeft[CombatObject::NUMBER_OF_SIDES];
	int numberOfFootSoldiersAtArea[CombatObject::NUMBER_OF_SIDES];
	//fire support
	int numberOfFireSupport[CombatObject::NUMBER_OF_SIDES][FireSupport::NUMBER_OF_TYPES];
	int numberOfFireSupportLeft[CombatObject::NUMBER_OF_SIDES][FireSupport::NUMBER_OF_TYPES];

	bool levelFinished;
	float levelPassingTimer;
	static const float LEVEL_PASSING_TIME;

	list<AppearanceInfo> tankAppearances[CombatObject::NUMBER_OF_SIDES];
	list<AppearanceInfo>::iterator nextTank[CombatObject::NUMBER_OF_SIDES];

	list<AppearanceInfo> footSoldierAppearances[CombatObject::NUMBER_OF_SIDES];
	list<AppearanceInfo>::iterator nextFootSoldier[CombatObject::NUMBER_OF_SIDES];

	list<AppearanceInfo> fireSupportAppearances[CombatObject::NUMBER_OF_SIDES][FireSupport::NUMBER_OF_TYPES];
	list<AppearanceInfo>::iterator nextFireSupport[CombatObject::NUMBER_OF_SIDES][FireSupport::NUMBER_OF_TYPES];

	list<AppearanceInfo> smokeAppearances[CombatObject::NUMBER_OF_SIDES];
	list<AppearanceInfo>::iterator nextSmoke[CombatObject::NUMBER_OF_SIDES];

	list<LevelSound> levelSounds;

	static float APPEARANCE_TIME_RANDOMNESS;
	static float TANK_APPEARANCE_X_RANDOMNESS;
	static float INDIRECT_FIRE_STRIKE_START_RANDOMNESS;
	static int NUMBER_OF_FOOT_SOLDIERS_IN_A_GROUP_RANDOMNESS;
	static int NUMBER_OF_CLASS_PER_NUMBER_OF_SOLDIERS[FootSoldier::NUMBER_OF_CLASSES];
	static float COMBAT_PAIR_DISTANCE_RELATION; //The distance between soldiers in a combat pair / unit distance
	static float ATTACKER_FOOT_SOLDIER_START_DIRECTION;
	static float ATTACKER_TANK_START_DIRECTION;
	static float DEFENDER_FOOT_SOLDIER_START_DIRECTION;

	float levelTime;
	float levelMaxTime;
	float scriptCheckTimer;
	int smokeNumber;

	//intro text
	float textSize;
	char textHeader[64];
	char textLocation[64];
	char textSituation[256];
	char textOrders[256];

	//intro music
	HEFFECT introMusic;

	//Background
	char backgroundFileName[30];

	//Player 1 start info
	float player1_start_x;
	float player1_start_y;
	float player1_start_direction;

	//Player 2 start info
	float player2_start_x;
	float player2_start_y;
	float player2_start_direction;

	Level(void);
	Level(char * levelName);
	~Level(void);
	void runScript(GameObjectCollection * objects);
	void checkVictory(void);
	void playerFireSupportCall(GameObjectCollection *, int fireSupportType);

	bool isThereTroopsLeft(CombatObject::SIDE side)
	{ 
		return ((numberOfTanksLeft[side] > 0 || numberOfFootSoldiersLeft[side]) > 0);
	}
	bool isThereTroopsAtArea(CombatObject::SIDE side)
	{ 
		return (getNumberOfTroopsActiveAtArea(side) > 0);
	}
	bool isThereOtherSideTroopsLeft(CombatObject::SIDE side)
	{ 
		side = CombatObject::getOppositeSide(side);
		return ((numberOfTanksLeft[side] + numberOfFootSoldiersLeft[side]) > 0);
	}
	bool isThereOtherSideTroopsAtArea(CombatObject::SIDE side)
	{ 
		side = CombatObject::getOppositeSide(side);
		return (getNumberOfTroopsActiveAtArea(side) > 0);
	}

	int getNumberOfTroopsActiveAtArea(CombatObject::SIDE side)
	{
		return numberOfTanksAtArea[side] + numberOfFootSoldiersAtArea[side];
	}
	int getNumberOfFootSoldiersActiveAtArea(CombatObject::SIDE side)
	{
		return numberOfFootSoldiersAtArea[side];
	}
	int getNumberOfTanksActiveAtArea(CombatObject::SIDE side)
	{
		return numberOfTanksAtArea[side];
	}

	void setStatistics(CombatObject * cObject, CombatObject::STATISTIC_VALUE newVal);

	int getStatistics_numberOfInfantry(CombatObject::SIDE side);
	int getStatistics_numberOfInfantryKilled(CombatObject::SIDE side);
	int getStatistics_numberOfInfantryWounded(CombatObject::SIDE side);
	int getStatistics_numberOfInfantryCaptured(CombatObject::SIDE side);
	int getStatistics_numberOfInfantryFled(CombatObject::SIDE side);
	int getStatistics_getInfantryStatistics(CombatObject::STATISTIC_VALUE val, CombatObject::SIDE side);
	int getStatistics_numberOfTanks(CombatObject::SIDE side);
	int getStatistics_numberOfTanksDestroyed(CombatObject::SIDE side);
	int getStatistics_numberOfFireSupportStrikes(CombatObject::SIDE side);
	int getStatistics_numberOfMortarGrenadesFired(CombatObject::SIDE side);
	int getStatistics_numberOfMissilesFired(CombatObject::SIDE side);
	int getStatistics_numberOfNapalmBombsDropped(CombatObject::SIDE side);

	DWORD getObjectColor(void);
};

class LevelManager
{
public:
	int levelNumber;
	int numberOfLevels;

	LevelManager(void);
	~LevelManager(void);
	Level * getNextLevel(void);
	Level * getLevel(int levelNumber);
	Level * getLevel(char * levelName);
};