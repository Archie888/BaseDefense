#include ".\footsoldier.h"
#include "main.h"

char FootSoldier::classNameStrings[NUMBER_OF_CLASSES][64];

//Weapon information:
const Time FootSoldier::ASSAULT_RIFLE_RATE_OF_FIRE_FULL_AUTO = 60.0f / 600.0f;
const int FootSoldier::ASSAULT_RIFLE_CLIP_SIZE_FULL = 30.0f;
const int FootSoldier::ASSAULT_RIFLE_SHOT_COUNT_BURST_MODE = 3;
float FootSoldier::ASSAULT_RIFLE_AIMING_ACCURACY = 1.1;

//Military doctrine/training information:
Distance FootSoldier::GRENADE_THROWING_DISTANCE = 150.0f;
#define GRENADE_THROWING_DISTANCE_SQUARED 40000.0f
Distance FootSoldier::ASSAULT_RIFLE_BURST_FIRE_DISTANCE = 200.0f;
Distance FootSoldier::COMBAT_DISTANCE = 500.0f;

//Behaviour:
Time FootSoldier::GRENADE_THROWING_INTERVAL = 30.0f;
#define PROPABILITY_OF_THROWING_LAST_GRENADE_FIGHTING 0.1f

//general footsoldier information:
int FootSoldier::FOOT_SOLDIER_AMOUNT_OF_GRENADES = 5;
Distance FootSoldier::FOOT_SOLDIER_ENEMY_AUTOMATIC_NOTIFICATION_DISTANCE = 400.0f;
Time FootSoldier::FOOT_SOLDIER_AVERAGE_REACTION_TIME = 1.0f;
Time FootSoldier::FOOT_SOLDIER_AVERAGE_REACTION_TIME_VARIATION = 0.3f;
Time FootSoldier::FOOT_SOLDIER_AVERAGE_AIMING_SPEED_FOR_100_METERS = 1.0f;
Time FootSoldier::FOOT_SOLDIER_AVERAGE_AIMING_SPEED_VARIATION_FOR_100_METERS = 0.2f;
float FootSoldier::FOOT_SOLDIER_AVERAGE_AIMING_ACCURACY = 0.2f;
float FootSoldier::FOOT_SOLDIER_AVERAGE_AIMING_ACCURACY_VARIATION = 0.02f;

//statics:
Angle FootSoldier::PRONE_ANIMATION_TURNING_ANGLE = 0.2617994f; //15 degrees
Angle FootSoldier::PRONE_ANIMATION_ADJUSTING_ANGLE = 0.08726646; //5 degrees
#define FOOT_SOLDIER_COLLISION_SPHERE_RADIUS 5.0f
Distance FootSoldier::ENEMY_TANK_MIN_DISTANCE = 200.0f;
Distance FootSoldier::FRIENDLY_TANK_MIN_DISTANCE = 60.0f;
Distance FootSoldier::ENEMY_TANK_RUN_DISTANCE = 250.0f;
Distance FootSoldier::ATTACK_MAX_ADVANCING_DISTANCE = Game::METER * 5.0f;
Distance FootSoldier::ATTACK_MAX_CRAWLING_FORWARD_DISTANCE = Game::METER * 2.0f;
Distance FootSoldier::ATTACK_MAX_CRAWLING_SIDEWAYS_DISTANCE = Game::METER * 1.0f;
Distance FootSoldier::ATTACK_MIN_ADVANCING_DISTANCE = Game::METER * 2.0f;
Distance FootSoldier::ATTACK_MIN_CRAWLING_FORWARD_DISTANCE = Game::METER * 0.5f;
Distance FootSoldier::ATTACK_MIN_CRAWLING_SIDEWAYS_DISTANCE = Game::METER * 0.5f;
Height FootSoldier::HEIGHT_PRONE = 2.5f;
Height FootSoldier::HEIGHT_STANDING = 17.0f;
Height FootSoldier::LOWER_BODY_HEIGHT_STANDING = 5.0f;
Height FootSoldier::WEAPON_HEIGHT_PRONE = 3.0f;
Height FootSoldier::WEAPON_HEIGHT_STANDING = 15.0f;
Probability FootSoldier::PROBABILITY_OF_DEATH_PRONE = 0.5f;	
Probability FootSoldier::PROBABILITY_OF_DEATH_STANDING = 0.4f;
Time FootSoldier::MAX_OBSERVATION_TIME_PER_OBJECT = 0.1f;
Velocity FootSoldier::SPEED_WALKING = 10.0f;
Velocity FootSoldier::SPEED_RUNNING = 20.0f;
Velocity FootSoldier::SPEED_RUNNING_CARRYING_FOOT_SOLDIER = 15.0f;
Velocity FootSoldier::SPEED_SNEAKING = 8.0f;
Velocity FootSoldier::SPEED_CRAWLING = 10.0f;
Velocity FootSoldier::SPEED_CRAWLING_SIDEWAYS = 10.0f;
Velocity FootSoldier::SPEED_CRAWLING_WOUNDED = 5.0f;
Velocity FootSoldier::TURNING_SPEED_WALKING = 1.0f;
Velocity FootSoldier::TURNING_SPEED_RUNNING = 2.0f;
Velocity FootSoldier::TURNING_SPEED_SNEAKING = 1.0f;

float FootSoldier::SCREAM_MIN_TIME = 0.3f;
float FootSoldier::HIT_STAY_UP_MAX_TIME = 1.0f;
Time FootSoldier::INDIRECT_FIRE_END_WAIT_TIME = 3.0f;
bool FootSoldier::attacker_taking_cover = false;
bool FootSoldier::defender_taking_cover = false;

int FootSoldier::animationBodyPositions[];
Location FootSoldier::animationDisplacements[];
int FootSoldier::animationWoundedFrames[];
int FootSoldier::animationDyingFramesMin[];
int FootSoldier::animationDyingFramesMax[];
bool FootSoldier::animationWoundedTwichForward[];
int FootSoldier::animationInfos[];

#define FOOTSOLDIER_POSSIBILITY_OF_HIT_EASY 0.6f
#define FOOTSOLDIER_POSSIBILITY_OF_HIT_NORMAL 0.4f
#define FOOTSOLDIER_POSSIBILITY_OF_HIT_HARD 0.3f

#define ATTACK_MIN_ADVANCING_DISTANCE_DEF Game::METER * 1.0f
#define ATTACK_MAX_ADVANCING_DISTANCE_DEF Game::METER * 2.0f
#define ATTACK_MIN_CRAWLING_FORWARD_DISTANCE_DEF Game::METER * 0.5f
#define ATTACK_MAX_CRAWLING_FORWARD_DISTANCE_DEF Game::METER * 1.0f

//smoke:
#define OFF_SMOKE_DISTANCE 200.0f

void FootSoldier::init()
{
	game.renderLoadScreen("LOADING FOOT_SOLDIER CLASS");

	sprintf(classNameStrings[RIFLEMAN], "RIFLEMAN");
	sprintf(classNameStrings[MACHINE_GUNNER], "MACHINE_GUNNER");
	sprintf(classNameStrings[AT_SOLDIER], "AT_SOLDIER");
	sprintf(classNameStrings[MEDIC], "MEDIC");
	
	//set body position information linked to actions
	{
	for (int i = ACTION_STANDING_FIRST; i <= ACTION_STANDING_MAX; i++)
		animationBodyPositions[i] = (int)BODY_POSITION_STANDING;
	for (int i = ACTION_PRONE_FIRST; i <= ACTION_PRONE_MAX; i++)
		animationBodyPositions[i] = (int)BODY_POSITION_PRONE;
	}

	//initialize rest
	{
		for (int i = 0; i < ACTION_MAX; i++)
		{
			animationDisplacements[i] = Location();
			animationWoundedFrames[i] = 1;
			animationDyingFramesMin[i] = 1;
			animationDyingFramesMax[i] = 1;
		}
	}

	//set animation displacement
	{
		animationDisplacements[ACTION_STANDING_WOUNDED_FALLING_FORWARD].x = 9.0f;
		animationDisplacements[ACTION_STANDING_WOUNDED_FALLING_BACKWARDS].x = 10.0f;
		animationDisplacements[ACTION_STANDING_WOUNDED_FALLING_BACKWARDS].y = 1.0f;
		animationDisplacements[ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD].x = 10.0f;
		animationDisplacements[ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD].y = 1.0f;
		animationDisplacements[ACTION_STANDING_GETTING_PRONE].x = 6.0f;
		animationDisplacements[ACTION_PRONE_GETTING_UP].x = 6.0f;
		animationDisplacements[ACTION_CARRIED_FALLING_1].x = 8.0f;
	}

	//wounded animations: start framecounts
	{
		animationWoundedFrames[ACTION_STANDING_WOUNDED_FALLING_FORWARD] = 13; 
		animationWoundedFrames[ACTION_STANDING_WOUNDED_FALLING_BACKWARDS] = 16;
		animationWoundedFrames[ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD] = 13;
		animationWoundedFrames[ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS] = 14;
		animationWoundedFrames[ACTION_PRONE_GETTING_WOUNDED_AIMING_1] = 13;
		animationWoundedFrames[ACTION_PRONE_GETTING_WOUNDED_AIMING_2] = 7;
		animationWoundedFrames[ACTION_PRONE_GETTING_WOUNDED_CRAWLING] = 6;
		animationWoundedFrames[ACTION_PRONE_GETTING_WOUNDED_TAKING_ITEM] = 8;
		animationWoundedFrames[ACTION_PRONE_GETTING_WOUNDED] = 6;
		animationWoundedFrames[ACTION_PRONE_WOUNDED_CRAWLING_1] = 15;
		animationWoundedFrames[ACTION_PRONE_WOUNDED_CRAWLING_2] = 7;
		animationWoundedFrames[ACTION_PRONE_THROWN_BY_FORCE_1] = 5;
		animationWoundedFrames[ACTION_PRONE_THROWN_BY_FORCE_2] = 5;
		animationWoundedFrames[ACTION_CARRIED_FALLING_1] = 8;
	}

	//wounded animations: death framecounts
	{
		animationDyingFramesMin[ACTION_STANDING_WOUNDED_FALLING_FORWARD] = 22; 
		animationDyingFramesMin[ACTION_STANDING_WOUNDED_FALLING_BACKWARDS] = 23;
		animationDyingFramesMin[ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD] = 20;
		animationDyingFramesMin[ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS] = 22;
		animationDyingFramesMin[ACTION_PRONE_GETTING_WOUNDED_AIMING_1] = 20;
		animationDyingFramesMin[ACTION_PRONE_GETTING_WOUNDED_AIMING_2] = 14;
		animationDyingFramesMin[ACTION_PRONE_GETTING_WOUNDED_CRAWLING] = 13;
		animationDyingFramesMin[ACTION_PRONE_GETTING_WOUNDED_TAKING_ITEM] = 15;
		animationDyingFramesMin[ACTION_PRONE_GETTING_WOUNDED] = 14;
		animationDyingFramesMin[ACTION_PRONE_WOUNDED_CRAWLING_1] = 16;
		animationDyingFramesMin[ACTION_PRONE_WOUNDED_CRAWLING_2] = 9;
		animationDyingFramesMin[ACTION_PRONE_THROWN_BY_FORCE_1] = 12;
		animationDyingFramesMin[ACTION_PRONE_THROWN_BY_FORCE_2] = 12;
		animationDyingFramesMin[ACTION_CARRIED_FALLING_1] = 4;
	
		animationDyingFramesMax[ACTION_STANDING_WOUNDED_FALLING_FORWARD] = 23; 
		animationDyingFramesMax[ACTION_STANDING_WOUNDED_FALLING_BACKWARDS] = 28;
		animationDyingFramesMax[ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD] = 21;
		animationDyingFramesMax[ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS] = 24;
		animationDyingFramesMax[ACTION_PRONE_GETTING_WOUNDED_AIMING_1] = 23;
		animationDyingFramesMax[ACTION_PRONE_GETTING_WOUNDED_AIMING_2] = 17;
		animationDyingFramesMax[ACTION_PRONE_GETTING_WOUNDED_CRAWLING] = 14;
		animationDyingFramesMax[ACTION_PRONE_GETTING_WOUNDED_TAKING_ITEM] = 18;
		animationDyingFramesMax[ACTION_PRONE_GETTING_WOUNDED] = 14;
		animationDyingFramesMax[ACTION_PRONE_WOUNDED_CRAWLING_1] = 18;
		animationDyingFramesMax[ACTION_PRONE_WOUNDED_CRAWLING_2] = 10;
		animationDyingFramesMax[ACTION_PRONE_THROWN_BY_FORCE_1] = 13;
		animationDyingFramesMax[ACTION_PRONE_THROWN_BY_FORCE_2] = 13;
		animationDyingFramesMax[ACTION_CARRIED_FALLING_1] = 7;
	}
	//animation types:
	{
		for (int i = 0; i < ACTION_MAX; i++)
		{
			animationInfos[i] = ANIMATION_INFO_BASIC;

			if ((i >= ACTION_STANDING_WOUNDED_FIRST &&
				i <= ACTION_STANDING_WOUNDED_MAX) ||
				(i >= ACTION_PRONE_GETTING_WOUNDED_FIRST &&
				i <= ACTION_PRONE_GETTING_WOUNDED_MAX))
				animationInfos[i] |= ANIMATION_INFO_WOUNDED_COMPLEX;
		}
		animationInfos[ACTION_PRONE_WOUNDED_CRAWLING_1] |= ANIMATION_INFO_WOUNDED_SIMPLE;
		animationInfos[ACTION_PRONE_WOUNDED_CRAWLING_2] |= ANIMATION_INFO_WOUNDED_SIMPLE;
		animationInfos[ACTION_CARRIED_FALLING_1] |= ANIMATION_INFO_WOUNDED_SIMPLE;
		animationInfos[ACTION_PRONE_THROWN_BY_FORCE_1] |= ANIMATION_INFO_WOUNDED_COMPLEX;
		animationInfos[ACTION_PRONE_THROWN_BY_FORCE_2] |= ANIMATION_INFO_WOUNDED_COMPLEX;
	}

	//wounded animations: playback direction when hit
	{
		animationWoundedTwichForward[ACTION_STANDING_WOUNDED_FALLING_FORWARD] = false; 
		animationWoundedTwichForward[ACTION_STANDING_WOUNDED_FALLING_BACKWARDS] = true;
		animationWoundedTwichForward[ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD] = true;
		animationWoundedTwichForward[ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS] = false;
		animationWoundedTwichForward[ACTION_PRONE_GETTING_WOUNDED_AIMING_1] = false;
		animationWoundedTwichForward[ACTION_PRONE_GETTING_WOUNDED_AIMING_2] = true;
		animationWoundedTwichForward[ACTION_PRONE_GETTING_WOUNDED_CRAWLING] = false;
		animationWoundedTwichForward[ACTION_PRONE_GETTING_WOUNDED_TAKING_ITEM] = false;
		animationWoundedTwichForward[ACTION_PRONE_GETTING_WOUNDED] = false;
		animationWoundedTwichForward[ACTION_PRONE_WOUNDED_CRAWLING_1] = true;
		animationWoundedTwichForward[ACTION_PRONE_WOUNDED_CRAWLING_2] = true;
		animationWoundedTwichForward[ACTION_PRONE_THROWN_BY_FORCE_1] = false;
		animationWoundedTwichForward[ACTION_PRONE_THROWN_BY_FORCE_2] = true;
		animationWoundedTwichForward[ACTION_CARRIED_FALLING_1] = true;
	}
}

void FootSoldier::release(void)
{
}

FootSoldier::FootSoldier(void)
{
	for (int i = 0; i < ACTION_MAX; i++)
		actions[i] = NULL;
}

FootSoldier::~FootSoldier(void)
{
	for (int i = 0; i < ACTION_MAX; i++)
	{
		if (actions[i] != NULL)
		{
			delete actions[i];
			actions[i] = NULL;
		}
	}
}

void FootSoldier::set(FOOT_SOLDIER_CLASS footSoldierClass, CombatObject::SIDE side, Location startingLocation, Direction startDirection)
{
	CombatAIObject::set(GameObject::FOOT_SOLDIER, startingLocation, startDirection, side);

	//physical object:
	setClass(footSoldierClass);

	setSide(side);
	setLocation(startingLocation);
	setDirection(startDirection);
	setCollisionSphereRadius(FOOT_SOLDIER_COLLISION_SPHERE_RADIUS);
	this->enemyTarget = NULL;
	this->adjust = false; 
	this->firing = false;
	this->throwing_grenade = false;
	this->engaged = false;
	this->computer_controlled = true;
	this->flying = false;
	this->burning = false;
	this->carrying_foot_soldier = false;
	this->carried = false;
	this->medic_target = false;
	this->in_medical_care = false;
	this->screamTimer = 0.0f;
	this->deathTimer = 0.0f;
	this->first_time_playing_frame = true;
	this->frame = 0;
	this->blocked = false;
	this->passing_from_left = false;
	this->passing_from_right = false;
	this->overlapping_soldier = false;
	this->scored = false;
	shadowOutLineRadius = 6.0f * GameObject::objectSize * size/* * 1.6f*/;

	//Personal abilities of this spesific individual
	{
		float rand = randFloat(0.0f, 100.0f);
		ability = 0.0f;
		if (rand >= 1.0f)
		{
			this->reactionTime = randFloat(FOOT_SOLDIER_AVERAGE_REACTION_TIME * (1.0f - FOOT_SOLDIER_AVERAGE_REACTION_TIME_VARIATION), FOOT_SOLDIER_AVERAGE_REACTION_TIME * (1.0f + FOOT_SOLDIER_AVERAGE_REACTION_TIME_VARIATION));
			this->aimingSpeed100Meters = randFloat(FOOT_SOLDIER_AVERAGE_AIMING_SPEED_FOR_100_METERS * (1.0f - FOOT_SOLDIER_AVERAGE_AIMING_SPEED_VARIATION_FOR_100_METERS), FOOT_SOLDIER_AVERAGE_AIMING_SPEED_FOR_100_METERS * (1.0f + FOOT_SOLDIER_AVERAGE_AIMING_SPEED_FOR_100_METERS));
			this->aimingAccuracy = randFloat(FOOT_SOLDIER_AVERAGE_AIMING_ACCURACY * (1.0f - FOOT_SOLDIER_AVERAGE_AIMING_ACCURACY_VARIATION), FOOT_SOLDIER_AVERAGE_AIMING_ACCURACY * (1.0f + FOOT_SOLDIER_AVERAGE_AIMING_ACCURACY_VARIATION));
		}
		else //this guy is a talent
		{
			ability = rand;

			this->reactionTime = FOOT_SOLDIER_AVERAGE_REACTION_TIME - FOOT_SOLDIER_AVERAGE_REACTION_TIME * ability;
			this->aimingSpeed100Meters = FOOT_SOLDIER_AVERAGE_AIMING_SPEED_FOR_100_METERS - FOOT_SOLDIER_AVERAGE_AIMING_SPEED_FOR_100_METERS * ability;
			this->aimingAccuracy = FOOT_SOLDIER_AVERAGE_AIMING_ACCURACY - FOOT_SOLDIER_AVERAGE_AIMING_ACCURACY * ability;
		}

		fallingTimer = HIT_STAY_UP_MAX_TIME + (HIT_STAY_UP_MAX_TIME * ability);

		//The personal speed of a soldier might vary?
		//In reality speed of action depends on:
		//-awareness
		//-awakeness
		//-physical condition
		//-physical status
		//-nutritional status
		//-morale/motivation/fear/panic?
		//-etc...
		//Such speed of action would correlate directly to animation speed.
	}

	//Setting the foot soldier texture by class:
	HTEXTURE footsoldiertex = NULL;
	{
		if (this->side == ATTACKER_SIDE)
		{
			switch (footSoldierClass)
			{
			case RIFLEMAN: footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_ATTACKER_RIFLEMAN]; break;
			case MACHINE_GUNNER: footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_ATTACKER_MACHINE_GUNNER]; break;
			case AT_SOLDIER: footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_ATTACKER_AT_SOLDIER]; break;
			case MEDIC: footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_ATTACKER_MEDIC]; break;
			}
		}
		else if (this->side == DEFENDER_SIDE)
		{
			switch (footSoldierClass)
			{
			case RIFLEMAN: footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_DEFENDER_RIFLEMAN]; break;
			case MACHINE_GUNNER: footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_DEFENDER_MACHINE_GUNNER]; break;
			case AT_SOLDIER: footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_DEFENDER_AT_SOLDIER]; break;
			case MEDIC: footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_DEFENDER_MEDIC]; break;
			}
		}
	}

	//set animations for actions
	{
		for (int i = 0; i < ACTION_MAX; i++)
			actions[i] = NULL;

		int ind = 0;
		actions[ACTION_STANDING_NOT_AIMING] = new hgeAnimation(footsoldiertex, 1, 13.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_WALKING_AIMING] = new hgeAnimation(footsoldiertex, 24, 7.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);	
		actions[ACTION_RUNNING_NOT_AIMING] = new hgeAnimation(footsoldiertex, 8, 13.0f + 13.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);	
		actions[ACTION_STANDING_NOT_AIMING_FIRING] = new hgeAnimation(footsoldiertex, 2, 13.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_STANDING_AIMING_FIRING] = new hgeAnimation(footsoldiertex, 2, 13.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_STANDING_SURRENDERING] = new hgeAnimation(footsoldiertex, 8, 8.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		ind--; //same as previous, but reversed.
		actions[ACTION_STANDING_UNSURRENDERING] = new hgeAnimation(footsoldiertex, 8, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_WALKING_SURRENDERED] = new hgeAnimation(footsoldiertex, 16, 10.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_STANDING_GETTING_PRONE] = new hgeAnimation(footsoldiertex, 6, 9.0f + 9.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);	
		actions[ACTION_STANDING_WOUNDED_FALLING_FORWARD] = new hgeAnimation(footsoldiertex, 13, 13.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_STANDING_WOUNDED_FALLING_BACKWARDS] = new hgeAnimation(footsoldiertex, 16, 13.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD] = new hgeAnimation(footsoldiertex, 13, 10.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS] = new hgeAnimation(footsoldiertex, 14, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_STANDING_DEATH_FALLING_FORWARD_1] = new hgeAnimation(footsoldiertex, randInt(10, 11), 8.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_STANDING_DEATH_FALLING_FORWARD_2] = new hgeAnimation(footsoldiertex, randInt(22, 23), 16.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_STANDING_DEATH_FALLING_BACKWARDS_1] = new hgeAnimation(footsoldiertex, randInt(16, 18), 18.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_STANDING_DEATH_FALLING_BACKWARDS_2] = new hgeAnimation(footsoldiertex, randInt(6, 7), 13.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);	
		actions[ACTION_STANDING_DEATH_FALLING_LEFT_1] = new hgeAnimation(footsoldiertex, randInt(9, 10), 13.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);	
		actions[ACTION_STANDING_DEATH_FALLING_LEFT_2] = new hgeAnimation(footsoldiertex, randInt(15, 16), 8.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);	
		actions[ACTION_STANDING_DEATH_FALLING_RIGHT_1] = new hgeAnimation(footsoldiertex, randInt(22, 23), 13.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);	
		actions[ACTION_STANDING_DEATH_FALLING_RIGHT_2] = new hgeAnimation(footsoldiertex, randInt(16, 18), 13.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);	
		actions[ACTION_PRONE] = new hgeAnimation(footsoldiertex, 1, 0.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_FIRING] = new hgeAnimation(footsoldiertex, 2, 13.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_RELOAD_1] = new hgeAnimation(footsoldiertex, 12, 8.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_RELOAD_2] = new hgeAnimation(footsoldiertex, 20, 8.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_GRENADE_THROWING] = new hgeAnimation(footsoldiertex, 32, 16.0f + 16.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_TAKING_COVER_FROM_INDIRECT_FIRE] = new hgeAnimation(footsoldiertex, 6, 8.0f + 8.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_CRAWLING] = new hgeAnimation(footsoldiertex, 8, SPEED_CRAWLING + SPEED_CRAWLING * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_CRAWLING_LEFT] = new hgeAnimation(footsoldiertex, 4, 10.0f + 10.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_CRAWLING_RIGHT] = new hgeAnimation(footsoldiertex, 4, 10.0f + 10.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_TURNING_LEFT] = new hgeAnimation(footsoldiertex, 6, 7.0f + 7.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_TURNING_RIGHT] = new hgeAnimation(footsoldiertex, 6, 7.0f + 7.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_ADJUSTING_LEFT] = new hgeAnimation(footsoldiertex, 2, 7.0f + 7.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_ADJUSTING_RIGHT] = new hgeAnimation(footsoldiertex, 2, 7.0f + 7.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_GETTING_UP] = new hgeAnimation(footsoldiertex, 12, 16.0f + 16.0f * ability, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);					
		actions[ACTION_PRONE_WOUNDED_CRAWLING_1] = new hgeAnimation(footsoldiertex, 14, 8.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_WOUNDED_CRAWLING_2] = new hgeAnimation(footsoldiertex, 7, 7.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_WOUNDED_TURNING_LEFT] = new hgeAnimation(footsoldiertex, 6, 7.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_WOUNDED_TURNING_RIGHT] = new hgeAnimation(footsoldiertex, 6, 7.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_GETTING_WOUNDED_AIMING_1] = new hgeAnimation(footsoldiertex, 13, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_GETTING_WOUNDED_AIMING_2] = new hgeAnimation(footsoldiertex, 7, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_GETTING_WOUNDED_CRAWLING] = new hgeAnimation(footsoldiertex, 6, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_GETTING_WOUNDED_TAKING_ITEM] = new hgeAnimation(footsoldiertex, 8, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_GETTING_WOUNDED] = new hgeAnimation(footsoldiertex, 6, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_DEATH_AIMING_1] = new hgeAnimation(footsoldiertex, 7, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_DEATH_AIMING_2] = new hgeAnimation(footsoldiertex, 4, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_DEATH_AIMING_3] = new hgeAnimation(footsoldiertex, 14, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_DEATH_AIMING_4] = new hgeAnimation(footsoldiertex, 8, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_DEATH_CRAWLING] = new hgeAnimation(footsoldiertex, 6, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_DEATH_TAKING_ITEM] = new hgeAnimation(footsoldiertex, 7, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_DEATH_1] = new hgeAnimation(footsoldiertex, 15, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_DEATH_2] = new hgeAnimation(footsoldiertex, 6, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_DEATH_3] = new hgeAnimation(footsoldiertex, 13, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
		actions[ACTION_PRONE_THROWN_BY_FORCE_1] = new hgeAnimation(footsoldiertex, 4, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);			
		actions[ACTION_PRONE_THROWN_BY_FORCE_2] = new hgeAnimation(footsoldiertex, 4, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);			
		actions[ACTION_CARRIED_FALLING_1] = new hgeAnimation(footsoldiertex, 8, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);			
		//set animation mode
		for (int i = 0; i < ACTION_MAX; i++)
		{
			if (i == ACTION_CARRIED_FALLING_1)
				int u = 2;

			actions[i]->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
		}
		actions[ACTION_WALKING_AIMING]->SetMode(HGEANIM_FWD | HGEANIM_LOOP);
		actions[ACTION_RUNNING_NOT_AIMING]->SetMode(HGEANIM_FWD | HGEANIM_LOOP);
		actions[ACTION_STANDING_UNSURRENDERING]->SetMode(HGEANIM_REV | HGEANIM_NOLOOP);
		actions[ACTION_WALKING_SURRENDERED]->SetMode(HGEANIM_FWD | HGEANIM_LOOP);
		actions[ACTION_PRONE_CRAWLING]->SetMode(HGEANIM_FWD | HGEANIM_LOOP);
		actions[ACTION_PRONE_CRAWLING_LEFT]->SetMode(HGEANIM_FWD | HGEANIM_LOOP);
		actions[ACTION_PRONE_CRAWLING_RIGHT]->SetMode(HGEANIM_FWD | HGEANIM_LOOP);
		actions[ACTION_PRONE_THROWN_BY_FORCE_1]->SetMode(HGEANIM_FWD | HGEANIM_LOOP);
		actions[ACTION_PRONE_THROWN_BY_FORCE_2]->SetMode(HGEANIM_FWD | HGEANIM_LOOP);

		//animation hot spot
		for (int i = 0; i < ACTION_MAX; i++)
			actions[i]->SetHotSpot(16.0f, 16.0f);

		int hotSpotXMinus6Images[] = {
			ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD,
			ACTION_STANDING_DEATH_FALLING_RIGHT_2	
		};
		for (int i = 0; i < sizeof(hotSpotXMinus6Images) / sizeof(int); i++)
			actions[hotSpotXMinus6Images[i]]->SetHotSpot(10.0f, 16.0f);

		int hotSpotXPlus6Images[] = {
			ACTION_STANDING_WOUNDED_FALLING_BACKWARDS,
			ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS,
			ACTION_STANDING_DEATH_FALLING_BACKWARDS_1,
			ACTION_STANDING_DEATH_FALLING_BACKWARDS_2,
			ACTION_CARRIED_FALLING_1
		};
		for (int i = 0; i < sizeof(hotSpotXPlus6Images) / sizeof(int); i++)
			actions[hotSpotXPlus6Images[i]]->SetHotSpot(22.0f, 16.0f);
	}

	//INIT SOLDIER, LOCK & LOAD!
	{
		clipAmmo = ASSAULT_RIFLE_CLIP_SIZE_FULL;
		assaultRifleFireMode = ASSAULT_RIFLE_FIRE_MODE_SINGLE_SHOT;
		grenades = FOOT_SOLDIER_AMOUNT_OF_GRENADES;	
		setStatus(STATUS_HEALTHY);

		for (int i = 0; i < BODY_PART_MAX; i++)
		{
			woundedBodyParts[i] = 0;
			separatedBodyParts[i] = false;
		}

 		this->currentAction = ACTION_STANDING_NOT_AIMING;
		setAction(ACTION_STANDING_NOT_AIMING);
	}

	//Orders could be given in level script by a commanding unit.
	if (getSide() == CombatObject::ATTACKER_SIDE)
	{
		orders = STATE_ATTACK;
	}
	if (getSide() == CombatObject::DEFENDER_SIDE)
	{
		orders = STATE_DEFEND;
	}
	setState((STATE)orders);
	//Muistio: ƒl‰ lis‰‰ t‰h‰n tavaraa debuggausta varten, vaan game.loadLevel:iss‰.

	this->statisticValue = CombatObject::STATISTIC_VALUE_OTHER;
}


/*
Jalkasotilaan liikkuminen, ampuminen ja esteiden huomioonotto:
Liikkuminen:
-Jos tietyn matkaa jalkasotilan edess‰ on este, jalkasotilas muuttaa suuntaansa niin,
 ett‰ se p‰‰see esteen ymp‰ri.
Ampuminen/t‰ht‰‰minen:
-Jos jalkasotilaan tiell‰, ampumalinjalla on iso este (esim. tankin
 raato, tankki, jalkasotilas ei mene t‰ht‰ysasentoon turhaan. N‰in,
 jos sotilas pys‰htyy makuulteen esim. tankinraadon taakse, h‰n on
 siell‰ ryˆmimisasennossa, kunnes p‰‰tt‰‰ taas jatkaa matkaansa.
*/
void FootSoldier::processAI(void)
{
	currentAction;
	actions[currentAction];
	image;

	//return;
	//processOrders() //ai processes it's orders and then it decides what to do. All orders can't be processed always.
	//Oikeastaan, 'k‰sky' on monimutkainen asia, jonka toteuttaminen vaatii yleens‰ monipuolista toimintaa. Jalkamiehill‰ 
	//saattaa olla esim. t‰ss‰ annettu k‰sky: 'edetk‰‰ vastustajan puolustuslinjaan ja tuhotkaa/otakaa haltuun vastustajan tukikohta'.
	//T‰m‰n toteuttaakseen jalkamiesten pit‰‰ tietenkin liikkua linjaa p‰in, pit‰‰ itsens‰ hengiss‰, eliminoida vihollisia.

	//Kun tullaan hyˆkk‰yset‰isyydelle, aloitetaan hyˆkk‰ys, sit‰ ennen k‰vell‰‰n rauhallisesti eteenp‰in.
	//Jos tankki tulee liian l‰helle, menn‰‰n tankinpakenemismoodiin.
	//Kun hyˆk‰t‰‰n:
	//1. dyykataan maihin
	//2. ammutaan maista: 1. valitaan kohde, 2. t‰hd‰t‰‰n, 3. ammutaan.
	//3. noustaan
	//4. juostaan hieman eteenp‰in.
	//toistetaan
	//Kun paetan tankkia:
	//1. juostaan tankin tulosuunnan vastakkaiseen suuntaan (tai tankin tulosuunntaan loivassa olevassa kulmassa olevaan vaarattomaan suuntaan, johon voidaan juosta). Pys‰hdyt‰‰n, kun et‰isyys tankista on riitt‰v‰n suuri.
	//2. Jatketaan hyˆkk‰yst‰.

	//dbg:
	if (side == DEFENDER_SIDE)
		int u = 2;

	if (footSoldierClass == AT_SOLDIER)
		int u = 2;

	setEnemyTargetProperties();

	//beginning of processing: set new image
	animationControl();

	footSoldierClass;
	number;
	switch (state)
	{
	case STATE_DEFEND: defend(); break;
	case STATE_ATTACK: attack(); break;
	case STATE_FIGHT: fight(); break;
	case STATE_FIRE: processFiring(); break;
	case STATE_RELOAD: reload(); break;
	case STATE_THROW_GRENADE: throwGrenade(); break;
	case STATE_GET_UP: getUp(); break;
	case STATE_RUN_AWAY: runAway(); break;
	//case STATE_RUN_AWAY_FROM_TREE_FALL: runAwayFromTreeFall(); break;
	case STATE_UNDER_INDIRECT_FIRE: takeCoverFromIndirectFire(); break;
	case STATE_THROWN_BY_FORCE: thrown(); break;
	case STATE_BURNING: burn(); break;
	case STATE_WOUNDED: wounded(); break;
	case STATE_DYING: dying(); break;
	case STATE_MEDIC_ACTION: fetchWounded(); break;
	case STATE_BEING_CARRIED: beingCarried(); break;
	case STATE_SURRENDER: surrender(); break;
	case STATE_FLEE: flee(); break;
	default: 
		{
			setOriginalState(); // B)
			break;
		}
	}
	CombatAIObject::processAI();
}

void FootSoldier::processAction(void)
{
	if (isTurning())
		turn();
	if (isMoving())
		move();
	if (flying)
		fly();
	if (direction > 2 * M_PI)
		direction -= 2 * M_PI;
	if (direction < 0.0f)
		direction += 2 * M_PI;
}

void FootSoldier::processInteraction(void)
{
	if (firing)
	{
		fireWeapon();
	}
	if (throwing_grenade)
	{
		game.objects.addGameObject(new Grenade(this, Projectile::HAND_GRENADE, getGrenadeThrowingLocation(), getGrenadeThrowingHeight(), aimingLocation));
		grenades--;
		throwing_grenade = false;
	}
	if (state == STATE_WOUNDED)
	{
		screamTimer -= timerDelta;

		if (statePhase == WOUNDED_PHASE_FALLING)
			fallingTimer -= timerDelta;
	}
	//end of object processing: final actions
	first_time_playing_frame = false;
	startChecks = true;
	overlapping_soldier = false;
}

void FootSoldier::render(void)
{
	if (!imageIsOnScreen())
		return;

	if (game.show_debug_text)
	{
		float help = fnt->GetScale();
		fnt->SetScale(0.12f);
		float spacing = 11.0f;
		float yy = spacing;
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getClassString());
		fnt->printf(x - 20.0f, y + (yy += spacing), "clip ammo: %i, grenades: %i", clipAmmo, grenades);
		fnt->printf(x - 20.0f, y + (yy += spacing), "id: %i", number);
		fnt->printf(x - 20.0f, y + (yy += spacing), "frame: %i", frame);
		fnt->printf(x - 20.0f, y + (yy += spacing), "statePhaseTimer = %.2f", statePhaseTimer);

		if (currentAction >= ACTION_MAX)
			int u = 2;

		fnt->printf(x - 20.0f, y + (yy += spacing), "action: %s", getActionString());
		fnt->printf(x - 20.0f, y + (yy += spacing), "state: %s", getStateString());
		fnt->printf(x - 20.0f, y + (yy += spacing), "statePhase: %s", getStatePhaseString());
		fnt->printf(x - 20.0f, y + (yy += spacing), "distance to move: %f", movingDistance);
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getStateString(previousStates[0][0]));
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getStateString(previousStates[1][0]));
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getStateString(previousStates[2][0]));
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getStateString(previousStates[3][0]));
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getStateString(previousStates[4][0]));
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getStateString(previousStates[5][0]));
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getStateString(previousStates[6][0]));
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getStateString(previousStates[7][0]));
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getStateString(previousStates[8][0]));
		fnt->printf(x - 20.0f, y + (yy += spacing), "%s", getStateString(previousStates[9][0]));
		fnt->printf(x - 20.0f, y + (yy += spacing), "passing left %i passing right %i", passing_from_left, passing_from_right);
		fnt->SetScale(help);
	}

	bool render_shadows = true;

	if (game.currentLevel->night)
	{
		render_shadows = (!burning);
	}

	render_shadows = (getBodyPosition() != BODY_POSITION_PRONE);

	if (render_shadows)
	{
		renderShadows();
	}

	GameObject::render();
}

void FootSoldier::destroy(void)
{
	setStatistics(CombatObject::STATISTIC_VALUE_PERSON_KILLED);

	if (getSide() == CombatObject::DEFENDER_SIDE)
	{
		if (this == game.currentLevel->firstDefenderFootSoldier)
			game.currentLevel->firstDefenderFootSoldier = NULL;
	}
	else 
	{
		if (this == game.currentLevel->firstAttackerFootSoldier)
			game.currentLevel->firstAttackerFootSoldier = NULL;
	}
	this->setDestroyed(true);
}


void FootSoldier::setState(int newState, Time stateChangeTimer, int startStatePhase , bool nextPhaseReturn)
{
	stop();
	CombatAIObject::setState(newState, stateChangeTimer, startStatePhase, nextPhaseReturn);
}

void FootSoldier::attack(void)
{
	switch (statePhase)
	{
	case ATTACK_PHASE_TURN_TO_ATTACK_DIRECTION:
		{
			if (first_time_in_state_phase)
			{
				setTurning(getBattleMainDirection());

				if (game.currentLevel->levelCombatStarted)
				{
					setTurningSpeed(10.0f);

					if (engaged)
					{
						if (getBodyPosition() == BODY_POSITION_STANDING)
						{
							if (compareDistance(getEnemyTargetDistance(), COMBAT_DISTANCE) < 0.0f)
							{
								setStatePhase(ATTACK_PHASE_TAKE_COVER);
								break;
							}
							else
							{
								setTurning(enemyTargetDirection);
								setMoving(ADVANCING_MODE_RUNNING_FORWARD, objectDistance(getEnemyTarget()) - COMBAT_DISTANCE + randFloat(-10.0f, 10.0f));
								setStatePhase(ATTACK_PHASE_ADVANCE);
								first_time_in_state_phase = false; //skip statePhase running orders, run till in combat distance.
								break;
							}
						}
						else
						{
							setStatePhase(ATTACK_PHASE_FIGHT);
							break;
						}
					}
				}

				first_time_in_state_phase = false;
			}

			if (!isTurning())
			{
				nextStatePhase();
			}
			break;
		}
	case ATTACK_PHASE_MOVE_FORWARD:
		{
			if (first_time_in_state_phase)
			{
				setMoving(ADVANCING_MODE_SNEAKING_FORWARD, Game::KILOMETER);
				setSpeed(SPEED_SNEAKING);

				first_time_in_state_phase = false;
			}

			if (compareDistance(getEnemyTargetDistance(), FOOT_SOLDIER_ENEMY_AUTOMATIC_NOTIFICATION_DISTANCE) < 0.0f
				&& !game.currentLevel->smokeTimer > 0.0f)
			{
				nextStatePhase();
			}

			if (game.currentLevel->smokeTimer > 0.0f)
			{
				if (y > OFF_SMOKE_DISTANCE)
				{
					nextStatePhase();
				}
			}

            if (game.currentLevel->levelCombatStarted)
			{
				if (game.currentLevel->isThereOtherSideTroopsAtArea(side))
				{
					nextStatePhase();
				}
			}
			if (game.currentLevel->levelCombatStarted && !isOnScreen())
			{
				if (game.currentLevel->isThereOtherSideTroopsAtArea(side))
				{
					setStatePhase(ATTACK_PHASE_TAKE_COVER);
				}
			}
			break;
		}
	case ATTACK_PHASE_FIRE_RANDOM_SHOTS:
		{
			if (first_time_in_state_phase)
			{
				assaultRifleFireMode = ASSAULT_RIFLE_FIRE_MODE_BURST;
				shots = ASSAULT_RIFLE_SHOT_COUNT_BURST_MODE;

				statePhaseTimer = getReactionTime();

				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				if (clipAmmo <= 0)
				{
					shots = 0;
				}
				else
				{
					Distance firingDistance = 0.0f;
					firingDistance = randFloat(300.0f, 600.0f);
					Angle firingAngle = randFloat(-0.1f, 0.1f); //just random figures
					aimingLocation = ArcPoint(getLocation(), firingDistance, direction + firingAngle).getLocation();
					firing = true;
					setAction(ACTION_STANDING_NOT_AIMING_FIRING);
					shots--;
				}

				if (shots > 0)
				{
					statePhaseTimer = ASSAULT_RIFLE_RATE_OF_FIRE_FULL_AUTO;
				}
				else
				{
					nextStatePhase();
				}
			}
			break;
		}
	case ATTACK_PHASE_GET_DOWN:
		{
			//dive

			if (first_time_in_state_phase)
			{
				setAction(ACTION_STANDING_GETTING_PRONE);

				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{
				//either support or move
				if (randInt(0, 1) == 0)
					setStatePhase(ATTACK_PHASE_FIGHT);
				else
					setStatePhase(ATTACK_PHASE_GET_UP);
			}
			break;
		}
	case ATTACK_PHASE_TAKE_COVER: 
		{
			//dive

			if (first_time_in_state_phase)
			{
				setAction(ACTION_STANDING_GETTING_PRONE);

				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{
				nextStatePhase();
			}
			break;
		}
	case ATTACK_PHASE_CRAWL_SIDEWAYS_1:
		{
			if (first_time_in_state_phase)
			{
				ADVANCING_MODE am;
				if (randInt(0, 1) == 1)
					am = ADVANCING_MODE_CRAWLING_SIDEWAYS_LEFT;
				else
					am = ADVANCING_MODE_CRAWLING_SIDEWAYS_RIGHT;
				setMoving(am, randFloat(ATTACK_MIN_CRAWLING_SIDEWAYS_DISTANCE, ATTACK_MAX_CRAWLING_SIDEWAYS_DISTANCE));
				first_time_in_state_phase = false;				
			}

			if (!isMoving())
			{
				nextStatePhase();
			}
			break;
		}
	case ATTACK_PHASE_FIGHT: 
		{
			if (first_time_in_state_phase)
			{
				setAction(ACTION_PRONE);
				first_time_in_state_phase = false;
			}

			//Time to observe and fight
			if (engaged && isOnScreen())
			{
				setState(STATE_FIGHT, 0, 0, false);
			}
			else
			{
				setStatePhase(ATTACK_PHASE_CRAWL_FORWARD);
			}

			break;
		}
	case ATTACK_PHASE_CRAWL_FORWARD:
		{
			if (first_time_in_state_phase)
			{
				if (getEnemyTarget())
				{
					if (getEnemyTargetDistance() < GRENADE_THROWING_DISTANCE_SQUARED * GameObject::objectSize)
					{
						setStatePhase(ATTACK_PHASE_FIGHT);
						break;
					}
				}

				setMoving(ADVANCING_MODE_CRAWLING_FORWARD, randFloat(ATTACK_MIN_CRAWLING_FORWARD_DISTANCE_DEF, ATTACK_MAX_CRAWLING_FORWARD_DISTANCE_DEF));

				first_time_in_state_phase = false;
			}

			if (!isMoving())
			{
				nextStatePhase();
			}
			break;
		}
	case ATTACK_PHASE_CRAWL_SIDEWAYS_2:
		{
			if (first_time_in_state_phase)
			{
				ADVANCING_MODE am;
				if (randInt(0, 1) == 1)
					am = ADVANCING_MODE_CRAWLING_SIDEWAYS_LEFT;
				else
					am = ADVANCING_MODE_CRAWLING_SIDEWAYS_RIGHT;
				setMoving(am, randFloat(ATTACK_MIN_CRAWLING_SIDEWAYS_DISTANCE, ATTACK_MAX_CRAWLING_SIDEWAYS_DISTANCE));
				first_time_in_state_phase = false;				
			}

			if (!isMoving())
			{
				nextStatePhase();
			}
			break;
		}
	case ATTACK_PHASE_GET_UP: 
		{
			if (first_time_in_state_phase)
			{
				setAction(ACTION_PRONE_GETTING_UP);
				first_time_in_state_phase = false;
			}

			if(isActionFinished())
			{
				if (engaged)
				{					
					nextStatePhase();
				}
				else
				{
					setStatePhase(ATTACK_PHASE_MOVE_FORWARD);
				}
			}			
			break;
		}
	case ATTACK_PHASE_ADVANCE: 
		{
			//run few meters

			if (first_time_in_state_phase)
			{
				if (randFloat(0.0f, 1.0f) < 2.0f)
					setMoving(ADVANCING_MODE_RUNNING_FORWARD, randFloat(ATTACK_MIN_ADVANCING_DISTANCE_DEF * 3.0f, ATTACK_MAX_ADVANCING_DISTANCE_DEF * 3.0f));
				else
					setMoving(ADVANCING_MODE_RUNNING_FORWARD, randFloat(ATTACK_MIN_ADVANCING_DISTANCE_DEF, ATTACK_MAX_ADVANCING_DISTANCE_DEF));

				if (getEnemyTarget())
				{
					if (getEnemyTarget()->getType() == GameObject::BASE)
					{
						setTurning(getEnemyTargetDirection());
					}
					else
						setTurning(getBattleMainDirection());
				}
				else
					setTurning(getBattleMainDirection());
				
				first_time_in_state_phase = false;
			}

			if (!isMoving())
			{
				setStatePhase(ATTACK_PHASE_TAKE_COVER);
			}
			break;					
		}
	default: break;
	}
}

void FootSoldier::defend(void)
{
	switch (statePhase)
	{
	case DEFENSE_PHASE_TURN_TO_MOVING_DIRECTION:
		{
			if (first_time_in_state_phase)
			{
				setAction(ACTION_STANDING_NOT_AIMING);
				setTurning(getBattleMainDirection());

				first_time_in_state_phase = false;
			}

			if (!isTurning())
			{
				nextStatePhase();
			}

			break;
		}
	case DEFENSE_PHASE_MOVE_TO_DEFENSE_LINE:
		{
			if (first_time_in_state_phase)
			{
				if (game.currentLevel->isThereOtherSideTroopsAtArea(side))
				{					
					setMoving(ADVANCING_MODE_RUNNING_FORWARD);
				}
				else
				{
					setMoving(ADVANCING_MODE_SNEAKING_FORWARD);				
				}

				first_time_in_state_phase = false;
			}


			if (getLocation().y <= game.currentLevel->defenderDefenseLineY &&
				randomEvent(0.1f))
			{
				//ryhm‰njohtaja voisi komentaa ryhm‰n maihin t‰ss‰ kohtaa.
				stop();
				nextStatePhase();
			}

			break;
		}
	case DEFENSE_PHASE_GOOD_TO_GET_DOWN:
		{
			if (!isMoving())
			{
				nextStatePhase();
			}
			break;
		}
	case DEFENSE_PHASE_GET_DOWN: //ket taun on dˆ graund!
		{
			if (first_time_in_state_phase)
			{
				setAction(ACTION_STANDING_GETTING_PRONE);
				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{				
				nextStatePhase();
			}
			break;
		}
	case DEFENSE_PHASE_OBSERVE:
		{
			if (first_time_in_state_phase)
			{
				setAction(ACTION_PRONE);
				setEnemyTarget(NULL);
				first_time_in_state_phase = false;
			}

			if (overlapping_soldier && !isMoving())
			{
				return;
			}
			else

			if (!isMoving())
			{
				setAction(ACTION_PRONE);
			}

			//Just observing before contact.
			if (engaged)
			{
				nextStatePhase();
			}

			break;
		}
	case DEFENSE_PHASE_TURN_TO_ENEMY_TARGET_DIRECTION:
		{
			if (first_time_in_state_phase)
			{
				setTurning(getEnemyTargetDirection());
				adjust = true;
				first_time_in_state_phase = false;
			}

			if (!isTurning())
				nextStatePhase();

			break;
		}
	case DEFENSE_PHASE_ADJUST_DIRECTION:
		{
			if (first_time_in_state_phase)
			{
				Direction dir = getEnemyTargetDirection();
				//for fast moving targets
				if (dir < getDirection() - 0.2f ||
					dir > getDirection() + 0.2f)
				{
					setStatePhase(DEFENSE_PHASE_TURN_TO_ENEMY_TARGET_DIRECTION);
					break;
				}
				else //good to adjust?
				{
					setTurningDirection(dir);
					adjust = true;
					if (turning_left)
						setAction(ACTION_PRONE_ADJUSTING_LEFT);
					else if (turning_right)
						setAction(ACTION_PRONE_ADJUSTING_RIGHT);
				}
				first_time_in_state_phase = false;
			}

			if (!isTurning())
			{
				if (hasEnemyTargetCrossedFireStartLevel() || game.currentLevel->levelCombatStarted)
				{
					nextStatePhase();
				}
			}

			break;
		}
	case DEFENSE_PHASE_FIRE_START:
		{
			if (first_time_in_state_phase)
			{
				statePhaseTimer = getReactionTime();
				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				nextStatePhase();				
			}

			break;
		}
	case DEFENSE_PHASE_FIGHT:
		{
			if (engaged)
			{
				setState(STATE_FIGHT, 0.0f, 0, false);
			}
			else //just observe main battle direction if no enemies
			{
				nextStatePhase();
			}
			break;
		}
	case DEFENSE_PHASE_TURN_TO_DEFENSE_DIRECTION:
		{
			if (first_time_in_state_phase)
			{
				setTurning(getBattleMainDirection());
				first_time_in_state_phase = false;
			}

			if (!isTurning())
			{
				setStatePhase(DEFENSE_PHASE_OBSERVE);
			}
			break;
		}
	default: setStatePhase(0); break;
	}
}

void FootSoldier::fight(void)
{
	if (side == ATTACKER_SIDE)
		int u = 2;

	if (footSoldierClass == MACHINE_GUNNER)
		int u = 2;

	switch (statePhase)
	{
	case FIGHTING_PHASE_FIGHT:
		{
			if (getEnemyTarget())
			{ 
				if (getEnemyTarget()->isCombatAIObject())
				{
					CombatAIObject * caio = static_cast<CombatAIObject *>(getEnemyTarget());
					if (caio->isNeutralized())
					{
						nextStatePhase();
						return;
					}
					else if (enemyTargetDistance < GRENADE_THROWING_DISTANCE_SQUARED * GameObject::objectSize)
					{
						if (grenades > 0)
						{
							if (randFloat(0.0f, 1.0f) < PROPABILITY_OF_THROWING_LAST_GRENADE_FIGHTING * grenades)
							{
								setState(STATE_THROW_GRENADE);
							}
						}
					}
					else
					{
						setState(STATE_FIRE);
					}
				}
			}
			else
			{				
				nextStatePhase();
			}
			break;
		}
	case FIGHTING_PHASE_END:
		{
			setEnemyTarget(NULL);
			previousState();
			break;
		}
	default: setStatePhase(0); break;
	}
}	

void FootSoldier::processFiring(void)
{
	//Foot soldier will turn to target direction, aim,
	//fire few times with single shot or burst fire,
	//and then switch back to previous state

	if (getEnemyTarget() == NULL)
		previousState();
	else if (getEnemyTarget()->isDestroyed())
		previousState();
	else if (getEnemyTarget()->isCombatAIObject())
	{
		if (static_cast<CombatAIObject *>(getEnemyTarget())->isNeutralized() && randomEvent(3.0f))		
			previousState();
	}

	switch (statePhase)
	{
	case FIRING_PHASE_TURN:
		{
			if (first_time_in_state_phase)
			{
				setTurning(getEnemyTargetDirection());
				adjust = true;
				first_time_in_state_phase = false;
			}

			//good to aim?
			if (!isTurning())
			{
				//fire few times 
				firingCounter = randInt(1, 5);
				nextStatePhase();
			}		
			break;
		}
	case FIRING_PHASE_ADJUST_POSITION:
		{
			if (number == 29)
				int u = 2;

			if (first_time_in_state_phase)
			{
				Direction dir = getEnemyTargetDirection();
				//for fast moving targets
				if (dir < getDirection() - 0.3f ||
					dir > getDirection() + 0.3f)
				{
					setStatePhase(FIRING_PHASE_TURN);
					break;
				}
				else //good to adjust?
				{
					setTurningDirection(dir);
					adjust = true;
					if (turning_left)
						setAction(ACTION_PRONE_ADJUSTING_LEFT);
					else if (turning_right)
						setAction(ACTION_PRONE_ADJUSTING_RIGHT);
				}
				first_time_in_state_phase = false;
			}

			if (!isTurning())
				nextStatePhase();

			break;
		}
	case FIRING_PHASE_SET_FIRE_MODE:
		{
			float dist = getEnemyTargetDistance();
			
			//If enemy gets close, it's time to fire some burst fire.
			if (compareDistance(dist, ASSAULT_RIFLE_BURST_FIRE_DISTANCE) < 0.0f)
			{
				assaultRifleFireMode = ASSAULT_RIFLE_FIRE_MODE_BURST;
				shots = ASSAULT_RIFLE_SHOT_COUNT_BURST_MODE;
			}
			else
			{
				assaultRifleFireMode = ASSAULT_RIFLE_FIRE_MODE_SINGLE_SHOT;
				shots = 1;
			}

			nextStatePhase();

			break;
		}
	case FIRING_PHASE_AIM:
		{
			if (first_time_in_state_phase)
			{
				//set aiming time:
				//-Aiming time takes a bit longer when target is further.
				float dist = (objectDistance(getEnemyTarget()));
				float m100 = (100.0f * Game::METER);
				float dist100s = dist / m100;
				float ait = getAimingTimeFor100Meters();
				statePhaseTimer =  dist100s  * ait * randFloat(0.5f, 2.0f);

				if (statePhaseTimer < ASSAULT_RIFLE_RATE_OF_FIRE_FULL_AUTO)
					statePhaseTimer = ASSAULT_RIFLE_RATE_OF_FIRE_FULL_AUTO + randFloat(0.05f, 0.1f);

				first_time_in_state_phase = false;
			}
			
			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				nextStatePhase();
			}
			break;
		}
	case FIRING_PHASE_FIRE:
		{
			if (first_time_in_state_phase)
			{
				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer <= 0.0f)
			{
				if (clipAmmo <= 0)
				{
					shots = 0;
					setState(STATE_RELOAD);
					break;
				}

				Location location = getEnemyTargetLocation();
				Distance dist = objectDistance(getEnemyTarget());
				float accuracy = aimingAccuracy * dist * ASSAULT_RIFLE_AIMING_ACCURACY;
				float radius =  ::randFloat(0.0f, accuracy);
				float angle = randomDirection();

				if (randFloat(0.0f, 1.0f) > 0.5f)
				{
					for (int i = 0; i < 1; i++)
					{
						aimingLocation = ArcPoint(location, sqrt(radius * dist), getDirection() + randFloat(-dist/5000.0f, dist/5000.0f)).getLocation();
					}
				}
				else
				{
					ArcPoint a(location, radius, angle);
					aimingLocation = a.getLocation();
				}

				firing = true;
				setAction(ACTION_PRONE_FIRING);
				shots--;

				if (shots > 0)
				{
					statePhaseTimer = ASSAULT_RIFLE_RATE_OF_FIRE_FULL_AUTO;
				}
				else
				{
					if (getEnemyTargetDirection() < getDirection() - 0.1f ||
						getEnemyTargetDirection() > getDirection() + 0.1f)
						setStatePhase(FIRING_PHASE_END);
					else
					{
						firingCounter--;

						if (firingCounter > 0)
						{
							setStatePhase(FIRING_PHASE_SET_FIRE_MODE);
						}
						else
						{
							nextStatePhase();
						}
					}
				}
			}
			break;
		}
	case FIRING_PHASE_END:
		{
			if (first_time_in_state_phase)
			{
				statePhaseTimer = getReactionTime() * 2.0f;
				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
				previousState();
			break;
		}
	default: setStatePhase(FIRING_PHASE_END); break;
	}
}

void FootSoldier::reload(void)
{
	switch(statePhase)
	{
	case RELOAD_PHASE_GET_OLD_CLIP:
		{
			if (first_time_in_state_phase)
			{
				setAction(randInt(ACTION_PRONE_RELOAD_FIRST, ACTION_PRONE_RELOAD_MAX));
				image->SetMode(HGEANIM_FWD);
				first_time_in_state_phase = false;
			}

			if (currentAction == ACTION_PRONE_RELOAD_1 &&
				image->GetFrame() == 8)
			{
				nextStatePhase();
			}
			else if (currentAction == ACTION_PRONE_RELOAD_2 &&
				image->GetFrame() == 16)
			{
				nextStatePhase();
			}			
			break;
		}
	case RELOAD_PHASE_GHANCE_CLIP:
		{
			if (first_time_in_state_phase)
			{
				image->Stop();										
				statePhaseTimer = randFloat(0.5f, 1.0f);
				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				nextStatePhase();
			}
			break;
		}
	case RELOAD_PHASE_PUT_NEW_CLIP:
		{
			if (first_time_in_state_phase)
			{
				image->SetMode(HGEANIM_REV);

				if (currentAction == ACTION_PRONE_RELOAD_1)
					image->SetFrame(8);
				if (currentAction == ACTION_PRONE_RELOAD_2)
					image->SetFrame(16);
				image->Resume();
				first_time_in_state_phase = false;
			}


			if (currentAction == ACTION_PRONE_RELOAD_1 &&
				image->GetFrame() == 2)
			{
				image->SetFrame(3);
				nextStatePhase();
			}
			if (currentAction == ACTION_PRONE_RELOAD_2 &&
				image->GetFrame() == 4)
			{
				image->SetFrame(5);
				nextStatePhase();
			}
			break;
		}
	case RELOAD_PHASE_PULL_LOADER:
		{
			if (first_time_in_state_phase)
			{
				image->SetMode(HGEANIM_REV);

				if (currentAction == ACTION_PRONE_RELOAD_1)
					image->SetFrame(11);
				if (currentAction == ACTION_PRONE_RELOAD_2)
					image->SetFrame(19);
				image->Resume();
				first_time_in_state_phase = false;
			}

			if (currentAction == ACTION_PRONE_RELOAD_1 &&
				image->GetFrame() == 8)
			{
				image->SetFrame(9);
				nextStatePhase();
			}
			else if (currentAction == ACTION_PRONE_RELOAD_2 &&
				image->GetFrame() == 16)
			{
				image->SetFrame(17);
				nextStatePhase();
			}
			break;
		}
	case RELOAD_PHASE_PULL_LOADER_2:
		{
			if (first_time_in_state_phase)
			{
				image->SetMode(HGEANIM_FWD);

				if (currentAction == ACTION_PRONE_RELOAD_1)
					image->SetFrame(9);
				if (currentAction == ACTION_PRONE_RELOAD_2)
					image->SetFrame(17);
				image->Resume();
				first_time_in_state_phase = false;
			}

			if (currentAction == ACTION_PRONE_RELOAD_1 &&
				image->GetFrame() == 10)			
			{
				image->SetFrame(11);
				nextStatePhase();
			}
			if (currentAction == ACTION_PRONE_RELOAD_2 &&
				isActionFinished())
			{
				nextStatePhase();
			}
			break;
		}
	case RELOAD_PHASE_RELEASE_LOADER:
		{
			if (first_time_in_state_phase)
			{
				image->SetMode(HGEANIM_REV);

				if (currentAction == ACTION_PRONE_RELOAD_1)
					image->SetFrame(11);
				if (currentAction == ACTION_PRONE_RELOAD_2)
					image->SetFrame(19);
				image->Resume();
				first_time_in_state_phase = false;
			}

			int frame  = image->GetFrame();

			if (currentAction == ACTION_PRONE_RELOAD_1 &&
				image->GetFrame() == 8)
			{
				image->SetFrame(2);
				nextStatePhase();
			}
			else if (currentAction == ACTION_PRONE_RELOAD_2 &&
				image->GetFrame() == 16)
			{
				image->SetFrame(4);
				nextStatePhase();
			}
			break;
		}
	case RELOAD_PHASE_RELOAD_DONE:
		{
			if (image->GetFrame() == 0)
			{
				setAction(ACTION_PRONE);
				clipAmmo = ASSAULT_RIFLE_CLIP_SIZE_FULL;
				
				previousState();
			}
			break;
		}
	default: break;
	}
}

void FootSoldier::fireAssaultRifle(void)
{
	clipAmmo--;
	playSoundAtLocation(game.sounds[randInt(Game::SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_1, Game::SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_MAX)], 50);
	game.objects.addGameObject(new Effect(Effect::ASSAULT_RIFLE_MUZZLE_FLASH, getBarrelTipLocation(), getDirection()));
	game.objects.addGameObject(new Projectile(this, Projectile::ASSAULT_RIFLE_BULLET, getBarrelTipLocation(8.0f), getBarrelTipHeight(), aimingLocation));
	game.objects.addGameObject(new Effect(Effect::BULLET_SHELL_FLY, Location(x + 13.0f * GameObject::objectSize * cos(direction), y + 13.0f * GameObject::objectSize * sin(direction)), direction + 1.3f));
	firing = false;
}

void FootSoldier::throwGrenade(void)
{
	if (getEnemyTarget() == NULL)
		previousState();

	switch (statePhase)
	{
	case GRENADE_THROWING_PHASE_TURN:
		{
			{
				nextStatePhase();
			}		
			break;
		}
	case GRENADE_THROWING_PHASE_THROW:
		{
			if (first_time_in_state_phase)
			{
				if (getEnemyTarget())
					aimingLocation = getEnemyTargetLocation();
				else //debug
					aimingLocation = ArcPoint(getLocation(), randFloat(60.0f, 100.0f), direction + randFloat(-0.2f, 0.2f)).getLocation(); //some random numbers there from the top of my head

				setAction(ACTION_PRONE_GRENADE_THROWING);
				first_time_in_state_phase = false;
			}

			if (image->GetFrame() == 25 &&
				first_time_playing_frame)
			{
				throwing_grenade = true;
			}

			if (isActionFinished())
			{
				previousState();
			}

			break;
		}
	default: setStatePhase(0); break;
	}
}

void FootSoldier::getUp(void)
{
	switch (statePhase)
	{
	case GET_UP_PHASE_GET_UP:
		{
			if (first_time_in_state_phase)
			{
				if (getBodyPosition() == BODY_POSITION_PRONE)
				{
					if (currentAction != ACTION_PRONE_GETTING_UP)
					{
						setAction(ACTION_PRONE_GETTING_UP);
						image->SetFrame(1); //for generic animation
					}
				}
				else
				{
					nextStatePhase();
					break;
				}

				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{
				nextStatePhase();
			}

			break;
		}
	case GET_UP_PHASE_FINISHED:
		{
			previousState();
				
			break;
		}
	default: setStatePhase(0);
	}
}

void FootSoldier::runAway(void)
{
	switch(statePhase)
	{
	case STATE_RUN_AWAY_PHASE_GET_UP:
		{
			setState(STATE_GET_UP);
			break;
		}
	case STATE_RUN_AWAY_PHASE_TURNING:
		{
			if (first_time_in_state_phase)
			{
				if (getBodyPosition() == BODY_POSITION_PRONE)
				{
					setStatePhase(STATE_RUN_AWAY_PHASE_GET_UP);
				}
				setTurning(getTurningDirection());
				setTurningSpeed(10.0f);
				first_time_in_state_phase = false;
			}

			if (!isTurning())
			{
				nextStatePhase();
			}
			break;
		}
	case STATE_RUN_AWAY_PHASE_RUNNING:
		{
			if (first_time_in_state_phase)
			{
				setMoving(ADVANCING_MODE_RUNNING_FORWARD, getMovingDistance());
				first_time_in_state_phase = false;
			}

			if (!isMoving())
			{
				nextStatePhase();
			}

			break;
		}
	case STATE_RUN_AWAY_PHASE_CLEAR:
		{
			if (first_time_in_state_phase)
			{
				if (getEnemyTarget())
				{
					setAction(ACTION_STANDING_NOT_AIMING);
					setTurning(getEnemyTargetDirection());
					setTurningSpeed(20.0f);
				}
				first_time_in_state_phase = false;
			}

			if (!isTurning())
			{
				setState(orders);
			}

			break;
		}
	default: setStatePhase(0); break;
	}
}

void FootSoldier::takeCoverFromIndirectFire(void)
{
	switch (statePhase)
	{
	case UNDER_INDIRECT_FIRE_PHASE_GET_DOWN:
		{
			if (first_time_in_state_phase)
			{
				if (getBodyPosition() == BODY_POSITION_STANDING)
				{
					setAction(ACTION_STANDING_GETTING_PRONE);
				}
				else
				{
					nextStatePhase();
					break;
				}

				first_time_in_state_phase = false;
			}

			if (isActionFinished())
				nextStatePhase();

			break;
		}
	case UNDER_INDIRECT_FIRE_PHASE_TAKE_COVER:
		{
			if (first_time_in_state_phase)
			{
				setAction(ACTION_PRONE_TAKING_COVER_FROM_INDIRECT_FIRE);

				first_time_in_state_phase = false;
			}

			if (isActionFinished())
				nextStatePhase();

			break;
		}
	case UNDER_INDIRECT_FIRE_PHASE_STAY_DOWN:
		{					
			if (first_time_in_state_phase)
			{
				statePhaseTimer = INDIRECT_FIRE_END_WAIT_TIME;

				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;
			if (statePhaseTimer < 0.0f)
			{
				nextStatePhase();
			}

			break;
		}
	case UNDER_INDIRECT_FIRE_PHASE_END:
		{
			if (first_time_in_state_phase)
			{
				statePhaseTimer = getReactionTime() * 3.0f;

				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				if (side == ATTACKER_SIDE &&
					attacker_taking_cover)
					attacker_taking_cover = false;
				else if (side == DEFENDER_SIDE &&
					defender_taking_cover)
					defender_taking_cover = false;

				previousState();
			}

			break;
		}
	default: setStatePhase(0); break;
	}
}

void FootSoldier::thrown(void)
{
	switch (statePhase)
	{
	case THROWN_BY_FORCE_PHASE_FLYING:
		{
			if (!flying)
				nextStatePhase();
			break;
		}
	case THROWN_BY_FORCE_PHASE_GOT_DOWN:
		{
			if (getStatus() == STATUS_WOUNDED)
			{
				setWoundedAction();
				setStatus(STATUS_WOUNDED);
				setState(STATE_WOUNDED);
			}
			else if (getStatus() == STATUS_SEVERELY_WOUNDED)
			{
				setDeathAction();
				setState(STATE_DYING);
			}
			else
				previousState();
			break;
		}
	default: setStatePhase(0); break;
	}
}

void FootSoldier::burn(void)
{
	switch (statePhase)
	{
	case BURNING_PHASE_GET_UP:
		{
			if (first_time_in_state_phase)
			{
				if (getBodyPosition() == BODY_POSITION_PRONE)
				{
					setAction(ACTION_PRONE_GETTING_UP);
				}
				else
				{
					nextStatePhase();
					break;
				}

				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{
				nextStatePhase();
			}
			break;
		}
	case BURNING_PHASE_RUN:
		{
			if (first_time_in_state_phase)
			{
				setTurning(randomDirection());
				setMoving(ADVANCING_MODE_RUNNING_FORWARD, randFloat(200.0f, 300.0f));
				first_time_in_state_phase = false;
			}
			
			if (!isMoving())
			{
				nextStatePhase();
			}

			break;
		}
	case BURNING_PHASE_FALL:
		{
			if (first_time_in_state_phase)
			{
				setDeathAction();
				setStatus(STATUS_SEVERELY_WOUNDED);	
				setState(STATE_DYING);
				first_time_in_state_phase = false;
			}

			break;
		}
	default: setStatePhase(0);
	}
}

void FootSoldier::wounded(void)
{
	/*
	Wounded phase and wounded animations:
	In the .pdn files of foot soldiers, in "notes" layer, there
	are frames marked 'a' ,'b', and 'c'.
	-'a' is the last frame of getting wounded.
	-When a wounded soldier leaves lying on the ground, his animation
	is played forward and back between frames 'a' and 'b' to 
	simulate some twitching on the ground.
	-When a soldier decides to move back to a safe direction,
	frames 'a' to 'c' are played to simulate the change of position
	into a turning-when-wounded position. After that the animation
	is changed into the crawling-when-wounded animation (which
	depends a bit of where he has got hit).
	-If a wounded lying soldier dies (either bleeds out or gets hit)
	the animation is played forward from frame 'd'.
	*/

	if (in_medical_care)
	{
		setStatePhase(WOUNDED_PHASE_IN_MEDICAL_ATTENTION);
	}

	switch (statePhase)
	{
	case WOUNDED_PHASE_FALLING:
		{
			if (first_time_in_state_phase)
			{
				setStatistics(STATISTIC_VALUE_PERSON_WOUNDED);
				first_time_in_state_phase = false;
			}

			if (animationInfos[currentAction] & ANIMATION_INFO_WOUNDED_COMPLEX)
			{
				if (!animationWoundedTwichForward[currentAction])
				{
					if (image->GetFrame() == animationWoundedFrames[currentAction] - 1 &&
						first_time_playing_frame)
					{
						image->Stop();
						nextStatePhase();
						break;
					}
				}
			}
			
			if (isActionFinished())
			{
				setBodyPosition(BODY_POSITION_PRONE);
				nextStatePhase();
			}
			break;
		}
	case WOUNDED_PHASE_CHECK_STATUS:
		{
			if (first_time_in_state_phase)
			{
				//lay lo for a while
				statePhaseTimer = randFloat(5.0f, 10.0f);
				first_time_in_state_phase = false;				
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				bool move = (randFloat(0.0f, 1.0f) > 0.33f);

				//Can't drag yourself with both arms down.
				if (getNumberOfWounds(BODY_PART_LEFT_ARM) > 0 &&
					getNumberOfWounds(BODY_PART_RIGHT_ARM) > 0)
				{
					move = false;
				}
				
				//if (true)
				if (!move)
				{
					setStatePhase(WOUNDED_PHASE_LYING);
					break;
				}

				nextStatePhase();
			}
			break;
		}
	case WOUNDED_PHASE_CHANGE_POSITION:
		{
			//change position for turning
			if (first_time_in_state_phase)
			{
				switch (currentAction)
				{
				case ACTION_PRONE_WOUNDED_CRAWLING_1:
				case ACTION_PRONE_WOUNDED_CRAWLING_2: break;
				case ACTION_CARRIED_FALLING_1:
					{
						setAction(ACTION_PRONE_GETTING_WOUNDED_TAKING_ITEM);
						setDirection(direction + M_PI);

						//NO BREAK
					}
				default:
					{
						//set frames till 'c':
						image->SetFrames(animationWoundedFrames[currentAction] + 5);
						image->SetSpeed(8.0f);
						image->SetMode(HGEANIM_FWD);
						image->Play();
						image->SetFrame(animationWoundedFrames[currentAction] + 2);
						break;
					}
				}

				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{
				nextStatePhase();
			}

			break;
		}
	case WOUNDED_PHASE_TURNING:
		{
			if (first_time_in_state_phase)
			{
				//action specific processing:
				if (currentAction == ACTION_STANDING_WOUNDED_FALLING_BACKWARDS)
				{
					setDirection(getDirection() + M_PI);
				}

				//Missing realism (simplification):
				//Soldier can turn direction using broken arm.
				//If anyone spots this 'bug', I'm pleased that
				//someone pays attention.

				//turn towards safe direction
				setBodyPosition(BODY_POSITION_PRONE); //for falling-from-standing animations
				setTurning(getBattleMainDirection() + M_PI + randFloat(-0.5f, 0.5f)); //random numbers
				first_time_in_state_phase = false;
			}

			if (!isTurning())
			{
				nextStatePhase();
			}
			break;
		}
	case WOUNDED_PHASE_MOVING:
		{
			if (first_time_in_state_phase)
			{
				setMoving(ADVANCING_MODE_CRAWLING_FORWARD, randFloat(0.0f, 1000.0f));
				statePhaseTimer = randFloat(2.0f, 5.0f);
				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				//can't carry on?
				if (randInt(1, 20) == 1)
				{
					nextStatePhase();
					break;
				}

				image->Play();
				statePhaseTimer = randFloat(2.0f, 5.0f);
			}
			break;
		}
	case WOUNDED_PHASE_LYING:
		{
			if (first_time_in_state_phase)
			{
				stop();
				setWoundedAction();

				statePhaseTimer = randFloat(3.0f, 6.0f);

				first_time_in_state_phase = false;
			}

			//lie here till help arrives (or death)

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				//death?
				if (randFloat(0.0f, 1.0f) < 0.03)
				{				
					setDeathAction();		
					setStatus(STATUS_SEVERELY_WOUNDED);	
					setState(STATE_DYING);
					break;
				}
	
				if (image->GetMode() & HGEANIM_REV)
				{
					image->SetMode(HGEANIM_FWD);
					image->SetFrame(animationWoundedFrames[currentAction] - 1);
				}
				else
					image->SetMode(HGEANIM_REV);

				image->Resume();
				statePhaseTimer = randFloat(3.0f, 6.0f);//randFloat(3.0f, 5.0f);
			}

			if (image->GetMode() & HGEANIM_REV &&
				image->GetFrame() == image->GetFrames() - 3)
			{
				image->Stop();
			}

			break;
		}
	case WOUNDED_PHASE_IN_MEDICAL_ATTENTION:
		{
			stop();
			break;
		}
	default: setStatePhase(0); break;
	}
}

void FootSoldier::dying(void)
{
	switch (statePhase)
	{
	case DYING_PHASE_FALLING:
		{
			if (first_time_in_state_phase)
			{
				//set statistics
				setStatistics(STATISTIC_VALUE_PERSON_KILLED);
				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{				
				nextStatePhase();
			}
			break;
		}
	case DYING_PHASE_LYING:
		{
			if (first_time_in_state_phase)
			{				
				first_time_in_state_phase = false;
			}

			deathTimer -= timerDelta;

			if (deathTimer < 0.0f)
				nextStatePhase();

			break;
		}
	case DYING_PHASE_DEATH:
		{
			if (first_time_in_state_phase)
			{
				setStatus(STATUS_DEAD);
				game.objects.background->modify(image, x, y, direction);
				visible = false;
				//add a blood pond - needs work if implemented!
				//game.objects.addGameObject(new Effect(Effect::BLOOD_POND, getLocation(), direction));
				destroy();

				first_time_in_state_phase = false;
			}

			//ad eternum

			break;
		}
	default: setStatePhase(0); break;
	}
}

void FootSoldier::beingCarried(void)
{
	switch (statePhase)
	{
	case BEING_CARRIED_PHASE_BE_CARRIED:
		{
			if (!carried)
			{
				setState(STATE_WOUNDED, 0.0f, 0);
			}

			break;
		}
	default: setStatePhase(0); break;
	}
}

void FootSoldier::surrender(void)
{
	switch (statePhase)
	{
	case SURRENDER_PHASE_GET_UP:
		{
			setState(STATE_GET_UP);
			break;
		}
	case SURRENDER_PHASE_LIFT_HANDS_IN_THE_AIR:
		{
			if (first_time_in_state_phase)
			{
				setAction(ACTION_STANDING_SURRENDERING);

				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{
				nextStatePhase();
			}
			break;
		}
	case SURRENDER_PHASE_WALK_TO_CAPTIVITY:
		{
			if (first_time_in_state_phase)
			{
				setMoving(ADVANCING_MODE_WALKING_FORWARD, 3000.0f);
				setTurning((side == CombatObject::ATTACKER_SIDE)? 1.0f:ONE_AND_A_HALF_PI);
				first_time_in_state_phase = false;
			}

			break;
		}
	default: setStatePhase(0); break;
	}
}

void FootSoldier::flee(void)
{
	switch (statePhase)
	{
	case FLEE_PHASE_GET_UP:
		{
			if (getBodyPosition() == BODY_POSITION_PRONE)
			{
				setState(STATE_GET_UP);
			}
			else 
			{
				nextStatePhase();
			}

			break;
		}
	case FLEE_PHASE_RUN_TO_SAFETY:
		{
			if (first_time_in_state_phase)
			{
				setTurning(getSafeDirection());
				setTurningSpeed(10.0f);
				setMoving(ADVANCING_MODE_RUNNING_FORWARD, 2000.0f);
				statePhaseTimer = randFloat(5.0f, 10.0f);
				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				nextStatePhase();
			}

			break;
		}
	case FLEE_PHASE_SAFE:
		{
			if (first_time_in_state_phase)
			{
				setActive(false);
				setStatistics(CombatObject::STATISTIC_VALUE_PERSON_FLED);
				first_time_in_state_phase = false;
			}
		}
	default: setStatePhase(0); break;
	}
}

void FootSoldier::checkTank(Tank * tank)
{
	//check if tank is too near
	if (
		getStatus() != STATUS_WOUNDED &&
		getStatus() != STATUS_SEVERELY_WOUNDED)
	{
		if (!tank->isNeutralized())
		{
			float tankDist = 0.0f, runningDist = 0.0f;
			if (tank->side != this->side)
			{
				tankDist = ENEMY_TANK_MIN_DISTANCE;
				runningDist = ENEMY_TANK_RUN_DISTANCE;
			}
			else
			{
				tankDist = FRIENDLY_TANK_MIN_DISTANCE;
				runningDist = 20.0f;
			}

			if (compareDistance(tank, tankDist) < 0.0f && randomEvent(0.2f))
			{
				Direction dir = objectAngle(tank) + M_PI;
				Distance dist = runningDist * randomVariation(0.2f);

				if (state == STATE_RUN_AWAY)
				{
					setDirection(dir);					
					setMovingDistance(dist);
				}
				else
					setRunningAway(dir, dist);
			}
		}
	}
}

void FootSoldier::setSurrendering(void)
{
	if (state != STATE_UNDER_INDIRECT_FIRE &&
		!isNeutralized())
	{
		surrendered = true;
		setActive(false);
		setState(STATE_SURRENDER);
		setStatistics(CombatObject::STATISTIC_VALUE_PERSON_CAPTURED);
	}
}

void FootSoldier::setFleeing(void)
{
	if (state != STATE_UNDER_INDIRECT_FIRE &&
		!isNeutralized())
	{
		fleeing = true;
		setActive(false);
		setState(STATE_FLEE);
		setStatistics(CombatObject::STATISTIC_VALUE_PERSON_FLED);
	}
}

void FootSoldier::setAction(int nextAction, bool displacement)
{
	if (displacement)
	{
		//set animation caused object displacement
		if (animationDisplacements[currentAction].x != 0.0f ||
			animationDisplacements[currentAction].y != 0.0f)
		{
			float a = animationDisplacements[currentAction].x;
			float b = animationDisplacements[currentAction].y;
			float displacement = sqrt(a * a + b * b);
			float displacementDirection = acos(a / displacement);
			x = x + displacement * cos(direction + displacementDirection)
			y = y + displacement * sin(direction + displacementDirection)
		}
	}

	//set animation
	currentAction = (ACTION)nextAction;
	image = actions[nextAction];
	if (image)
		image->Play();
	frame = image->GetFrame();
	first_time_playing_frame = true;

	//set body position
	setBodyPosition((BODY_POSITION)animationBodyPositions[currentAction]);
}

bool FootSoldier::isActionFinished(void)
{
	return (!image->IsPlaying());
}

void FootSoldier::setMoving(ADVANCING_MODE advancingMode, Distance distance)
{
	setAdvancingMode(advancingMode);

	bool starting_side_switch = false;

	float speedVariation = randFloat(0.9f, 1.1f);

	if (game.skillLevel == Game::SKILL_LEVEL_EASY) speedVariation -= 0.3f;
	if (game.skillLevel == Game::SKILL_LEVEL_HARD) speedVariation += 0.3f;

	if (advancingMode == ADVANCING_MODE_WALKING_FORWARD)
	{
		if (surrendered)
		{
			setAction(ACTION_WALKING_SURRENDERED);
		}

		setSpeed(SPEED_WALKING * speedVariation);
		setMovingDirection(getDirection_FORWARD());
		starting_side_switch = true;
	}
	else if (advancingMode == ADVANCING_MODE_RUNNING_FORWARD)
	{
		setAction(ACTION_RUNNING_NOT_AIMING);		
		starting_side_switch = true;
		setMovingDirection(getDirection_FORWARD());

		if (!carrying_foot_soldier)
			setSpeed((SPEED_RUNNING + SPEED_RUNNING * ability) * speedVariation);
		else
			setSpeed((SPEED_RUNNING_CARRYING_FOOT_SOLDIER + SPEED_RUNNING_CARRYING_FOOT_SOLDIER * ability) * speedVariation);
	}
	else if (advancingMode == ADVANCING_MODE_SNEAKING_FORWARD)
	{
		setAction(ACTION_WALKING_AIMING);
		starting_side_switch = true;
		setMovingDirection(getDirection_FORWARD());
		setSpeed(SPEED_SNEAKING * speedVariation);
	}
	else if (advancingMode == ADVANCING_MODE_CRAWLING_FORWARD)
	{
		if (status == STATUS_HEALTHY)
		{
			setAction(ACTION_PRONE_CRAWLING);
			setSpeed((SPEED_CRAWLING + SPEED_CRAWLING * ability) * speedVariation);
			starting_side_switch = true;			
		}
		else if (status == STATUS_WOUNDED)
		{
			actions[ACTION_PRONE_WOUNDED_CRAWLING_1]->SetFrames(15);
			actions[ACTION_PRONE_WOUNDED_CRAWLING_2]->SetFrames(7);

			if (getNumberOfWounds(BODY_PART_LEFT_ARM) > 0 &&
				getNumberOfWounds(BODY_PART_RIGHT_ARM) > 0)
				return;
			else if (getNumberOfWounds(BODY_PART_LEFT_ARM) > 0)
				setAction(ACTION_PRONE_WOUNDED_CRAWLING_1);
			else if (getNumberOfWounds(BODY_PART_RIGHT_ARM) > 0)
				setAction(ACTION_PRONE_WOUNDED_CRAWLING_2);
			else
				setAction((ACTION)randInt((int)ACTION_PRONE_WOUNDED_CRAWLING_FIRST, (int)ACTION_PRONE_WOUNDED_CRAWLING_MAX));

			image->GetFrame();

			setSpeed(SPEED_CRAWLING_WOUNDED * speedVariation);
		}
		setMovingDirection(getDirection_FORWARD());
	}
	else if (advancingMode == ADVANCING_MODE_CRAWLING_SIDEWAYS_LEFT)
	{
		setAction(ACTION_PRONE_CRAWLING_LEFT);
		setMovingDirection(getDirection_LEFT());
		setSpeed((SPEED_CRAWLING_SIDEWAYS + SPEED_CRAWLING_SIDEWAYS * ability) * speedVariation);
	}
	else if (advancingMode == ADVANCING_MODE_CRAWLING_SIDEWAYS_RIGHT)
	{
		setAction(ACTION_PRONE_CRAWLING_RIGHT);
		setMovingDirection(getDirection_RIGHT());
		setSpeed((SPEED_CRAWLING_SIDEWAYS + SPEED_CRAWLING_SIDEWAYS * ability) * speedVariation);
	}

	if (starting_side_switch)
	{
		if (randInt(0, 1) == 0) image->SetFrame(image->GetFrames() / 2);
	}

	setMovingDistance(distance);
	moving = true;
}

void FootSoldier::setTurning(Direction turningDirection)
{
	if (state == STATE_RUN_AWAY)
		int u = 2;
	if (side == DEFENDER_SIDE)
		int u = 2;

	setTurningDirection(turningDirection);
	float speedVariation = randFloat(0.9f, 1.1f);

	if (getBodyPosition() == BODY_POSITION_PRONE)
	{
		stopMoving();
		if (status == STATUS_HEALTHY)
		{
			if (turning_left)
				setAction(ACTION_PRONE_TURNING_LEFT);
			else if (turning_right)
				setAction(ACTION_PRONE_TURNING_RIGHT);
		}
		else if (status == STATUS_WOUNDED)
		{
			if (turning_left)
				setAction(ACTION_PRONE_WOUNDED_TURNING_LEFT);
			else if (turning_right)
				setAction(ACTION_PRONE_WOUNDED_TURNING_RIGHT);
		}
	}
	else if (getBodyPosition() == BODY_POSITION_STANDING)
	{
		if (isMoving())
		{
			if (advancingMode == ADVANCING_MODE_SNEAKING_FORWARD)
				setTurningSpeed(TURNING_SPEED_SNEAKING * speedVariation);
			else if (advancingMode == ADVANCING_MODE_RUNNING_FORWARD)
				setTurningSpeed((TURNING_SPEED_RUNNING + TURNING_SPEED_RUNNING * ability) * speedVariation);
			else if (advancingMode == ADVANCING_MODE_WALKING_FORWARD)
				setTurningSpeed(TURNING_SPEED_WALKING * speedVariation);
		}
		else
			setTurningSpeed(10.0f * speedVariation);
	}
}

void FootSoldier::setRunningAway(Direction direction, Distance distance)
{
	if (getStatus() != STATUS_HEALTHY)
		return;

	setTurningDirection(direction);
	setMovingDistance(distance);
	setState(STATE_RUN_AWAY);
}

/*
Jalkasotilaan toiminta, kun halutun suunnan tiell‰ on este, joka est‰‰ liikkumisen:
-Valitaan suunta, jota kautta on lyhyin matka kohteeseen.

Toiminta, jos tulitettavan viholliskohteen tiell‰ on este
-Valitaan uusi viholliskohde
-Teknisesti:
 -Merkit‰‰n ylˆs, jos havaitaan, ett‰ viholliskohteen ja itsen v‰liss‰ on este.
 -Valitaan seuraavalla objektien checkauksella toisiksi viimeinen vaihtoehto sopivaksi viholliseksi
-Tai:
 -Merkataan ylˆs ja valitaan randomi vihollinen tulitettavaksi seuraavaksi.
*/

void FootSoldier::move(void)
{

	switch (currentAction)
	{
	case ACTION_PRONE_CRAWLING:
		{
			if ((frame == 2 ||
				frame == 3 ||
				frame == 6 ||
				frame == 7) &&
				first_time_playing_frame)
			{
				pixelMove();
			}

			break;
		}
	case ACTION_PRONE_CRAWLING_LEFT:
	case ACTION_PRONE_CRAWLING_RIGHT:
		{
			if ((frame == 2 ||
				frame == 3) &&
				first_time_playing_frame)
			{
				pixelMove();
			}
			break;
		}
	case ACTION_PRONE_WOUNDED_CRAWLING_1:
		{
			if (image->GetFrame() >= 8 &&
				first_time_playing_frame)
			{
				pixelMove(0.5f);
			}
			break;
		}
	case ACTION_PRONE_WOUNDED_CRAWLING_2:
		{
			if (image->GetFrame() >= 4 &&
				first_time_playing_frame)
			{
				pixelMove();
			}
			break;
		}
	default: CombatAIObject::move();
	}
}

void FootSoldier::turn(void)
{
	if (bodyPosition == BODY_POSITION_PRONE)
	{
		if (currentAction == ACTION_PRONE_TURNING_LEFT ||
			currentAction == ACTION_PRONE_TURNING_RIGHT ||
			currentAction == ACTION_PRONE_WOUNDED_TURNING_LEFT ||
			currentAction == ACTION_PRONE_WOUNDED_TURNING_RIGHT)
		{
			if (adjust && angleToTurn < PRONE_ANIMATION_TURNING_ANGLE)
			{
				stopTurning();						
				adjust = false;
			}
			else if (isActionFinished())
			{
				if (turning_left)
					direction -= PRONE_ANIMATION_TURNING_ANGLE;
				else if (turning_right)
					direction += PRONE_ANIMATION_TURNING_ANGLE;
				angleToTurn -= PRONE_ANIMATION_TURNING_ANGLE;
				setAction(currentAction);

				if (angleToTurn < PRONE_ANIMATION_TURNING_ANGLE)
				{
					if (!adjust)
					{
						if (turning_left)
							direction -= angleToTurn;
						else if (turning_right)
							direction += angleToTurn;

						angleToTurn = 0.0f;
						stopTurning();

						if (getStatus() == STATUS_HEALTHY)
							setAction(ACTION_PRONE);	
					}
				}
			}
		}
		else if (currentAction == ACTION_PRONE_ADJUSTING_LEFT ||
				currentAction == ACTION_PRONE_ADJUSTING_RIGHT)
		{
			if (isActionFinished())
			{
				if (angleToTurn < PRONE_ANIMATION_ADJUSTING_ANGLE)
				{
					if (turning_left)
						direction -= angleToTurn;
					else if (turning_right)
						direction += angleToTurn;

					angleToTurn = 0.0f;
					stopTurning();
					setAction(ACTION_PRONE);
				}
				else
				{
					if (turning_left)
						direction -= PRONE_ANIMATION_ADJUSTING_ANGLE;
					else if (turning_right)
						direction += PRONE_ANIMATION_ADJUSTING_ANGLE;
					angleToTurn -= PRONE_ANIMATION_ADJUSTING_ANGLE;
					setAction(currentAction);
				}
			}
		}
		else
			stopTurning();
	}
	else
		CombatAIObject::turn();
}

void FootSoldier::stop(void)
{
	switch (currentAction)
	{
	case ACTION_WALKING_AIMING: 
	case ACTION_RUNNING_NOT_AIMING:
		{
			setAction(ACTION_STANDING_NOT_AIMING);
			break;
		}
	case ACTION_PRONE_CRAWLING:
	case ACTION_PRONE_CRAWLING_LEFT:
	case ACTION_PRONE_CRAWLING_RIGHT:
	case ACTION_PRONE_TURNING_LEFT:
	case ACTION_PRONE_TURNING_RIGHT:
	//default: 
		{
			image->Stop();
			break;
		}
	}
	MovingObject::stop();
}

void FootSoldier::fly(void)
{
	speed -= deacceleration * timerDelta;
	dx = speed * cos(movingDirection) * timerDelta;
	dy = speed * sin(movingDirection) * timerDelta;
	x += dx;
	y += dy;

	if (speed <= 0.0f)
		flying = false;
}

void FootSoldier::collide(GameObject * otherObject, ArcPoint hittingPoint)
{
	//No collisions when carried for now.
	if (carried)
		return;

	if (!isOnScreen())
		return;

	if (otherObject->type == PROJECTILE)
	{
		collide(static_cast<Projectile *>(otherObject), hittingPoint);
	}
	else if (otherObject->type == GameObject::TANK)
	{
		collide(static_cast<Tank *>(otherObject), hittingPoint);
	}
	else if (otherObject->type == GameObject::EFFECT)
	{
		Effect * e = static_cast<Effect *>(otherObject);
		if (e->collisionTimer > 0.0f)
			collide(e, hittingPoint);
	}
	else if (otherObject->type == GameObject::FOOT_SOLDIER)
	{
		collide(static_cast<FootSoldier *>(otherObject), hittingPoint);
	}
}

void FootSoldier::collide(Projectile * projectile, ArcPoint hittingPoint)
{
	//only players shoot fleeing soldiers and medics
	if (fleeing || footSoldierClass == MEDIC)
	{
		if (projectile->shooter->type != TANK)
			return;
		Tank * t = static_cast<Tank *>(projectile->shooter);

		if (t->tankType != Tank::PLAYER_1_TANK &&
			t->tankType != Tank::PLAYER_2_TANK)
			return;			
	}

	if (!game.friendly_fire && 
		projectile->getSide() == getSide())
			return;

	{
		float rand = randFloat(0.0f, 1.0f);
		//Harder to hit prone
		if (getBodyPosition() == BODY_POSITION_PRONE) rand += 0.2f;
		if (side == DEFENDER_SIDE)
		{
			if (rand > FOOTSOLDIER_POSSIBILITY_OF_HIT_NORMAL)
				return;
		}
		else if (side == ATTACKER_SIDE)
		{
			if (game.skillLevel == Game::SKILL_LEVEL_EASY)
			{
				if (rand > FOOTSOLDIER_POSSIBILITY_OF_HIT_EASY)
					return;
			}
			else if (game.skillLevel == Game::SKILL_LEVEL_NORMAL)
			{
				if (rand > FOOTSOLDIER_POSSIBILITY_OF_HIT_NORMAL)
					return;
			}
			else if (game.skillLevel == Game::SKILL_LEVEL_HARD)
			{
				if (rand > FOOTSOLDIER_POSSIBILITY_OF_HIT_HARD)
					return;
			}		
		}
	}

	if (projectile->projectileType == Projectile::TANK_MG_BULLET)
	{
		if (projectile->getHeight() <= getHeight())
		{
			float size = 1.5f;

			if (projectile->shooter != NULL)
			{
				if (!scored)
				{
					bool positive = true;
					if (surrendered) positive = false;
					if (projectile->shooter == game.player_1.tank)
					{
						game.player_1.addScore(this, positive);
					}
					else if (game.two_player_game && projectile->shooter == game.player_2.tank)
					{
						game.player_1.addScore(this, positive);
					}
					scored = true;
				}
			}

			projectileHit(hittingPoint, projectile->getHeight(), projectile->getDirection(), size);
			projectile->hit();		
		}
	}
	if (projectile->projectileType == Projectile::ASSAULT_RIFLE_BULLET)
	{

		if (projectile->getHeight() <= getHeight())
		{
			float size = 1.0f;
			projectileHit(hittingPoint, projectile->getHeight(), projectile->getDirection(), size);
			projectile->hit();		
		}
	}
	else if (projectile->projectileType == Projectile::HAND_GRENADE)
	{
		if (state != STATE_RUN_AWAY && randomEvent(2.0f))
		{
			Distance runDistance = 10.0f;

			if (compareDistance(projectile, runDistance) < 0.0f)
			{
				Direction dir = randomDirection();
				Distance dist = randFloat(40.0f, 50.0f);
				setRunningAway(dir, dist);
			}
		}
	}
}

void FootSoldier::collide(Tank * tank, ArcPoint hittingPoint)
{

	if (!tank->isOverLapping(this) || tank->moving == false)
		return;

	if (state != STATE_DYING)
	{
		if (!scored)
		{
			bool positive = true;
			if (surrendered) positive = false;
			if (tank == game.player_1.tank)
			{
				game.player_1.addScore(this, positive);
			}
			else if (game.two_player_game && tank == game.player_2.tank)
			{
				game.player_1.addScore(this, positive);
			}
			scored = true;
		}

		runOverByTank();
	}
}

void FootSoldier::collide(Effect * effect, ArcPoint hittingPoint)
{
	switch (effect->effectType)
	{
	case Effect::EFFECTTYPE::MISSILE_EXPLOSION:
	case Effect::EFFECTTYPE::MORTARSHELL_EXPLOSION:
		{
			if (
				state != STATE_UNDER_INDIRECT_FIRE &&
				nextState != STATE_UNDER_INDIRECT_FIRE &&
				!isNeutralized())
				setState(STATE_UNDER_INDIRECT_FIRE);//, getReactionTime() * 2.0f);

			//NO BREAK - CONTINUE PROCESSING
		}
	case Effect::EFFECTTYPE::TANKSHELL_EXPLOSION:
	case Effect::EFFECTTYPE::HAND_GRENADE_EXPLOSION:
		{
		
		/*
		Osumisen periaate:
	    -Jos sotilas seisoo, h‰neen osuu varmemmin, 
		 koska osumapinta-alaa on enemm‰n.
		 ->Voisi siis k‰ytt‰‰ getHeight()-metodia hyv‰ksi.
		-Osumisen todenn‰kˆisyys riippuu sotilaan korkeudesta
		 ja et‰isyydest‰ r‰j‰hdykseen.
	    -Lent‰v‰ projektiili on todenn‰kˆisemmin matalampana mit‰
		 l‰hemp‰n‰ sotilasta r‰j‰hdys tapahtuu.
		-Mit‰ l‰hemp‰n‰ r‰j‰hdyst‰ sotilas on, sit‰ todenn‰kˆisemmin
		 h‰neen osuus, koska sirpaleet ovat tiiviimp‰n‰ l‰hell‰
		 ja levi‰v‰t siit‰ mit‰ kauemmaksi lent‰v‰t. Myˆs
		 painevaikutus on kovempi mit‰ l‰hemp‰n‰ sotilas on.
		*/

			Distance dist = squaredObjectDistance(effect);
			Distance dangerRadius = 0.0f;
			Distance flightRadius = 0.0f;
			Distance deadlyRadius = 0.0f;
			float power = 0.0f;
			float rand = randFloat(0.0f, 1.0f);
			bool hit = false;

			if (effect->effectType == Effect::MISSILE_EXPLOSION)
			{
				dangerRadius = 80.0f;
				flightRadius = 60.0f;
				deadlyRadius = 40.0f;
				power = 1.0f;
			}
			else if (effect->effectType == Effect::MORTARSHELL_EXPLOSION)
			{
				dangerRadius = 80.0f;
				flightRadius = 40.0f;
				deadlyRadius = 20.0f;
				power = 1.0f;
			}
			else if (effect->effectType == Effect::TANKSHELL_EXPLOSION)
			{
				dangerRadius = 60.0f;
				flightRadius = 40.0f;
				deadlyRadius = 10.0f;
				power = 1.0f;
			}
			else if (effect->effectType == Effect::HAND_GRENADE_EXPLOSION)
			{
				dangerRadius = 30.0f;
				flightRadius = 10.0f;
				deadlyRadius = 5.0f;
				power = 0.3f;
			}
			dangerRadius *= dangerRadius;
			flightRadius *= flightRadius;
			deadlyRadius *= deadlyRadius;
			float possibilityOfNotGettingHit =  dist / dangerRadius;
			Direction effectDir = objectAngle(effect->getLocation());

			if (dist < deadlyRadius)
			{				
				float minTearingForce = 20.0f * power;
				float maxTearingForce = 100.0f * power;
				float forceMultiplier = 1 - dist / deadlyRadius;
				float tearingForce = forceMultiplier * (maxTearingForce - minTearingForce) + minTearingForce;
				setTornApart(tearingForce, hittingPoint);
			}
			if (dist < flightRadius)
			{
				float minFlightVelocity = 10.0f * power;
				float maxFlightVelocity = 200.0f * power;
				float forceMultiplier = 1 - dist / flightRadius;
				Velocity flyingSpeed = forceMultiplier * (maxFlightVelocity - minFlightVelocity) + minFlightVelocity;
				if (flyingSpeed > 200.0f) flyingSpeed = 200.0f;
				setFlying(flyingSpeed, objectAngle(effect) + M_PI, randFloat(0.0f, 1.0f) < 0.5f);
				hit = true;
			}
			else if (getBodyPosition() == BODY_POSITION_PRONE)
			{
				if (rand > possibilityOfNotGettingHit)
					hit = true;
			}
			else if (getBodyPosition() == BODY_POSITION_STANDING)
			{
				if (rand > possibilityOfNotGettingHit * 0.33f)
					hit = true;
			}

			if (hit)
			{
				//hits from shrapnel, or damage by pressure, heat simulated with
				//projectile hits here
				int MAX_HITS = 5;
				int hits = randInt(0, MAX_HITS);
				for (int i = 0; i < hits; i++)
				{
					projectileHit(
						ArcPoint(getLocation(), 0.0f, effectDir),
						randFloat(0.0f, getHeight()),
						effectDir,
						0.5f);
				}

				if (effect->effectType == Effect::NAPALM_EXPLOSION)
				{
					game.objects.addGameObject(new Effect(Effect::FIRE, getLocation(), randomDirection(), this, 1.0f));
					float darkening = getRelativeFigure(dist, 0.0f, dangerRadius, 0.0f, 1.0f);
					setDarkening(0.8f);
				}
			}
			break;
		}
	case Effect::EFFECTTYPE::NAPALM_EXPLOSION:
		{
			if (burning)
				return;

			Distance dangerRadius = effect->counter1 - 10.0f; //napalm spread in time
			Distance dist = squaredObjectDistance(effect);

			if (compareDistance(dist, dangerRadius) < 0.0f)
			{
				setBurning();
			}
			break;
		}
	case Effect::EFFECTTYPE::TREE_FALL:
		{
			if (state != STATE_DYING)
			{
				if (randFloat(0.0f, 1.0f) > 0.33f)
				{
					Direction dir = effect->getDirection() + randFloat(-2.3f, 2.3f);
					Distance dist = randFloat(50.0f, 100.0f);
					setRunningAway(dir, dist);
				}
				if (effect->timer1 < 0.3f)
				{

					setDeathAction();
					deathTimer = 30.0f * randFloat(1.0f, 2.0f); //random numbers
					setStatus(STATUS_SEVERELY_WOUNDED);	
					setState(STATE_DYING);
				}
			}
			break;
		}
	}
}

void FootSoldier::collide(FootSoldier * other, ArcPoint hittingPoint)
{

}

/*
Projektiiliosumat:
-Jos saanut jo kuolettavan osuman, ei noteeraa.
-Jos ei:
 -Seisaaltaan:
  -Yl‰ruumiiseen:
   -Kuolema:
    -Valitaan jokin seisaaltaan kuolinanimaatioista.
   -Haavoittuminen:
    -Valitaan jokin 'haavoittuminen seisaaltaan yl‰ruumiiseen' -animaatioista.
  -Jalkaan:
   -Valitaan jokin 'haavoittuminen seisaaltaan jalkaan' -animaatioista.
 -Makuulta:
  -Jos ei olla viel‰ saatu osumaa:
   -Kuolema:
    -Asetetaan t‰m‰nhetkisen animaation perusteella sopiva kuolinanimaatio.
   -Haavoittuminen:
    -Asetetaan t‰m‰nhetkisen animaation perusteella sopiva haavoittumisanimaatio.
  -Ollaan saatu osuma jo aiemmin:
   -Kuolema:
    -Nytk‰hdet‰‰n ja kuollaan.
  -Haavoittumine:
   -Nytk‰hdet‰‰n ja ollaan edelleen haavoittuneena. Mahdollinen rahautuminen pys‰htyy.

Animaatioista:
-Kuolinanimaatioista ei ole en‰‰ paluuta takaisin. Ei ole nytkymist‰ 
 eik‰ hytkymist‰ osumien voimasta. Ne on toteutettu niin.
-Jos sotilas on haavoittunut, haavoittumisanimaatioissa on 
 mahdollisuuksia nytkymiseen osumista.
*/

//For 'imaginary' projectiles also (indirect fire).
void FootSoldier::projectileHit(ArcPoint hittingPoint, Height projectileHeight, Direction projectileDirection, float size)
{
	stop();
	game.objects.addGameObject(new Effect(Effect::PROJECTILE_HIT_MAN, hittingPoint.getLocation(), projectileDirection, NULL, /*getCollisionSphereRadius() / objectDistance(hittingPoint.getLocation()) **/ size));

	//no processing hits anymore if had a fatal hit
	if (getStatus() != STATUS_SEVERELY_WOUNDED)
	{	
		bool death = false;

		if (getBodyPosition() == BODY_POSITION_STANDING)
		{
			death = (randFloat(0.0f, 1.0f) - 0.5f * ability) < PROBABILITY_OF_DEATH_STANDING;			
		}
		else if (getBodyPosition() == BODY_POSITION_PRONE)
		{
			death = (randFloat(0.0f, 1.0f) - 0.5f * ability) < PROBABILITY_OF_DEATH_PRONE;
		}

		if (getStatus() == STATUS_WOUNDED && (1.0f / timerDelta) < 30)
			death = true;

		if (death)
		{
			setDeathAction();
			deathTimer = 30.0f * randFloat(1.0f, 2.0f); //random numbers
			setStatus(STATUS_SEVERELY_WOUNDED);	
			setState(STATE_DYING);
		}
		else
		{
			setWoundedAction(projectileHeight);
			setStatus(STATUS_WOUNDED);
			setState(STATE_WOUNDED);
		}

		if (randFloat(0.0f, 1.0f) < 0.8)
			scream();
	}
	else
	{
		deathTimer -= randFloat(0.0f, 60.0f);
	}
}

void FootSoldier::runOverByTank(void)
{
	setDeathAction();

	//some animations don't look good for this purpose
	if (currentAction == ACTION_PRONE_DEATH_AIMING_3)
		setAction(ACTION_PRONE_DEATH_AIMING_4);
	else if (currentAction == ACTION_PRONE_DEATH_1 ||
			currentAction == ACTION_PRONE_DEATH_3)
		setAction(ACTION_PRONE_DEATH_2);

	deathTimer = 1.0f * randFloat(1.0f, 2.0f); //random numbers
	setStatus(STATUS_SEVERELY_WOUNDED);	
	setState(STATE_DYING);
	scream();
}

void FootSoldier::setFlying(Velocity speed, Direction flyingDirection, bool death)
{
	if (getStatus() != STATUS_SEVERELY_WOUNDED)
	{
		if (death)
		{
			deathTimer = 30.0f * randFloat(1.0f, 2.0f); //random numbers
			setStatus(STATUS_SEVERELY_WOUNDED);
		}
		else
		{
			setStatus(STATUS_WOUNDED);
		}
	}
	else
		deathTimer -= randFloat(0.0f, 60.0f);

	setState(STATE_THROWN_BY_FORCE);
	setAction(randInt(ACTION_PRONE_THROWN_BY_FORCE_FIRST, ACTION_PRONE_THROWN_BY_FORCE_MAX));

	bool swithcStartingSide = (randInt(0, 1) == 0);
	
	if (swithcStartingSide) image->SetFrame(image->GetFrames() / 2);
	setDirection(flyingDirection);
	setMovingDirection(flyingDirection);
	setSpeed(speed);
	deacceleration = speed * 2.0f;
	flying = true;
}

void FootSoldier::setTornApart(float force, ArcPoint pointOfForce)
{
	setDirection(direction + randFloat(-1.0f, 1.0f));

	float maxForce = 100; //maximum in this game
	float appliedForce = 0.0f;
	int rippedBodyParts = 0;
	Direction directionMin = 0.0f;
	Direction directionMax = 0.0f;
	
	//find out total applied force
	{
		//the effect of distance:
		force = checkRange(force, 0.0f, maxForce);
		appliedForce = force - pointOfForce.radius;
	}

	//find out direction
	{
		Angle a = atan(getCollisionSphereRadius() / pointOfForce.radius);
		Angle a2 = (pointOfForce.angle + M_PI);
		directionMin = a2 - a;
		directionMax = a2 + a;
	}

	//decide how many limbs will be ripped away
	{
		rippedBodyParts = appliedForce / (maxForce / NUMBER_OF_SEPARABLE_BODY_PARTS)  + randInt(-1, 1);

		rippedBodyParts = checkRange(rippedBodyParts, 0, NUMBER_OF_SEPARABLE_BODY_PARTS);
	}	
	//decide which limbs will be torn off
	{
		if (rippedBodyParts < NUMBER_OF_SEPARABLE_BODY_PARTS)
		{
			for (int i = 0; i < rippedBodyParts; i++)
			{
				int bodyPart = 0;
				
				//ehdotetaan ruumiinosia niin kauan kunnes saadaan vapaa
				while (true)
				{
					bodyPart = randInt(0, BODY_PART_MAX - 1);

					//slow algorithm but ok
					if (!separatedBodyParts[bodyPart])
					{
						separatedBodyParts[bodyPart] = true;
						break;
					}

					int sbp = 0;
					for (int j = 0; j < NUMBER_OF_SEPARABLE_BODY_PARTS; j++)
					{
						if (separatedBodyParts[j]) sbp++;
					}
					if (sbp == NUMBER_OF_SEPARABLE_BODY_PARTS)
						break;
				}
			}
		}
		else
			for (int i = 0; i < BODY_PART_MAX; i++)
				separatedBodyParts[i] = true;
	}

	//create a torn soldier image and make bodyparts fly
	{
		Velocity startSpeed = getRelativeFigure(appliedForce, 0.0f, maxForce, SeparatedBodyPart::MIN_START_SPEED, SeparatedBodyPart::MAX_START_SPEED);
		float randMin = 0.0f;
		float randMax = 2.0f;
		hgeSprite s(game.textures[Game::TEXTURE_FOOT_SOLDIER_BODY_PARTS], 0 * 32.0f, 0 * 32.0f, 32.0f, 32.0f);
		s.SetHotSpot(16.0f, 16.0f);
		hgeParticleSystem bloodBurstPS("particle5.psi", Effect::bloodSprite, 30.0f); 
		float efs = 2.0f;
		if (separatedBodyParts[BODY_PART_HEAD])
		{
			Velocity startSpeed2 = startSpeed * randFloat(randMin, randMax);
			game.objects.addGameObject(new SeparatedBodyPart(getLocation(), startSpeed2, randFloat(directionMin, directionMax), direction, BODY_PART_HEAD));
		}
		else
		{
			s.SetTextureRect(0 * 32.0f, 2 * 32.0f, 32.0f, 32.0f);
			game.objects.background->modify(&s, x, y, direction);
		}
		if (separatedBodyParts[BODY_PART_LEFT_ARM])
		{
			Velocity startSpeed2 = startSpeed * randFloat(randMin, randMax);
			game.objects.addGameObject(new SeparatedBodyPart(getLocation(), startSpeed2, randFloat(directionMin, directionMax), direction, BODY_PART_LEFT_ARM));
		}
		else
		{
			s.SetTextureRect(1 * 32.0f, 2 * 32.0f, 32.0f, 32.0f);
			game.objects.background->modify(&s, x, y, direction);
		}
		if (separatedBodyParts[BODY_PART_RIGHT_ARM])
		{
			Velocity startSpeed2 = startSpeed * randFloat(randMin, randMax);
			game.objects.addGameObject(new SeparatedBodyPart(getLocation(), startSpeed2, randFloat(directionMin, directionMax), direction, BODY_PART_RIGHT_ARM));
		}
		else
		{
			s.SetTextureRect(2 * 32.0f, 2 * 32.0f, 32.0f, 32.0f);
			game.objects.background->modify(&s, x, y, direction);
		}
		if (separatedBodyParts[BODY_PART_LEFT_LEG])
		{
			Velocity startSpeed2 = startSpeed * randFloat(randMin, randMax);
			game.objects.addGameObject(new SeparatedBodyPart(getLocation(), startSpeed2, randFloat(directionMin, directionMax), direction, BODY_PART_LEFT_LEG));
		}
		else
		{
			s.SetTextureRect(3 * 32.0f, 2 * 32.0f, 32.0f, 32.0f);
			game.objects.background->modify(&s, x, y, direction);
		}
		if (separatedBodyParts[BODY_PART_RIGHT_LEG])
		{
			Velocity startSpeed2 = startSpeed * randFloat(randMin, randMax);
			game.objects.addGameObject(new SeparatedBodyPart(getLocation(), startSpeed2, randFloat(directionMin, directionMax), direction, BODY_PART_RIGHT_LEG));
		}
		else
		{
			s.SetTextureRect(0 * 32.0f, 3 * 32.0f, 32.0f, 32.0f);
			game.objects.background->modify(&s, x, y, direction);
		}
	
		s.SetTextureRect(1 * 32.0f, 3 * 32.0f, 32.0f, 32.0f);
		game.objects.background->modify(&s, x, y, direction);
	

		//make weapon fly:
		switch (footSoldierClass)
		{
		case RIFLEMAN: 
			{
				game.objects.addGameObject(new SeparatedBodyPart(getLocation(), startSpeed, randFloat(directionMin, directionMax), direction, ASSAULT_RIFLE));
				break;
			}
		case MACHINE_GUNNER: 
			{
				game.objects.addGameObject(new SeparatedBodyPart(getLocation(), startSpeed, randFloat(directionMin, directionMax), direction, MACHINE_GUN));
				break;
			}
		case AT_SOLDIER: 
			{
				game.objects.addGameObject(new SeparatedBodyPart(getLocation(), startSpeed, randFloat(directionMin, directionMax), direction, ASSAULT_RIFLE));
				game.objects.addGameObject(new SeparatedBodyPart(getLocation(), startSpeed, randFloat(directionMin, directionMax), direction, AT_WEAPON));
				break;
			}
//		case SNIPER: break;
		case MEDIC: break;
		}		
	}
	destroy();
}

void FootSoldier::setBurning(void)
{
	game.objects.addGameObject(new Effect(Effect::FIRE, getLocation(), randomDirection(), this, 1.0f));
	setDarkening(randFloat(0.0f, 0.2f));

	deathTimer = 10.0f * randomVariation(0.2);
	if (getStatus() == STATUS_HEALTHY)
		setState(STATE_BURNING);
	else
	{
		setStatus(STATUS_SEVERELY_WOUNDED);	
		setState(STATE_DYING);
	}
	scream();
	burning = true;
}

void FootSoldier::setDarkening(float darkening)
{
	darkening = checkRange(darkening, 0.0f, 1.0f);
	byte col = (byte)(255.0f * darkening);
	for (int i = 0; i < ACTION_MAX; i++)
	{
		actions[i]->SetColor(getColor(255, col, col, col));
		actions[i]->GetColor();
	}
}

void FootSoldier::scream(void)
{
	//scream if not screaming
	if (screamTimer <= 0.0f)
	{
		screamTimer = SCREAM_MIN_TIME;
		hge->Channel_Stop(channel1);
		int soundNumber = randInt(Game::SOUND_FOOT_SOLDIER_DEATH_1, Game::SOUND_FOOT_SOLDIER_DEATH_MAX);
		channel1 = playSoundAtLocation(game.sounds[soundNumber], 50);
	}
}

void FootSoldier::soldiersTakeCover(CombatAIObject::SIDE side)
{
	list<GameObject *>::iterator it;

	for (it = game.objects.footSoldiers.begin(); it != game.objects.footSoldiers.end(); it++)
	{
		FootSoldier * fs = static_cast<FootSoldier *>((*it));
		if (fs->side == side)
		{
			if (!fs->isNeutralized())
				fs->setState(STATE_UNDER_INDIRECT_FIRE, getReactionTime() * 2.0f);
		}
	}
}

void FootSoldier::startCheckingObjects(void)
{
	blocked = false;
	field_of_fire_blocked = false;
	CombatAIObject::startCheckingObjects();
}

void FootSoldier::checkObject(GameObject * otherObject)
{
	if (startChecks)
	{
		startCheckingObjects();
		startChecks = false;
	}

	if (otherObject->type == GameObject::TANK && !fleeing && !surrendered)
	{
		checkTank(static_cast<Tank *>(otherObject));
	}
	else if (otherObject->type == GameObject::TANK_CARCASS)
	{

	}
	else if (otherObject->type == GameObject::BASE)
	{
		if (squaredObjectDistance(otherObject) < 10000)
		{
			Angle a = objectAngle(otherObject);
			if (a > direction - 1.0f && a < direction + 1.0f)
			{
				blocked = true;
			}
		}
	}
	else if (otherObject->type == GameObject::FOOT_SOLDIER)
	{
		CombatObject * co = static_cast<CombatObject *>(otherObject);

		if (co->side == this->side)
		{
			if (squaredObjectDistance(otherObject) < 80)
			{
				overlapping_soldier = true;
			}
		}
	}
	CombatAIObject::checkObject(otherObject);
}

void FootSoldier::stopCheckingObjects(void)
{
	if (side == ATTACKER_SIDE)
		int u = 2;
	CombatAIObject::stopCheckingObjects();
}

void FootSoldier::setActive(bool activeSet)
{
	if (activeSet == false && isActive())
	{
		game.currentLevel->numberOfFootSoldiersLeft[side]--;
		game.currentLevel->numberOfFootSoldiersAtArea[side]--;
		CombatAIObject::setActive(false);
	}
}

Time FootSoldier::getReactionTime(void)
{
	float varMin = 1.0f - FOOT_SOLDIER_AVERAGE_REACTION_TIME_VARIATION;
	float varMax = 1.0f + FOOT_SOLDIER_AVERAGE_REACTION_TIME_VARIATION;
	return reactionTime * randFloat(varMin, varMax);
}

Time FootSoldier::getAimingTimeFor100Meters(void)
{
	float var = FOOT_SOLDIER_AVERAGE_AIMING_SPEED_VARIATION_FOR_100_METERS;
	return randFloat(aimingSpeed100Meters * (1.0f - var), aimingSpeed100Meters * (1.0f + var));
}

Height FootSoldier::getHeight(void)
{
	if (getBodyPosition() == BODY_POSITION_STANDING)
	{
		if (currentAction >= ACTION_GETTING_DOWN_FIRST &&
			currentAction <= ACTION_GETTING_DOWN_MAX)
		{
			Height h1 = HEIGHT_STANDING - HEIGHT_PRONE;
			int frames = actions[currentAction]->GetFrames();
			Height h2 = h1 / (float)frames;
			Height height = HEIGHT_PRONE + h2;
			return height;
		}
		else
			return HEIGHT_STANDING;
	}
	else if (getBodyPosition() == BODY_POSITION_PRONE)
	{
		return HEIGHT_PRONE;
	} 

	return 0.0f;
}

Height FootSoldier::getBarrelTipHeight(void)
{
	if (getBodyPosition() == BODY_POSITION_STANDING)
	{
		return WEAPON_HEIGHT_STANDING;
	}
	else if (getBodyPosition() == BODY_POSITION_PRONE)
	{
		return WEAPON_HEIGHT_PRONE;
	}

	return 0.0f;
}

bool FootSoldier::isNeutralized(void)
{
	return (getStatus() == STATUS_WOUNDED ||
			getStatus() == STATUS_SEVERELY_WOUNDED ||
			isDestroyed() ||
			surrendered ||
			fleeing); //check if this is a ghost in the GameObjectCollection killList.
}

CollisionSphere FootSoldier::getCollisionSphere(int index)
{
	float hsx = 0.0f, hsy = 0.0f;
	if (image)
		image->GetHotSpot(&hsx, &hsy);
	hsx = 16.0f - hsx;
	hsy = 16.0f - hsy;

	Location l = getLocation();
	
	if (hsx != 0.0f ||
		hsy != 0.0f)
	{
		float a = hsx;
		float b = hsy;
		float displacement = sqrt(a * a + b * b);
		float displacementDirection = acos(a / displacement);
		l.x = x + displacement * cos(direction + displacementDirection) * GameObject::objectSize;
		l.y = y + displacement * sin(direction + displacementDirection) * GameObject::objectSize;
	}

	return CollisionSphere(l, getCollisionSphereRadius());
}

Distance FootSoldier::getAutomaticNotificationDistance(void)
{
	return FOOT_SOLDIER_ENEMY_AUTOMATIC_NOTIFICATION_DISTANCE; 
}

Location FootSoldier::getBodyCenter(void)
{
	Location l = getLocation();
	float dx = 0.0f, dy = 0.0f;
	float rel = (float)image->GetFrame() / (float)image->GetFrames();
	switch (currentAction)
	{
		case ACTION_STANDING_WOUNDED_FALLING_FORWARD: dx = 8.0f; break;
		case ACTION_STANDING_WOUNDED_FALLING_BACKWARDS: dx = -8.0f; break;
		case ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD: dx = 8.0f; break;
		case ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS: dx = -8.0f; break;
		case ACTION_STANDING_DEATH_FALLING_FORWARD_1: dx = 7.0f; break;
		case ACTION_STANDING_DEATH_FALLING_FORWARD_2: dx = 8.0f; break;
		case ACTION_STANDING_DEATH_FALLING_BACKWARDS_1: dx = -9.0f; break;
		case ACTION_STANDING_DEATH_FALLING_BACKWARDS_2: dx = -9.0f; break;
		case ACTION_STANDING_DEATH_FALLING_LEFT_1: dy = -7.0f; break;
		case ACTION_STANDING_DEATH_FALLING_LEFT_2: dx = 8.0f; dy = -8.0f; break;
		case ACTION_STANDING_DEATH_FALLING_RIGHT_1: dx = 8.0f; dy = 8.0f; break;
		case ACTION_STANDING_DEATH_FALLING_RIGHT_2: dx = 12.0f; dy = 9.0f; break;
	}

	dx *= rel; dy *= rel; 
	Location l0 = Location(0.0f, 0.0f);
	Location l2 = Location(dx, dy);
	Distance d = l0.distance(l2); 
	Angle a = l0.angle(l2);

	return ArcPoint(l, d, a + direction).getLocation(); 
}

Distance FootSoldier::getCrawlingDistanceBehindCover(Location coverLocation, Distance coverRadius)
{
	Location A = getLocation();
	Location B = coverLocation;
	Distance c = coverRadius;
	Distance d;
	Angle alpha = direction;
	Angle beta = A.angle(B);
	Angle gamma = smallestAngleBetween(alpha, beta);
	Angle delta = HALF_PI - gamma;
	d = c - c * cos(delta);
	return d;
}

void FootSoldier::levelStart(void)
{
	FootSoldier::attacker_taking_cover = false;
	FootSoldier::defender_taking_cover = false;
}

void FootSoldier::setWoundedAction(Height height)
{
	number;
	if (getBodyPosition() == BODY_POSITION_STANDING)
	{
		//upper body hit:
		if (height > LOWER_BODY_HEIGHT_STANDING)
		{	
			woundedBodyParts[BODY_PART_MIDDLE_BODY]++;
			setAction((ACTION)randInt((int)ACTION_STANDING_WOUNDED_FALLING_FIRST, (int)ACTION_STANDING_WOUNDED_FALLING_MAX), false);		
		}
		//lower body hit:
		else
		{
			if (randInt(0, 1) == 0)
			{
				woundedBodyParts[BODY_PART_LEFT_LEG]++;
				setAction(ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD, false);
			}
			else
			{
				woundedBodyParts[BODY_PART_RIGHT_LEG]++;
				setAction(ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS, false);
			}				
		}
		setTurning(randFloat(getDirection() - 0.5f, getDirection() + 0.5f)); //fall into a slightly random direction
		actions[currentAction]->SetFrames(animationWoundedFrames[currentAction]);
	}
	else if (getBodyPosition() == BODY_POSITION_PRONE)
	{
		if (getStatus() != STATUS_WOUNDED)
		{
			switch (currentAction)
			{
			//aiming animations
			case ACTION_PRONE:
			case ACTION_PRONE_FIRING:
			case ACTION_PRONE_ADJUSTING_LEFT:
			case ACTION_PRONE_ADJUSTING_RIGHT:
				{
					//arm hit (for no particular reason, just for the animation):

					if (true)
					{
						woundedBodyParts[BODY_PART_LEFT_ARM]++;
						setAction(ACTION_PRONE_GETTING_WOUNDED_AIMING_1);
					}
					else
					{
						woundedBodyParts[BODY_PART_RIGHT_ARM]++;
						setAction(ACTION_PRONE_GETTING_WOUNDED_AIMING_2);
					}	

					break;
				}
			//crawling with gun in front animations
			case ACTION_PRONE_CRAWLING: 
			case ACTION_PRONE_CRAWLING_LEFT: 
			case ACTION_PRONE_CRAWLING_RIGHT: 
			case ACTION_PRONE_TURNING_LEFT: 
			case ACTION_PRONE_TURNING_RIGHT: 
				{
					woundedBodyParts[BODY_PART_MIDDLE_BODY]++;
					setAction(ACTION_PRONE_GETTING_WOUNDED_CRAWLING);
					break;
				}
			//taking item animations
			case ACTION_PRONE_RELOAD_1: 
			case ACTION_PRONE_RELOAD_2: 
			case ACTION_PRONE_GRENADE_THROWING: 
				{
					woundedBodyParts[BODY_PART_MIDDLE_BODY]++;
					setAction(ACTION_PRONE_GETTING_WOUNDED_TAKING_ITEM);
					break;
				}
			case ACTION_PRONE_THROWN_BY_FORCE_1:
			case ACTION_PRONE_THROWN_BY_FORCE_2:
				{
					if (randFloat(0.0f, 1.0f) < 5.0f) woundedBodyParts[BODY_PART_HEAD]++;
					if (randFloat(0.0f, 1.0f) < 3.0f) woundedBodyParts[BODY_PART_LEFT_ARM]++;
					if (randFloat(0.0f, 1.0f) < 3.0f) woundedBodyParts[BODY_PART_RIGHT_ARM]++;
					if (randFloat(0.0f, 1.0f) < 4.0f) woundedBodyParts[BODY_PART_LEFT_LEG]++;
					if (randFloat(0.0f, 1.0f) < 4.0f) woundedBodyParts[BODY_PART_RIGHT_LEG]++;
					if (randFloat(0.0f, 1.0f) < 2.0f) woundedBodyParts[BODY_PART_MIDDLE_BODY]++;

					image->SetFrames(animationWoundedFrames[currentAction] + 2);

					if (animationWoundedTwichForward[currentAction])
					{
						image->SetMode(HGEANIM_FWD); 
						image->Play(); 
						image->SetFrame(animationWoundedFrames[currentAction] - 1);
					}
					else
					{
						image->SetMode(HGEANIM_REV); 
						image->Play(); 
						image->SetFrame(animationWoundedFrames[currentAction] + 1);
					}
					break;
				}
			case ACTION_CARRIED_FALLING_1:
				{
					woundedBodyParts[BODY_PART_MIDDLE_BODY]++;
					setDeathAction();
					break;
				}
			//other
			case ACTION_PRONE_TAKING_COVER_FROM_INDIRECT_FIRE:
			case ACTION_PRONE_GETTING_UP:
			case ACTION_PRONE_WOUNDED_TURNING_LEFT:
			case ACTION_PRONE_WOUNDED_TURNING_RIGHT:
			default:
				{
					woundedBodyParts[BODY_PART_MIDDLE_BODY]++;
					setAction(ACTION_PRONE_GETTING_WOUNDED);

					break;
				}
			}
		}
		else
		{
			//hit while prone and wounded:
			switch (currentAction)
			{
			case ACTION_PRONE_WOUNDED_CRAWLING_1:
			case ACTION_PRONE_WOUNDED_CRAWLING_2:
				{
					woundedBodyParts[BODY_PART_MIDDLE_BODY]++;
					image->SetFrames(animationDyingFramesMax[currentAction]);
					image->SetMode(HGEANIM_FWD); 
					image->Play(); 
					image->SetFrame(animationDyingFramesMax[currentAction] - 4);
					break;
				}
			case ACTION_PRONE_WOUNDED_TURNING_LEFT:
			case ACTION_PRONE_WOUNDED_TURNING_RIGHT:
				{
					woundedBodyParts[BODY_PART_MIDDLE_BODY]++;
					setAction(ACTION_PRONE_GETTING_WOUNDED);
					image->SetFrames(animationWoundedFrames[currentAction]);
					image->SetMode(HGEANIM_FWD); 
					image->Play(); 
					break;
				}
			case ACTION_CARRIED_FALLING_1:
				{
					woundedBodyParts[BODY_PART_MIDDLE_BODY]++;
					setDeathAction();
					break;
				}
			default:
				{
					woundedBodyParts[BODY_PART_MIDDLE_BODY]++;
					image->SetFrames(animationWoundedFrames[currentAction] + 2);

					if (animationWoundedTwichForward[currentAction])
					{
						image->SetMode(HGEANIM_FWD); 
						image->Play(); 
						image->SetFrame(animationWoundedFrames[currentAction] - 1);
					}
					else
					{
						image->SetMode(HGEANIM_REV); 
						image->Play(); 
						image->SetFrame(animationWoundedFrames[currentAction] + 1);
					}
					break;
				}
			}
		}
	}

}

void FootSoldier::setDeathAction(void)
{
	if (getBodyPosition() == BODY_POSITION_STANDING)
	{
		setAction((ACTION)randInt((int)ACTION_STANDING_DEATH_FIRST, (int)ACTION_STANDING_DEATH_MAX), false);
		setTurning(randFloat(getDirection() - 0.5f, getDirection() + 0.5f)); //fall into a slightly random direction
	}
	else if (getBodyPosition() == BODY_POSITION_PRONE)
	{
		if (getStatus() != STATUS_WOUNDED)
		{
			switch (currentAction)
			{
			//aiming animations
			case ACTION_PRONE:
			case ACTION_PRONE_FIRING:
			case ACTION_PRONE_ADJUSTING_LEFT:
			case ACTION_PRONE_ADJUSTING_RIGHT:
				{
					setAction((ACTION)randInt((int)ACTION_PRONE_DEATH_AIMING_FIRST, (int)ACTION_PRONE_DEATH_AIMING_MAX));
					break;
				}
			//crawling with gun in front animations
			case ACTION_PRONE_CRAWLING: 
			case ACTION_PRONE_CRAWLING_LEFT: 
			case ACTION_PRONE_CRAWLING_RIGHT: 
			case ACTION_PRONE_TURNING_LEFT: 
			case ACTION_PRONE_TURNING_RIGHT: 
				{
					setAction(ACTION_PRONE_DEATH_CRAWLING);
					break;
				}
			//taking item animations
			case ACTION_PRONE_RELOAD_1: 
			case ACTION_PRONE_RELOAD_2: 
			case ACTION_PRONE_GRENADE_THROWING: 
				{
					setAction(ACTION_PRONE_DEATH_TAKING_ITEM);
					break;
				}
			case ACTION_PRONE_THROWN_BY_FORCE_1:
			case ACTION_PRONE_THROWN_BY_FORCE_2:
				{
					image->SetFrames(randInt(animationDyingFramesMin[currentAction], animationDyingFramesMax[currentAction]));
					image->SetMode(HGEANIM_FWD); 
					image->Play(); 
					image->SetFrame(animationWoundedFrames[currentAction] + 5);
					break;
				}
			case ACTION_CARRIED_FALLING_1:
				{
					if (image->GetFrame() >= 6)
					{
						image->Play();
						image->SetFrame(6);
						break;
					}
				}
			//other
			case ACTION_PRONE_TAKING_COVER_FROM_INDIRECT_FIRE:
			case ACTION_PRONE_GETTING_UP:
			case ACTION_PRONE_WOUNDED_TURNING_LEFT:
			case ACTION_PRONE_WOUNDED_TURNING_RIGHT:
			default:
				{
					setAction((ACTION)randInt((int)ACTION_PRONE_DEATH_FIRST, (int)ACTION_PRONE_DEATH_MAX));
					break;
				}
			}
		}
		else
		{
			//hit while prone and wounded:
			switch (currentAction)
			{
			case ACTION_PRONE_WOUNDED_CRAWLING_1:
			case ACTION_PRONE_WOUNDED_CRAWLING_2:
				{
					image->SetFrames(randInt(animationDyingFramesMin[currentAction], animationDyingFramesMax[currentAction]));
					image->SetMode(HGEANIM_FWD); 
					image->Play(); 
					image->SetFrame(animationDyingFramesMax[currentAction] - 4);
					break;
				}
			case ACTION_PRONE_WOUNDED_TURNING_LEFT:
			case ACTION_PRONE_WOUNDED_TURNING_RIGHT:
				{
					setAction(randInt(ACTION_PRONE_DEATH_FIRST, ACTION_PRONE_DEATH_MAX));
					break;
				}
			case ACTION_CARRIED_FALLING_1:
				{
					if (image->GetFrame() >= 6)
					{
						image->Play();
						image->SetFrame(6);
						break;
					}
				}
			default:
				{
					image->SetFrames(randInt(animationDyingFramesMin[currentAction], animationDyingFramesMax[currentAction]));
					image->SetMode(HGEANIM_FWD); 
					image->Play(); 
					image->SetFrame(animationWoundedFrames[currentAction] + 5);
					break;
				}
			}
		}
	}
}

bool FootSoldier::isAnimationLastFramePlaying(void)
{
	return (image->GetFrame() == image->GetFrames() - 1);
}

void FootSoldier::animationControl(void)
{
	//update animation image
	if (image)
	{
		image->Update(timerDelta);

		int thisFrame = image->GetFrame();
		if (frame != thisFrame)
		{
			frame = thisFrame;
			first_time_playing_frame = true;
		}

		switch (currentAction)
		{
		case ACTION_STANDING_WOUNDED_FALLING_FORWARD:
			{
				if (image->GetFrame() == 12 && first_time_playing_frame)
				{
					setBodyPosition(BODY_POSITION_PRONE);
					setRenderOrder(0);
				}
				break;
			}
		case ACTION_STANDING_WOUNDED_FALLING_BACKWARDS:
			{
				if (image->GetFrame() == 9 && first_time_playing_frame)
				{
					setBodyPosition(BODY_POSITION_PRONE);
					setRenderOrder(0);
				}
				break;
			}
		case ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD:
			{
				if (image->GetFrame() == 11 && first_time_playing_frame)
				{
					setBodyPosition(BODY_POSITION_PRONE);
					setRenderOrder(0);
				}
				break;
			}
		case ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS:
			{
				if (image->GetFrame() == 13 && first_time_playing_frame)
				{
					setBodyPosition(BODY_POSITION_PRONE);
					setRenderOrder(0);
				}
				break;
			}
		case ACTION_STANDING_GETTING_PRONE:
			{
				if (image->GetFrame() == 4 && first_time_playing_frame)
				{
					setBodyPosition(BODY_POSITION_PRONE);
					setRenderOrder(0);
				}
				break;
			}
		case ACTION_PRONE_GETTING_UP:
			{
				if (image->GetFrame() == 3 && first_time_playing_frame)
				{
					setBodyPosition(BODY_POSITION_STANDING);
					setRenderOrder(GameObject::renderOrders[GameObject::FOOT_SOLDIER]);
				}
				break;
			}
		case ACTION_STANDING_DEATH_FALLING_FORWARD_1:
		case ACTION_STANDING_DEATH_FALLING_FORWARD_2:
		case ACTION_STANDING_DEATH_FALLING_BACKWARDS_1:
		case ACTION_STANDING_DEATH_FALLING_BACKWARDS_2:
		case ACTION_STANDING_DEATH_FALLING_LEFT_1:
		case ACTION_STANDING_DEATH_FALLING_LEFT_2:
		case ACTION_STANDING_DEATH_FALLING_RIGHT_1:
		case ACTION_STANDING_DEATH_FALLING_RIGHT_2:		
			{
				if (image->GetFrame() == 8 && first_time_playing_frame)
				{
					setBodyPosition(BODY_POSITION_PRONE);
					setRenderOrder(0);
				}
				break;
			}
		default: break;
		}
	}
}

int FootSoldier::getNumberOfWounds(BODY_PART bodyPart)
{
	int retVal = woundedBodyParts[bodyPart];
	if (bodyPart == (BODY_PART)-1)
	{
		retVal = 0;

		for (int i = 0; i < BODY_PART_MAX; i++)
			retVal += woundedBodyParts[bodyPart];
	}

	return (BODY_PART)retVal;
}

void FootSoldier::setStatistics(STATISTIC_VALUE val)
{
	CombatObject::setStatistics(val);
}

Rifleman::Rifleman(CombatObject::SIDE side, Location startingLocation, Direction startDirection)
:combatPair(NULL)
,turn_to_move(false)
{
	set(side, startingLocation, startDirection);
}

void Rifleman::set(CombatObject::SIDE side, Location startingLocation, Direction startDirection)
{
	FootSoldier::set(RIFLEMAN, side, startingLocation, startDirection);
	this->type = GameObject::FOOT_SOLDIER;
}

void Rifleman::processAI(void)
{
	if (getCombatPair())
	{
		if (getCombatPair()->isNeutralized())
		{
			setCombatPair(NULL);
		}
	}
	
	FootSoldier::processAI();
 }

void Rifleman:: render(void)
{
	FootSoldier::render();
}
void Rifleman::checkObject(GameObject * otherObject)
{
	if (side == DEFENDER_SIDE)
	{
		int u = 2;
	}
	if (side == CombatObject::DEFENDER_SIDE && 
		otherObject->isCombatAIObject() &&
		static_cast<CombatObject *>(otherObject)->side != side &&
		otherObject->type != GameObject::MILITARY_UNIT)
	{
		int yt = 3;
		if (yt);
		if (otherObject->y > OFF_SMOKE_DISTANCE)
		{
			otherObject->x;
			otherObject->y;
			int u = 2;
		}
	}

	if (game.currentLevel->smokeTimer > 0.0)
	{
		//inside smoke?
		if (y < OFF_SMOKE_DISTANCE)
		{
			//can't see a thing!
			return;
		}

		//other object inside smoke?
		if (otherObject->y < OFF_SMOKE_DISTANCE)
		{
			//can't see other object.
			return;
		}
	}

	FootSoldier::checkObject(otherObject);

	if (!engaged && !(field_of_fire_blocked && suggestedEnemyTarget))
	{
		if (otherObject->isCombatAIObject())
		{
			CombatAIObject * caio = static_cast<CombatAIObject *>(otherObject);

			if (!caio->isAtBattleArea())
				return;

			if (caio->side == this->side)
			{
				return;
			}

			else if (caio->isNeutralized())
			{
				return;
			}
			else if (caio->fleeing || caio->surrendered)
			{
				return;
			}
			else if (otherObject->type == GameObject::FOOT_SOLDIER)
			{
				if (static_cast<FootSoldier *>(otherObject)->footSoldierClass == FootSoldier::MEDIC)
					return;				

				if (side == ATTACKER_SIDE)
					int u = 2;
				int rand = randInt(0, 3);
				Distance enemyDistance = squaredObjectDistance(otherObject);
				if (enemyDistance < nearestSuitableEnemyDistance)		
				{
					if (field_of_fire_blocked)
					{
						if (rand == 0)
						{
							nearestSuitableEnemyDistance = enemyDistance;
							suggestedEnemyTarget = otherObject;
						}
					}
					else
					{
						nearestSuitableEnemyDistance = enemyDistance;
						suggestedEnemyTarget = otherObject;
					}
				}
			}
			else if (otherObject->type == GameObject::BASE)
			{
				if (suggestedEnemyTarget == NULL)
					suggestedEnemyTarget = otherObject;
				else if (suggestedEnemyTarget->type == GameObject::TANK)				
					suggestedEnemyTarget = otherObject;
			}
		}
	}
}


void Rifleman::fireWeapon(void)
{
	fireAssaultRifle();
}

void Rifleman::setCombatPair(FootSoldier * fs)
{
	if (fs != NULL)
	{
		this->combatPair = fs; 
		addReference(); 
	}
	else
	{
		if (getCombatPair() != NULL)
		{
			getCombatPair()->removeReference();
		}
	}
	this->combatPair = fs;
}
/*
kk-miehen taistelu:
-Kun tuli alkaa, kk-mies j‰‰ sopivaan paikkaan (etenee, ryˆmii, juoksee sopivalle y-tasalle)
-kk-mies ampuu ns. suppressing fire -tulitukea muille jalkamiehille:
 -Valitaan satunnaisia kohteita
 -ammutaan pyyhk‰isytulta
-Kun hyˆkk‰ys on ohi, kk-mies palaa ryhm‰n mukaan (ts. nousee seisomaan ja alkaa etenem‰‰n).

Vaihtoehtoja toteuttaa kk-miehen ampuminen suhteessa rk-tulitukseen, jota
kiv‰‰ri- ja pst-sotilaat tekev‰t:
-vaihtoehto a: mahdollisimman v‰h‰n luokkamallinnusta ja geneerisyytt‰:
 -omat tulituksen prosessointirutiinit, joissa kovakoodattu jutut.
 -Niist‰ tilavaiheista, mihin halutaan muutosta, tehd‰‰n omat versiot.
  +varma
  +voi aina muutella miten p‰in haluaa ja mihin suuntaan haluaa,
   geneerisess‰ rakenteessa yksi muutos perusrakenteeseen
   tarkoittaa kaikkien muiden asioiden huomioonottamista.
  -paljon "turhaakin" koodia.
-vaihtoehto b: keskitie:
 -muutama hassu muuttuja kuvaamaan arvoja, jotka halutaan
  geneerisiksi, kuten tulituskertojen m‰‰r‰, tuliopeus, jne asiat
  tulituksenprosessointirutiinissa.
-vaihtoehto c: hardcore object orientation:
 -Kaikki aseet jne. omiksi luokikseen.
 -Geneerinen prosessointi, jossa tarkistettava olion tyyppi ja
  osoitteen validisuus ennen kuin asioita voidaan k‰sitell‰.
  -Menee viikko kun miettii ja p‰hk‰ilee kaikkia hienouksia...
*/

Time MachineGunner::MACHINE_GUN_RATE_OF_FIRE_FULL_AUTO = 60.0f / 800.0f;
int MachineGunner::MACHINE_GUN_AMMO_CONTAINER_SIZE_FULL = 100;
int MachineGunner::MACHINE_GUN_MIN_SHOTS_PER_TRIGGER_PULL = 2;
float MachineGunner::MACHINE_GUN_AIMING_ACCURACY = 1.5;
Distance MachineGunner::MAX_SUPPORT_DISTANCE = 500.0f; //just a figure
MachineGunner::MachineGunner(CombatObject::SIDE side, Location startingLocation, Direction startDirection)
:distanceFromSupportedTroops(0.0f)
{
	set(side, startingLocation, startDirection);
}

void MachineGunner::set(CombatObject::SIDE side, Location startingLocation, Direction startDirection)
{
	supporting_unit = true;

	FootSoldier::set(MACHINE_GUNNER, side, startingLocation,startDirection);
	clipAmmo = MACHINE_GUN_AMMO_CONTAINER_SIZE_FULL;

	actions[ACTION_PRONE_RELOAD_1]->SetFrames(31);
	actions[ACTION_PRONE_RELOAD_2]->SetFrames(31);
}
void MachineGunner::render(void)
{
	FootSoldier::render();
}

void MachineGunner::startCheckingObjects(void)
{
	cumulativeDistance = 0.0f;
	numberOfSupportedTroops = 0.0f;
	FootSoldier::startCheckingObjects();
}

void MachineGunner::checkObject(GameObject * otherObject)
{
	if (game.currentLevel->smokeTimer > 0.0)
	{
		//inside smoke?
		if (y < OFF_SMOKE_DISTANCE)
		{
			//can't see a thing!
			return;
		}

		//other object inside smoke?
		if (otherObject->y < OFF_SMOKE_DISTANCE)
		{
			//can't see other object.
			return;
		}
	}

	FootSoldier::checkObject(otherObject);

	if (otherObject->isCombatAIObject())
	{
		CombatAIObject * caio = static_cast<CombatAIObject *>(otherObject);

		if (caio->side == this->side)
		{
			if (!caio->supporting_unit && caio->type != GameObject::MILITARY_UNIT)
			{
				numberOfSupportedTroops++;
				cumulativeDistance += squaredObjectDistance(caio);
			}
			return;
		}

		if (!engaged)
		{
			if (!caio->isAtBattleArea())
				return;

			if (caio->isNeutralized())
			{
				return;
			}
			else if (caio->fleeing || caio->surrendered)
			{
				return;
			}

			if (otherObject->type == GameObject::FOOT_SOLDIER)
			{
				if (static_cast<FootSoldier *>(otherObject)->footSoldierClass == FootSoldier::MEDIC)
					return;

				Distance enemyDistance = squaredObjectDistance(otherObject);
				if (enemyDistance < nearestSuitableEnemyDistance)		
				{
					nearestSuitableEnemyDistance = enemyDistance;
					suggestedEnemyTarget = otherObject;

					if (randInt(0, game.currentLevel->getNumberOfFootSoldiersActiveAtArea(getOppositeSide())) == 0)
						setEnemyTarget(otherObject);
				}
			}
			else if (otherObject->type == GameObject::BASE)
			{
				if (suggestedEnemyTarget == NULL)
					suggestedEnemyTarget = otherObject;
				else if (suggestedEnemyTarget->type == GameObject::TANK)				
					suggestedEnemyTarget = otherObject;
			}
		}
	}

}

void MachineGunner::stopCheckingObjects(void)
{
	if (numberOfSupportedTroops == 0)
		numberOfSupportedTroops = 0.00001f;
	distanceFromSupportedTroops = cumulativeDistance / numberOfSupportedTroops;
	FootSoldier::stopCheckingObjects();
}

void MachineGunner::attack(void)
{
	switch (statePhase)
	{
	case ATTACK_PHASE_FIRE_RANDOM_SHOTS:
		{
			if (first_time_in_state_phase)
			{
				shots = MACHINE_GUN_MIN_SHOTS_PER_TRIGGER_PULL + randInt(0, 3);

				statePhaseTimer = getReactionTime() * 2.0f;

				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				if (clipAmmo <= 0)
				{
					shots = 0;
				}
				else
				{
					Distance firingDistance = 0.0f;
					if (getEnemyTarget())
						firingDistance = objectDistance(getEnemyTarget()) + randFloat(-50.0f, 50.0f); //just random figures
					else
						firingDistance = randFloat(300.0f, 600.0f);
					Angle firingAngle = randFloat(-0.1f, 0.1f); //just random figures
					aimingLocation = ArcPoint(getLocation(), firingDistance, direction + firingAngle).getLocation();
					firing = true;
					setAction(ACTION_STANDING_NOT_AIMING_FIRING);
					shots--;
				}

				if (shots > 0)
				{
					statePhaseTimer = MACHINE_GUN_RATE_OF_FIRE_FULL_AUTO;
				}
				else
				{
					nextStatePhase();
				}
			}
			break;
		}
	default: FootSoldier::attack();
	}
}

void MachineGunner::defend(void)
{
	FootSoldier::defend();
}

void MachineGunner::fight(void)
{
	if (side == ATTACKER_SIDE)
		int u = 2;

	switch (statePhase)
	{
	case FIGHTING_PHASE_END:
		{
			if (compareDistance(distanceFromSupportedTroops, MAX_SUPPORT_DISTANCE) < 0.0f)
			{
				setStatePhase(FIGHTING_PHASE_FIGHT);
			}
			else
				previousState();
			break;
		}
	default: FootSoldier::fight(); break;
	}
}	

void MachineGunner::processFiring(void)
{
	//Machine gunner will turn to target direction, aim,
	//fire few bursts,
	//and then switch back to previous state.

	if (side == ATTACKER_SIDE)
		int u = 2;
	else
		int u = 2;

	if (getEnemyTarget() == NULL)
		previousState();
	else if (getEnemyTarget()->isDestroyed())
		previousState();

	switch (statePhase)
	{
	case FIRING_PHASE_SET_FIRE_MODE:
		{
			shots = MACHINE_GUN_MIN_SHOTS_PER_TRIGGER_PULL + randInt(0, 20);		

			nextStatePhase();

			break;
		}
	case FIRING_PHASE_AIM:
		{
			if (first_time_in_state_phase)
			{
				//set aiming time:
				//-Aiming time takes a bit longer when target is further.
				float dist = (objectDistance(getEnemyTarget()));
				float m100 = (100.0f * Game::METER);
				float dist100s = dist / m100;
				float ait = getAimingTimeFor100Meters();
				statePhaseTimer =  dist100s  * ait * randFloat(0.5f, 2.0f);

				if (statePhaseTimer < MACHINE_GUN_RATE_OF_FIRE_FULL_AUTO)
					statePhaseTimer = MACHINE_GUN_RATE_OF_FIRE_FULL_AUTO + randFloat(0.05f, 0.1f);

				first_time_in_state_phase = false;
			}
			
			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				nextStatePhase();
			}
			break;
		}
	case FIRING_PHASE_FIRE:
		{
			if (first_time_in_state_phase)
			{
				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer <= 0.0f)
			{
				if (clipAmmo <= 0)
				{
					shots = 0;
					setState(STATE_RELOAD);
					break;
				}

				Location location = getEnemyTargetLocation();
				Distance dist = objectDistance(getEnemyTarget());
				float accuracy = aimingAccuracy * dist * MACHINE_GUN_AIMING_ACCURACY;
				float radius =  ::randFloat(0.0f, accuracy);
				float angle = randomDirection();
				
				if (randFloat(0.0f, 1.0f) > 0.5f)
				{
					for (int i = 0; i < 1; i++)
					{
						aimingLocation = ArcPoint(location, sqrt(radius * dist), getDirection() + randFloat(-dist/5000.0f, dist/5000.0f)).getLocation();
					}
				}
				else
				{
					ArcPoint a(location, radius, angle);
					aimingLocation = a.getLocation();
				}

				firing = true;
				setAction(ACTION_PRONE_FIRING);
				shots--;

				if (shots > 0)
				{
					statePhaseTimer = MACHINE_GUN_RATE_OF_FIRE_FULL_AUTO;
				}
				else
				{
					nextStatePhase();
				}
			}
			break;
		}
	default: FootSoldier::processFiring();
	}
}

void MachineGunner::reload(void)
{
	switch(statePhase)
	{
	case RELOAD_PHASE_GET_OLD_CLIP:
		{
			if (first_time_in_state_phase)
			{
				setAction(ACTION_PRONE_RELOAD_1);
				image->SetMode(HGEANIM_FWD);
				first_time_in_state_phase = false;
			}

			if (image->GetFrame() == 5)
			{
				nextStatePhase();
			}
		
			break;
		}
	case RELOAD_PHASE_GHANCE_CLIP:
		{
			if (first_time_in_state_phase)
			{
				image->Stop();										
				statePhaseTimer = randFloat(1.0f, 2.0f);
				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				nextStatePhase();
			}
			break;
		}
	case RELOAD_PHASE_PUT_NEW_CLIP:
		{
			if (first_time_in_state_phase)
			{
				image->Resume();
				statePhaseTimer = randFloat(2.0f, 3.0f);
				first_time_in_state_phase = false;
			}

			if (image->GetFrame() == 18)
			{
				image->Stop();
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				nextStatePhase();
			}

			break;
		}
	case RELOAD_PHASE_PULL_LOADER:
		{
			if (first_time_in_state_phase)
			{
				image->Resume();
				first_time_in_state_phase = false;
			}
	
			if (image->GetFrame() == 21)
			{
				nextStatePhase();
			}

			break;
		}
	case RELOAD_PHASE_PULL_LOADER_2:
		{

			if (image->GetFrame() == 25)
			{
				nextStatePhase();
			}

			break;
		}
	case RELOAD_PHASE_RELEASE_LOADER:
		{
			if (image->GetFrame() == 28)
			{
				nextStatePhase();
			}
			break;
		}
	case RELOAD_PHASE_RELOAD_DONE:
		{
			if (image->GetFrame() == 30)
			{
				setAction(ACTION_PRONE);
				clipAmmo = MACHINE_GUN_AMMO_CONTAINER_SIZE_FULL;
				
				previousState();
			}
			break;
		}
	default: break;
	}
}

void MachineGunner::fireWeapon(void)
{
	fireMachineGun();
}
void MachineGunner::fireMachineGun(void)
{
	clipAmmo--;
	playSoundAtLocation(game.sounds[randInt(Game::SOUND_FOOT_SOLDIER_MACHINE_GUN_FIRING_1, Game::SOUND_FOOT_SOLDIER_MACHINE_GUN_FIRING_2)]);
	game.objects.addGameObject(new Effect(Effect::ASSAULT_RIFLE_MUZZLE_FLASH, getBarrelTipLocation(), getDirection()));
	game.objects.addGameObject(new Projectile(this, Projectile::ASSAULT_RIFLE_BULLET, getBarrelTipLocation(5.0f), getBarrelTipHeight(), aimingLocation));
	game.objects.addGameObject(new Effect(Effect::BULLET_SHELL_FLY, Location(x + 13.0f * GameObject::objectSize * cos(direction), y + 13.0f * GameObject::objectSize * sin(direction)), direction + 1.3f));
	firing = false;
}

Location MachineGunner::getBarrelTipLocation(float addedLength)
{
	Location l;
	l.x = x + (16.0f + addedLength) * cos(direction + 0.15f) * GameObject::objectSize;
	l.y = y + (16.0f + addedLength) * sin(direction + 0.15f) * GameObject::objectSize;
	return l;
}


/*
Pst-miehen taistelutoiminta:

-Hyˆkk‰ys:
 -Pst-miehet etenev‰t yksikseen ilman taisteluparia, kuten 
  kk-miehetkin (spesialisteja).
-Puolustus
 -Toimivat kuten muutkin taistelijat.

-Aseistus:
 -P‰‰ase rynn‰kkˆkiv‰‰ri
 -pst-ase sivuaseena. Sinkotyyppinen ase, ladattava.
 -pst-ase ladattava, kun sill‰ on ammuttu yksi laukaus.

-Viholliskohteen valinta:
 -Valitsevat ensisijaisesti vihollisen panssarivaunuja kohteekseen.
 -Kun pst-ammukset ovat lopussa, k‰ytt‰ytyv‰t kuten tavalliset
  jalkamiehet.

-Taistelu:
 -Etenev‰t ensimm‰iseen hyv‰‰n sijaintiin josta kykenev‰t vaikuttamaan
  vihollisen panssaroituun ajoneuvoon.
 -Vaunua ammutaan, kunnes se on tuhoutunut tai pst-ammukset (3 kpl)
 ovat lopussa.

-Taistelu viholliskohdetta vastaan pst-aseella yksityiskohtaisesti:
 -Jos kohde muuttaa sijaintia niin ett‰ sen kulma ylitt‰‰ tietyn lukeman:
  -Aina k‰‰nnet‰‰n suuntaa asianmukaisesti.
*/

int AT_Soldier::AT_SOLDIER_INITIAL_AT_AMMO = 3;
float AT_Soldier::AT_WEAPON_ACCURACY = 0.5f;
Distance AT_Soldier::AT_WEAPON_MIN_FIRING_DISTANCE = Game::METER * 20.0f;

void AT_Soldier::init(void)
{
	game.renderLoadScreen("LOADING AT_SOLDIER CLASS");
	
	//set body position information linked to actions
	for (int i = AT_ACTION_FIRST; i < AT_ACTION_MAX; i++)
		animationBodyPositions[i] = (int)BODY_POSITION_PRONE;

	//initialize rest
	for (int i = AT_ACTION_FIRST; i < AT_ACTION_MAX; i++)
	{
		animationDisplacements[i] = Location();
		animationWoundedFrames[i] = 0;
		animationDyingFramesMin[i] = 0;
		animationDyingFramesMax[i] = 0;
	}

	//set animation displacement
	//...

	//wounded animations: start framecounts
	animationWoundedFrames[AT_ACTION_GETTING_HIT_AIMING] = 7; 
	animationWoundedFrames[AT_ACTION_GETTING_HIT_RELOADING] = 8;

	//wounded animations: death framecounts
	animationDyingFramesMin[AT_ACTION_GETTING_HIT_AIMING] = 14; 
	animationDyingFramesMin[AT_ACTION_GETTING_HIT_RELOADING] = 16;

	animationDyingFramesMax[AT_ACTION_GETTING_HIT_AIMING] = 15; 
	animationDyingFramesMax[AT_ACTION_GETTING_HIT_RELOADING] = 18;

	//animation types:
	for (int i = AT_ACTION_FIRST; i < AT_ACTION_MAX; i++)
	{
		animationInfos[i] = ANIMATION_INFO_BASIC;

		if (i >= AT_ACTION_GETTING_HIT_FIRST &&
			i <= AT_ACTION_GETTING_HIT_MAX)
			animationInfos[i] |= ANIMATION_INFO_WOUNDED_COMPLEX;
	}

	//wounded animations: playback direction when hit
	animationWoundedTwichForward[AT_ACTION_GETTING_HIT_AIMING] = false; 
	animationWoundedTwichForward[AT_ACTION_GETTING_HIT_RELOADING] = false;
}

void AT_Soldier::release(void)
{
}

AT_Soldier::AT_Soldier(CombatObject::SIDE side, Location startingLocation, Direction startDirection)
{
	set(side, startingLocation, startDirection);
}

AT_Soldier::~AT_Soldier()
{
	for (int i = 0; i < NUMBER_OF_AT_ACTIONS; i++)
	{
		if (ATActions[i] != NULL)
		{
			delete ATActions[i];
			ATActions[i] = NULL;
		}
	}
}

void AT_Soldier::set(CombatObject::SIDE side, Location startingLocation, Direction startDirection)
{
	FootSoldier::set(AT_SOLDIER, side, startingLocation,startDirection);
	ATAmmo = AT_SOLDIER_INITIAL_AT_AMMO;
	ATWeaponLoaded = true;
	fire_with_AT_weapon = false;
	tank_at_area = false;

	HTEXTURE footsoldiertex = NULL;
	{
		if (this->side == ATTACKER_SIDE)
		{
			footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_ATTACKER_AT_SOLDIER];
		}
		else if (this->side == DEFENDER_SIDE)
		{
			footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_DEFENDER_AT_SOLDIER];
		}
	}

	for (int i = 0; i < NUMBER_OF_AT_ACTIONS; i++)
	{
		ATActions[i] = NULL;
	}

	int ind = (int)FootSoldier::NUMBER_OF_ANIMATIONS;
	ATActions[AT_ACTION_TAKE_WEAPON - AT_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 13, 8.0f, 0.0f, ind * 32.0f, 32.0f, 32.0f);
	//This animation uses the same images as previous, but reversed.
	ATActions[AT_ACTION_PUT_AWAY_WEAPON - AT_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 13, 8.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
	ATActions[AT_ACTION_FIRING_POSITION - AT_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 1, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
	ATActions[AT_ACTION_FIRING_AT_WEAPON - AT_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 2, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
	ATActions[AT_ACTION_ADJUSTING_LEFT - AT_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 2, 7.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
	ATActions[AT_ACTION_ADJUSTING_RIGHT - AT_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 2, 7.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
	ATActions[AT_ACTION_RELOADING_AT_WEAPON - AT_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 12, 7.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
	ATActions[AT_ACTION_GETTING_HIT_AIMING - AT_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 15, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
	ATActions[AT_ACTION_GETTING_HIT_RELOADING - AT_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 18, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);

	//set animation mode
	for (int i = 0; i < NUMBER_OF_AT_ACTIONS; i++)
	{
		ATActions[i]->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
	}
	ATActions[AT_ACTION_PUT_AWAY_WEAPON - AT_ACTION_FIRST]->SetMode(HGEANIM_REV | HGEANIM_NOLOOP);

	//animation hot spot
	for (int i = 0; i < NUMBER_OF_AT_ACTIONS; i++)
		ATActions[i]->SetHotSpot(16.0f, 16.0f);
}

void AT_Soldier::startCheckingObjects(void)
{
	tank_at_area = false;
	FootSoldier::startCheckingObjects();
}

void AT_Soldier::checkObject(GameObject * otherObject)
{
	if (game.currentLevel->smokeTimer > 0.0)
	{
		//inside smoke?
		if (y < OFF_SMOKE_DISTANCE)
		{
			//can't see a thing!
			return;
		}

		//other object inside smoke?
		if (otherObject->y < OFF_SMOKE_DISTANCE)
		{
			//can't see other object.
			return;
		}
	}

	if (game.currentLevel->smokeTimer > 0.0)
	{
		//inside smoke?
		if (y < OFF_SMOKE_DISTANCE)
		{
			int u = 2;
		}
	}
	FootSoldier::checkObject(otherObject);

	if (!engaged)
	{
	if (side == CombatObject::ATTACKER_SIDE &&
		otherObject->type == GameObject::TANK)
		int u = 2;

		if (otherObject->isCombatAIObject())
		{
			CombatAIObject * caio = static_cast<CombatAIObject *>(otherObject);

			if (caio->side == this->side)
				return;

			else if (caio->isNeutralized())
			{
				return;
			}

			else if (caio->fleeing || caio->surrendered)
			{
				return;
			}
			if (otherObject->type == GameObject::TANK && ATAmmo > 0)
			{
				tank_at_area = true;
				Distance enemyDistance = squaredObjectDistance(otherObject);
				if (enemyDistance < nearestSuitableEnemyDistance)		
				{
					nearestSuitableEnemyDistance = enemyDistance;
					suggestedEnemyTarget = otherObject;
				}
			}
			else if (otherObject->type == GameObject::FOOT_SOLDIER && !tank_at_area)
			{
				if (static_cast<FootSoldier *>(otherObject)->footSoldierClass == FootSoldier::MEDIC)
					return;

				Distance enemyDistance = squaredObjectDistance(otherObject);
				if (enemyDistance < nearestSuitableEnemyDistance)		
				{
					nearestSuitableEnemyDistance = enemyDistance;
					suggestedEnemyTarget = otherObject;
				}
			}
			else if (otherObject->type == GameObject::BASE && !tank_at_area)
			{
				if (suggestedEnemyTarget == NULL)
					suggestedEnemyTarget = otherObject;
			}
		}
	}
}

void AT_Soldier::fight(void)
{
	if (side == CombatObject::ATTACKER_SIDE)
		int u = 2;

	switch (statePhase)
	{
	case FIGHTING_PHASE_FIGHT:
		{
			if (getEnemyTarget())
			{ 
				if (getEnemyTarget()->isCombatAIObject())
				{
					CombatAIObject * caio = static_cast<CombatAIObject *>(getEnemyTarget());
					if (caio->isNeutralized())
					{
						nextStatePhase();
						return;
					}
					else if (getEnemyTargetType() == GameObject::TANK)
					{
						if (ATAmmo > 0)	
						{
							fire_with_AT_weapon = true;
							setState(STATE_FIRE);
						}
						else
							setEnemyTarget(NULL);
					}
					else
					{
						if (compareDistance(getEnemyTargetDistance(), GRENADE_THROWING_DISTANCE) < 0.0f &&
								grenades > 0 &&
								randFloat(0.0f, 0.1f * grenades) < 0.1f)
						{
							setState(STATE_THROW_GRENADE);
						}
						else
						{
							if (getEnemyTargetType() == GameObject::BASE && ATAmmo > 0 && randInt(1, 5) == 1)
								fire_with_AT_weapon = true;
							else
								fire_with_AT_weapon = false;

							setState(STATE_FIRE);
						}
					}
				}					
			}
			else
			{
				nextStatePhase();
			}
			break;
		}
	case FIGHTING_PHASE_END:
		{
			setEnemyTarget(NULL);
			previousState();
			break;
		}
	default: setStatePhase(0); break;
	}
}	

void AT_Soldier::processFiring(void)
{
	if (getEnemyTargetType() == GameObject::BASE)
		int u = 2;

	if (!fire_with_AT_weapon ||
		!isOnScreen())
	{
		FootSoldier::processFiring();
		return;
	}

	if (getEnemyTargetType() == GameObject::BASE)
		int u = 2;

	if (getEnemyTarget() == NULL)
		previousState();
	else if (getEnemyTarget()->isDestroyed())
		previousState();
	else if (getEnemyTarget()->isCombatAIObject())
	{
		if (static_cast<CombatAIObject *>(getEnemyTarget())->isNeutralized())// && randomEvent(0.5f))		
			previousState();
	}

	//Firing AT weapon:
	switch (statePhase)
	{
	case AT_FIRING_PHASE_TURN:
		{
			if (first_time_in_state_phase)
			{
				setTurning(getATFiringDirection(getEnemyTarget()));
				first_time_in_state_phase = false;
			}

			if (!isTurning())
			{	
				if (isGoodToFire())
					nextStatePhase();
				else
					setStatePhase(AT_FIRING_PHASE_TURN);
			}

			break;
		}
	case AT_FIRING_PHASE_GET_WEAPON: 
		{
			if (first_time_in_state_phase)
			{
				setAction(AT_ACTION_TAKE_WEAPON);
				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{				
				nextStatePhase();
			}

			break;
		}
	case AT_FIRING_PHASE_ADJUST:  
		{
			if (first_time_in_state_phase)
			{
				setTurningDirection(getATFiringDirection(getEnemyTarget()));
				adjust = true;
				if (turning_left)
					setAction(AT_ACTION_ADJUSTING_LEFT);
				else if (turning_right)
					setAction(AT_ACTION_ADJUSTING_RIGHT);

				first_time_in_state_phase = false;
			}

			if (!isTurning())
			{
				if (isGoodToFire())
					nextStatePhase();
				else
					setStatePhase(AT_FIRING_PHASE_ADJUST);
			}

			break;
		}
	case AT_FIRING_PHASE_AIM:  
		{
			if (first_time_in_state_phase)
			{
				float dist = (objectDistance(getEnemyTarget()));
				float m100 = (100.0f * Game::METER);
				float dist100s = dist / m100;
				float ait = getAimingTimeFor100Meters();

				float aimTimeVariation = 0.0f;
				switch (game.skillLevel)
				{
				case Game::SKILL_LEVEL_EASY: aimTimeVariation = randFloat(7.0f, 15.0f);
				case Game::SKILL_LEVEL_NORMAL: aimTimeVariation = randFloat(5.0f, 10.0f);
				case Game::SKILL_LEVEL_HARD: aimTimeVariation = randFloat(3.0f, 6.0f);
				}
				statePhaseTimer =  dist100s  * ait * aimTimeVariation;

				first_time_in_state_phase = false;
			}

			if (!isGoodToFire())
				setStatePhase(AT_FIRING_PHASE_ADJUST);

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer <= 0.0f)
			{
				nextStatePhase();
			}

			break;
		}
	case AT_FIRING_PHASE_FIRE:
		{
			if (!ATWeaponLoaded)
			{
				setStatePhase(AT_FIRING_PHASE_RELOAD);
				break;
			}

			//todo: taking a lead on moving target when aiming

			Location location = getEnemyTargetLocation();
			Distance dist = objectDistance(getEnemyTarget());
			float accuracy = aimingAccuracy * dist * AT_WEAPON_ACCURACY;
			float radius =  ::randFloat(0.0f, accuracy);
			float angle = randomDirection();

			ArcPoint a(location, radius, angle);
			aimingLocation = a.getLocation();

			firing = true;
			setAction(AT_ACTION_FIRING_AT_WEAPON);
			nextStatePhase();
			break;
		}
	case AT_FIRING_PHASE_PAUSE:
		{
			if (first_time_in_state_phase)
			{	
				statePhaseTimer = getReactionTime();

				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				if (getEnemyTargetType() == GameObject::BASE ||
					game.skillLevel == Game::SKILL_LEVEL_EASY) //On easy level they are a bit more stupid.
				{
					setStatePhase(AT_FIRING_PHASE_PUT_AWAY_WEAPON);
					break;
				}
				else
					nextStatePhase();
			}

			break;
		}
	case AT_FIRING_PHASE_RELOAD:  
		{
			if (first_time_in_state_phase)
			{
				if (ATAmmo <= 0)
				{
					setStatePhase(AT_FIRING_PHASE_PUT_AWAY_WEAPON);
					break;
				}
				setAction(AT_ACTION_RELOADING_AT_WEAPON);
				statePhaseTimer = 0.0f;

				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
			{
				if (image->GetFrame() == 5 &&
					first_time_playing_frame)
				{
					statePhaseTimer = randFloat(0.5f, 1.0f);
					image->Stop();
				}

				if (image->GetFrame() == 10 &&
					first_time_playing_frame)
				{
					statePhaseTimer = randFloat(0.5f, 1.0f);
					image->Stop();
				}

				if (isActionFinished() && 
					image->GetFrame() == 11)
				{
					ATWeaponLoaded = true;
					setStatePhase(AT_FIRING_PHASE_ADJUST);
				}
				image->Resume();
			}

			break;
		}
	case AT_FIRING_PHASE_PUT_AWAY_WEAPON:  
		{
			if (first_time_in_state_phase)
			{
				setAction(AT_ACTION_PUT_AWAY_WEAPON);

				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{
				nextStatePhase();
			}

			break;
		}
	case AT_FIRING_PHASE_END:
		{
			if (first_time_in_state_phase)
			{
				statePhaseTimer = getReactionTime();
				first_time_in_state_phase = false;
			}

			statePhaseTimer -= timerDelta;

			if (statePhaseTimer < 0.0f)
				previousState();
			break;
		}
	default: setStatePhase(0); break;
	}
}

void AT_Soldier::fireWeapon(void)
{
	if (currentAction == AT_ACTION_FIRING_AT_WEAPON)
		fireAntiTankWeapon();
	else
		fireAssaultRifle();
}

void AT_Soldier::fireAntiTankWeapon(void)
{
	ATAmmo--;
	ATWeaponLoaded = false;
	playSoundAtLocation(game.sounds[Game::SOUND_FOOT_SOLDIER_AT_WEAPON_FIRING]);

	Location atwcl = getAimedATWeaponCenter();
	Direction atwd = getATWeaponAimingDirection();

	Location projectileStartLocation = ArcPoint(atwcl, 15.0f, atwd).getLocation(); 	
	game.objects.addGameObject(new Projectile(this, Projectile::AT_ROCKET, projectileStartLocation, getATWeaponBarrelTipheight(), aimingLocation)); 

	Location frontSmokeLocation = ArcPoint(atwcl, 10.0f, atwd).getLocation(); 
    game.objects.addGameObject(new Effect(Effect::AT_WEAPON_FIRE, frontSmokeLocation, atwd));
	
	Location backPSStartLocation = ArcPoint(atwcl, 10.0f, atwd + M_PI).getLocation(); 
	for (int i = 0; i < 4; i++)
	{
		game.objects.addGameObject(new Effect(Effect::AT_WEAPON_BACKFIRE, backPSStartLocation, atwd + M_PI));
	}
 
	hgeParticleSystem * ps = new hgeParticleSystem(*(game.particleSystems[Game::PARTICLE_SYSTEM_AT_FIRING_BACKFIRE]));	
	ps->info.fLifetime = 0.05f;
	ps->info.fDirection = direction;
	game.objects.fireParticleSystem(ps, backPSStartLocation.x, backPSStartLocation.y, atwd + M_PI);
	delete ps;
	firing = false;
}

void AT_Soldier::getUp(void)
{
	switch (statePhase)
	{
	case GET_UP_PHASE_GET_UP:
		{
			if (first_time_in_state_phase)
			{
				if (currentAction >= AT_ACTION_FIRST &&
					currentAction < AT_ACTION_MAX)
				{
					setAction(AT_ACTION_PUT_AWAY_WEAPON);
				}
				else if (getBodyPosition() == BODY_POSITION_PRONE)
				{
					setAction(ACTION_PRONE_GETTING_UP);
				}
				else
				{
					nextStatePhase();
					break;
				}
				
				first_time_in_state_phase = false;
			}

			if (currentAction == AT_ACTION_PUT_AWAY_WEAPON &&
				isActionFinished())
			{
				setAction(ACTION_PRONE_GETTING_UP);
			}
			else if (currentAction == ACTION_PRONE_GETTING_UP &&
				isActionFinished())
			{
				nextStatePhase();
			}

			break;
		}
	case GET_UP_PHASE_FINISHED:
		{
			previousState();
			break;
		}
	default: setStatePhase(0);
	}
}

bool AT_Soldier::isGoodToFire(void)
{
	//check if target has left instant field of fire
	float maxTargetAngle = 0.3f;
	Direction dir = enemyTargetDirection;
	if (dir < getATWeaponAimingDirection() - maxTargetAngle ||
		dir > getATWeaponAimingDirection() + maxTargetAngle)
	{
		return false;
	}

	return true;
}

void AT_Soldier::setAction(int nextAction, bool displacement)
{
	if (nextAction < AT_ACTION_FIRST)
	{
		if (currentAction >= AT_ACTION_FIRST)
			currentAction = ACTION_PRONE;
		FootSoldier::setAction(nextAction, displacement);
		return;
	}

	//set animation
	currentAction = (ACTION)nextAction;
	image = ATActions[nextAction - AT_ACTION_FIRST];
	image->Play();
	frame = image->GetFrame();
	first_time_playing_frame = true;

	//set body position
	setBodyPosition((BODY_POSITION)animationBodyPositions[currentAction - AT_ACTION_FIRST]);
}

void AT_Soldier::move(void)
{
	FootSoldier::move();
}

void AT_Soldier::turn(void)
{
	if (currentAction == AT_ACTION_ADJUSTING_LEFT ||
		currentAction == AT_ACTION_ADJUSTING_RIGHT)
	{
		if (isActionFinished())
		{
			if (angleToTurn < PRONE_ANIMATION_ADJUSTING_ANGLE)
			{
				if (turning_left)
					direction -= angleToTurn;
				else if (turning_right)
					direction += angleToTurn;

				angleToTurn = 0.0f;
				stopTurning();
				setAction(AT_ACTION_FIRING_POSITION);
			}
			else
			{
				if (turning_left)
					direction -= PRONE_ANIMATION_ADJUSTING_ANGLE;
				else if (turning_right)
					direction += PRONE_ANIMATION_ADJUSTING_ANGLE;
				angleToTurn -= PRONE_ANIMATION_ADJUSTING_ANGLE;
				setAction(currentAction);
			}
		}
	}
	else
		FootSoldier::turn();
}

void AT_Soldier::setDarkening(float darkening)
{
	byte col = (byte)(255.0f * darkening);
	for (int i = 0; i < NUMBER_OF_AT_ACTIONS; i++)
		ATActions[i]->SetColor(getColor(255, col, col, col));

	FootSoldier::setDarkening(darkening);
}
Direction AT_Soldier::getATFiringDirection(GameObject * target)
{
	return getAimedATWeaponCenter().angle(target->getLocation()) + M_PI / 4.0f;
}

Location AT_Soldier::getATWeaponBarrelTipLocation(void)
{
	return ArcPoint(getLocation(), 9.7f, direction + PI_TIMES_TWO - 0.25f).getLocation();
}

Height AT_Soldier::getATWeaponBarrelTipheight(void)
{
	if (getBodyPosition() == BODY_POSITION_PRONE)
	{
		return HEIGHT_STANDING;
	}
	else
		return HEIGHT_STANDING;
}

Direction AT_Soldier::getATWeaponAimingDirection(void)
{
	float dir = direction - ANGLE_BETWEEN_BODY_AND_AIMED_AT_WEAPON;
	if (dir < 0)
		return direction + PI_TIMES_TWO - ANGLE_BETWEEN_BODY_AND_AIMED_AT_WEAPON;
	else
		return dir;
}

Location AT_Soldier::getATWeaponBarrelBackLocation(void)
{
	return ArcPoint(getLocation(), 6.0f, direction + 1.4f).getLocation();
}

Direction AT_Soldier::getATWeaponBarrelBackDirection(void)
{
	return direction + 2.3561945;
}

Location AT_Soldier::getAimedATWeaponCenter(void)
{
	float dir = getDirection() + M_PI / 4.0f;
	return ArcPoint(Location(x + 2.0f * GameObject::objectSize * cos(dir), y + 2.0f * GameObject::objectSize * sin(dir)), 4.0f, getDirection()).getLocation();
}

void AT_Soldier::setWoundedAction(Height height)
{
	switch (currentAction)
	{
	case AT_ACTION_TAKE_WEAPON: 
	case AT_ACTION_PUT_AWAY_WEAPON: 
	case AT_ACTION_FIRING_POSITION: 
	case AT_ACTION_FIRING_AT_WEAPON: 
	case AT_ACTION_ADJUSTING_LEFT: 
	case AT_ACTION_ADJUSTING_RIGHT: 
		{
			setAction(AT_ACTION_GETTING_HIT_AIMING);
			break;
		}
	case AT_ACTION_RELOADING_AT_WEAPON: 
		{
			setAction(AT_ACTION_GETTING_HIT_RELOADING);
			break;
		}
	case AT_ACTION_GETTING_HIT_AIMING: 
	case AT_ACTION_GETTING_HIT_RELOADING: 
		{
			image->SetFrames(animationWoundedFrames[currentAction] + 2);

			if (animationWoundedTwichForward[currentAction])
			{
				image->SetMode(HGEANIM_FWD);
				image->Play();
				image->SetFrame(animationWoundedFrames[currentAction] - 1);
			}
			else
			{
				image->SetMode(HGEANIM_REV);
				image->Play();
				image->SetFrame(animationWoundedFrames[currentAction] + 1);
			}
			break;
		}
	default: FootSoldier::setWoundedAction(height); break;
	}
}

void AT_Soldier::setDeathAction(void)
{
	switch ((AT_ACTION)currentAction)
	{
	case AT_ACTION_TAKE_WEAPON: 
	case AT_ACTION_PUT_AWAY_WEAPON: 
	case AT_ACTION_FIRING_POSITION: 
	case AT_ACTION_FIRING_AT_WEAPON: 
	case AT_ACTION_ADJUSTING_LEFT: 
	case AT_ACTION_ADJUSTING_RIGHT: 
		{
			setAction(AT_ACTION_GETTING_HIT_AIMING);
			break;
		}
	case AT_ACTION_RELOADING_AT_WEAPON: 
		{
			setAction(AT_ACTION_GETTING_HIT_RELOADING);
			break;
		}
	case AT_ACTION_GETTING_HIT_AIMING: 
	case AT_ACTION_GETTING_HIT_RELOADING: 
		{
			break;
		}
	default: FootSoldier::setDeathAction(); return;
	}

	image->SetFrames(randInt(animationDyingFramesMin[currentAction], animationDyingFramesMax[currentAction]));
	image->SetMode(HGEANIM_FWD);
	image->Play();
	image->SetFrame(animationWoundedFrames[currentAction] + 5);
}

#define MEDIC_MIN_DISTANCE_FROM_WOUNDED 8.0f
#define MEDIC_MIN_DISTANCE_FROM_WOUNDED_SQUARED 64.0f
//Kantajalle k‰y huonosti:
//-projektiili osuu
// -kannettava:
//  -n‰kyviin
//  -asetetaan n‰kym‰‰n kantajan p‰‰ll‰ (setOnTop)
//  -soitetaan kannettavanaolosta kaatumisanimaatio
//  -kannettavan 'carried' -booleani 'false':ksi
//  -kannettavan AI:ssa:
//   -kun animaatio on soitettu, edellinen tila, miss‰ oltiin, luultavasti haavoittunut.
//   -edellisen tilan mukaan soitetaan transitioanimaatioita kannettava-tilassa, koska se parhaiten tiet‰‰ mit‰ soittaa
// -kantaja
//  -soitetaan projektiiliosuman mukaan normaalia k‰ytˆst‰.
//-tankki ajaa p‰‰lle:
// -kanjtaja
//  -soitetaan normaali k‰ytˆs
// -kanettava:
//  -n‰kyviin
//  -soitetaan kuolemaa kannettavaoloanimaatiolle tavallisin menoin.
//-koko ukko r‰j‰ht‰‰
// -sama kuin projektiiliosumassa, jos t‰m‰kin r‰j‰ht‰‰, sittenh‰n r‰j‰ht‰‰.
//-ukko palaa
// -projektiilijuttuja, luultavasti tulee palamaan myˆs.

//
void Medic::init(void)
{
	game.renderLoadScreen("LOADING MEDIC CLASS");
	
	//set body position information linked to actions
	animationBodyPositions[MEDIC_ACTION_LIFTING_FOOT_SOLDIER] = (int)BODY_POSITION_STANDING;
	animationBodyPositions[MEDIC_ACTION_CARRYING_FOOT_SOLDIER_RUNNING] = (int)BODY_POSITION_STANDING;

	//initialize rest
	for (int i = MEDIC_ACTION_FIRST; i < MEDIC_ACTION_MAX; i++)
	{
		animationDisplacements[i] = Location();
		animationWoundedFrames[i] = 0;
		animationDyingFramesMin[i] = 0;
		animationDyingFramesMax[i] = 0;
	}

	//set animation displacement
	//...

	//wounded animations: start framecounts

	//wounded animations: death framecounts

	//animation types:
	for (int i = 0; i < NUMBER_OF_MEDIC_ACTIONS; i++)
	{
		animationInfos[i] = ANIMATION_INFO_BASIC;
	}

	//wounded animations: playback direction when hit

}

void Medic::release(void)
{
}

Medic::Medic(CombatObject::SIDE side, Location startingLocation, Direction startDirection)
{
	set(side, startingLocation, startDirection);
}

Medic::~Medic()
{
	for (int i = 0; i < NUMBER_OF_MEDIC_ACTIONS; i++)
	{
		if (medicActions[i] != NULL)
		{
			delete medicActions[i];
			medicActions[i] = NULL;
		}
	}
}

void Medic::set(CombatObject::SIDE side, Location startingLocation, Direction startDirection)
{
	FootSoldier::set(MEDIC, side, startingLocation,startDirection);
	setRenderOrder(2);
	this->woundedSoldier = NULL;
	this->busy = false;
	this->dropping_soldier = false;
	this->timer = 120.0f;

	HTEXTURE footsoldiertex = NULL;
	{
		if (this->side == ATTACKER_SIDE)
		{
			footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_ATTACKER_MEDIC];
		}
		else if (this->side == DEFENDER_SIDE)
		{
			footsoldiertex = game.textures[Game::TEXTURE_FOOT_SOLDIER_DEFENDER_MEDIC];
		}
	}
	for (int i = 0; i < NUMBER_OF_MEDIC_ACTIONS; i++)
	{
		medicActions[i] = NULL;
	}

	int ind = (int)FootSoldier::NUMBER_OF_ANIMATIONS;
	medicActions[MEDIC_ACTION_LIFTING_FOOT_SOLDIER - MEDIC_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 16, 8.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);
	medicActions[MEDIC_ACTION_CARRYING_FOOT_SOLDIER_RUNNING - MEDIC_ACTION_FIRST] = new hgeAnimation(footsoldiertex, 8, 12.0f, 0.0f, ind++ * 32.0f, 32.0f, 32.0f);

	//set animation mode
	for (int i = 0; i < NUMBER_OF_MEDIC_ACTIONS; i++)
	{
		medicActions[i]->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
	}
	medicActions[MEDIC_ACTION_CARRYING_FOOT_SOLDIER_RUNNING - MEDIC_ACTION_FIRST]->SetMode(HGEANIM_FWD | HGEANIM_LOOP);

	//animation hot spot
	for (int i = 0; i < NUMBER_OF_MEDIC_ACTIONS; i++)
		medicActions[i]->SetHotSpot(16.0f, 16.0f);

	orders = STATE_MEDIC_ACTION;
	setState(STATE_MEDIC_ACTION);
}

void Medic::render(void)
{
	//if (woundedSoldier)
	//{
	//	rl(woundedSoldier->getBodyCenter());
	//	hudfont->printf(x, y + 10, "dist: %f.2f", objectDistance(woundedSoldier->getBodyCenter()));
	//}
	FootSoldier::render();
}

void Medic::startCheckingObjects(void)
{
	if (!busy)
		setWoundedSoldier(NULL);
	blocked = false;
	FootSoldier::startCheckingObjects();
}

void Medic::checkObject(GameObject * otherObject)
{
	if (game.currentLevel->smokeTimer > 0.0)
	{
		//inside smoke?
		if (y < OFF_SMOKE_DISTANCE)
		{
			//can't see a thing!
			return;
		}

		//other object inside smoke?
		if (otherObject->y < OFF_SMOKE_DISTANCE)
		{
			//can't see other object.
			return;
		}
	}

	FootSoldier::checkObject(otherObject);

	if (!busy && status == STATUS_HEALTHY)
	{
		if (otherObject->isCombatAIObject())
		{
			if (side == ATTACKER_SIDE)
				int u = 2;
			CombatAIObject * caio = static_cast<CombatAIObject *>(otherObject);

			if (caio->side != this->side)
				return;

			else if (otherObject->type == GameObject::FOOT_SOLDIER)
			{
				FootSoldier * fs = static_cast<FootSoldier *>(otherObject);

				int status = fs->getStatus();
				bool mc = fs->in_medical_care;
				
				if (fs->getStatus() == STATUS_WOUNDED && !fs->medic_target &&!fs->in_medical_care)
				{
					setWoundedSoldier(fs);
				}
			}
		}
	}
}

void Medic::stopCheckingObjects(void)
{
	//FootSoldier::stopCheckingObjects();
	//if (!busy)
	//	setWoundedSoldier(suggestedEnemyTarget);
}

void Medic::fetchWounded(void)
{
	timer -= timerDelta;

	number;
	switch (statePhase)
	{
	case MEDIC_ACTION_PHASE_NO_WORK_GO_TO_MEDEVAC_ZONE:
		{
			if (first_time_in_state_phase)
			{
				setMoving(ADVANCING_MODE_RUNNING_FORWARD, FLT_MAX);
				first_time_in_state_phase = false;
			}

			Location mez = getMedEvacZone();
			if (randomEvent(0.3f))
			{
				setTurning(objectAngle(mez));
				setMovingDistance(objectDistance(mez) + 30.0f);
			}

			//there
			if (squaredObjectDistance(getMedEvacZone()) < 800.0f)
			{
				nextStatePhase();
			}

			if (busy)
			{
				setStatePhase(MEDIC_ACTION_PHASE_GO_TO_WOUNDED_SOLDIER);
				break;
			}

			break;
		}
	case MEDIC_ACTION_PHASE_TURN_TO_BATTLEFIELD:
		{
			if (first_time_in_state_phase)
			{
				stop();
				setTurning(getBattleMainDirection());
				first_time_in_state_phase = false;
			}

			if (!isTurning())
			{
				nextStatePhase();
			}

			break;
		}
	case MEDIC_ACTION_PHASE_OBSERVE_BATTLEFIELD:
		{
			if (busy)
			{
				nextStatePhase();
			}
			break;
		}
	case MEDIC_ACTION_PHASE_GO_TO_WOUNDED_SOLDIER:
		{
			if (first_time_in_state_phase)
			{
				if (getWoundedSoldier())
				{
					setTurning(objectAngle(woundedSoldier->getBodyCenter()));
					setMoving(ADVANCING_MODE_RUNNING_FORWARD, FLT_MAX);
				}			
				first_time_in_state_phase = false;
			}

			if (getWoundedSoldier())
			{
				if (randomEvent(0.3f))
				{
					number;
					setTurning(objectAngle(woundedSoldier->getBodyCenter()));
					moving = true;
					speed = SPEED_RUNNING;
				}

				//check if he is dying: no hurry then
				if (getWoundedSoldier()->status != STATUS_WOUNDED)
				{
					setWoundedSoldier(NULL);
					setStatePhase(MEDIC_ACTION_PHASE_NO_WORK_GO_TO_MEDEVAC_ZONE);
					break;
				}

				//there
				Location l = woundedSoldier->getBodyCenter();
				float dist = squaredObjectDistance(l);
				if (dist < MEDIC_MIN_DISTANCE_FROM_WOUNDED_SQUARED)
				{
					nextStatePhase();
					break;
				}
			}	
			else
			{
				setStatePhase(MEDIC_ACTION_PHASE_NO_WORK_GO_TO_MEDEVAC_ZONE);
				break;
			}

			break;
		}
	case MEDIC_ACTION_PHASE_GET_WOUNDED_SOLDIER:
		{
			if (first_time_in_state_phase)
			{
				stop();
				setAction(MEDIC_ACTION_LIFTING_FOOT_SOLDIER);
				first_time_in_state_phase = false;
			}

			if (getWoundedSoldier())
			{
				//dead now?
				if (getWoundedSoldier()->status != STATUS_WOUNDED)
				{
					setWoundedSoldier(NULL);
					setStatePhase(MEDIC_ACTION_PHASE_NO_WORK_GO_TO_MEDEVAC_ZONE);
					break;
				}			

				//set carry
				if (!woundedSoldier->carried)
				{
					if (image->GetFrame() == 9)
					{
						woundedSoldier->setState(STATE_BEING_CARRIED);
						woundedSoldier->visible = false;
						woundedSoldier->carried = true;
						carrying_foot_soldier = true;
					}
				}
			}

			if (isActionFinished())
			{
				nextStatePhase();
			}

			break;
		}
	case MEDIC_ACTION_PHASE_GET_WOUNDED_TO_MEDEVAC_ZONE:
		{
			Location mez = getMedEvacZone();
			
			if (first_time_in_state_phase)
			{				
				setTurning(objectAngle(mez));
				setMoving(ADVANCING_MODE_RUNNING_FORWARD, FLT_MAX);

				first_time_in_state_phase = false;
			}

			//movement control
			if (randomEvent(0.3f))
			{
				setTurning(objectAngle(mez));
				moving = true;
				speed = SPEED_RUNNING_CARRYING_FOOT_SOLDIER;
			}

			//control carried foot soldier
			if (moving && carrying_foot_soldier && getWoundedSoldier())
			{
				woundedSoldier->setLocation(getLocation());
				woundedSoldier->direction = direction;
			}

			if (getMovingDistance() <= 0.0f || squaredObjectDistance(getMedEvacZone()) < 800.0f)
			{
				stop();
				setAction(ACTION_STANDING_NOT_AIMING);

				if (getWoundedSoldier())		
				{
					woundedSoldier->in_medical_care = true;
					dropSoldier();
					woundedSoldier->setDestroyed(true);
					setWoundedSoldier(NULL);
					
					if (timer < 0.0f)
					{
						this->setDestroyed(true);
					}
				}

				setStatePhase(MEDIC_ACTION_PHASE_TURN_TO_BATTLEFIELD);
			}

			break;
		}
	}
}

void Medic::getUp(void)
{
	switch (statePhase)
	{
	case GET_UP_PHASE_GET_UP:
		{
			if (first_time_in_state_phase)
			{
				//If doing lifting action, finish it.
				if (currentAction != MEDIC_ACTION_LIFTING_FOOT_SOLDIER &&
					getBodyPosition() == BODY_POSITION_PRONE)
				{
					setAction(ACTION_PRONE_GETTING_UP);
					break;
				}
				else if (currentAction != MEDIC_ACTION_LIFTING_FOOT_SOLDIER &&
					getBodyPosition() != BODY_POSITION_PRONE)
				{
					nextStatePhase();
					break;
				}
				
				first_time_in_state_phase = false;
			}

			if (isActionFinished())
			{
				nextStatePhase();
			}

			break;
		}
	case GET_UP_PHASE_FINISHED:
		{
			previousState();
			break;
		}
	default: setStatePhase(0); break;
	}
}

void Medic::setSurrendering(void)
{
	//if (state != STATE_UNDER_INDIRECT_FIRE &&
	//	!isNeutralized())
	//{
	//	surrendered = true;
	//	setActive(false);
	//	setState(STATE_SURRENDER);
	//	setStatistics(CombatObject::STATISTIC_VALUE_PERSON_CAPTURED);
	//}
}

void Medic::setFleeing(void)
{
	//fleeing = true;
	//setActive(false);
	//setState(STATE_FLEE);
}

void Medic::setAction(int nextAction, bool displacement)
{
	if (nextAction < MEDIC_ACTION_FIRST)
	{
		if (currentAction >= MEDIC_ACTION_FIRST)
			currentAction = ACTION_STANDING_FIRST;
		FootSoldier::setAction(nextAction, displacement);
		return;
	}

	//set animation
	currentAction = (ACTION)nextAction;
	image = medicActions[nextAction - MEDIC_ACTION_FIRST];
	image->Play();
	frame = image->GetFrame();
	first_time_playing_frame = true;

	//set body position
	setBodyPosition((BODY_POSITION)animationBodyPositions[currentAction - MEDIC_ACTION_FIRST]);
}

void Medic::setMoving(ADVANCING_MODE advancingMode, Distance distance)
{
	if (!carrying_foot_soldier)
	{
		FootSoldier::setMoving(advancingMode, distance);
		return;
	}

	setAdvancingMode(advancingMode);

	bool swithcStartingSide = (randInt(0, 1) == 0);
	float speedVariation = randFloat(0.9f, 1.1f);

	if (advancingMode == ADVANCING_MODE_RUNNING_FORWARD)
	{
		setAction(MEDIC_ACTION_CARRYING_FOOT_SOLDIER_RUNNING);	
		if (swithcStartingSide) image->SetFrame(image->GetFrames() / 2);
		image->Play();
		setMovingDirection(getDirection_FORWARD());

		setSpeed((SPEED_RUNNING_CARRYING_FOOT_SOLDIER + SPEED_RUNNING_CARRYING_FOOT_SOLDIER * ability) * speedVariation);
	}
	else
		FootSoldier::setMoving(advancingMode, distance);

	setMovingDistance(distance);
	moving = true;
}

void Medic::stop(void)
{
	switch (currentAction)
	{
	case MEDIC_ACTION_CARRYING_FOOT_SOLDIER_RUNNING: 
		{
			image->SetFrame(0);
			image->Stop();
		}
	default: FootSoldier::stop();
	}
}

void Medic::projectileHit(ArcPoint hittingPoint, Height projectileHeight, Direction projectileDirection, float size)
{
	if (carrying_foot_soldier)
	{
		dropSoldier();
	}
	if (getWoundedSoldier())
	{
		setWoundedSoldier(NULL);
	}

	FootSoldier::projectileHit(hittingPoint, projectileHeight, projectileDirection, size);
}

void Medic::runOverByTank(void)
{
	if (carrying_foot_soldier)
	{
		dropSoldier();
		woundedSoldier->runOverByTank();
	}
	if (getWoundedSoldier())
	{
		setWoundedSoldier(NULL);
	}

	FootSoldier::runOverByTank();
}

void Medic::setFlying(Velocity speed, Direction flyingDirection, bool death)
{
	if (carrying_foot_soldier)
	{
		dropSoldier();
		woundedSoldier->setFlying(speed, flyingDirection, death);
	}
	if (getWoundedSoldier())
	{
		setWoundedSoldier(NULL);
	}

	FootSoldier::setFlying(speed, flyingDirection, death);
}

void Medic::setTornApart(float force, ArcPoint pointOfForce)
{
	if (carrying_foot_soldier)
	{
		dropSoldier();
		woundedSoldier->setTornApart(force, pointOfForce);
	}
	if (getWoundedSoldier())
	{
		setWoundedSoldier(NULL);
	}

	FootSoldier::setTornApart(force, pointOfForce);
}

void Medic::setBurning(void)
{
	if (carrying_foot_soldier)
	{
		dropSoldier();
		woundedSoldier->setBurning();
	}
	if (getWoundedSoldier())
	{
		setWoundedSoldier(NULL);
	}

	FootSoldier::setBurning();
}

void Medic::setDarkening(float darkening)
{
	byte col = (byte)(255.0f * darkening);
	for (int i = 0; i < NUMBER_OF_MEDIC_ACTIONS; i++)
		medicActions[i]->SetColor(getColor(255, col, col, col));

	FootSoldier::setDarkening(darkening);
}

void Medic::dropSoldier(void)
{
	if (getWoundedSoldier())
	{
		woundedSoldier->visible = true;
		woundedSoldier->carried = false;
		woundedSoldier->stop();
		//game.objects.setOnTop(woundedSoldier, this);
		woundedSoldier->setAction(ACTION_CARRIED_FALLING_1);
		carrying_foot_soldier = false;
		dropping_soldier = true;
	}
}

void Medic::setWoundedSoldier(FootSoldier * fs)
{
	if (woundedSoldier)
	{
		//enemy target moved to killList, or other enemy closer
		woundedSoldier->removeReference();
		woundedSoldier->medic_target = false;
	}
	woundedSoldier = fs;
	if (fs)
	{
		fs->addReference();
		fs->medic_target = true;
		busy = true;
	}
	else
		busy = false;
}

Location Medic::getMedEvacZone(void)
{
	if (side == CombatObject::DEFENDER_SIDE)
	{
		return Location(400.0f, 800.0f);
	}
	else if (side == CombatObject::ATTACKER_SIDE)
	{
		return Location(400.0f, -200.0f);
	}
}

void Medic::animationControl(void)
{
	if (dropping_soldier)
	{
		if (image->GetFrame() > 3)
		{
			if (getWoundedSoldier())
			{
				//game.objects.setOnTop(this, getWoundedSoldier());
				setWoundedSoldier(NULL);
				dropping_soldier = false;
			}
		}
	}
	FootSoldier::animationControl();
}

char * FootSoldier::getClassString(void)
{
	switch (footSoldierClass)
	{
		case RIFLEMAN: return "RIF";
		case MACHINE_GUNNER: return "MG";
		case AT_SOLDIER: return "AT";
//		case SNIPER: return "SNI";
		case MEDIC: return "MED";
		default: return "UNKNOWN";
	}
	return "UNKNOWN";
}
char * FootSoldier::getStateString(int index)
{
	if (index == -2)
		index = state;

	switch (index)
	{
		case STATE_DEFEND: return "STATE_DEFEND"; 
		case STATE_ATTACK: return "STATE_ATTACK"; 
		case STATE_FIGHT: return "STATE_FIGHT"; 
		case STATE_FIRE: return "STATE_FIRE"; 
		case STATE_RELOAD: return "STATE_RELOAD"; 
		case STATE_THROW_GRENADE: return "STATE_THROW_GRENADE"; 
		case STATE_RUN_AWAY: return "STATE_RUN_AWAY"; 
		case STATE_UNDER_INDIRECT_FIRE: return "STATE_UNDER_INDIRECT_FIRE"; 
		case STATE_THROWN_BY_FORCE: return "STATE_THROWN_BY_FORCE"; 
		case STATE_BURNING: return "STATE_BURNING";
		case STATE_WOUNDED: return "STATE_WOUNDED"; 
		case STATE_DYING: return "STATE_DYING"; 
		case STATE_CLEAR_OF_ENEMY: return "STATE_CLEAR_OF_ENEMY"; 
		case STATE_DEBUG: return "STATE_DEBUG"; 
		default: "NO_STATE";
	}
	return "NO_STATE";
}

char * FootSoldier::getStatePhaseString(void)
{
	switch (state)
	{
		case STATE_DEFEND:
			{
				switch (statePhase)
				{
				case 0: return "DEFENSE_PHASE_TURN_TO_MOVING_DIRECTION";
				case 1: return "DEFENSE_PHASE_MOVE_TO_DEFENSE_LINE";
				case 2: return "DEFENSE_PHASE_GET_DOWN"; //"ket taun!" -Arnold Schwarzenegger
				case 3: return "DEFENSE_PHASE_OBSERVE";
				case 4: return "DEFENSE_PHASE_TURN_TO_ENEMY_TARGET_DIRECTION";
				case 5: return "DEFENSE_PHASE_ADJUST_DIRECTION";
				case 6: return "DEFENSE_PHASE_FIRE_START";
				case 7: return "DEFENSE_PHASE_FIGHT";
				case 8: return "DEFENSE_PHASE_TURN_TO_DEFENSE_DIRECTION";
				}
			}
		case STATE_ATTACK:
			{
				switch (statePhase)
				{
				case 0: return "ATTACK_PHASE_TURN_TO_ATTACK_DIRECTION";
				case 1: return "ATTACK_PHASE_MOVE_FORWARD";
				case 2: return "ATTACK_PHASE_FIRE_RANDOM_SHOTS";
				case 3: return "ATTACK_PHASE_TAKE_COVER";
				case 4: return "ATTACK_PHASE_CRAWL_SIDEWAYS_1";
				case 5: return "ATTACK_PHASE_FIGHT";
				case 6: return "ATTACK_PHASE_CRAWL_FORWARD";
				case 7: return "ATTACK_PHASE_CRAWL_SIDEWAYS_2";
				case 8: return "ATTACK_PHASE_GET_UP";
				case 9: return "ATTACK_PHASE_ADVANCE";
				case 10: return "ATTACK_PHASE_LAST";
				}
			}
		case STATE_FIGHT: 
			{
				switch (statePhase)
				{
				case 0: return "FIGHTING_PHASE_FIGHT";
				case 1: return "FIGHTING_PHASE_END";
				}
			}
		case STATE_FIRE:
			{
				switch (statePhase)
				{
				case 0: return "FIRING_PHASE_TURN";
				case 1: return "FIRING_PHASE_ADJUST_POSITION";
				case 2: return "FIRING_PHASE_SET_FIRE_MODE";
				case 3: return "FIRING_PHASE_AIM";
				case 4: return "FIRING_PHASE_FIRE";
				case 5: return "FIRING_PHASE_END";
				}			
			}
		case STATE_RELOAD:
			{
				switch (statePhase)
				{
				case 0: return "RELOAD_PHASE_GET_OLD_CLIP";
				case 1: return "RELOAD_PHASE_GHANCE_CLIP";
				case 2: return "RELOAD_PHASE_PUT_NEW_CLIP";
				case 3: return "RELOAD_PHASE_PULL_LOADER";
				case 4: return "RELOAD_PHASE_PULL_LOADER_2";
				case 5: return "RELOAD_PHASE_RELEASE_LOADER";
				case 6: return "RELOAD_PHASE_RELOAD_DONE";
				}
			}

		case STATE_THROW_GRENADE:
			{
				switch (statePhase)
				{
				case 0: return "GRENADE_THROWING_PHASE_TURN";
				case 1: return "GRENADE_THROWING_PHASE_THROW";
				}
			}
		case STATE_RUN_AWAY: 
			{
				switch (statePhase)
				{
				case 0: return "STATE_RUN_AWAY_PHASE_GET_UP";
				case 1: return "STATE_RUN_AWAY_PHASE_TURNING";
				case 2: return "STATE_RUN_AWAY_PHASE_RUNNING";
				case 3: return "STATE_RUN_AWAY_PHASE_CLEAR";
				}
			}
		case STATE_UNDER_INDIRECT_FIRE:
			{
				switch (statePhase)
				{
				case 0: return "UNDER_INDIRECT_FIRE_PHASE_GET_DOWN"; //"get taun!" -Ahnuld Schwarzenegger
				case 1: return "UNDER_INDIRECT_FIRE_PHASE_STAY_DOWN";
				case 2: return "UNDER_INDIRECT_FIRE_PHASE_GET_UP";
				}
			}
		case STATE_THROWN_BY_FORCE:
			{
				switch (statePhase)
				{
				case THROWN_BY_FORCE_PHASE_FLYING: return "THROWN_BY_FORCE_PHASE_FLYING";
				case THROWN_BY_FORCE_PHASE_GOT_DOWN: return "THROWN_BY_FORCE_PHASE_GOT_DOWN";
				}
			}
		case STATE_BURNING:
			{
				switch (statePhase)
				{
				case BURNING_PHASE_GET_UP: return "BURNING_PHASE_GET_UP";
				case BURNING_PHASE_RUN: return "BURNING_PHASE_RUN";
				case BURNING_PHASE_FALL: return "BURNING_PHASE_FALL";
				}
				break;
			}
		case STATE_WOUNDED:
			{
				switch (statePhase)
				{
				case 0: return "WOUNDED_PHASE_FALLING";
				case 1: return "WOUNDED_PHASE_CHECK_STATUS";
				case 2: return "WOUNDED_PHASE_CHANGE_POSITION";
				case 3: return "WOUNDED_PHASE_TURNING";
				case 4: return "WOUNDED_PHASE_MOVING";
				case 5: return "WOUNDED_PHASE_LYING";
				}
			}
		case STATE_DYING:
			{
				switch (statePhase)
				{
				case 0: return "DYING_PHASE_FALLING";
				case 1: return "DYING_PHASE_LYING";
				case 2: return "DYING_PHASE_DEATH";
				}
			}
		case STATE_CLEAR_OF_ENEMY:
			{
				switch (statePhase)
				{
				}
			}
		case STATE_DEBUG:
			{
				switch (statePhase)
				{
				case 0: return "DEBUG_PHASE_1";
				case 1: return "DEBUG_PHASE_2";
				case 2: return "DEBUG_PHASE_3";
				case 3: return "DEBUG_PHASE_4";
				}
			}
	}
	return "UNKWNOWN";
}

char * FootSoldier::getActionString(void)
{
	static char * strings[] = 
	{
		//actions in standing position
		"ACTION_STANDING_NOT_AIMING",
		"ACTION_WALKING_AIMING",
		"ACTION_RUNNING_NOT_AIMING",
		"ACTION_STANDING_NOT_AIMING_FIRING",
		"ACTION_STANDING_AIMING_FIRING",
		"ACTION_STANDING_SURRENDERING",
		"ACTION_STANDING_UNSURRENDERING",
		"ACTION_WALKING_SURRENDERED",
		"ACTION_STANDING_GETTING_PRONE",
		"ACTION_STANDING_WOUNDED_FALLING_FORWARD",
		"ACTION_STANDING_WOUNDED_FALLING_BACKWARDS",
		"ACTION_STANDING_WOUNDED_LEG_FALLING_FORWARD",
		"ACTION_STANDING_WOUNDED_LEG_FALLING_BACKWARDS",
		"ACTION_STANDING_DEATH_FALLING_FORWARD_1",
		"ACTION_STANDING_DEATH_FALLING_FORWARD_2",
		"ACTION_STANDING_DEATH_FALLING_BACKWARDS_1",
		"ACTION_STANDING_DEATH_FALLING_BACKWARDS_2",
		"ACTION_STANDING_DEATH_FALLING_LEFT_1",
		"ACTION_STANDING_DEATH_FALLING_LEFT_2",
		"ACTION_STANDING_DEATH_FALLING_RIGHT_1",
		"ACTION_STANDING_DEATH_FALLING_RIGHT_2",

		//actions in prone position
		"ACTION_PRONE",
		"ACTION_PRONE_FIRING",
		"ACTION_PRONE_RELOAD_1",
		"ACTION_PRONE_RELOAD_2",
		"ACTION_PRONE_GRENADE_THROWING",
		"ACTION_PRONE_TAKING_COVER_FROM_INDIRECT_FIRE",
		"ACTION_PRONE_CRAWLING",
		"ACTION_PRONE_CRAWLING_LEFT",
		"ACTION_PRONE_CRAWLING_RIGHT",
		"ACTION_PRONE_TURNING_LEFT",
		"ACTION_PRONE_TURNING_RIGHT",
		"ACTION_PRONE_ADJUSTING_LEFT",
		"ACTION_PRONE_ADJUSTING_RIGHT",
		"ACTION_PRONE_GETTING_UP",
		"ACTION_PRONE_WOUNDED_CRAWLING_1",
		"ACTION_PRONE_WOUNDED_CRAWLING_2",
		"ACTION_PRONE_WOUNDED_TURNING_LEFT",
		"ACTION_PRONE_WOUNDED_TURNING_RIGHT",
		"ACTION_PRONE_GETTING_WOUNDED_AIMING_1",
		"ACTION_PRONE_GETTING_WOUNDED_AIMING_2",
		"ACTION_PRONE_GETTING_WOUNDED_CRAWLING",
		"ACTION_PRONE_GETTING_WOUNDED_TAKING_ITEM",
		"ACTION_PRONE_GETTING_WOUNDED",
		"ACTION_PRONE_DEATH_AIMING_1",
		"ACTION_PRONE_DEATH_AIMING_2",
		"ACTION_PRONE_DEATH_AIMING_3",
		"ACTION_PRONE_DEATH_AIMING_4",
		"ACTION_PRONE_DEATH_CRAWLING",
		"ACTION_PRONE_DEATH_TAKING_ITEM",
		"ACTION_PRONE_DEATH_1",
		"ACTION_PRONE_DEATH_2",
		"ACTION_PRONE_DEATH_3",
		"ACTION_PRONE_THROWN_BY_FORCE_1",
		"ACTION_PRONE_THROWN_BY_FORCE_2",
		"ACTION_CARRIED_FALLING_1"
	};

	return strings[currentAction];
}


char * FootSoldier::getBodyPositionString(void)
{
	if (getBodyPosition() == BODY_POSITION_STANDING)
		return "BODY_POSITION_STANDING";
	else if (getBodyPosition() == BODY_POSITION_PRONE)
		return "BODY_POSITION_PRONE";
	
	return "NO_POSITION";
}

char * FootSoldier::getStatusString(void)
{
	switch (getStatus())
	{
	case STATUS_HEALTHY: return "STATUS_HEALTHY";
	case STATUS_WOUNDED: return "STATUS_WOUNDED";
	case STATUS_SEVERELY_WOUNDED: return "STATUS_SEVERELY_WOUNDED";
	case STATUS_DEAD: return "STATUS_DEAD";
	}

	return "STATUS_UNKNOWN";
}

char * AT_Soldier::getStatePhaseString(void)
{
	switch (state)
	{
	case STATE_FIRE:
		{
			if (!fire_with_AT_weapon)
			{
				return FootSoldier::getStatePhaseString();
			}

			switch (statePhase)
			{
			case AT_FIRING_PHASE_TURN: return "AT_FIRING_PHASE_TURN";
			case AT_FIRING_PHASE_GET_WEAPON: return "AT_FIRING_PHASE_GET_WEAPON";
			case AT_FIRING_PHASE_ADJUST: return "AT_FIRING_PHASE_ADJUST";
			case AT_FIRING_PHASE_AIM: return "AT_FIRING_PHASE_AIM";
			case AT_FIRING_PHASE_FIRE: return "AT_FIRING_PHASE_FIRE";
			case AT_FIRING_PHASE_PAUSE: return "AT_FIRING_PHASE_PAUSE";
			case AT_FIRING_PHASE_RELOAD: return "AT_FIRING_PHASE_RELOAD";
			case AT_FIRING_PHASE_PUT_AWAY_WEAPON: return "AT_FIRING_PHASE_PUT_AWAY_WEAPON";
			case AT_FIRING_PHASE_END: return "AT_FIRING_PHASE_END";
			}
		}
	default: return FootSoldier::getStatePhaseString();
	}
	return "UNKNOWN STATE PHASE";
}

char * AT_Soldier::getActionString(void)
{
	static char * strings[] = 
	{
		//actions in standing position
		"AT_ACTION_TAKE_WEAPON",
		"AT_ACTION_PUT_AWAY_WEAPON",
		"AT_ACTION_FIRING_POSITION",
		"AT_ACTION_FIRING_AT_WEAPON",
		"AT_ACTION_ADJUSTING_LEFT",
		"AT_ACTION_ADJUSTING_RIGHT",
		"AT_ACTION_RELOADING_AT_WEAPON",
		"AT_ACTION_GETTING_HIT_AIMING",
		"AT_ACTION_GETTING_HIT_RELOADING"
	};

	return strings[currentAction - ACTION_MAX];
}

char * Medic::getActionString(void)
{
	switch (currentAction)
	{
	case MEDIC_ACTION_LIFTING_FOOT_SOLDIER: return "MEDIC_ACTION_LIFTING_FOOT_SOLDIER";
	case MEDIC_ACTION_CARRYING_FOOT_SOLDIER_RUNNING: return "MEDIC_ACTION_CARRYING_FOOT_SOLDIER_RUNNING";
	}

	return FootSoldier::getActionString();
}

Velocity SeparatedBodyPart::MIN_START_SPEED = 50.0f;
Velocity SeparatedBodyPart::MAX_START_SPEED = 300.0f;
Velocity SeparatedBodyPart::MIN_SPINNING_SPEED = 20.0f;
Velocity SeparatedBodyPart::MAX_SPINNING_SPEED = 30.0f;

SeparatedBodyPart::~SeparatedBodyPart()
{
	if (image) { delete image; image = NULL; }
	if (bloodBurstPS) { delete bloodBurstPS; bloodBurstPS = NULL; }
	if (bloodSpillPS) { delete bloodSpillPS; bloodSpillPS = NULL; }
}

SeparatedBodyPart::SeparatedBodyPart(Location startLocation, Velocity startSpeed, Direction startDirection, Direction bodyDirection, FootSoldier::BODY_PART bodyPart)
{
	centreLoc = startLocation;
	MovingObject::set(GameObject::SEPARATED_BODY_PART, startLocation, startDirection);
	setMovingDirection(startDirection);
	startSpeed = checkRange(startSpeed, MIN_START_SPEED, MAX_START_SPEED);
	setSpeed(startSpeed);
	spinningSpeed = randFloat(MIN_SPINNING_SPEED, MAX_SPINNING_SPEED);
	timer = randFloat(0.1f, 0.3f);

	bloodBurstPS = NULL;
	bloodSpillPS = NULL;

	if (randInt(0, 1) == 0)
		spinningSpeed = -spinningSpeed;

	deacceleration = startSpeed * 2.0f;

	bodyPartType = bodyPart;

	flying = true;
	blood = true;
	burstStop = false;

	loc = startLocation;

	switch (bodyPartType)
	{
	case FootSoldier::BODY_PART_HEAD:      image = new hgeAnimation(game.textures[Game::TEXTURE_FOOT_SOLDIER_BODY_PARTS], 1, 0.0f, 0 * 32.0f, 0 * 32.0f, 32.0f, 32.0f); loc = ArcPoint(getLocation(), 5.0f, 0.0f + bodyDirection).getLocation(); break;
	case FootSoldier::BODY_PART_LEFT_ARM:  image = new hgeAnimation(game.textures[Game::TEXTURE_FOOT_SOLDIER_BODY_PARTS], 1, 0.0f, 1 * 32.0f, 0 * 32.0f, 32.0f, 32.0f); loc = ArcPoint(getLocation(), 6.0f, (M_PI * 1.75f) + bodyDirection).getLocation(); break;
	case FootSoldier::BODY_PART_RIGHT_ARM: image = new hgeAnimation(game.textures[Game::TEXTURE_FOOT_SOLDIER_BODY_PARTS], 1, 0.0f, 2 * 32.0f, 0 * 32.0f, 32.0f, 32.0f); loc = ArcPoint(getLocation(), 6.0f, (M_PI * 0.25f) + bodyDirection).getLocation(); break;
	case FootSoldier::BODY_PART_LEFT_LEG:  image = new hgeAnimation(game.textures[Game::TEXTURE_FOOT_SOLDIER_BODY_PARTS], 1, 0.0f, 3 * 32.0f, 0 * 32.0f, 32.0f, 32.0f); loc = ArcPoint(getLocation(), 3.0f, (M_PI + M_PI / 4.0f) + bodyDirection).getLocation(); break;
	case FootSoldier::BODY_PART_RIGHT_LEG: image = new hgeAnimation(game.textures[Game::TEXTURE_FOOT_SOLDIER_BODY_PARTS], 1, 0.0f, 0 * 32.0f, 1 * 32.0f, 32.0f, 32.0f); loc = ArcPoint(getLocation(), 2.0f, (M_PI * 0.75f) + bodyDirection).getLocation(); break;
	case FootSoldier::ASSAULT_RIFLE:       image = new hgeAnimation(game.textures[Game::TEXTURE_FOOT_SOLDIER_BODY_PARTS], 1, 0.0f, 1 * 32.0f, 1 * 32.0f, 32.0f, 32.0f); loc = ArcPoint(getLocation(), 8.0f, (M_PI / 4.0f) + bodyDirection).getLocation(); blood = false; break;
	case FootSoldier::MACHINE_GUN:         image = new hgeAnimation(game.textures[Game::TEXTURE_FOOT_SOLDIER_BODY_PARTS], 1, 0.0f, 2 * 32.0f, 1 * 32.0f, 32.0f, 32.0f); loc = ArcPoint(getLocation(), 8.0f, (M_PI / 4.0f) + bodyDirection).getLocation(); blood = false; break;
	case FootSoldier::AT_WEAPON:           image = new hgeAnimation(game.textures[Game::TEXTURE_FOOT_SOLDIER_BODY_PARTS], 1, 0.0f, 3 * 32.0f, 1 * 32.0f, 32.0f, 32.0f); loc = ArcPoint(getLocation(), 2.0f, (ONE_AND_A_HALF_PI) + bodyDirection).getLocation(); blood = false; break;
	}
	if (image)
		image->SetHotSpot(16.0f, 16.0f);



	if (blood)
	{
		bloodBurstPS = new hgeParticleSystem("blood_gush.psi", Effect::bloodSprite2, 30.0f); 
		bloodBurstPS->info.nEmission *= (float)game.amountOfParticles / 100.0f;
		bloodBurstPS->info.fDirection = startDirection;
		bloodSpillPS = new hgeParticleSystem("blood_spill.psi", Effect::bloodSprite, 30.0f); 
		bloodSpillPS->info.nEmission *= (float)game.amountOfParticles / 100.0f;
		if (randFloat(0.0f, 1.0f) > 0.1)
			bloodSpillPS->info.fParticleLifeMax = randFloat(0.5f, 1.5f); 
		else
			bloodSpillPS->info.fParticleLifeMax = randFloat(1.5f, 10.0f); 
		bloodBurstPS->FireAt(loc.x, loc.y);
		bloodSpillPS->FireAt(loc.x, loc.y);
	}
}

void SeparatedBodyPart::processAction(void)
{
	timer -= timerDelta;

	if (flying)
	{
		//spin
		direction += spinningSpeed * timerDelta;

		//fly
		speed -= deacceleration * timerDelta;

		dx = speed * cos(movingDirection) * timerDelta;
		dy = speed * sin(movingDirection) * timerDelta;
		x += dx;
		y += dy;
	}

	if (blood && timer < 0.0f && !burstStop)
	{
		game.objects.background->modify(bloodBurstPS);
		burstStop = true;
	}

	if (speed <= 0.0f && flying)
	{
		if (blood)
		{
			game.objects.background->modify(bloodSpillPS);
			bloodSpillPS->Stop(false);
		}
		else
			destroy();

		game.objects.background->modify(image, x,y, direction);
		flying = false;
	}
	else if (speed < 100.0f && flying)
	{
		if (spinningSpeed > 0.0f)
			spinningSpeed -= 100.0f * timerDelta;
		else
			spinningSpeed += 100.0f * timerDelta;
	}
	else if (blood && !flying)
	{
		if (bloodBurstPS->GetParticlesAlive() <= 0 &&
			bloodSpillPS->GetParticlesAlive() <= 0)
			destroy();
	}

	if (blood && flying)
	{
		bloodSpillPS->MoveTo(x, y, false);
	}
}

void SeparatedBodyPart::render(void)
{
	if (flying)
		GameObject::render();

	if (blood)
	{
		bloodBurstPS->Update(timerDelta);
		bloodSpillPS->Update(timerDelta);
		bloodBurstPS->Render();
		bloodSpillPS->Render();
	}
}

char * SeparatedBodyPart::getTypeString(void)
{
	static char * strings[] = 
	{
		"BODY_PART_HEAD",
		"BODY_PART_LEFT_ARM",
		"BODY_PART_RIGHT_ARM",
		"BODY_PART_LEFT_LEG",
		"BODY_PART_RIGHT_LEG",
		"BODY_PART_MIDDLE_BODY",
		"ASSAULT_RIFLE",
		"MACHINE_GUN",
		"AT_WEAPON"
	};

	return strings[bodyPartType];
}

#define MILITARY_UNIT_MIN_BATTLE_SIZE_RATIO 0.2f
#define MILITARY_UNIT_MIN_SURRENDER_DISTANCE 300.0f
#define MILITARY_UNIT_MIN_SURRENDER_DISTANCE_SQUARED 90000.0f
char MilitaryUnit::messages[CombatObject::NUMBER_OF_SIDES][NUMBER_OF_MESSAGE_TYPES][256];

void MilitaryUnit::init(void)
{
	hge->System_SetState(HGE_INIFILE, "Settings.ini");

	char * sideNames[2] = {"DEFENDER", "ATTACKER"};
	char * messageTypes[NUMBER_OF_MESSAGE_TYPES] = {"PULLING_BACK", "SURRENDERING"};

	hge->System_SetState(HGE_INIFILE, "Settings.ini");

	char text[256];
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < NUMBER_OF_MESSAGE_TYPES; j++)
		{
			sprintf(text, "MESSAGE_%s_INFANTRY_%s", sideNames[i], messageTypes[j]);
			sprintf(messages[i][j], hge->Ini_GetString("MESSAGES", text, ""));
		}
	}
}

void MilitaryUnit::release(void)
{
}

MilitaryUnit::MilitaryUnit(CombatObject::SIDE side)
{
	CombatAIObject::set(GameObject::MILITARY_UNIT, Location(FLT_MAX, FLT_MAX), 0.0f, side);
	startSize = 0;
	numberOfActiveUnits = 0;
	numberOfNonCombatantUnits = 0;
	for (int i = 0; i < MAX_SIZE; i++)
	{
		units[i] = NULL;
	}
}

void MilitaryUnit::add(CombatAIObject * caio)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (units[i] == NULL)
		{
			units[i] = caio;
			units[i]->addReference();
			startSize++;
			numberOfActiveUnits++;

			if (units[i]->type == GameObject::FOOT_SOLDIER)
			{
				if (static_cast<FootSoldier *>(units[i])->footSoldierClass == FootSoldier::MEDIC)
				{
					numberOfNonCombatantUnits++;
				}
			}
			break;
		}
	}
}

void MilitaryUnit::processInteraction(void)
{
	//Check number of fighting units
	{
		if (randomEvent(1.0f))
		{
			for (int i = 0; i < MAX_SIZE; i++)
			{
				if (units[i])
				{
					if (!units[i]->isActive())
					{
						if (units[i]->type == GameObject::FOOT_SOLDIER)
						{
							if (static_cast<FootSoldier *>(units[i])->footSoldierClass == FootSoldier::MEDIC)
							{
								numberOfNonCombatantUnits--;
							}
						}

						units[i]->removeReference();
						units[i] = NULL;
						numberOfActiveUnits--;
					}
				}
			}
		}
	}

	//Check for fleeing or surrendering
	{
		if (randomEvent(3.0f))
		{
			float help1 = (numberOfActiveUnits - numberOfNonCombatantUnits);
			float help2 = help1 / startSize;
			bool help3 = help2 < MILITARY_UNIT_MIN_BATTLE_SIZE_RATIO;
			if (help3 && !fleeing && !surrendered && game.currentLevel->numberOfTanksAtArea[side] == 0 && game.currentLevel->numberOfFootSoldiersAtArea[side] < startSize / 2.0f)
			{
				float distSum = 0;
				int numOfDistances = 0;

				for (int i = 0; i < MAX_SIZE; i++)
				{
					if (units[i])
					{
						if (static_cast<FootSoldier *>(units[i])->footSoldierClass != FootSoldier::MEDIC)
						{
							if (units[i]->getEnemyTarget())
							{
								float dist = units[i]->getNearestEnemyDistance();

								if (dist != FLT_MAX)
								{
									distSum += dist;
									numOfDistances++;
								}
							}
						}
					}
				}

				float unitDistanceFromEnemy = distSum / numOfDistances;

				if (unitDistanceFromEnemy < MILITARY_UNIT_MIN_SURRENDER_DISTANCE_SQUARED)
				{
					surrender();
				}
				else
				{
					flee();
				}
			}
		}
	}
}

void MilitaryUnit::surrender(void)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (units[i])
		{
			units[i]->setSurrendering();

			if (units[i]->surrendered)
			{
				surrendered = true;

				if (!units[i]->scored)
				{

					//Add scores to players
					Distance distPlayer1 = FLT_MAX;
					Distance distPlayer2 = FLT_MAX;
					if (game.player_1.tank)
						distPlayer1 = units[i]->objectDistance(game.player_1.tank);
					if (game.player_2.tank)
						distPlayer2 = units[i]->objectDistance(game.player_2.tank);

					if (distPlayer1 < distPlayer2)
					{
						game.player_1.addScore(units[i]);
					}
					else
					{
						game.player_2.addScore(units[i]);
					}
				}
			}
		}
	}

	if (surrendered)
	{
		game.showMessage(messages[getOppositeSide()][MESSAGE_SURRENDERING], CombatObject::colorIndexes[side]);
	}
}

void MilitaryUnit::flee(void)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (units[i])
		{
			units[i]->setFleeing();

			if (units[i]->fleeing)
				fleeing = true;
		}
	}

	if (fleeing)
	{
		game.showMessage(messages[getOppositeSide()][MESSAGE_PULLING_BACK], CombatObject::colorIndexes[side]);
	}
}