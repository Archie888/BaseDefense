#pragma once
#include "player.h"
#include "GameObjectcollection.h"
#include "level.h"
#include <hgefont.h>
#include <hgegui.h>
#include <hgeguictrls.h>
#include <map>

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

class BackgroundObjectFactory
{
public:
	BackgroundObjectFactory(void);
	~BackgroundObjectFactory(void);

	GameObject * makeBackgroundObject(BackgroundObject::BACKGROUNDOBJECT_TYPE bgObjectType, Location location, Size size);
};

class OutGameScene
{
public:

	float timerDelta;

	float sceneTimer;

	OutGameScene(void) {}
	virtual ~OutGameScene(void) {}
	virtual bool play(void) { return false; }
};

class ImageIndexer
{
private:
	int currentImageIndex;
public:
	ImageIndexer(void) {}
	virtual ~ImageIndexer(void) {}
	virtual void setCurrentImageIndex(int index) { currentImageIndex = index; }
	int getCurrentImageIndex(void) { return currentImageIndex; }
};

class SlideShow : public OutGameScene, public ImageIndexer
{
public:

	enum INTS
	{
		MAX_NUMBER_OF_IMAGES = 15,
		NUMBER_OF_LETTERS = 1000
	};

	hgeAnimation * images[MAX_NUMBER_OF_IMAGES];
	float imageTimes[MAX_NUMBER_OF_IMAGES];
	float imageScales[MAX_NUMBER_OF_IMAGES];
	string textureFileNames[MAX_NUMBER_OF_IMAGES];
	float imageWidths[MAX_NUMBER_OF_IMAGES];
	float imageHeights[MAX_NUMBER_OF_IMAGES];
	hgeAnimation * currentImage;

	//music
	HEFFECT music;
	HCHANNEL musicChannel;

	//images
	int nImages;
	float imageTimer;
	float musicStartTimer;
	bool playing;
	bool images_showing;

	//text data
	char text[NUMBER_OF_LETTERS];
	float textX, textY;
	float letterTimer;
	float textTimer;
	int nLetters;

	SlideShow(void);
	virtual ~SlideShow(void);
	virtual bool play(void);
	virtual void controlMedia(void);
	virtual void controlImages(void) {};
	virtual void controlText(void) {};
	virtual void startRendering(void); 
	virtual void stopRendering(void);
	virtual void renderBackground(void);
	virtual void renderImages(void) {};
	virtual void renderText(void) {};
	virtual void renderBoundedString(hgeFont * fnt, float x, float y, int align, float width, int nRenderedChars, const char * fmt, ...);
};

class Intro : public SlideShow
{
public:

	enum INTS
	{
		NUMBER_OF_IMAGES = 15
	};

	HTEXTURE imagebackgroundtex;
	HTEXTURE textbackgroundtex;
	HTEXTURE textshadowfiltertex;

	hgeAnimation * imageBackground;
	hgeAnimation * textBackground;
	hgeAnimation * textShadowFilterLayer;

	//map zoom effect:
	float flipTimer; 
	int flips;

	HTARGET pretest;

	Intro(void);
	~Intro(void);
	void controlImages(void);
	void controlText(void);
	void renderImages(void);
	void renderText(void);
};

class VictoryScene : public SlideShow
{
public:

	enum INTS
	{
		NUMBER_OF_IMAGES = 4
	};

	HTEXTURE imagebackgroundtex;
	HTEXTURE textbackgroundtex;
	HTEXTURE textshadowfiltertex;

	hgeAnimation * imageBackground;
	hgeAnimation * textBackground;
	hgeAnimation * textShadowFilterLayer;

	HTARGET pretest;

	hgeGUIText * textObject;

	VictoryScene(void);
	~VictoryScene(void);
	void controlImages(void);
	void controlText(void);
	void renderImages(void);
	void renderText(void);
};

class GameOverScene : public SlideShow
{
public:

	enum INTS
	{
		NUMBER_OF_IMAGES = 2
	};

	hgeGUIText * textObject;

	GameOverScene(void);
	~GameOverScene(void);
	void controlImages(void);
	void controlText(void);
	void renderImages(void);
	void renderText(void);
};

class LevelFinishedScene : public OutGameScene
{
public:

	bool key_pressed;

    float incrementSpeed;
	float statInterval;
	int statsIndex;
	bool next_index;
	Timer statTimer;
	Timer p1StatTimer;
	Timer p2StatTimer;

	int nPlayer1EnemyInfantryTotalSum;
	int nPlayer2EnemyInfantryTotalSum;
	int nPlayer1EnemyInfantryTotal;
	int nPlayer2EnemyInfantryTotal;
	int nPlayer1EnemyInfantry[FootSoldier::NUMBER_OF_CLASSES];
	int nPlayer2EnemyInfantry[FootSoldier::NUMBER_OF_CLASSES];
	int nPlayer1EnemyTanks;
	int nPlayer2EnemyTanks;
	int player1LevelScoreSum;
	int player2LevelScoreSum;
	int player1LevelScore;
	int player2LevelScore;

	LevelFinishedScene(void);
	~LevelFinishedScene(void);
	bool play(void);
	void render(void);
};

class Message
{
float y;

public:
	enum INTS
	{
		MESSAGE_MAX_LENGTH = 256
	};

	//Linked list -style handling
	Message * nextMessage;
	char text[MESSAGE_MAX_LENGTH];
	Timer messageDelayTimer;
	Time addTime;
	DWORD color;
	
	void setY(float y);
	float getY();

	Message(void);
	Message(const char * text, DWORD color, Time delay);
	~Message(void) {}
	void render(void);
};

class MessageManager
{
public:
	enum INTS
	{
		MESSAGES_MAX = 5,
	};

	Message * messages[MESSAGES_MAX];
	Message * waitingMessagesNod;
	float nextMessageY;

	MessageManager(void);
	~MessageManager(void);
	void addMessage(const char * message, DWORD color, Time delay = 0.0f);
	void addMessage(Message * message);
	void render(void);
	void init(void);
	void clear(void);
};

class Game
{
public:
	//game objects
	Player player_1;
	Player player_2;
	GameObjectCollection objects;
	BackgroundObjectFactory backgroundObjectFactory;
	LevelManager levelMan;
	Level * currentLevel;
	int startlevelNumber;
	OutGameScene * outGameScene;
	bool two_player_game;
	float fadeOutTimer;
	float fadeInTimer;
	bool take_input;

	//messages
	MessageManager messageMan;
	static Time MESSAGE_TIME;

	enum TEXT_OBJECT
	{
		TEXT_OBJECT_MIDSCREEN,
		TEXT_OBJECT_LEVEL_INTRO_HEADER,
		TEXT_OBJECT_LEVEL_INTRO_HEADER_SHADOW,
		TEXT_OBJECT_QUIT_GAME_TEXT_BOX,
		TEXT_OBJECT_QUIT_GAME_TEXT_BOX_SHADOW,
		TEXT_OBJECT_MAX
	};
	hgeGUIText * textObjects[TEXT_OBJECT_MAX];

	enum SKILL_LEVEL
	{
		SKILL_LEVEL_EASY,
		SKILL_LEVEL_NORMAL,
		SKILL_LEVEL_HARD,
		SKILL_LEVEL_MAX = SKILL_LEVEL_HARD
	}
	skillLevel;
	char * skillLevelNames[SKILL_LEVEL_MAX + 1];

	enum TEXTURE
	{
		TEXTURE_LIGHT,
		TEXTURE_DEBRIS,
		TEXTURE_EFFECTS,
		TEXTURE_FOOT_SOLDIER_ATTACKER_RIFLEMAN,
		TEXTURE_FOOT_SOLDIER_ATTACKER_MACHINE_GUNNER,
		TEXTURE_FOOT_SOLDIER_ATTACKER_AT_SOLDIER,
		TEXTURE_FOOT_SOLDIER_ATTACKER_SNIPER,
		TEXTURE_FOOT_SOLDIER_ATTACKER_MEDIC,
		TEXTURE_FOOT_SOLDIER_DEFENDER_RIFLEMAN,
		TEXTURE_FOOT_SOLDIER_DEFENDER_MACHINE_GUNNER,
		TEXTURE_FOOT_SOLDIER_DEFENDER_AT_SOLDIER,
		TEXTURE_FOOT_SOLDIER_DEFENDER_SNIPER,
		TEXTURE_FOOT_SOLDIER_DEFENDER_MEDIC,
		TEXTURE_FOOT_SOLDIER_BODY_PARTS,
		TEXTURE_BUSH_1,
		TEXTURE_BUSH_2,
		TEXTURE_BUSH_3,
		TEXTURE_BUSH_4,
		TEXTURE_BUSH_MAX,
		TEXTURE_INTRO_IMAGE_1,
		TEXTURE_INTRO_IMAGE_2,
		TEXTURE_INTRO_IMAGE_3,
		TEXTURE_INTRO_IMAGE_4,
		TEXTURE_INTRO_IMAGE_5,
		TEXTURE_INTRO_IMAGE_6,
		TEXTURE_INTRO_IMAGE_7,
		TEXTURE_INTRO_IMAGE_8,
		TEXTURE_INTRO_IMAGE_9,
		TEXTURE_INTRO_IMAGE_10,
		TEXTURE_INTRO_IMAGE_11,
		TEXTURE_INTRO_IMAGE_12,
		TEXTURE_INTRO_IMAGE_13,
		TEXTURE_INTRO_IMAGE_14,
		TEXTURE_INTRO_IMAGE_15,
		TEXTURE_TITLE,
		TEXTURE_EMPTY,
		TEXTURE_FADE,
		TEXTURE_GUI_BACKGROUND_1,
		TEXTURE_GAME_OVER_1,
		TEXTURE_GAME_OVER_2,
		TEXTURE_VICTORY_1,
		TEXTURE_VICTORY_2,
		TEXTURE_VICTORY_3,
		TEXTURE_VICTORY_4,
		TEXTURE_MAX
	};
	HTEXTURE textures[TEXTURE_MAX];

	enum TARGET
	{
		TARGET_PARTICLES,
		TARGET_MAX
	};
	HTARGET targets[TARGET_MAX];

	enum ANIMATION
	{
		ANIMATION_SHADOW,
		ANIMATION_LIGHT,
		ANIMATION_WATER_DROP,
		ANIMATION_PARTICLE_SPRITE_DEBRIS_1,
		ANIMATION_PARTICLE_SPRITE_DEBRIS_MAX,
		ANIMATION_PARTICLE_SPRITE_CAMO_NET_1,
		ANIMATION_PARTICLE_SPRITE_SANDBAG,
		ANIMATION_PARTICLE_SPRITE_CLOUD_1,
		ANIMATION_PARTICLE_SPRITE_CLOUD_2,
		ANIMATION_PARTICLE_SPRITE_BOLT,
		ANIMATION_PARTICLE_SPRITE_FLAME,
		ANIMATION_PARTICLE_SPRITE_RUBBLE,
		ANIMATION_PARTICLE_SPRITE_FIRE_CLOUD,
		ANIMATION_PARTICLE_SPRITE_SMOKE_1_LIGHT,
		ANIMATION_PARTICLE_SPRITE_SMOKE_2_LIGHT,
		ANIMATION_PARTICLE_SPRITE_SMOKE_3_LIGHT,
		ANIMATION_PARTICLE_SPRITE_SMOKE_4_LIGHT,
		ANIMATION_PARTICLE_SPRITE_SMOKE_1_DARK,
		ANIMATION_PARTICLE_SPRITE_SMOKE_2_DARK,
		ANIMATION_PARTICLE_SPRITE_SMOKE_3_DARK,
		ANIMATION_PARTICLE_SPRITE_SMOKE_4_DARK,
		ANIMATION_BUSH_1,
		ANIMATION_BUSH_2,
		ANIMATION_BUSH_3,
		ANIMATION_BUSH_4,
		ANIMATION_BUSH_MAX,
		ANIMATION_ASSAULT_RIFLE_MUZZLE_FLASH,
		ANIMATION_TANK_CANNON_MUZZLE_FLASH,
		ANIMATION_HAND_GRENADE_EXPLOSION_CRATER,
		ANIMATION_INTRO_IMAGE_1,
		ANIMATION_INTRO_IMAGE_2,
		ANIMATION_INTRO_IMAGE_3,
		ANIMATION_INTRO_IMAGE_4,
		ANIMATION_INTRO_IMAGE_5,
		ANIMATION_INTRO_IMAGE_6,
		ANIMATION_INTRO_IMAGE_7,
		ANIMATION_INTRO_IMAGE_8,
		ANIMATION_INTRO_IMAGE_9,
		ANIMATION_INTRO_IMAGE_10,
		ANIMATION_INTRO_IMAGE_11,
		ANIMATION_INTRO_IMAGE_12,
		ANIMATION_INTRO_IMAGE_13,
		ANIMATION_INTRO_IMAGE_14,
		ANIMATION_INTRO_IMAGE_15,
		ANIMATION_TITLE,
		ANIMATION_TITLE_GLOOM,
		ANIMATION_FADE_FILTER,
		ANIMATION_GUI_BACKGROUND_1,
		ANIMATION_GAME_OVER_1,
		ANIMATION_GAME_OVER_2,
		ANIMATION_VICTORY_1,
		ANIMATION_VICTORY_2,
		ANIMATION_VICTORY_3,
		ANIMATION_VICTORY_4,
		ANIMATION_MAX
	};
	hgeAnimation * animations[ANIMATION_MAX];

	//Äänen lisääminen:
	//-enum
	//-loadgameen efektin lataus

	enum SOUND
	{
		SOUND_SMOKE_GRENADE_SHOOT,
		SOUND_SMOKE_GRENADE_EXPLODE,
		SOUND_FLARE,
		SOUND_RAIN,
		SOUND_DEBRIS_HITS_GROUND_1,
		SOUND_DEBRIS_HITS_GROUND_2,
		SOUND_DEBRIS_HITS_GROUND_MAX,
		SOUND_DEBRIS_ROCK_HITS_GROUND_1,
		SOUND_DEBRIS_ROCK_HITS_GROUND_2,
		SOUND_DEBRIS_ROCK_HITS_GROUND_MAX,
		SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_1,
		SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_2,
		SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_3,
		SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_4,
		SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_5,
		SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_MAX,
		SOUND_DEBRIS_HITS_METAL_1,
		SOUND_DEBRIS_HITS_METAL_2,
		SOUND_DEBRIS_HITS_METAL_MAX,
		SOUND_DEBRIS_SMALL_HITS_METAL_1,
		SOUND_DEBRIS_SMALL_HITS_METAL_2,
		SOUND_DEBRIS_SMALL_HITS_METAL_MAX,
		SOUND_DEBRIS_METAL_HITS_GROUND_1,
		SOUND_DEBRIS_METAL_HITS_GROUND_2,
		SOUND_DEBRIS_METAL_HITS_GROUND_MAX,
		SOUND_DEBRIS_SANDBAG_FALL_HIT_1,
		SOUND_DEBRIS_SANDBAG_FALL_HIT_MAX = SOUND_DEBRIS_SANDBAG_FALL_HIT_1,
		SOUND_SUPPRESSED_EXPLOSION_1,
		SOUND_SUPPRESSED_EXPLOSION_MAX = SOUND_SUPPRESSED_EXPLOSION_1,
		SOUND_TREE_FALL_1,
		SOUND_TREE_FALL_2,
		SOUND_TREE_FALL_MAX = SOUND_TREE_FALL_2,
		SOUND_NAPALM_EXPLOSION_1,
		SOUND_NAPALM_EXPLOSION_2,
		SOUND_NAPALM_EXPLOSION_3,
		SOUND_NAPALM_EXPLOSION_MAX = SOUND_NAPALM_EXPLOSION_3,
		SOUND_FIGHTER_PLANE_FLY_BY,
		SOUND_MISSILE_FLY_1,
		SOUND_MISSILE_FLY_2,
		SOUND_MISSILE_FLY_3,
		SOUND_MISSILE_FLY_MAX = SOUND_MISSILE_FLY_3,
		SOUND_MISSILE_EXPLOSION_1,
		SOUND_MISSILE_EXPLOSION_2,
		SOUND_MISSILE_EXPLOSION_3,
		SOUND_MISSILE_EXPLOSION_MAX = SOUND_MISSILE_EXPLOSION_3,
		SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_1,
		SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_2,
		SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_3,
		SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_MAX = SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_3,
		SOUND_FOOT_SOLDIER_MACHINE_GUN_FIRING_1,
		SOUND_FOOT_SOLDIER_MACHINE_GUN_FIRING_2,
		SOUND_FOOT_SOLDIER_AT_WEAPON_FIRING,
		SOUND_FOOT_SOLDIER_DEATH_1,
		SOUND_FOOT_SOLDIER_DEATH_2,
		SOUND_FOOT_SOLDIER_DEATH_3,
		SOUND_FOOT_SOLDIER_DEATH_4,
		SOUND_FOOT_SOLDIER_DEATH_5,
		SOUND_FOOT_SOLDIER_DEATH_6,
		SOUND_FOOT_SOLDIER_DEATH_7,
		SOUND_FOOT_SOLDIER_DEATH_8,
		SOUND_FOOT_SOLDIER_DEATH_9,
		SOUND_FOOT_SOLDIER_DEATH_10,
		SOUND_FOOT_SOLDIER_DEATH_11,
		SOUND_FOOT_SOLDIER_DEATH_12,
		SOUND_FOOT_SOLDIER_DEATH_13,
		SOUND_FOOT_SOLDIER_DEATH_14,
		SOUND_FOOT_SOLDIER_DEATH_15,
		SOUND_FOOT_SOLDIER_DEATH_16,
		SOUND_FOOT_SOLDIER_DEATH_MAX = SOUND_FOOT_SOLDIER_DEATH_16,
		SOUND_PROJECTILE_HIT_TREE_1,
		SOUND_PROJECTILE_HIT_TREE_LAST,
		SOUND_MORTAR_FIRE_1,
		SOUND_MORTAR_FIRE_2,
		SOUND_MORTAR_FIRE_LAST,
		SOUND_MORTAR_WHISTLE_1,
		SOUND_MORTAR_WHISTLE_LAST,
		SOUND_MORTAR_SHELL_EXPLOSION_1,
		SOUND_MORTAR_SHELL_EXPLOSION_LAST,
		SOUND_PRESSURE,
		SOUND_BASE_MG_FIRE_1,
		SOUND_BASE_MG_FIRE_2,
		SOUND_BASE_MG_FIRE_3,
		SOUND_BASE_MG_FIRE_LAST,
		SOUND_HAND_GRENADE_EXPLOSION,
		SOUND_INTRO_MUSIC,
		SOUND_INTRO_MUSIC_2,
		SOUND_INTRO_MUSIC_3,
		SOUND_MENU_MUSIC,
		SOUND_LEVEL_INTRO_MUSIC_1,
		SOUND_LEVEL_INTRO_MUSIC_2,
		SOUND_LEVEL_INTRO_MUSIC_3,
		SOUND_GAME_OVER_MUSIC_1,
		SOUND_VICTORY_MUSIC_1,
		SOUND_MAX
	};
	HEFFECT sounds[SOUND_MAX];

	HCHANNEL musicChannel;
	
	//Particle effects:
	enum PARTICLE_SYSTEM
	{
		PARTICLE_SYSTEM_DUST_RISE,
		PARTICLE_SYSTEM_ASSAULT_RIFLE_FIRING_SMOKE_1,
		PARTICLE_SYSTEM_CANNON_FIRING_SMOKE,
		PARTICLE_SYSTEM_AT_FIRING_SMOKE,
		PARTICLE_SYSTEM_AT_FIRING_BACKFIRE,
		PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_DIRT,
		PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_SHRAPNEL,
		PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_FIRE,
		PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_SMOKE,
		PARTICLE_SYSTEM_MAX
	};
	hgeParticleSystem * particleSystems[PARTICLE_SYSTEM_MAX];

	enum COLOR
	{
		COLOR_HUD_TEXT_NORMAL,
		COLOR_HUD_TEXT_CRITICAL,
		COLOR_TEXT_MESSAGE_DEFENDER_SIDE,
		COLOR_TEXT_MESSAGE_ATTACKER_SIDE,
		COLOR_MAX
	};
	DWORD colors[COLOR_MAX];

	//general game information
	static int SCREENWIDTH;
	static int SCREENHEIGHT;
	static int MIDSCREENWIDTH;
	static int MIDSCREENHEIGHT;
	static float FIRE_START_Y;

	static Distance METER;
	static Distance KILOMETER;
	static Distance BIG_DISTANCE;
	static Distance GROUND_DISTANCE_FROM_SCREEN;

	int scores[CombatObject::NUMBER_OF_SIDES][GameObject::NUMBER_OF_TYPES][GameObject::SUBTYPE_MAX];

	//State is a state of graphical output.
	enum STATE 
	{
		GAME_LOAD,
		INTRO,
		MENU,
		OPTIONS_MENU,
		START_TRAINING_GAME,
		START_GAME,
		LEVEL_LOAD, 
		LEVEL_START_INTRO,
		LEVEL_RUN,
		PAUSED,
		MIDLEVEL_DIALOG,
		LEVEL_FINISHED,
		AFTER_LEVEL_BRIEFING,
		GAME_FINISHED,
		GAME_OVER,
		QUIT_GAME,
	}
	state,
	previousState,
	nextState;
	bool training;

	float stateTimer;
	float stateChangeTimer;

	enum MENU_CHOICE
	{
		 ONE_PLAYER_GAME, 
		 TWO_PLAYERS_GAME, 
		 TRAINING,
		 OPTIONS, 
		 QUIT
	};
	int menu_choice;

	enum OPTIONS_CHOICE
	{
		START_LEVEL,
		OPTIONS_CHOICE_SKILL_LEVEL,
		MUSIC_VOLUME,
		FX_VOLUME,
		OBJECT_SIZE,
		SHOW_MG_TRACERS,
		SHOW_CANNON_TRACERS,
		MAX_NUMBER_OF_SHADOWS,
		FRIENDLY_FIRE,
		ZOOM,
		TIME_ADJUST,
		PLAY_SOUNDS,
		DISPLAY_MESSAGES,
		MODIFY_BACKGROUND,
		AMOUNT_OF_PARTICLES,
		AMOUNT_OF_BLOOD,
		BACK,
		OPTIONS_CHOICE_MAX
	};
	int options_choice;
	 
	bool show_collisionspheres;
	bool show_locations;
	bool show_mg_tracers;
	bool show_cannon_tracers;
	bool show_debug_text;
	bool list_all_objects;
	bool friendly_fire;
	bool zoom;
	bool time_adjust;
	bool play_sounds;
	bool display_messages;
	bool modify_background;

	map<const char*, int, ltstr> options;

	int musicVolume;
	int fxVolume;
	int amountOfParticles;
	int amountOfBlood;
	int maxShadows; //maximum number of shadows cast by object
	float timeAdjust;
	float zoomLevel; 
	float dpx, dpy;
	bool godMode;

	enum MIDLEVEL_CHOICE
	{
		BACK_TO_GAME,
		GOTO_MENU,
		GOTO_OPTIONS,
		CLOSE_GAME,
		MIDLEVEL_CHOICE_MAX
	};
	int midlevel_choice;

	bool application_close_flag;

	HTEXTURE particletex;
	HTEXTURE particletex2;
	HTEXTURE particletex3;
	hgeSprite * smokeParticleSprite;
	hgeSprite * smokeParticleSprite2;
	hgeSprite * smokeParticleSprite3;
	hgeSprite * fireParticleSprite;
	hgeSprite * fireCloudParticleSprite;
	hgeSprite * debrisParticleSprite;
	hgeSprite * debrisParticleSprite2;

	//Constructors, destructors
	Game(void);
	~Game(void);

	//Graphical game state functions
	bool run(void);
	void loadGame(void);
	void playIntro(void);
	void menu(void);
	void optionsDialog(void);
	void startTrainingGame(void);
	void startGame(void);
	void loadLevel(void);
	void levelStartIntro(void);
	void runLevel(void);
	void pause(void);
	void midLevelDialog(void);
	void levelFinished(void);
	void afterLevelBriefing(void);
	void gameFinished(void);
	void gameOver(void);
	void quit(void);

	//runLevel -functions
	void setTimerDelta(void);
	void processInput(void);
	void render(void);
	void checkGameStatus(void);

	//Misc. public
	void setState(int nextState, Time stateChangeDelay = 0.0f, Time fadingOutTime = 0.0f, Time fadingInTime = 0.0f);
	void processStateFlow(void);
	void renderLoadScreen(char *);
	void freeLevelResources(void);
	void freeGameResources(void);
	HEFFECT playSound(HEFFECT effect, int volume = 100, float pitch = 1.0f, bool loop = false);
	HEFFECT playMusic(HEFFECT effect, int volume = 100, bool loop = true);
	void processScreenFading(void);
	void setOutGameScene(OutGameScene * slideShow);
	bool playOutGameScene(void);
	void showMessage(const char * message, int colorIndex, Time delay = 0.0f);

private:
	void renderDebugText(void);
	void zoomInput(int key);

	//other:
	float optts; //options dialog text size, you can adjust this with mouse if needed.
};