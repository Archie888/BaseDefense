#include ".\level.h"
#include "main.h"

//ATTACKER TROOPS:
//Rules of random appearance: 
//TANKS:
//-appear from a slightly random horizontal level
//-appear at a slightly random time
//INDIRECT_FIRE_STRIKES
//-start at a slightly random time
//FOOT SOLDIERS:
//-appear in groups
//-a group appears at a slightly random time
//-a group attacks as a line
//-in a group, a unit is either a specialist or a combat pair of regular riflemen.
//-unit placing in a group is always random.
//-in the groupmembercount, there is randomness of -2 to +2
//-in a group, there is from 1 to (groupsize / 4) at soldiers
//-in a group, there is from 1 to (groupsize / 6) machine gunners

//DEFENDER TROOPS:
//FOOT SOLDIERS:
//-a group appears at a given time
//-usually two groups appear, in either side of the base.
//-same randomity as with attacker groups.

float Level::APPEARANCE_TIME_RANDOMNESS = 10.0f;
float Level::TANK_APPEARANCE_X_RANDOMNESS = 50.0f;
float Level::INDIRECT_FIRE_STRIKE_START_RANDOMNESS = 5.0f;
int Level::NUMBER_OF_FOOT_SOLDIERS_IN_A_GROUP_RANDOMNESS = 2;
int Level::NUMBER_OF_CLASS_PER_NUMBER_OF_SOLDIERS[];
float Level::COMBAT_PAIR_DISTANCE_RELATION = 0.5f;
#define ATTACKER_FOOT_SOLDIER_APPEARANCE_Y -50
float Level::ATTACKER_FOOT_SOLDIER_START_DIRECTION = M_PI / 2.0f + 0.001f;
#define ATTACKER_TANK_APPEARANCE_Y -200.0f
float Level::ATTACKER_TANK_START_DIRECTION = M_PI / 2.0f + 0.001f;
#define DEFENDER_FOOT_SOLDIER_APPEARANCE_Y 620.0f
float Level::DEFENDER_FOOT_SOLDIER_START_DIRECTION = M_PI * 1.5f + 0.001f;

#define SCRIPT_CHECK_TIME 0.5f

//Set enemy infantry troops to come 30 seconds after smoke is initiated.
//Tanks should be held 40 seconds.
//There will be time, when nobody is picking targets in checkObject -routines because of the smoke.
#define SMOKE_TIME 70.0f

const float LEVEL_PASSING_TIME = 3.0f; //when final enemy is destroyed, 3 sec to level end

Level::Level(void)
{
}

Level::Level(char * levelName)
:firstTree(NULL),
introMusic(NULL),
defenderFlareShooter(NULL),
attackerFlareShooter(NULL),
defenderBase(NULL)
{
	int def = CombatObject::DEFENDER_SIDE;
	int att = CombatObject::ATTACKER_SIDE;

	//background
	{
		hge->System_SetState(HGE_INIFILE, "Backgounds.ini");
		strcpy(backgroundFileName, hge->Ini_GetString(levelName, "BACKGROUND_FILENAME", "NO_BG_FILENAME"));

		int numberOf = 0;
		char numberOfObjectString[52], xString[33], yString[33], sString[33], *name;
		for (int i = 0; i < BackgroundObject::BACKGROUNDOBJECT_TYPE::BG_MAX; i++)
		{
			name = BackgroundObject::names[i];
			sprintf(numberOfObjectString, "NUMBER_OF_%s", name);
			numberOf = hge->Ini_GetInt(levelName, numberOfObjectString, 0);
			
			for (int j = 1; j <= numberOf; j++)
			{
				sprintf(xString, "%s_%i_X", name, j);
				sprintf(yString, "%s_%i_Y", name, j);
				sprintf(sString, "%s_%i_SIZE", name, j);

				float x = hge->Ini_GetFloat(levelName, xString, 0.0f);
				float y = hge->Ini_GetFloat(levelName, yString, 0.0f);
				float s = hge->Ini_GetFloat(levelName, sString, 0.0f);

				game.objects.addGameObject(game.backgroundObjectFactory.makeBackgroundObject(BackgroundObject::BACKGROUNDOBJECT_TYPE(i), Location(x, y), (Size)s)); 
			}
		}
	}

	//weather
	{
		hge->System_SetState(HGE_INIFILE, "Levels.ini");
		raining = (bool)(hge->Ini_GetInt(levelName, "RAIN", 0));

		if (raining)
		{
			game.objects.addGameObject(new Effect(Effect::RAIN, Location(), 0.0f), 4);
		}
	}

	//time of day
	{
		hge->System_SetState(HGE_INIFILE, "Levels.ini");
		night = (bool)(hge->Ini_GetInt(levelName, "NIGHT", 0));

		if (night)
		{
			game.animations[Game::ANIMATION_SHADOW]->SetColor(0xffffffff, 0);
			game.animations[Game::ANIMATION_SHADOW]->SetColor(0, 1);
			game.animations[Game::ANIMATION_SHADOW]->SetColor(0, 2);
			game.animations[Game::ANIMATION_SHADOW]->SetColor(0xffffffff, 3);
		}
		else
		{
			game.animations[Game::ANIMATION_SHADOW]->SetColor(0x22ffffff);
		}
	}
	//events
	{
		if (night)
		{
			flareTimer1 = randFloat(5.0f, 7.0f);
			flareTimer2 = randFloat(20.0f, 25.0f);
			defenderFlareShooter = new Rifleman(CombatObject::DEFENDER_SIDE, Location(400.0f, 800.0f), ONE_AND_A_HALF_PI);
			attackerFlareShooter = new Rifleman(CombatObject::ATTACKER_SIDE, Location(400.0f, -200.0f), HALF_PI);
		}


	}

	//dbg:
	firstAttackerTank = NULL;
	firstAttackerFootSoldier = NULL;
	firstDefenderFootSoldier = NULL;

	//level setup
	{
		sprintf(this->levelName, levelName);
		levelFinished = false;
		levelPassingTimer = 3.0f;
		levelTime = 0.00000000f;
		scriptCheckTimer = 0.0f;

		hge->System_SetState(HGE_INIFILE, "Levels.ini");

		levelNumber = hge->Ini_GetInt(levelName, "LEVEL_NUMBER", 0);		
		strcpy(textHeader, hge->Ini_GetString(levelName, "START_INTRO_TEXT_HEADER", "NO_HEADER"));
		textSize = hge->Ini_GetFloat(levelName, "START_INTRO_TEXT_SIZE", 1.0f);
		strcpy(textLocation, hge->Ini_GetString(levelName, "START_INTRO_TEXT_LOCATION", "Unknown"));
		strcpy(textSituation, hge->Ini_GetString(levelName, "START_INTRO_TEXT_SITUATION", "NO_SITUATION"));
		strcpy(textOrders, hge->Ini_GetString(levelName, "START_INTRO_TEXT_ORDERS", "NO_ORDERS"));	
		introMusic = hge->Effect_Load(hge->Ini_GetString(levelName, "INTRO_MUSIC_FILENAME", NULL));
		levelTime = hge->Ini_GetFloat(levelName, "LEVEL_TIME_CUR", 0.00000000f);
		levelMaxTime = hge->Ini_GetFloat(levelName, "LEVEL_TIME", 200.0f);
		char * mj = hge->Ini_GetString(levelName, "RANDOMNESS_IN_ATTACKER_TROOP_APPEARANCE", "false");
		randomness = (strcmp(mj, "true") == 0);
		defenderDefenseLineY = hge->Ini_GetFloat(levelName, "DEFENDER_DEFENSE_LINE_Y", game.SCREENHEIGHT);
		defenderFireStartY = hge->Ini_GetFloat(levelName, "DEFENDER_FIRE_START_Y", game.SCREENHEIGHT);
		levelCombatStarted = false;
		player1_start_direction = hge->Ini_GetFloat(levelName, "PLAYER_1_START_DIRECTION", 0.0f);
		player1_start_x = hge->Ini_GetFloat(levelName, "PLAYER_1_START_POSITION_X", 0.0f);
		player1_start_y = hge->Ini_GetFloat(levelName, "PLAYER_1_START_POSITION_Y", 0.0f);
		player2_start_direction = hge->Ini_GetFloat(levelName, "PLAYER_2_START_DIRECTION", 0.0f);
		player2_start_x = hge->Ini_GetFloat(levelName, "PLAYER_2_START_POSITION_X", 0.0f);
		player2_start_y = hge->Ini_GetFloat(levelName, "PLAYER_2_START_POSITION_Y", 0.0f);

		//amounts of soldiers of classes per numbers of soldiers:
		char text[254];
		for (int i = 0; i < FootSoldier::NUMBER_OF_CLASSES; i++)
		{	
			sprintf(text, "ONE_%s_IN_A_GROUP_PER_NUMBER_OF_SOLDIERS", FootSoldier::classNameStrings[i]);
			NUMBER_OF_CLASS_PER_NUMBER_OF_SOLDIERS[i] = hge->Ini_GetInt("GENERAL", text, 0);
		}
		if (game.skillLevel == Game::SKILL_LEVEL_HARD)
			NUMBER_OF_CLASS_PER_NUMBER_OF_SOLDIERS[FootSoldier::AT_SOLDIER] /= 1.3f;

		BYTE a = hge->Ini_GetInt(levelName, "DUST_COLOR_A", 255);
		BYTE r = hge->Ini_GetInt(levelName, "DUST_COLOR_R", 255);
		BYTE g = hge->Ini_GetInt(levelName, "DUST_COLOR_G", 200);
		BYTE b = hge->Ini_GetInt(levelName, "DUST_COLOR_B", 100);

		dustColor = getColor(a, r, g, b);
	}

	//filling up appearanceinfo lists
	{
		char * sideNames[2] = {"DEFENDER", "ATTACKER"};
		char text[128];

		for (int side = 0; side < 2; side++)
		{
			float troopMultiplier = (game.two_player_game)? 1.0f : 0.7f;
			troopMultiplier = (side == 0)? 1.0f : troopMultiplier;
			troopMultiplier *= (side == 1 && game.skillLevel == Game::SKILL_LEVEL_HARD)? 1.5f : 1.0f;

			//fire support
			{
				char * suppNames[3] = {"MORTAR", "MISSILE", "NAPALM"};

				for (int type = 0; type < 3; type++)
				//Filling up mortar strike AppearanceInfo list:
				{
					numberOfFireSupportLeft[side][type] = 0;
					sprintf(text, "NUMBER_OF_%s_%s_STRIKES", sideNames[side], suppNames[type]);
					numberOfFireSupport[side][type] = hge->Ini_GetInt(levelName, text, 0);

					for (int i = 1; i <= numberOfFireSupport[side][type]; i++)
					{
						sprintf(text, "%s_%s_STRIKE_%i_TIME", sideNames[side], suppNames[type], i);

						AppearanceInfo etai;
						etai.type = 0;
						etai.time = hge->Ini_GetFloat(levelName, text, 0.0f);
						if (randomness) etai.time = INDIRECT_FIRE_STRIKE_START_RANDOMNESS * randFloat(-1.0f, 1.0f);
						etai.x = 0.0f;
						fireSupportAppearances[side][type].push_back(etai);
					}
					nextFireSupport[side][type] = fireSupportAppearances[side][type].begin();
				}
			}
			//Foot soldier appearances:
			{
				//Filling up 'attackerFootSoldierAppearances' and
				//'defenderFootSoldierAppearances'

				//numbers of foot soldier groups for each side:
				int numbersOfFootSoldierGroups[2] = 
				{
					hge->Ini_GetInt(levelName, "NUMBER_OF_DEFENDER_FOOT_SOLDIER_GROUPS", 0),
					hge->Ini_GetInt(levelName, "NUMBER_OF_ATTACKER_FOOT_SOLDIER_GROUPS", 0)
				};

				numberOfFootSoldiers[side] = 0;
				numberOfFootSoldiersLeft[side] = 0;

				for (int i = 1; i <= numbersOfFootSoldierGroups[side]; i++)
				{
					char amountString[64];
					char timeString[63];
					sprintf(amountString, "%s_FOOT_SOLDIER_GROUP_%i_NUMBER_OF", sideNames[side], i);
					sprintf(timeString, "%s_FOOT_SOLDIER_GROUP_%i_APPEARANCE_TIME", sideNames[side], i);

					int numberInGroup = hge->Ini_GetInt(levelName, amountString, 0);
					if (randomness) numberInGroup += randInt(-NUMBER_OF_FOOT_SOLDIERS_IN_A_GROUP_RANDOMNESS, NUMBER_OF_FOOT_SOLDIERS_IN_A_GROUP_RANDOMNESS);
					if (numberInGroup < 0)
						numberInGroup = 0;
					numberInGroup = numberInGroup * troopMultiplier;
					numberOfFootSoldiers[side] += numberInGroup;
					numberOfFootSoldiersLeft[side] += numberInGroup;
					float appearanceTime = hge->Ini_GetFloat(levelName, timeString, 0.0f);
					if (randomness) appearanceTime = APPEARANCE_TIME_RANDOMNESS * randFloat(-1.0f, 1.0f);

					//different foot soldier classes:
					int numberOfClass[FootSoldier::NUMBER_OF_CLASSES];
					
					numberOfClass[FootSoldier::RIFLEMAN] = numberInGroup;
					int nonRiflemanUnitCount = 0;
					for (int j = 0; j < FootSoldier::NUMBER_OF_CLASSES; j++)
					{
						if (j != FootSoldier::RIFLEMAN)
						{
							numberOfClass[j] = numberInGroup / NUMBER_OF_CLASS_PER_NUMBER_OF_SOLDIERS[j];
							if (randomness)
								numberOfClass[j] = randInt(1, numberOfClass[j]);
							numberOfClass[FootSoldier::RIFLEMAN] -= numberOfClass[j];
							nonRiflemanUnitCount += numberOfClass[j];
						}
					}
			
					bool singleRifleman = (numberOfClass[FootSoldier::RIFLEMAN] % 2 == 1);//? : ;numberOfClass[FootSoldier::RIFLEMAN] > numberOfRiflemanCombatPairs * 2;
					int numberOfRiflemanCombatPairs = numberOfClass[FootSoldier::RIFLEMAN] / 2;
					numberOfClass[FootSoldier::RIFLEMAN] = numberOfRiflemanCombatPairs;					

					int TOTAL_UNITS = nonRiflemanUnitCount + numberOfRiflemanCombatPairs;
					int totalUnits = TOTAL_UNITS;

					int unitClasses[MAX_NUMBER_OF_FOOTSOLDIER_UNITS];
					float propabilitiesOfUnits[FootSoldier::NUMBER_OF_CLASSES];

					for (int j = 0; j < TOTAL_UNITS; j++)
					{
						int DEBUG_unitsum = 0;
						float DEBUG_propabilitySum = 0.0f;
						for (int k = 0; k < FootSoldier::NUMBER_OF_CLASSES; k++)
						{
							propabilitiesOfUnits[k] = (float)numberOfClass[k] / (float)totalUnits;
							DEBUG_unitsum += numberOfClass[k];
							DEBUG_propabilitySum += propabilitiesOfUnits[k];
						}
						float propabilitySum = 0;

						float random = randFloat(0.0f, 1.0f);

						for (int k = 0; k  < FootSoldier::NUMBER_OF_CLASSES; k++)
						{
							float propabilityOfThisUnit = propabilitySum + propabilitiesOfUnits[k];
							if (random <= propabilityOfThisUnit)
							{
								unitClasses[j] = k;
								numberOfClass[k]--;
								break;
							}

							propabilitySum += propabilitiesOfUnits[k];
						}

						totalUnits--;
					}

					//distance of units
					float unitDistance = 0.0f;
					if (side == CombatObject::ATTACKER_SIDE)
						unitDistance = (float)game.SCREENWIDTH / (float)((TOTAL_UNITS + (singleRifleman?1:0) + 1) + COMBAT_PAIR_DISTANCE_RELATION * numberOfRiflemanCombatPairs);
					else if (side == CombatObject::DEFENDER_SIDE)
						unitDistance = ((float)game.SCREENWIDTH - (float)Base::TEXTURE_WIDTH) / (float)((TOTAL_UNITS + (singleRifleman?1:0) + 2) + COMBAT_PAIR_DISTANCE_RELATION * numberOfRiflemanCombatPairs);

					//the distance between the soldiers in a combat pair
					float combatPairDistance = unitDistance * COMBAT_PAIR_DISTANCE_RELATION;
					float thisX = /*100.0f;*/unitDistance;

					bool do_once_flag = true;

					for (int j = 0; j < TOTAL_UNITS; j++)
					{
						float baseLeftSideX =  ((float)game.SCREENWIDTH - (float)Base::TEXTURE_WIDTH) / 2.0f;

						if (side == CombatObject::DEFENDER_SIDE && do_once_flag)
						{					
							if (thisX >= baseLeftSideX)
							{
								thisX += ((float)Base::TEXTURE_WIDTH);
								do_once_flag = false;
							}
						}

						if (unitClasses[j] == (int)FootSoldier::RIFLEMAN)
						{
							if (side == CombatObject::DEFENDER_SIDE && do_once_flag)
							{
								if (thisX + combatPairDistance >= baseLeftSideX)
								{
									thisX += ((float)Base::TEXTURE_WIDTH);
									do_once_flag = false;
								}
							}

							//combat pair1
							AppearanceInfo fsai1;
							fsai1.type = (int)FootSoldier::RIFLEMAN;
							fsai1.time = appearanceTime;
							fsai1.x = thisX;
							footSoldierAppearances[side].push_back(fsai1);
							thisX += combatPairDistance;
							//combat pair2
							AppearanceInfo fsai2;
							fsai2.type = (int)FootSoldier::RIFLEMAN;
							fsai2.time = appearanceTime;
							fsai2.x = thisX;
							footSoldierAppearances[side].push_back(fsai2);
							thisX += unitDistance;
						}
						else
						{
							//specialist
							AppearanceInfo fsai;
							fsai.type = (unitClasses[j]);
							fsai.time = appearanceTime;
							fsai.x = thisX;
							footSoldierAppearances[side].push_back(fsai);
							thisX += unitDistance;
						}
					}
					if (singleRifleman)
					{
						AppearanceInfo fsai1;
						fsai1.type = FootSoldier::RIFLEMAN;
						fsai1.time = appearanceTime;
						fsai1.x = thisX;
						footSoldierAppearances[side].push_back(fsai1);
					}
				}
				nextFootSoldier[side] = footSoldierAppearances[side].begin();	
			}
			numberOfTanksAtArea[side] = 0;
			numberOfFootSoldiersAtArea[side] = 0;
		}

		//Filling up AttackerTankAppearanceInfo list:
		{
			numberOfTanksLeft[att] = numberOfTanks[att] = hge->Ini_GetInt(levelName, "NUMBER_OF_ATTACKER_TANKS ", 0);

			for (int i = 1; i < numberOfTanks[att] + 1; i++)
			{
				AppearanceInfo etai;
				etai.type = 0;
				sprintf(text, "ATTACKER_TANK_%i_APPEARANCE_TIME", i);
				etai.time = hge->Ini_GetFloat(levelName, text, 0.0f);
				if (randomness) etai.time += APPEARANCE_TIME_RANDOMNESS * randFloat(-1.0f, 1.0f);
				sprintf(text, "ATTACKER_TANK_%i_APPEARANCE_X ", i);
				etai.x = hge->Ini_GetFloat(levelName, text, 0.0f);
				if (randomness) etai.x += TANK_APPEARANCE_X_RANDOMNESS * randFloat(-1.0f, 1.0f);
				tankAppearances[att].push_back(etai);
			}
			nextTank[att] = tankAppearances[att].begin();
		}
		//defender tanks:
		numberOfTanksLeft[def] = numberOfTanks[def] = game.player_1.lives + ((game.two_player_game) ? game.player_2.lives : 0);

		//smokes:
		{
			smokeTimer = 0.0f;
			smokeNumber = 0;
			hge->System_SetState(HGE_INIFILE, "Levels.ini");
			int smokes = hge->Ini_GetInt(levelName, "ATTACKER_SMOKES", 0);

			for (int i = 1; i <= smokes; i++)
			{
				char str[64];
				sprintf(str, "ATTACKER_SMOKE_%i_TIME", i);
				float time = hge->Ini_GetFloat(levelName, str, FLT_MAX);

				float t1 = 1.0f, t2 = 1.0f;
				AppearanceInfo ai1;
				ai1.time = time;
				AppearanceInfo ai2;
				ai2.time = ai1.time + randFloat(t1, t2);
				AppearanceInfo ai3;
				ai3.time = ai2.time + randFloat(t1, t1);
				smokeAppearances[CombatObject::ATTACKER_SIDE].push_back(ai1);
				smokeAppearances[CombatObject::ATTACKER_SIDE].push_back(ai2);
				smokeAppearances[CombatObject::ATTACKER_SIDE].push_back(ai3);
			}
			nextSmoke[CombatObject::ATTACKER_SIDE] = smokeAppearances[CombatObject::ATTACKER_SIDE].begin();
		}
	}

	//sounds
	{
		int i = 1;
		while (true)
		{
			LevelSound ls;
			char s[64];
			sprintf(s,"SOUND_%i_TIME", i);
			float time = hge->Ini_GetFloat(levelName, s, -1.0f);
			sprintf(s,"SOUND_%i_VOLUME", i);
			int volume = hge->Ini_GetInt(levelName, s, 100);
			sprintf(s,"SOUND_%i_LOOP", i);
			bool loop = (bool)hge->Ini_GetInt(levelName, s, 0);
			sprintf(s,"SOUND_%i_PITCH", i);
			float pitch = hge->Ini_GetFloat(levelName, s, 1.0f);
			sprintf(s,"SOUND_%i_RANDOM", i);
			bool random = hge->Ini_GetFloat(levelName, s, false);
			sprintf(s,"SOUND_%i_INTERVAL_MIN", i);
			float intervalMin = hge->Ini_GetFloat(levelName, s, FLT_MAX);
			sprintf(s,"SOUND_%i_INTERVAL_MAX", i);
			float intervalMax = hge->Ini_GetFloat(levelName, s, FLT_MAX);

			if (time != -1)
			{
				sprintf(s, "SOUND_%i_FILENAME", i);
				char * name;
				name = hge->Ini_GetString(levelName, s, "");
				ls.sound = hge->Effect_Load(name);
				ls.time = time;
				ls.volume = volume;
				ls.loop = loop;
				ls.pitch = pitch;
				ls.played = false;
				ls.channel = NULL;
				ls.random = random;
				ls.intervalMin = intervalMin;
				ls.intervalMax = intervalMax;

				if (random)
				{
					ls.time = randFloat(intervalMin, intervalMax);
				}

				list<LevelSound>::iterator it = levelSounds.begin();

				int size = levelSounds.size();
				for (; it != levelSounds.end(); it++)
				{
					if ((*it).time > ls.time)
					{
						//insert sound before the sound with greater time.
						levelSounds.insert(it, ls);
					}
				}
				if (size == levelSounds.size())
				{
					levelSounds.push_back(ls);
				}
				

				i++;
			}
			else
				break;
		}
	}

	//statistics
	{
		for (int i = 0; i < STAT_0; i++)
			for (int j = 0; j < STAT_1; j++)
				for (int k = 0; k < STAT_2; k++)
					for (int l = 0; l < STAT_3; l++)
					{
						statisticsForSides[i][j][k][l] = 0;
					}
	}
}

Level::~Level(void)
{
	for (int i = 0; i < CombatObject::NUMBER_OF_SIDES; i++)
	{
		tankAppearances[i].clear();
		footSoldierAppearances[i].clear();

		for (int j = 0; j < FireSupport::NUMBER_OF_TYPES; j++)
			fireSupportAppearances[i][j].clear();
	}

	if (defenderFlareShooter)
		delete defenderFlareShooter;
	if (attackerFlareShooter)
		delete attackerFlareShooter;
}

void Level::runScript(GameObjectCollection * objects)
{
	if (!night && levelTime < timerDelta / 2.0f)
	{
		game.objects.addGameObject(new Effect(Effect::SUN, Location(0, 0), 0.0f));
	}

	//NOTE:
	//The game will get into a halt (eternal loop)if 
	//the appropriate appearanceInfo objects are not erased.
	levelTime += timerDelta;
	smokeTimer -= timerDelta;

	scriptCheckTimer -= timerDelta;

	if (scriptCheckTimer > 0.0f)
		return;

	scriptCheckTimer = SCRIPT_CHECK_TIME;

	int def = CombatObject::DEFENDER_SIDE;
	int att = CombatObject::ATTACKER_SIDE;

	//scripted objects:
	{
		//fire support
		{
			//defender fire support
			{
				char * messages[FireSupport::NUMBER_OF_TYPES] = 
				{
					"Headquarters: A mortar unit is ready to support you with a strike.",		
					"Headquarters: Ready to provide a missile strike.",
					"Headquarters: Ready to drop some napalm on your area."
				};

				for (int suppType = 0; suppType < FireSupport::NUMBER_OF_TYPES; suppType++)
				{
					while (!fireSupportAppearances[def][suppType].empty() &&
						nextFireSupport[def][suppType]->time <= levelTime)
					{
						game.showMessage(messages[suppType], Game::COLOR_TEXT_MESSAGE_DEFENDER_SIDE);

						fireSupportAppearances[def][suppType].erase(nextFireSupport[def][suppType]);
						nextFireSupport[def][suppType] = fireSupportAppearances[def][suppType].begin();
						numberOfFireSupport[def][suppType]++;
						numberOfFireSupportLeft[def][suppType]++;
					}
				}
			}

			//attacker fire support
			{
				for (int suppType = 0; suppType < FireSupport::NUMBER_OF_TYPES; suppType++)
				{
					while (!fireSupportAppearances[att][suppType].empty() &&
						nextFireSupport[att][suppType]->time <= levelTime)
					{
						objects->addGameObject(new FireSupport((FireSupport::FIRE_SUPPORT_TYPE)suppType, (CombatObject::SIDE)att));

						fireSupportAppearances[att][suppType].erase(nextFireSupport[att][suppType]);
						nextFireSupport[att][suppType] = fireSupportAppearances[att][suppType].begin();
						numberOfFireSupportLeft[att][suppType]--;
					}
				}
			}
		}

		//attacker tanks
		{
			while (!tankAppearances[att].empty() && 
					nextTank[att]->time <= levelTime)
			{
				Tank * tank = new Tank
				//game.objects.addTank 
					(
					Tank::ENEMY_TANK, 
					CombatObject::ATTACKER_SIDE,
					Location(
						nextTank[att]->x, ATTACKER_TANK_APPEARANCE_Y),
					ATTACKER_TANK_START_DIRECTION
					);
				objects->addGameObject(tank);

				if (firstAttackerTank == NULL)
					firstAttackerTank = tank;

				tankAppearances[att].erase(nextTank[att]);
				nextTank[att] = tankAppearances[att].begin();
				numberOfTanksAtArea[att]++;
			}
		}	

		//foot soldiers
		{
			CombatObject::SIDE sides[2] = 
			{ 
				CombatObject::DEFENDER_SIDE,
				CombatObject::ATTACKER_SIDE
			};
			FootSoldier ** firstFootSoldiersForSide[2] = 
			{
				&firstAttackerFootSoldier,
				&firstDefenderFootSoldier
			};
			float appearanceYForSide[2] = 
			{
				DEFENDER_FOOT_SOLDIER_APPEARANCE_Y,
				ATTACKER_FOOT_SOLDIER_APPEARANCE_Y
			};
			Direction startDirections[2] = 
			{
				DEFENDER_FOOT_SOLDIER_START_DIRECTION,
				ATTACKER_FOOT_SOLDIER_START_DIRECTION
			};

			MilitaryUnit * footSoldierUnit = NULL;
			
			for (int side = 0; side < 2; side++)
			{
				if (!footSoldierAppearances[side].empty() && 
						nextFootSoldier[side]->time <= levelTime)
				{
					footSoldierUnit = new MilitaryUnit((CombatObject::SIDE)side/*, MilitaryUnit::TEAM*/);
					game.objects.addGameObject(footSoldierUnit);
				}
			
				while (!footSoldierAppearances[side].empty() && 
						nextFootSoldier[side]->time <= levelTime)
				{						
					int type = (GameObject::TYPE)nextFootSoldier[side]->type;
					FootSoldier::FOOT_SOLDIER_CLASS c = FootSoldier::FOOT_SOLDIER_CLASS(type);
					float x = nextFootSoldier[side]->x;

					if (type == FootSoldier::RIFLEMAN)
					{
						Rifleman * footSoldier = new Rifleman
 							(
							sides[side],
							Location(x, appearanceYForSide[side]),
							startDirections[side]
							);
						objects->addGameObject(footSoldier);

						if ((*(firstFootSoldiersForSide[side])) == NULL)
							(*(firstFootSoldiersForSide[side])) = footSoldier;

						footSoldierAppearances[side].erase(nextFootSoldier[side]);
						nextFootSoldier[side] = footSoldierAppearances[side].begin();
						numberOfFootSoldiersAtArea[side]++;
						footSoldierUnit->add((CombatAIObject *)footSoldier);

						//combat pair:
						if (!footSoldierAppearances[side].empty())
						{
							GameObject::TYPE type = (GameObject::TYPE)nextFootSoldier[side]->type;
							x = nextFootSoldier[side]->x;

							if (type == FootSoldier::RIFLEMAN)
							{
								Rifleman * footSoldier2 = new Rifleman
 									(
									sides[side],
									Location(x, appearanceYForSide[side]),
									startDirections[side]
									);
								objects->addGameObject(footSoldier2);

								footSoldier->setCombatPair(footSoldier2);
								footSoldier2->setCombatPair(footSoldier);

								//first to move:
								footSoldier->turn_to_move = (randInt(0, 1) == 1);
								footSoldier2->turn_to_move = !footSoldier->turn_to_move;

								footSoldierAppearances[side].erase(nextFootSoldier[side]);
								nextFootSoldier[side] = footSoldierAppearances[side].begin();
								numberOfFootSoldiersAtArea[side]++;
								footSoldierUnit->add((CombatAIObject *)footSoldier);
							}
							else
								footSoldier->setCombatPair(NULL);
						}
						else
							footSoldier->setCombatPair(NULL);
					}
					else if (type == FootSoldier::AT_SOLDIER)
					{
						AT_Soldier * footSoldier = new AT_Soldier
 							(
							sides[side],
							Location(x, appearanceYForSide[side]),
							startDirections[side]
							);
						objects->addGameObject(footSoldier);

						if ((*(firstFootSoldiersForSide[side])) == NULL)
							(*(firstFootSoldiersForSide[side])) = footSoldier;

						footSoldierAppearances[side].erase(nextFootSoldier[side]);
						nextFootSoldier[side] = footSoldierAppearances[side].begin();
						numberOfFootSoldiersAtArea[side]++;
						footSoldierUnit->add((CombatAIObject *)footSoldier);
					}
					else if (type == FootSoldier::MACHINE_GUNNER)
					{
						MachineGunner * footSoldier = new MachineGunner
 							(
							sides[side],
							Location(x, appearanceYForSide[side]),
							startDirections[side]
							);
						objects->addGameObject(footSoldier);

						if ((*(firstFootSoldiersForSide[side])) == NULL)
							(*(firstFootSoldiersForSide[side])) = footSoldier;

						footSoldierAppearances[side].erase(nextFootSoldier[side]);
						nextFootSoldier[side] = footSoldierAppearances[side].begin();
						numberOfFootSoldiersAtArea[side]++;
						footSoldierUnit->add((CombatAIObject *)footSoldier);
					}
					else if (type == FootSoldier::MEDIC)
					{
						Medic * footSoldier = new Medic
 							(
							sides[side],
							Location(x, appearanceYForSide[side]),
							startDirections[side]
							);
						objects->addGameObject(footSoldier);

						if ((*(firstFootSoldiersForSide[side])) == NULL)
							(*(firstFootSoldiersForSide[side])) = footSoldier;

						footSoldierAppearances[side].erase(nextFootSoldier[side]);
						nextFootSoldier[side] = footSoldierAppearances[side].begin();
						numberOfFootSoldiersAtArea[side]++;
						numberOfFootSoldiersLeft[side]--;
						footSoldierUnit->add((CombatAIObject *)footSoldier);
					}
				}
				//set first defender team waiting:
				if (levelTime < 0.1f && side == 0)
				{					
					//Straight change to gameObjectCollection objects:
					list<GameObject *>::iterator oit;

					for (oit = game.objects.objects.begin(); oit != game.objects.objects.end(); oit++)
					{
						if ((*oit)->type == GameObject::FOOT_SOLDIER)
						{
							FootSoldier * fit = static_cast<FootSoldier *>((*oit));
							if ((fit)->footSoldierClass == FootSoldier::MEDIC)
							{
								(fit)->y = 650.0f;
							}
							else
							{
								(fit)->y = defenderDefenseLineY + randFloat(-2.0f, 2.0f);
								(fit)->setAction(FootSoldier::ACTION_PRONE);
								(fit)->setDirection((fit)->getBattleMainDirection());
								(fit)->setState(FootSoldier::STATE_DEFEND);
								(fit)->setStatePhase(FootSoldier::DEFENSE_PHASE_OBSERVE);
							}
						}
					}
				}
			}
		}

		//smokes
		{
			while (!smokeAppearances[att].empty() && 
					nextSmoke[att]->time <= levelTime)
			{
				//Shooter:
				Rifleman grenadier(CombatObject::ATTACKER_SIDE, Location(400.0f, -150.0f), HALF_PI);
				
				//Target location:
				Location targetLocation;
				float x1 = 125.0f;
				float x2 = 400.0f;
				float x3 = 800 - x1;
				float smokeY = 100.0f;
				float yVar = 5.0f;
				if (smokeNumber == 0)
				{
					smokeTimer = SMOKE_TIME;
					targetLocation.x = randFloat(x1 - yVar, x1 + yVar);
					targetLocation.y = randFloat(smokeY - yVar, smokeY + yVar);
				}
				else if (smokeNumber == 1)
				{
					targetLocation.x = randFloat(x2 - yVar, x2 + yVar);
					targetLocation.y = randFloat(smokeY - yVar, smokeY + yVar);
				}
				else if (smokeNumber == 2)
				{
					targetLocation.x = randFloat(x3 - yVar, x3 + yVar);
					targetLocation.y = randFloat(smokeY - yVar, smokeY + yVar);
				}
				smokeNumber++;

				if (smokeNumber == 3)
					smokeNumber = 0;

				//Fire smoke grenade:
				Projectile * smokeGrenade = new Grenade(&grenadier, Projectile::SMOKE_GRENADE, grenadier.getLocation(), 10.0f, targetLocation);
				objects->addGameObject(smokeGrenade);
				grenadier.playSoundAtLocation(game.sounds[Game::SOUND_SMOKE_GRENADE_SHOOT]);

				smokeAppearances[att].erase(nextSmoke[att]);
				nextSmoke[att] = smokeAppearances[att].begin();
			}
		}
	}

	//level sounds
	{
		list <LevelSound>::iterator it = levelSounds.begin();
		list <LevelSound>::iterator end = levelSounds.end();

		for (; it != end; it++)
		{
			if (it->time < levelTime)
			{
				if (!it->played)
				{
					it->channel = game.playSound(it->sound, it->volume, it->pitch, it->loop);
					it->played = true;
				}
				if (!(hge->Channel_IsPlaying(it->channel)) && !it->random)
				{
					hge->Effect_Free(it->sound);
					it->sound = NULL;
					levelSounds.erase(it);

					break;
				}
			}
		}
	}

	//special events
	{

		//nightbattle flares
		{
			if (night && levelCombatStarted)
			{

				//defender flares
				{
					flareTimer1 -= SCRIPT_CHECK_TIME;

					if (flareTimer1 < 0.0f)
					{
						Location targetLocation(randFloat(350.0f, 450.0f), -500.0f);
						game.objects.GameObjectCollection::addGameObject(new Projectile(defenderFlareShooter, Projectile::FLARE, defenderFlareShooter->getLocation(), 0.0f, targetLocation));
						game.Game::playSound(game.sounds[Game::SOUND_FLARE]);

						flareTimer1 = randFloat(20.0f, 25.0f);
					}
				}

				//attacker flares
				{
					flareTimer2 -= SCRIPT_CHECK_TIME;

					if (flareTimer2 < 0.0f)
					{
						Location targetLocation(randFloat(350.0f, 450.0f), 800 + 500.0f);
						game.objects.GameObjectCollection::addGameObject(new Projectile(attackerFlareShooter, Projectile::FLARE, attackerFlareShooter->getLocation(), 0.0f, targetLocation));
						game.Game::playSound(game.sounds[Game::SOUND_FLARE]);

						flareTimer2 = randFloat(20.0f, 25.0f);
					}
				}
			}
		}
	}
}

void Level::checkVictory(void)
{
	if (!isThereTroopsLeft(CombatObject::ATTACKER_SIDE) ||
		(levelMaxTime < levelTime && getNumberOfTanksActiveAtArea(CombatObject::ATTACKER_SIDE) <= 0))
	{
		levelFinished = true;
	}
}

void Level::playerFireSupportCall(GameObjectCollection * objects, int fireSupportType)
{
	int side = CombatObject::DEFENDER_SIDE;
	if (game.currentLevel->numberOfFireSupportLeft[side][fireSupportType] > 0)
	{
		objects->addGameObject(new FireSupport((FireSupport::FIRE_SUPPORT_TYPE)fireSupportType, CombatObject::DEFENDER_SIDE));
		game.currentLevel->numberOfFireSupportLeft[side][fireSupportType]--;
	}
}

void Level::setStatistics(CombatObject * cObject, CombatObject::STATISTIC_VALUE newVal)
{
	if (cObject->statisticValue >= CombatObject::STATISTIC_VALUE_FIRST && cObject->statisticValue <= CombatObject::STATISTIC_VALUE_LAST &&
		newVal >= CombatObject::STATISTIC_VALUE_FIRST && newVal <= CombatObject::STATISTIC_VALUE_LAST)
	{
		statisticsForSides[cObject->getSide()][cObject->type][cObject->getSubType()][cObject->statisticValue]--;
		statisticsForSides[cObject->getSide()][cObject->type][cObject->getSubType()][newVal]++;
		cObject->statisticValue = newVal;
	}
}

int Level::getStatistics_numberOfInfantry(CombatObject::SIDE side)
{
	return numberOfFootSoldiers[side];
}

int Level::getStatistics_numberOfInfantryKilled(CombatObject::SIDE side)
{
	return getStatistics_getInfantryStatistics(CombatObject::STATISTIC_VALUE_PERSON_KILLED, side);
}

int Level::getStatistics_numberOfInfantryWounded(CombatObject::SIDE side)
{
	return getStatistics_getInfantryStatistics(CombatObject::STATISTIC_VALUE_PERSON_WOUNDED, side);
}

int Level::getStatistics_numberOfInfantryCaptured(CombatObject::SIDE side)
{
	return getStatistics_getInfantryStatistics(CombatObject::STATISTIC_VALUE_PERSON_CAPTURED, side);
}

int Level::getStatistics_numberOfInfantryFled(CombatObject::SIDE side)
{
	return getStatistics_getInfantryStatistics(CombatObject::STATISTIC_VALUE_PERSON_FLED, side);
}

int Level::getStatistics_getInfantryStatistics(CombatObject::STATISTIC_VALUE val, CombatObject::SIDE side)
{
	int sum = 0;
	int type = GameObject::FOOT_SOLDIER;
	int value = val;

	for (int i = 0; i < FootSoldier::NUMBER_OF_CLASSES; i++)
	{
		sum += statisticsForSides[side][type][i][value];
	}
	return sum;
}

int Level::getStatistics_numberOfTanks(CombatObject::SIDE side)
{
	return numberOfTanks[side];
}

int Level::getStatistics_numberOfTanksDestroyed(CombatObject::SIDE side)
{
	return numberOfTanks[side] - numberOfTanksLeft[side];
}

int Level::getStatistics_numberOfFireSupportStrikes(CombatObject::SIDE side)
{
	return statisticsForSides[side][GameObject::FIRE_SUPPORT][FireSupport::NUMBER_OF_TYPES][0];
}

int Level::getStatistics_numberOfMortarGrenadesFired(CombatObject::SIDE side)
{
	return statisticsForSides[side][GameObject::FIRE_SUPPORT][FireSupport::MORTAR_STRIKE][0];
}

int Level::getStatistics_numberOfMissilesFired(CombatObject::SIDE side)
{
	return statisticsForSides[side][GameObject::FIRE_SUPPORT][FireSupport::MISSILE_STRIKE][0];
}

int Level::getStatistics_numberOfNapalmBombsDropped(CombatObject::SIDE side)
{
	return statisticsForSides[side][GameObject::FIRE_SUPPORT][FireSupport::NAPALM_STRIKE][0];
}

DWORD Level::getObjectColor(void)
{
	if (night)
	{
		return 0xff777777;
	}
	return 0xffffffff;
}

LevelManager::LevelManager(void)
{
	levelNumber = 0;
}

LevelManager::~LevelManager(void)
{
}

Level * LevelManager::getNextLevel(/*Level * level*/)
{
	levelNumber++;
	hge->System_SetState(HGE_INIFILE, "Levels.ini");
	char levelName[128];
	sprintf(levelName, "LEVEL_%i", levelNumber);
	int num = hge->Ini_GetInt(levelName, "LEVEL_NUMBER ", 0);

	if (num > 0)
	{
		return new Level(levelName);
		levelNumber++;
	}

	return NULL;
}

Level * LevelManager::getLevel(char * levelName)
{
	hge->System_SetState(HGE_INIFILE, "Levels.ini");
	int num = hge->Ini_GetInt(levelName, "LEVEL_NUMBER", -1);

	if (num != -1)
	{
		return new Level(levelName);
		levelNumber = num;
	}

	return NULL;
}

Level * LevelManager::getLevel(int _levelNumber)
{
	hge->System_SetState(HGE_INIFILE, "Levels.ini");
	char levelName[128];
	sprintf(levelName, "LEVEL_%i", _levelNumber);
	int num = hge->Ini_GetInt(levelName, "LEVEL_NUMBER", 0);

	if (num > 0)
	{

		return new Level(levelName);
		levelNumber = num;
	}

	return NULL;
}

