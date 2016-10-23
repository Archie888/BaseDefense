#include ".\player.h"
#include "main.h"

HTEXTURE Pointer::p1pointertex = NULL;
HTEXTURE Pointer::p2pointertex = NULL;
hgeAnimation * Pointer::player1PointerImage = NULL;
hgeAnimation * Pointer::player2PointerImage = NULL;
hgeAnimation * Pointer::tankSymbol = NULL;
hgeAnimation * Pointer::turretSymbol = NULL;

void Pointer::init(void)
{
	p1pointertex = hge->Texture_Load("player1_tank.png");
	p2pointertex = hge->Texture_Load("player2_tank.png");

	player1PointerImage = new hgeAnimation(p1pointertex, 1, 0.0f, 9 * 8, 520, 5 * 8, 7 * 8);
	player1PointerImage->SetHotSpot((float)(5 * 8) / 2.0f, (float)(7 * 8) / 2.0f);
	player2PointerImage = new hgeAnimation(p2pointertex, 1, 0.0f, 9 * 8, 520, 5 * 8, 7 * 8);
	player2PointerImage->SetHotSpot((float)(5 * 8) / 2.0f, (float)(7 * 8) / 2.0f);

	tankSymbol = new hgeAnimation(p1pointertex, 1, 0.0f, 16 * 8, 520, 1 * 8, 3 * 8);
	tankSymbol->SetHotSpot(4.0f, 14.0f);
	turretSymbol = new hgeAnimation(p1pointertex, 1, 0.0f, 17 * 8, 520, 1 * 8, 2 * 8);
	turretSymbol->SetHotSpot(4.0f, 10.0f);
}

void Pointer::release(void)
{
	hge->Texture_Free(p1pointertex);
	hge->Texture_Free(p2pointertex);
	delete player1PointerImage; player1PointerImage = NULL;
	delete player2PointerImage; player2PointerImage = NULL;
	delete tankSymbol; tankSymbol = NULL;
	delete turretSymbol; turretSymbol = NULL;
}

const int Player::PLAYER_1 = 1;
const int Player::PLAYER_2 = 2;
float Player::PLAYER_REAPPEARANCE_TIME = 0.0f;
int Player::SCORE_RIFLEMAN = 10;
int Player::SCORE_MACHINE_GUNNER = 20;
int Player::SCORE_AT_SOLDIER = 30;
int Player::SCORE_TANK = 200;

#define PLAYER_REAPPEARANCE_TIME_EASY_DEF 0.0f
#define PLAYER_REAPPEARANCE_TIME_NORMAL_DEF 3.0f
#define PLAYER_REAPPEARANCE_TIME_HARD_DEF 5.0f

#define PLAYER_EXTRA_TANK_SCORE_EASY 500
#define PLAYER_EXTRA_TANK_SCORE_NORMAL 1000
#define PLAYER_EXTRA_TANK_SCORE_HARD 2000

Player::Player(void)
:
tank(NULL)
{
}

Player::~Player(void)
{
	tank = NULL;
}

//Called per player in Game::loadGame()
void Player::init(int id)
{
	game.renderLoadScreen("LOADING PLAYER CLASS");
	this->id = id;

	if (id == PLAYER_1)
		pointer.image = Pointer::player1PointerImage;
	else if (id == PLAYER_2)
		pointer.image = Pointer::player2PointerImage;

	if (id == PLAYER_1)
	{
		input.setKeys("INPUT_PLAYER_1");
	}
	if (id == PLAYER_2)
	{
		input.setKeys("INPUT_PLAYER_2");
	}
}

void Player::initTank(void)
{
	if (id == PLAYER_1)
	{
		tank = new Tank(
			Tank::PLAYER_1_TANK,
			CombatObject::DEFENDER_SIDE,
			Location(
				200.0f,//game.currentLevel->player1_start_x,
				800.0f),//game.currentLevel->player1_start_y),
			ONE_AND_A_HALF_PI);//game.currentLevel->player1_start_direction);
		game.objects.player1_cannonSight = &tank->cannonSight;
		game.objects.player1_MGSight = &tank->MGSight;
	}
	else
	{
		tank = new Tank(
			Tank::PLAYER_2_TANK,
			CombatObject::DEFENDER_SIDE,
			Location(
				600.0f,//game.currentLevel->player2_start_x,
				800.0f),//game.currentLevel->player2_start_y),
			ONE_AND_A_HALF_PI); //game.currentLevel->player2_start_direction);
		game.objects.player2_cannonSight = &tank->cannonSight;
		game.objects.player2_MGSight = &tank->MGSight;
	}
	game.objects.addGameObject(tank);
}
void Player::processPlayer(void)
{
	processInput();

	if (!dead)
	{
		//Process bonuses
		if (bonusTime > 0.0f)
		{
			bonusTime -= timerDelta;

			switch (bonusType)
			{
			case Player::BONUS::FAST_CANNON:
				{
					tank->cannonLoadTimer -= 2.0f * timerDelta;
					break;
				}
			case Player::BONUS::FAST_MG:
				{
					tank->mgDelay -= 2.0f * timerDelta;
					tank->mgHeat = 0.0f;
					break;
				}
			default: break;
			}
		}

		//Bonus lives
		switch (game.skillLevel)
		{
		case Game::SKILL_LEVEL_EASY: 
			{
				if (extraTankScore >= PLAYER_EXTRA_TANK_SCORE_EASY)
				{
					extraTankScore -= PLAYER_EXTRA_TANK_SCORE_EASY;
					lives++;
				}
				break;
			}
		case Game::SKILL_LEVEL_NORMAL:
			{
				if (extraTankScore >= PLAYER_EXTRA_TANK_SCORE_NORMAL)
				{
					extraTankScore -= PLAYER_EXTRA_TANK_SCORE_NORMAL;
					lives++;
				}
				break;
			}
		case Game::SKILL_LEVEL_HARD:
			{
				if (extraTankScore >= PLAYER_EXTRA_TANK_SCORE_HARD)
				{
					extraTankScore -= PLAYER_EXTRA_TANK_SCORE_HARD;
					lives++;
				}
				break;
			}
		default: break;
		}
	}
	else
	{
		if (!gameOver && waiting_for_reappearance)
		{
			reappearanceTimer -= timerDelta;

			if (reappearanceTimer < 0.0f)
			{
				startLife();
			}
		}
	}
}

void Player::processInput(void)
{
	if (!dead && tank)
		input.processInput(tank);
	else if (continue_dialog)
	{
		if (id == PLAYER_1 && hge->Input_GetKeyState(HGEK_SHIFT))
		{
			startContinue();
		}
		else if (id == PLAYER_2 && hge->Input_GetKeyState(HGEK_ENTER))
		{
			startContinue();
		}
	}
}

void Player::renderHUD(void)
{
	hudfont->SetScale(0.5f);
	if (!gameOver)
		hudfont->SetColor(game.colors[Game::COLOR_HUD_TEXT_NORMAL]);
	else
		hudfont->SetColor(game.colors[Game::COLOR_HUD_TEXT_CRITICAL]);

	float x = (id == PLAYER_1)? 10.0f: 705.0f;

	//pointer rendering
	if (tank)
	{
		float tankX = tank->x, tankY = tank->y;
		pointer.x = tankX, pointer.y = tankY;
		bool show_pointer = false;

		if (tankX < 40.0f)
		{
			if (tankX < -10.0f)
				show_pointer = true;
			pointer.x = 40.0f;
		}
		else if (tankX > Game::SCREENWIDTH - 40)
		{
			if (tankX > Game::SCREENWIDTH + 10)
				show_pointer = true;
			pointer.x = Game::SCREENWIDTH - 40;
		}
		if (tankY < 40.0f)
		{
			if (tankY < -10.0f)
				show_pointer = true;
			pointer.y = 40.0f;
		}
		else if (tankY > Game::SCREENHEIGHT - 40)
		{
			if (tankY > Game::SCREENHEIGHT + 10)
				show_pointer = true;
			pointer.y = Game::SCREENHEIGHT - 40.0f;
		}

		pointer.visible = show_pointer;
		if (show_pointer)
		{
			pointer.direction = pointer.objectAngle(tank) - HALF_PI;
		}
	}
	else
		pointer.visible = false;

	if (!gameOver && !continue_dialog)
	{
		hudfont->printf(x, 538.0f, "%s", (id == PLAYER_1)? "PLAYER 1": "PLAYER 2");
		hudfont->printf(x, 549.0f, "Tanks: %i", lives);
		hudfont->printf(x, 560.0f, "score: %i", score);
	}
	else if (continue_dialog)
	{
		if (id == PLAYER_1)
			hudfont->printf(x, 570.0f, "CONTINUE? (PRESS SHIFT)");
		else
			hudfont->printf(x, 570.0f, "CONTINUE? (PRESS ENTER)");
	}
	else if (gameOver)
	{
		hudfont->printf(x, 570.0f, "GAME OVER");
	}
}

void Player::die(void)
{
	finishLife();
}

void Player::startGame(void)
{
	gameOver = false;
	hge->System_SetState(HGE_INIFILE, "Settings.ini");
	continues = hge->Ini_GetInt("PLAYER_PROPERTIES", "CONTINUES", 99);

	score = 0;

	extraTankScore = 0;

	for (int i = 0; i < CombatObject::NUMBER_OF_SIDES; i++)
	{
		for (int j = 0; j < FootSoldier::NUMBER_OF_CLASSES; j++)
		{
	        nDecapasitatedInfantry[i][j] = 0;
		}
	}
	nDecapasitatedTanks[CombatObject::ATTACKER_SIDE] = 0;
	nDecapasitatedTanks[CombatObject::DEFENDER_SIDE] = 0;

	startContinue();
}

void Player::finishGame(void)
{
	gameOver = true;
}

void Player::startLevel(void)
{
	levelScore = 0;
	hge->System_SetState(HGE_INIFILE, "Settings.ini");
	cannonAmmo = hge->Ini_GetInt("PLAYER_PROPERTIES", "CANNON_AMMO", 99);
	MGAmmo = hge->Ini_GetInt("PLAYER_PROPERTIES", "MG_AMMO", 999);
	bonusTime = 0.0f;
	for (int i = 0; i < CombatObject::NUMBER_OF_SIDES; i++)
	{
		for (int j = 0; j < FootSoldier::NUMBER_OF_CLASSES; j++)
		{
	        nDecapasitatedInfantryLevel[i][j] = 0;
		}
	}
	nDecapasitatedTanksLevel[CombatObject::ATTACKER_SIDE] = 0;
	nDecapasitatedTanksLevel[CombatObject::DEFENDER_SIDE] = 0;

	//init tank for level
	{
		dead = false;
		waiting_for_reappearance = false;

		if (id == PLAYER_1)
		{
			tank = new Tank(
				Tank::PLAYER_1_TANK,
				CombatObject::DEFENDER_SIDE,
				Location(
					game.currentLevel->player1_start_x,
					game.currentLevel->player1_start_y),
				game.currentLevel->player1_start_direction);
			game.objects.player1_cannonSight = &tank->cannonSight;
			game.objects.player1_MGSight = &tank->MGSight;
		}
		else
		{
			tank = new Tank(
				Tank::PLAYER_2_TANK,
				CombatObject::DEFENDER_SIDE,
				Location(
				game.currentLevel->player2_start_x,
				game.currentLevel->player2_start_y),
				game.currentLevel->player2_start_direction);
			game.objects.player2_cannonSight = &tank->cannonSight;
			game.objects.player2_MGSight = &tank->MGSight;
		}
		game.objects.addGameObject(tank);
	}
}

void Player::finishLevel(void)
{
	//player tank is deleted in GameObjectCollection.
	tank = NULL;
}

void Player::startContinue(void)
{
	continue_dialog = false;
	hge->System_SetState(HGE_INIFILE, "Settings.ini");
	lives = hge->Ini_GetInt("PLAYER_PROPERTIES", "LIVES", 99);
	startLife();
}

void Player::finishContinue(void)
{
	continues--;
	continue_dialog = true;
}

void Player::startLife(void)
{
	dead = false;
	waiting_for_reappearance = false;
	hge->System_SetState(HGE_INIFILE, "Settings.ini");
	cannonAmmo = hge->Ini_GetInt("PLAYER_PROPERTIES", "CANNON_AMMO", 99);
	MGAmmo = hge->Ini_GetInt("PLAYER_PROPERTIES", "MG_AMMO", 999);
	initTank();
}

void Player::finishLife(void)
{
	dead = true;
	tank = NULL;
	bonusTime = 0.0f;

	if (this->id == PLAYER_1)
	{
		game.objects.player1_cannonSight = NULL;
		game.objects.player1_MGSight = NULL;
	}
	if (this->id == PLAYER_2)
	{
		game.objects.player2_cannonSight = NULL;
		game.objects.player2_MGSight = NULL;
	}
	//lives left
	if (lives > 0)
	{
		lives--;
		
		//reappearance:
		waiting_for_reappearance = true;

		switch (game.skillLevel)
		{
		case Game::SKILL_LEVEL_EASY: reappearanceTimer = PLAYER_REAPPEARANCE_TIME_EASY_DEF; break;
		case Game::SKILL_LEVEL_NORMAL: reappearanceTimer = PLAYER_REAPPEARANCE_TIME_NORMAL_DEF; break;
		case Game::SKILL_LEVEL_HARD: reappearanceTimer = PLAYER_REAPPEARANCE_TIME_HARD_DEF; break;
		default: reappearanceTimer = 0.0f;
		}
	}
	else
	{
		if (continues > 0)
		{
			finishContinue();
		}
		else
			gameOver = true;
	}
}

void Player::addScore(GameObject * object, bool positive)
{
	if (object->isCombatObject())
	{
		CombatObject * co = static_cast<CombatObject *>(object);

		if (co->side == CombatObject::ATTACKER_SIDE)
		{
			float bonus = randFloat(0.0f, 1.0f);

			//Weapon bonus?
			if (bonusTime <= 0.0f)
			{
				if (bonus < 0.1f)
				{
					//fast cannon!
					if (bonus < 0.05f && bonus > 0.03f)
					{
						if (id == PLAYER_1)
							game.showMessage("PLAYER 1 BONUS: FAST CANNON!", Game::COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 0.0f);
						else
							game.showMessage("PLAYER 2 BONUS: FAST CANNON!", Game::COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 0.0f);
						bonusTime = 15.0f;
						bonusType = BONUS::FAST_CANNON;
					}
					//fast mg!
					else
					{
						if (id == PLAYER_1)
							game.showMessage("PLAYER 1 BONUS: FAST MACHINE GUN!", Game::COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 0.0f);
						else
							game.showMessage("PLAYER 2 BONUS: FAST MACHINE GUN!", Game::COLOR_TEXT_MESSAGE_DEFENDER_SIDE, 0.0f);
						bonusTime = 10.0f;
						bonusType = BONUS::FAST_MG;
					}
				}
			}

			//Fire support bonus?
			if (bonus < 0.03f)
			{
				if (bonus < 0.02f)
				{
					if (bonus < 0.01f)
					{
						game.showMessage("BONUS: NAPALM STRIKE!", Game::COLOR_TEXT_MESSAGE_DEFENDER_SIDE);
						game.currentLevel->numberOfFireSupport[CombatObject::DEFENDER_SIDE][FireSupport::NAPALM_STRIKE]++;
						game.currentLevel->numberOfFireSupportLeft[CombatObject::DEFENDER_SIDE][FireSupport::NAPALM_STRIKE]++;			
					}
					//missile strike bonus!
					else
					{
						game.showMessage("BONUS: MISSILE STRIKE!", Game::COLOR_TEXT_MESSAGE_DEFENDER_SIDE);
						game.currentLevel->numberOfFireSupport[CombatObject::DEFENDER_SIDE][FireSupport::MISSILE_STRIKE]++;
						game.currentLevel->numberOfFireSupportLeft[CombatObject::DEFENDER_SIDE][FireSupport::MISSILE_STRIKE]++;			
					}
				}
				//mortar strike bonus!
				else
				{
					game.showMessage("BONUS: MORTAR STRIKE!", Game::COLOR_TEXT_MESSAGE_DEFENDER_SIDE);
					game.currentLevel->numberOfFireSupport[CombatObject::DEFENDER_SIDE][FireSupport::MORTAR_STRIKE]++;
					game.currentLevel->numberOfFireSupportLeft[CombatObject::DEFENDER_SIDE][FireSupport::MORTAR_STRIKE]++;			
				}
			}
		}
	}

	switch (object->type)
	{
	case GameObject::FOOT_SOLDIER:
		{
			FootSoldier * fs = static_cast<FootSoldier *>(object);

			INFANTRY_SCORES type;

			switch (fs->footSoldierClass)
			{
			case FootSoldier::RIFLEMAN: type = INFANTRY_SCORES::RIFLEMAN; break;
			case FootSoldier::MACHINE_GUNNER: type = INFANTRY_SCORES::MACHINE_GUNNER; break;
			case FootSoldier::AT_SOLDIER: type = INFANTRY_SCORES::AT_SOLDIER; break;
			case FootSoldier::MEDIC: type = INFANTRY_SCORES::MEDIC; break;
			}

			if (fs->captured == true)
			{
				if (positive)
					type = INFANTRY_SCORES::CAPTIVE_TAKEN;
				else
					type = INFANTRY_SCORES::CAPTIVE_KILLED;
			}

			score += game.scores[fs->side][fs->type][type];
			levelScore += game.scores[fs->side][fs->type][type];
			extraTankScore += game.scores[fs->side][fs->type][type];
			nDecapasitatedInfantry[fs->side][type]++;
			nDecapasitatedInfantryLevel[fs->side][type]++;
			break;
		}
	case GameObject::TANK:
		{
			Tank * tank = static_cast<Tank *>(object);
			score += game.scores[tank->side][tank->type][0];
			levelScore += game.scores[tank->side][tank->type][0];
			extraTankScore += game.scores[tank->side][tank->type][0];
			nDecapasitatedTanks[tank->side]++;
			nDecapasitatedTanksLevel[tank->side]++;
			break;
		}
	default: break;
	}
}

int Player::getNDecapasitatedInfantry(CombatObject::SIDE side)
{
	int sum = 0;
	for (int i = 0; i < FootSoldier::NUMBER_OF_CLASSES; i++)
	{
		sum += nDecapasitatedInfantry[side][i];
	}
	return sum;
}

int Player::getNDecapasitatedInfantryLevel(CombatObject::SIDE side)
{
	int sum = 0;
	for (int i = 0; i < FootSoldier::NUMBER_OF_CLASSES; i++)
	{
		sum += nDecapasitatedInfantryLevel[side][i];
	}
	return sum;
}