#pragma once
#include "tank.h"
#include "footsoldier.h"
#include "input.h"

class Pointer : public GameObject
{
public:
	static HTEXTURE p1pointertex;
	static HTEXTURE p2pointertex;
	static hgeAnimation * player1PointerImage;
	static hgeAnimation * player2PointerImage;
	static hgeAnimation * tankSymbol;
	static hgeAnimation * turretSymbol;

	static void init(void);
	static void release(void);
	Pointer(void) { setType(GameObject::POINTER); }
};

class Player
{
public:
	Input input;
	Tank * tank;

	int id;
	static const int PLAYER_1;
	static const int PLAYER_2;
	static float PLAYER_REAPPEARANCE_TIME;

	int lives;
	int continues;
	int cannonAmmo;
	int MGAmmo;

	bool dead;
	bool waiting_for_reappearance;
	float reappearanceTimer;
	bool continue_dialog;
	bool gameOver;
	
	Pointer pointer;

	enum INFANTRY_SCORES
	{
		RIFLEMAN,
		MACHINE_GUNNER,
		AT_SOLDIER,
		CAPTIVE_TAKEN,
		MEDIC,
		CAPTIVE_KILLED,
		FLEEING_SOLDIER,
		NUMBER_OF_INFANTRY_SCORES
	};

	enum BONUS
	{
		FAST_CANNON,
		FAST_MG
	} bonusType;

	//score etc
	int score;
	int levelScore;
	int extraTankScore;
	int nDecapasitatedInfantry[CombatObject::NUMBER_OF_SIDES][NUMBER_OF_INFANTRY_SCORES];
	int nDecapasitatedTanks[CombatObject::NUMBER_OF_SIDES];
	int nDecapasitatedInfantryLevel[CombatObject::NUMBER_OF_SIDES][NUMBER_OF_INFANTRY_SCORES];
	int nDecapasitatedTanksLevel[CombatObject::NUMBER_OF_SIDES];
	//bonus
	Time bonusTime;

	static int SCORE_RIFLEMAN;
	static int SCORE_MACHINE_GUNNER;
	static int SCORE_AT_SOLDIER;
	static int SCORE_TANK;
	static float BONUS_TIME_FAST_CANNON;
	static float BONUS_TIME_FAST_MG;

	Player(void);
	~Player(void);
	void init(int id);
	void initTank(void);
	void processPlayer(void);
	void processInput(void);
	void renderHUD(void);
	void die(void);
	void startGame(void);
	void finishGame(void);
	void startLevel(void);
	void finishLevel(void);
	void startContinue(void);
	void finishContinue(void);
	void startLife(void);
	void finishLife(void);
	void addScore(GameObject * object, bool positive = true);

	int getNDecapasitatedInfantry(CombatObject::SIDE side);
	int getNDecapasitatedInfantryLevel(CombatObject::SIDE side);
};