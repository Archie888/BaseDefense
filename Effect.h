#pragma once
#include "GameObject.h"
#include "CombatObject.h"
#include "hgeparticle.h"

struct Particle
{
	float x;
	float y;
	float h;

	float dx;
	float dy;
	float dh;

	float speed;
	float horiDir;
	float vertDir;
	float direction;
	float size;
	float spin;
	DWORD color;
	bool active;
	int bounces;

	bool rotation_flag;
	int rotationAnimationFrame;
	float animationRotationTimer;
	float animationRotationTime;
	int spriteIndex;
};

class FlyingObjectsParticleSystem
{
public:
	enum INTS
	{
		PARTICLES_MAX = 5000,
		MAX_SPRITES = 5
	};

	Particle particles[PARTICLES_MAX];

	hgeAnimation * sprites[MAX_SPRITES];
	int nSprites;
	int emission;
	int thisEmission;
	int particlesAlive;
	float startTime;
	float updateTime;
	float updateTimer;
	DWORD color1;
	DWORD color2;
	float a1, r1, g1, b1;
	float da, dr, dg, db;
	bool colorSpin;
	float particleWeight;
	float backgroundModificationRatio;
	int maxBounces;

	//dbg:
	float leftHorizontalBorder;
	float rightHorizontalBorder;
	float x, y;

	static float FLOATING_WEIGHT;

	FlyingObjectsParticleSystem();
	~FlyingObjectsParticleSystem() {}
	void set(
		float FPS,
		int emission,
		DWORD color1,
		DWORD color2,
		bool colorSpin,
		float sizeMin,
		float sizeMax,
		float particleWeight,
		int maxBounces,
		float spinMin,
		float spinMax,
		float backgroundModificationRatio);

	void Update(float timerDelta); //Updates particle system.
	void MoveTo(float x, float y, bool bMoveParticles = false); //Zoves particle system to a new position.
	void FireAt(float x, float y, float h,
		float emissionRatio,
		float horizontalDirection,
		float verticalDirection,
		float horizontalSpread,
		float verticalSpread,
		float speedMin,
		float speedMax,
		float startDirectionMin = 0.0f,
		float startDirectionMax = 6.2831855f,
		float preUpdate = 0.0f); //Fires particle system at the specified screen position.
	void Render(); //Renders particle system to the screen.
	int GetParticlesAlive(); //Returns current number of active particles.
	float GetAge(); //Returns the particle system's age.
	virtual void createSoundEmitter(float x, float y) {}
};

class FlyingDebris : public FlyingObjectsParticleSystem
{
public:
	FlyingDebris() {}
	~FlyingDebris() {}
	void createSoundEmitter(float x, float y);
	virtual void makeSoundEmitter(float x, float y) {}
};

class FlyingSoil : public FlyingDebris
{
public:
	FlyingSoil();
	~FlyingSoil() {}
	void makeSoundEmitter(float x, float y);
};

class FlyingMetal : public FlyingDebris
{
public:
	FlyingMetal();
	~FlyingMetal() {}
	void makeSoundEmitter(float x, float y);
};

class FlyingSand : public FlyingDebris
{
public:
	FlyingSand();
	~FlyingSand() {}
	void makeSoundEmitter(float x, float y);
};

class RippedCamoNet : public FlyingDebris
{
public:
	RippedCamoNet();
	~RippedCamoNet() {}
};

class MovedSandBags : public FlyingDebris
{
public:
	MovedSandBags();
	~MovedSandBags() {}
	void makeSoundEmitter(float x, float y);
};

class ComplexParticleSystem
{
public:

#define MAX_TRANSITIONS 3

	hgeParticleSystem * particleSystems[MAX_TRANSITIONS];

	float x, y;

	int numberOfInfos; //Number of numberOfInfos. If 0, there are no numberOfInfos.
	int currentInfo; //Index of the current transition. From 0 to MAX_TRANSITIONS - 1.
	float currentInfoTime; //The time when the particle system was launched. This is changed on every transition.
	float infoTimes[MAX_TRANSITIONS]; //Transition times for each transition. Contains nothing, if there are no numberOfInfos.
	hgeParticleSystemInfo infos[MAX_TRANSITIONS]; //hgeParticleSystemInfo:s for the numberOfInfos. Contains nothing, if there are no numberOfInfos.
	bool fired; //Is the particle system launched?

	ComplexParticleSystem();
	~ComplexParticleSystem();

	void setPS(hgeParticleSystemInfo * psi);
	void setPS(ComplexParticleSystem * cps);
	void addInfo(hgeParticleSystemInfo * psi, float time);
	void deletePS();

	void fireAt(float x, float y);
	void update(float timerDelta);
	void render(void);
	void stop(void);
	bool isActive(void);
};

class CustomParticleManager
{
public:

#define CPM_NUMBER_OF_PS 100

	ComplexParticleSystem cpSystems[CPM_NUMBER_OF_PS];

	//dbg:
	int lastInd;
	float x, y;

	CustomParticleManager();
	~CustomParticleManager();
	bool spawnPS(hgeParticleSystemInfo * psi, float x, float y);
	bool spawnPS(ComplexParticleSystem * cps, float x, float y);
	void update(float timerDelta);
	void render(bool fromBeginningOfList);
	void stopAll(void);
	bool areAllFinished(void);
};

class EffectData
{
public:
	EffectData() {}
	~EffectData() {}
};

class TestEffectData : public EffectData
{
public:
	TestEffectData() {}
	~TestEffectData() {}

	FlyingObjectsParticleSystem foParticleSystem;
};

class ExplosionEffectData : public EffectData
{
public:
	ExplosionEffectData() {}
	~ExplosionEffectData() {}

	FlyingSoil foParticleSystem;
};

class TankExplosionEffectData : public EffectData
{
public:
	FlyingMetal foParticleSystem;

	TankExplosionEffectData() {}
	~TankExplosionEffectData() {}
};

class BaseExplosionData : public EffectData
{
public:
	FlyingSand sand;
	RippedCamoNet camoNet;
	MovedSandBags sandbags;

	BaseExplosionData() {}
	~BaseExplosionData() {}
};

class NapalmExplosionEffectData : public EffectData
{
public:
	enum INTS
	{
		MAX_BURSTS = 100
	};

	GameObject * bursts[MAX_BURSTS];
	int index;
};

class DebrisFallEffectData : public EffectData
{
public:
	enum ints
	{
		SOUND_MAX = 10
	};
	HEFFECT groundHitSounds[SOUND_MAX];
	HEFFECT metalHitSounds[SOUND_MAX];
	int nGroundHitSounds;
	int nMetalHitSounds;
	int volume;

	DebrisFallEffectData();
	~DebrisFallEffectData() {}
};

class SoilDebrisFallEffectData : public DebrisFallEffectData
{
public:
	SoilDebrisFallEffectData();
	~SoilDebrisFallEffectData() {}
};

class MetalDebrisFallEffectData : public DebrisFallEffectData
{
public:
	MetalDebrisFallEffectData();
	~MetalDebrisFallEffectData() {}
};

class SandDebrisFallEffectData : public DebrisFallEffectData
{
public:
	SandDebrisFallEffectData();
	~SandDebrisFallEffectData() {}
};

class SandbagFallEffectData : public DebrisFallEffectData
{
public:
	SandbagFallEffectData();
	~SandbagFallEffectData() {}
};

class RainEffectData : public EffectData
{
public:

	enum INTS
	{
		WATER_DROPS_MAX = 5000
	};

	float waterDropSizes[WATER_DROPS_MAX];
	Location waterDropLocations[WATER_DROPS_MAX];

	RainEffectData() {}
	~RainEffectData() {}
};

class Effect :
	public MovingObject
{
public:

	enum EFFECTTYPE
	{
		ASSAULT_RIFLE_MUZZLE_FLASH,
		TANK_CANNON_MUZZLE_FLASH,
		MISSILE_FLY,
		AT_WEAPON_BACKFIRE,
		AT_WEAPON_FIRE,
		TANK_MG_BULLET_HIT_GROUND,
		ASSAULT_RIFLE_BULLET_HIT_GROUND,
		PROJECTILE_HIT_TANK,
		PROJECTILE_HIT_MAN,
		PROJECTILE_HIT_CORPSE, //separate types for corpses, because with corpses you have to render stuff to the corpses render targets while the blood pond is growing (a haX0r).
		LARGE_PROJECTILE_HIT_MAN,
		LARGE_PROJECTILE_HIT_CORPSE,
		BLOOD_POND,
		MISSILE_EXPLOSION,
		TANKSHELL_EXPLOSION,
		MORTARSHELL_EXPLOSION,
		HAND_GRENADE_EXPLOSION,
		NAPALM_EXPLOSION,
		NAPALM_FIRE_BURST,
		TANK_EXPLOSION,
		BASE_EXPLOSION,
		BULLET_SHELL_FLY,
		MAN_EXPLOSION,
		SHAKE,
		FLYING_SMOKING_DEBRIS,
		FIRE,
		TREE_FALL,
		DEBRIS_FALL_SOUND_EMITTER,
		SMOKE_GRENADE,
		SNOW_RAIN,
		RAIN,
		FLARE_LIGHT,
		SUN,
		TEST_EFFECT_1,
		TEST_EFFECT_2
	};
	EFFECTTYPE effectType;

	static int amount;

	//generic
	Timer timer1;
	Timer timer2;
	Timer timer3;
	Timer timer4;
	Timer collisionTimer;
	Timer illuminationTimer;
	float illuminationSize;
	bool fire;
	float counter1;
	float counter2;
	int intCounter1;
	bool flag1;
#define MAX_PARTICLE_SYSTEMS 3
	hgeParticleSystem * particleSystems[MAX_PARTICLE_SYSTEMS];
	CustomParticleManager * cpManager;
	int frameCounter;
	GameObject * targetObject;

	EffectData * ed;

	list<Effect *>::iterator illuminatorIterator;

	//static effect data:

	enum SOUND
	{
		SOUND_PROJECTILE_HIT_MAN_1,
		SOUND_PROJECTILE_HIT_MAN_2,
		SOUND_PROJECTILE_HIT_MAN_3,
		SOUND_PROJECTILE_HIT_MAN_4,
		SOUND_PROJECTILE_HIT_MAN_MAX,
		//SOUND_BULLET_DROP = SOUND_PROJECTILE_HIT_MAN_MAX,
		SOUND_MAX
	};
	static HEFFECT sounds[SOUND_MAX];

	//*** things about things ***

	//one texture to rule them all
	static HTEXTURE tex;

	//* effect specific information *

	//assault rifle muzzle flash
	static Time ASSAULT_RIFLE_MUZZLE_FLASH_TIME;

	//tank cannon muzzle flash
	static Time TANK_CANNON_MUZZLE_FLASH_TIME;

	//missile
	static Distance MISSILE_FLY_DISTANCE_BETWEEN_PARTICLE_SYSTEMS_FIRING;

	//missile explosion
	static int missileExplosionCounter;

	//tank mg / assault rifle bullet hit ground
	static const Time BULLET_DIRT_STOP_TIME; 
	static hgeSprite * bulletHitSprite1; //bullet mark on the ground
	static hgeSprite * bulletHitSprite2; //bullet mark on the ground
	static hgeSprite * bulletHitSprite3; //bullet mark on the ground
	static HEFFECT bulletHitGroundSound;

	//bullet hit tank
	static hgeParticleSystem * bulletHitTankPS; //sparks
	static HEFFECT bulletHitTankSound;

	//bullet hit man
	static hgeSprite * bloodSprite;
	static hgeSprite * bloodSprite2;
	static hgeParticleSystem * bulletHitManPS; //blood burst

	//generic shell hit:
	static const Time SHELL_EXPLOSION_DANGEROUS_TIME;

	//missile
	static Distance MISSILE_EXPLOSION_NOTIFY_RADIUS;

	//tankshell hit ground:
	static Distance TANKSHELL_EXPLOSION_DANGER_RADIUS;
	static Distance TANKSHELL_EXPLOSION_DEADLY_RADIUS;
	static const Time TANKSHELL_SMOKE_TIME;
	static const Time TANKSHELL_EXPLOSION_DEBRIS_SLOWDOWN_TIME;
	static const Time TANKSHELL_EXPLOSION_DEBRIS_STOP_TIME;
	static hgeSprite * tankShellHitSprite;
	static hgeParticleSystem * shellHitGroundFire; //fire
	static hgeParticleSystem * tankShellHitParticleSystem3; //big smoke
	static HEFFECT shellHitExplosionSound;

	//tankshell hit tank
	static Distance TANK_EXPLOSION_DANGER_RADIUS;
	static const Time TANK_EXPLOSION_FIRE_TIME;
	static const Time TANK_EXPLOSION_SMOKE_TIME;
	static hgeParticleSystem * tankShellHitTankParticleSystem1;  //explosion fire
	static hgeParticleSystem * tankShellHitTankParticleSystem3; //big smoke
	static hgeParticleSystem * tankShellHitTankParticleSystem4; // long lasting smoke
	static hgeParticleSystem * tankShellHitTankParticleSystem5; // last fire
	static HEFFECT shellHitTankExplosionSound;

	//mortarshell hit
	static Distance MORTARSHELL_EXPLOSION_NOTIFY_RADIUS;
	static Distance MORTARSHELL_EXPLOSION_DANGER_RADIUS;
	static Distance MORTARSHELL_EXPLOSION_DEADLY_RADIUS;

	//napalm
	static Distance NAPALM_EXPLOSION_SPREAD_RADIUS;

	Effect(void);
	Effect(EFFECTTYPE effectType, Location location, float direction, GameObject * targetObject = NULL, float scale = 1.0f, int info = 0, Velocity speed = 0.0f);
	~Effect(void);
	void set(EFFECTTYPE effectType, Location location, float direction, GameObject * targetObject = NULL, float scale = 1.0f, int info = 0, Velocity speed = 0.0f);
	static void init(void);
	static void release(void);
	virtual void processInteraction(void);
	virtual void render(void);
	virtual void destroy(void);
};

class BulletHitGround_Effect : public Effect
{
	
};

class Shake_Effect : public Effect
{
public:

	float origDpx, origDpy;
	float force; //1.0f +-...
	float xtrans;
	float ytrans;
	bool right;
	bool up;

	Shake_Effect(void);
	Shake_Effect(float force, Location location);
	void set(float force, Location location);
	virtual void processInteraction(void);
	virtual void render(void) {}
	void shake(void);
};