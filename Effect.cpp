#include ".\effect.h"
#include "main.h"

float FlyingObjectsParticleSystem::FLOATING_WEIGHT = 0.1f;

FlyingObjectsParticleSystem::FlyingObjectsParticleSystem()
{
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		sprites[i] = NULL;
	}
}


void FlyingObjectsParticleSystem::set(
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
	float backgroundModificationRatio)
{
	this->updateTime = 1.0f / FPS;
	this->updateTimer = 0.0f;
	this->emission = emission;
	this->color1 = color1;
	this->color2 = color2;
	BYTE a1, r1, g1, b1;
	getColorComponents(color1, &a1, &r1, &g1, &b1);
	BYTE a2, r2, g2, b2;
	getColorComponents(color2, &a2, &r2, &g2, &b2);	
	da = a2 - a1;
	dr = r2 - r1;
	dg = g2 - g1;
	db = b2 - b1;
	this->a1 = a1;
	this->r1 = r1;
	this->g1 = g1;
	this->b1 = b1;
	this->colorSpin = colorSpin;
	this->particleWeight = particleWeight;
	this->maxBounces = maxBounces;
	this->backgroundModificationRatio = backgroundModificationRatio;

	emission = checkRange(emission, 0, PARTICLES_MAX);
	for (int i = 0; i < emission; i++)
	{
		particles[i].color = color1;
		particles[i].size = randFloat(sizeMin, sizeMax);
		particles[i].spin = randFloat(spinMin, spinMax);
		if (randInt(0, 1) == 0)
			particles[i].spin = -particles[i].spin;
		particles[i].spriteIndex = randInt(0, nSprites - 1);
	}
	particlesAlive = emission;
}

void FlyingObjectsParticleSystem::FireAt(
	float x,
	float y, 
	float h,
	float emissionRatio,
	float horizontalDirection,
	float verticalDirection,
	float horizontalSpread,
	float verticalSpread,
	float speedMin,
	float speedMax,	
	float startDirectionMin,
	float startDirectionMax,
	float preUpdate)
{
	this->x = x, this->y = y;
	float halfHorizontalSpread = horizontalSpread / 2.0f;
	float horizontalDirectionMin = horizontalDirection - halfHorizontalSpread;
	float horizontalDirectionMax = horizontalDirection + halfHorizontalSpread;
	float halfVerticalSpread = verticalSpread / 2.0f;
	float verticalDirectionMin = verticalDirection - halfVerticalSpread;
	float verticalDirectionMax = verticalDirection + halfVerticalSpread;

	leftHorizontalBorder = horizontalDirectionMin;
	rightHorizontalBorder = horizontalDirectionMax;

	thisEmission = emission * emissionRatio;
	thisEmission = checkRange(thisEmission, 0, PARTICLES_MAX);
	particlesAlive = thisEmission;
	h = checkRange(h, 1.0f, 300.0f);

	float help1 = verticalSpread / thisEmission;
	float help2 = verticalDirectionMin;

	for (int i = 0; i < thisEmission; i++)
	{
		particles[i].active = true;
		particles[i].x = x;
		particles[i].y = y;
		particles[i].h = h;
		
		float thisHorizontalDirection = randFloat(horizontalDirectionMin, horizontalDirectionMax);
		float thisVerticalDirection = help2 += help1; //randFloat(verticalDirectionMin, verticalDirectionMax);
		float thisSpeed = randFloat(speedMin, speedMax);

		particles[i].dx = thisSpeed * cos(thisHorizontalDirection) * cos(thisVerticalDirection);
		particles[i].dy = thisSpeed * sin(thisHorizontalDirection) * cos(thisVerticalDirection);
		particles[i].dh = thisSpeed * sin(thisVerticalDirection);
		particles[i].speed = thisSpeed;
		particles[i].horiDir = thisHorizontalDirection;
		particles[i].vertDir = thisVerticalDirection;
		particles[i].direction = randFloat(startDirectionMin, startDirectionMax); //thisHorizontalDirection - HALF_PI;
		particles[i].bounces = 0;

		int nFrames = sprites[particles[i].spriteIndex]->GetFrames();
		particles[i].rotation_flag = (randInt(0, 1) == 0) && nFrames > 1;

		if (particles[i].rotation_flag)
		{
			particles[i].animationRotationTimer = 10.0f;
			int rotationFPS = randInt(0.0f, 24.0f);
			particles[i].animationRotationTime = 1.0f / rotationFPS;
			particles[i].rotationAnimationFrame = randInt(0, nFrames - 1);
		}
	}
	startTime = hge->Timer_GetTime();
	updateTimer = preUpdate;
}

#define PARTICLE_DROP_SPEED 50.0f
#define PARTICLE_FLOAT_SPEED 1.0f
void FlyingObjectsParticleSystem::Update(float timerDelta)
{
	//Change the color by spin speed. Different colors in the beginning and in the end of the spin cycle.

	updateTimer += timerDelta;

	if (updateTimer >= updateTime)
	{
		//Was the game FPS higher than particle system FPS?
		int updates = updateTimer / updateTime;
		float updateAmount = (float)updates * updateTime;
		updateTimer -= updateAmount;

		for (int i = 0; i < thisEmission; i++)
		{
			if (particles[i].active == false)
				continue;

			particles[i].dh -= PARTICLE_DROP_SPEED * particleWeight * updateAmount;
			particles[i].x += particles[i].dx * updateAmount;
			particles[i].y += particles[i].dy * updateAmount;
			particles[i].h += particles[i].dh * updateAmount;

			//rotation:
			if (!particles[i].rotation_flag)
			{
				particles[i].direction += particles[i].spin * updateAmount;
				float cosine = cos(particles[i].direction);
				BYTE a = fabs(a1 + da * cosine);
				BYTE r = fabs(r1 + dr * cosine);
				BYTE g = fabs(g1 + dg * cosine);
				BYTE b = fabs(b1 + db * cosine);
				particles[i].color = ((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b));
			}
			else
			{
				particles[i].animationRotationTimer += updateAmount;
				if (particles[i].animationRotationTimer >= particles[i].animationRotationTime)
				{
					int times = particles[i].animationRotationTimer / particles[i].animationRotationTime;
					particles[i].animationRotationTimer -= times * particles[i].animationRotationTime;

					int nFrames = sprites[particles[i].spriteIndex]->GetFrames();
					particles[i].rotationAnimationFrame += times;
					if (particles[i].rotationAnimationFrame >= nFrames)
					{
						int times = particles[i].rotationAnimationFrame / nFrames;
						int help = particles[i].rotationAnimationFrame - times * nFrames;
						particles[i].rotationAnimationFrame = help;
					}

					float anglePerFrame = PI_TIMES_TWO / nFrames; 
					float help1 = PI_TIMES_TWO - (float)particles[i].rotationAnimationFrame * anglePerFrame;
					float cosine = cos(help1);
					BYTE a = fabs(a1 + da * cosine);
					BYTE r = fabs(r1 + dr * cosine);
					BYTE g = fabs(g1 + dg * cosine);
					BYTE b = fabs(b1 + db * cosine);
					particles[i].color = ((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b));
				}
			}

			if (particles[i].h <= 0.0f)
			{
				particles[i].h == 0.0f;

				if (particles[i].bounces >= maxBounces)
				{
					if (randFloat(0.0f, 1.0f) <= backgroundModificationRatio)
					{
						if (game.modify_background)
						{
							DWORD col = sprites[particles[i].spriteIndex]->GetColor();
							sprites[particles[i].spriteIndex]->SetColor(particles[i].color);
							sprites[particles[i].spriteIndex]->RenderEx(particles[i].x, particles[i].y, particles[i].direction, particles[i].size * GameObject::objectSize, particles[i].size * GameObject::objectSize);
							sprites[particles[i].spriteIndex]->SetColor(col);
						}

					}
					particles[i].active = false;
					particlesAlive--;
				}
				else
				{	
					bool firstBounce = particles[i].bounces == 0;

					if (firstBounce)
					{
						createSoundEmitter(particles[i].x, particles[i].y);
					}

					float thisHorizontalDirection;
					
					if (particles[i].bounces == 0)
						thisHorizontalDirection = particles[i].horiDir - randFloat(-1.0f, 1.0f); 
					else
						thisHorizontalDirection = particles[i].horiDir;
					float thisVerticalDirection = randFloat(0.0f, HALF_PI); //help2 += help1; //randFloat(verticalDirectionMin, verticalDirectionMax);
					
					
					float sinVert = sin(thisVerticalDirection);
					particles[i].speed *= 0.7f * sinVert;
					float thisSpeed = particles[i].speed; //randFloat(speedMin, speedMax);
					
					float speedCosVert = thisSpeed * cos(thisVerticalDirection);
					particles[i].dx = cos(thisHorizontalDirection) * speedCosVert;
					particles[i].dy = sin(thisHorizontalDirection) * speedCosVert;
					particles[i].dh = thisSpeed * sinVert;
					particles[i].horiDir = thisHorizontalDirection;
					particles[i].vertDir = thisVerticalDirection;


					if (!particles[i].rotation_flag)
					{
						if (firstBounce)
							particles[i].spin *= particles[i].speed / 5.0f * cos(particles[i].vertDir);
						else
							particles[i].spin *= 0.5f;

						if (randInt(0, 1) == 0)
							particles[i].spin = -particles[i].spin;
					}
					else
					{
						if (firstBounce)
						{
							particles[i].animationRotationTime = particles[i].speed / 1200.0f * cos(particles[i].vertDir * 0.8f);
						}
						else
							particles[i].animationRotationTime = randFloat(0.03f, 0.06f);

					}


					particles[i].bounces++;
				}
			}
		}
	}
}

void FlyingObjectsParticleSystem::Render(void)
{
	DWORD col[MAX_SPRITES];
	for (int i = 0; i < nSprites; i++)
	{
		col[i] = sprites[i]->GetColor();
	}

	for (int i = 0; i < thisEmission; i++)
	{
		if (particles[i].active == false)
			continue;

		//color
		sprites[particles[i].spriteIndex]->SetColor(particles[i].color);
		//float thisSize = (particles[i].size * 100.0f) / (100.0f - particles[i].h);

		//visual size
		float thisSize = (particles[i].size * 170) / (170.0f - particles[i].h);

		//frame
		sprites[particles[i].spriteIndex]->SetFrame(particles[i].rotationAnimationFrame);

		sprites[particles[i].spriteIndex]->RenderEx(particles[i].x, particles[i].y, particles[i].direction, thisSize * GameObject::objectSize, thisSize * GameObject::objectSize);
		float yy = -11.0f;
	}
	for (int i = 0; i < nSprites; i++)
	{
		sprites[i]->SetColor(col[i]);
	}
	for (int i = 0; i < nSprites; i++)
	{
		sprites[i]->SetFrame(0);
	}
}

int FlyingObjectsParticleSystem::GetParticlesAlive(void)
{
	return particlesAlive;
}

float FlyingObjectsParticleSystem::GetAge(void)
{
	return hge->Timer_GetTime() - startTime;
}

#define OBJECT_LIMIT_FOR_SOUND_EMITTERS 70
void FlyingDebris::createSoundEmitter(float x, float y)
{
	if (GameObject::numberOfObjects < OBJECT_LIMIT_FOR_SOUND_EMITTERS)
	{

		float seconds = 0.1f;
		bool play = randFloat(0.0f, seconds / timerDelta) < 1.0f;
		if (play)
			makeSoundEmitter(x, y);
	}
}

FlyingSoil::FlyingSoil()
{
	nSprites = 1;
	sprites[0] = game.animations[Game::ANIMATION_PARTICLE_SPRITE_RUBBLE];
}

void FlyingSoil::makeSoundEmitter(float x, float y)
{
	Effect * e = new Effect(Effect::DEBRIS_FALL_SOUND_EMITTER, Location(x, y), 0.0f);
	e->ed = new SoilDebrisFallEffectData();
	game.objects.addGameObject(e);
}

FlyingMetal::FlyingMetal()
{
	nSprites = Game::ANIMATION_PARTICLE_SPRITE_DEBRIS_MAX - Game::ANIMATION_PARTICLE_SPRITE_DEBRIS_1 + 1;
	for (int i = 0; i < nSprites; i++)
	{
		sprites[i] = game.animations[Game::ANIMATION_PARTICLE_SPRITE_DEBRIS_1 + i];
	}	
}

void FlyingMetal::makeSoundEmitter(float x, float y)
{
	Effect * e = new Effect(Effect::DEBRIS_FALL_SOUND_EMITTER, Location(x, y), 0.0f);
	e->ed = new MetalDebrisFallEffectData();
	game.objects.addGameObject(e);
}

FlyingSand::FlyingSand()
{
	nSprites = 1;
	sprites[0] = game.animations[Game::ANIMATION_PARTICLE_SPRITE_RUBBLE];
}

void FlyingSand::makeSoundEmitter(float x, float y)
{
	Effect * e = new Effect(Effect::DEBRIS_FALL_SOUND_EMITTER, Location(x, y), 0.0f);
	e->ed = new SandDebrisFallEffectData();
	game.objects.addGameObject(e);
}

RippedCamoNet::RippedCamoNet()
{
	nSprites = 1;
	sprites[0] = game.animations[Game::ANIMATION_PARTICLE_SPRITE_CAMO_NET_1];
}

MovedSandBags::MovedSandBags()
{
	nSprites = 1;
	sprites[0] = game.animations[Game::ANIMATION_PARTICLE_SPRITE_SANDBAG];
}

void MovedSandBags::makeSoundEmitter(float x, float y)
{
	Effect * e = new Effect(Effect::DEBRIS_FALL_SOUND_EMITTER, Location(x, y), 0.0f);
	e->ed = new SandbagFallEffectData();
	game.objects.addGameObject(e);
}

#define COMPLEX_PS_UPDATE_SPEED 30.0f
ComplexParticleSystem::ComplexParticleSystem()
:numberOfInfos(0)
,currentInfo(0)
,currentInfoTime(0.0f)
,fired(false)
{
	for (int i = 0; i < MAX_TRANSITIONS; i++)
	{
		particleSystems[i] = NULL;
		infoTimes[i] = -1.0f;
		memset(&(infos[i]), 0, sizeof(hgeParticleSystemInfo));
	}
}

ComplexParticleSystem::~ComplexParticleSystem()
{
	for (int i = 0; i < MAX_TRANSITIONS; i++)
	{
		bool test = particleSystems[i] == (hgeParticleSystem *)0xffffffff;
		if (particleSystems[i] != NULL && !test)
		{
			delete particleSystems[i]; 
			particleSystems[i] = NULL; 
		}
	}
}


void ComplexParticleSystem::setPS(hgeParticleSystemInfo * psi)
{
    numberOfInfos = 1;
	currentInfo = 0;
	currentInfoTime = 0.0f;
	for (int i = 0; i < MAX_TRANSITIONS; i++)
	{
		infoTimes[i] = -1.0f;
		memset(&(infos[i]), 0, sizeof(hgeParticleSystemInfo));
	}
	infos[0] = *psi;
	fired = false;
}

void ComplexParticleSystem::setPS(ComplexParticleSystem * cps)
{
	this->numberOfInfos = cps->numberOfInfos;
	this->currentInfo = 0;
	this->currentInfoTime = 0.0f;
	for (int i = 0; i < MAX_TRANSITIONS; i++)
	{
		this->infoTimes[i] = cps->infoTimes[i];
		this->infos[i] = cps->infos[i];
	}
	fired = false;
}

void ComplexParticleSystem::addInfo(hgeParticleSystemInfo * psi, float time)
{
	numberOfInfos++;
	infos[numberOfInfos - 1] = *psi;
	infoTimes[numberOfInfos - 1] = time;
}

void ComplexParticleSystem::deletePS()
{
	for (int i = 0; i < MAX_TRANSITIONS; i++)
	{
		bool test = particleSystems[i] == (hgeParticleSystem *)0xffffffff;
		if (particleSystems[i] != NULL && !test)
		{
			delete particleSystems[i]; 
			particleSystems[i] = NULL; 
		}
	}
	numberOfInfos = 0;
	currentInfo = 0;
	currentInfoTime = 0.0f;

	for (int i = 0; i < MAX_TRANSITIONS; i++)
	{
		infoTimes[i] = -1.0f;
		memset(&(infos[i]), 0, sizeof(hgeParticleSystemInfo));
	}
	fired = false;
}

void ComplexParticleSystem::fireAt(float x, float y)
{
	if (!particleSystems[0])
	{
		particleSystems[0] = new hgeParticleSystem(&(infos[0]), COMPLEX_PS_UPDATE_SPEED);
	}
	else
		particleSystems[0]->info = infos[0];
	this->x = x;
	this->y = y;
	currentInfo = 0;
	currentInfoTime = hge->Timer_GetTime();
	particleSystems[0]->FireAt(x, y);
	fired = true;
}

void ComplexParticleSystem::update(float timerDelta)
{
	if (fired)
	{
		for (int i = 0; i < MAX_TRANSITIONS; i++)
		{
			bool test = particleSystems[i] == (hgeParticleSystem *)0xffffffff;
			if (particleSystems[i] != NULL && !test)
			{
				particleSystems[i]->Update(timerDelta);
			}
		}
		
		if (numberOfInfos > 1)
		{
			float time = hge->Timer_GetTime();
			if (time - infoTimes[currentInfo] > currentInfoTime)
			{
				if (currentInfo < numberOfInfos - 1)
				{
					currentInfo++;
					if (!particleSystems[currentInfo])
					{
						particleSystems[currentInfo] = new hgeParticleSystem(&(infos[currentInfo]), COMPLEX_PS_UPDATE_SPEED);
					}
					else
						particleSystems[currentInfo]->info = infos[currentInfo];
					particleSystems[currentInfo]->FireAt(x, y);
					//particleSystem->info = infos[currentInfo];
					currentInfoTime = time;			
				}
			}
		}

		if (!isActive()) deletePS();
	}
}

void ComplexParticleSystem::render(void)
{
	for (int i = 0; i < MAX_TRANSITIONS; i++)
	{
		bool test = particleSystems[i] == (hgeParticleSystem *)0xffffffff;
		if (particleSystems[i] != NULL && !test)
		{
			particleSystems[i]->Render();
		}
	}
}

void ComplexParticleSystem::stop(void)
{
	for (int i = 0; i < MAX_TRANSITIONS; i++)
	{
		bool test = particleSystems[i] == (hgeParticleSystem *)0xffffffff;
		if (particleSystems[i] != NULL && !test)
		{
			particleSystems[i]->Stop();
		}
	}

	currentInfo = 0;
	fired = false;
}

bool ComplexParticleSystem::isActive(void)
{
	bool ret = false;


	for (int i = 0; i < MAX_TRANSITIONS; i++)
	{
		bool test = particleSystems[i] == (hgeParticleSystem *)0xffffffff;
		if (particleSystems[i] != NULL && !test)
		{	
			int particlesAlive = particleSystems[i]->GetParticlesAlive();
			float age = particleSystems[i]->GetAge();

			if (particlesAlive > 0 || age != -2)
			{
				ret = true;
				break;
			}
		}
	}

	return ret;
}

CustomParticleManager::CustomParticleManager()
:lastInd(0)
,x(0.0f)
,y(0.0f)
{
}

CustomParticleManager::~CustomParticleManager()
{
}

bool CustomParticleManager::spawnPS(hgeParticleSystemInfo * psi, float x, float y)
{
	if (this->x == 0.0f && this->y == 0.0f)
	{
		this->x = x;
		this->y = y;
	}

	for (int i = 0; i < CPM_NUMBER_OF_PS; i++)
	{
		if (!cpSystems[i].isActive())
		{
			cpSystems[i].setPS(psi);
			cpSystems[i].fireAt(x, y);
			lastInd = i;
			return true;
		}
	}
	return false;
}

bool CustomParticleManager::spawnPS(ComplexParticleSystem * cps, float x, float y)
{
	if (this->x == 0.0f && this->y == 0.0f)
	{
		this->x = x;
		this->y = y;
	}

	for (int i = 0; i < CPM_NUMBER_OF_PS; i++)
	{
		if (!cpSystems[i].isActive())
		{
			cpSystems[i].setPS(cps);
			cpSystems[i].fireAt(x, y);
			lastInd = i;
			return true;
		}
	}
	return false;
}

void CustomParticleManager::update(float timerDelta)
{
	for (int i = 0; i < CPM_NUMBER_OF_PS; i++)
	{
		cpSystems[i].update(timerDelta);
	}
}

//custom render order options
void CustomParticleManager::render(bool fromBeginningOfList)
{
	if (fromBeginningOfList)
	{
		for (int i = 0; i < CPM_NUMBER_OF_PS; i++)
		{
			cpSystems[i].render();
		}
	}
	else
	{
		for (int i = CPM_NUMBER_OF_PS - 1; i >= 0; i--)
		{
			cpSystems[i].render();
		}
	}
}

void CustomParticleManager::stopAll(void)
{
	for (int i = 0; i < CPM_NUMBER_OF_PS; i++)
	{
		cpSystems[i].stop();
	}
}

bool CustomParticleManager::areAllFinished(void)
{
	bool ret = true;

	for (int i = 0; i < CPM_NUMBER_OF_PS; i++)
	{
		if (cpSystems[i].isActive())
		{
			ret = false;
			break;
		}
	}

	return ret;
}

DebrisFallEffectData::DebrisFallEffectData()
{
	nGroundHitSounds = 0;
	nMetalHitSounds = 0;

	for (int i = 0; i < SOUND_MAX; i++)
	{
		groundHitSounds[i] = NULL;
		metalHitSounds[i] = NULL;
	}

	volume = 100;
}

SoilDebrisFallEffectData::SoilDebrisFallEffectData()
{
	nGroundHitSounds = Game::SOUND_DEBRIS_HITS_GROUND_MAX - Game::SOUND_DEBRIS_HITS_GROUND_1 + 1;
	for (int i = 0; i < nGroundHitSounds; i++)
	{
		groundHitSounds[i] = game.sounds[Game::SOUND_DEBRIS_HITS_GROUND_1 + i];
	}

	nMetalHitSounds = Game::SOUND_DEBRIS_HITS_METAL_MAX - Game::SOUND_DEBRIS_HITS_METAL_1 + 1;
	for (int i = 0; i < nMetalHitSounds; i++)
	{
		metalHitSounds[i] = game.sounds[Game::SOUND_DEBRIS_HITS_METAL_1 + i];
	}

	volume = 70;
}

MetalDebrisFallEffectData::MetalDebrisFallEffectData()
{
	nGroundHitSounds = Game::SOUND_DEBRIS_METAL_HITS_GROUND_MAX - Game::SOUND_DEBRIS_METAL_HITS_GROUND_1 + 1;
	for (int i = 0; i < nGroundHitSounds; i++)
	{
		groundHitSounds[i] = game.sounds[Game::SOUND_DEBRIS_METAL_HITS_GROUND_1 + i];
	}

	nMetalHitSounds = Game::SOUND_DEBRIS_HITS_METAL_MAX - Game::SOUND_DEBRIS_HITS_METAL_1 + 1;
	for (int i = 0; i < nMetalHitSounds; i++)
	{
		metalHitSounds[i] = game.sounds[Game::SOUND_DEBRIS_HITS_METAL_1 + i];
	}

	volume = 70;
}

SandDebrisFallEffectData::SandDebrisFallEffectData()
{
	nGroundHitSounds = Game::SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_MAX - Game::SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_1 + 1;
	for (int i = 0; i < nGroundHitSounds; i++)
	{
		groundHitSounds[i] = game.sounds[Game::SOUND_DEBRIS_ROCK_SAND_HITS_GROUND_1 + i];
	}

	nMetalHitSounds = Game::SOUND_DEBRIS_SMALL_HITS_METAL_MAX - Game::SOUND_DEBRIS_SMALL_HITS_METAL_1 + 1;
	for (int i = 0; i < nMetalHitSounds; i++)
	{
		metalHitSounds[i] = game.sounds[Game::SOUND_DEBRIS_SMALL_HITS_METAL_1 + i];
	}

	volume = 30;
}

SandbagFallEffectData::SandbagFallEffectData()
{
	nGroundHitSounds = Game::SOUND_DEBRIS_SANDBAG_FALL_HIT_MAX - Game::SOUND_DEBRIS_SANDBAG_FALL_HIT_1 + 1;
	for (int i = 0; i < nGroundHitSounds; i++)
	{
		groundHitSounds[i] = game.sounds[Game::SOUND_DEBRIS_SANDBAG_FALL_HIT_1 + i];
	}

	volume = 100;
}

int Effect::amount = 0;

Time Effect::ASSAULT_RIFLE_MUZZLE_FLASH_TIME = 0.1f;
Time Effect::TANK_CANNON_MUZZLE_FLASH_TIME = 0.05f;
Distance Effect::MISSILE_FLY_DISTANCE_BETWEEN_PARTICLE_SYSTEMS_FIRING = Projectile::MISSILE_SPEED / 15.0f / MAX_PARTICLE_SYSTEMS;
int Effect::missileExplosionCounter = 0;
#define MISSILE_FLY_SMOKE_DISTANCE 1500.0f
#define MISSILE_FLY_WHISTLE_DISTANCE 2000.0f
const Time Effect::BULLET_DIRT_STOP_TIME = 0.2f;
const Time Effect::TANKSHELL_SMOKE_TIME = 0.3f;
const Time Effect::TANK_EXPLOSION_FIRE_TIME = 6.0f;
const Time Effect::TANK_EXPLOSION_SMOKE_TIME = 1.0f;
const Time Effect::TANKSHELL_EXPLOSION_DEBRIS_SLOWDOWN_TIME = 0.3f;
const Time Effect::TANKSHELL_EXPLOSION_DEBRIS_STOP_TIME = 2.0f;
const Timer Effect::SHELL_EXPLOSION_DANGEROUS_TIME = 0.2f;
Distance Effect::TANKSHELL_EXPLOSION_DANGER_RADIUS = 20.0f;
Distance Effect::TANKSHELL_EXPLOSION_DEADLY_RADIUS = 7.0f;
Distance Effect::TANK_EXPLOSION_DANGER_RADIUS = 30.0f;
Distance Effect::MISSILE_EXPLOSION_NOTIFY_RADIUS = 300.0f;
#define NAPALM_EXPLOSION_NOTIFY_RADIUS 400.0f
Distance Effect::MORTARSHELL_EXPLOSION_NOTIFY_RADIUS = 300.0f;
Distance Effect::MORTARSHELL_EXPLOSION_DANGER_RADIUS = 200.0f;
Distance Effect::MORTARSHELL_EXPLOSION_DEADLY_RADIUS = 30.0f;

#define MORTARSHELL_EXPLOSION_NOTIFY_RADIUS_DEF 200.0f

#define NAPALM_EXPLOSION_FIRE_CLOUD_SIZE_START 0.0f
#define NAPALM_EXPLOSION_FIRE_CLOUD_SIZE_END 5.0f
#define NAPALM_EXPLOSION_FIRE_CLOUD_EMISSION 50
#define NAPALM_EXPLOSION_FIRE_CLOUD_CHANGE_TIME 1.0f
#define NAPALM_EXPLOSION_FIRE_CLOUD_LIFETIME 0.6f //0.47f
#define NAPALM_EXPLOSION_FIRE_FLAME_SIZE_START 2.0f//DEBUG 3.0f
#define NAPALM_EXPLOSION_FIRE_FLAME_SIZE_END 5.0f
#define NAPALM_EXPLOSION_FIRE_FLAME_EMISSION 50
#define NAPALM_EXPLOSION_FIRE_FLAME_LIFETIME 0.8f//0.6f
#define NAPALM_EXPLOSION_SMOKE_CLOUD_SIZE_END 5.0f
#define NAPALM_EXPLOSION_SMOKE_CLOUD_EMISSION 50
#define NAPALM_EXPLOSION_SMOKE_CLOUD_LIFETIME 10.0f// 0.6f //0.47f
Distance Effect::NAPALM_EXPLOSION_SPREAD_RADIUS = 150.0f;
#define NAPALM_EXPLOSION_SPREAD_TIME 1.7f
#define NAPALM_EXPLOSION_DANGEROUS_TIME 6.0f

#define HAND_GREMADE_EXPLOSION_RADIUS 30.0f

#define FIRE_BURNING_TIME 50.0f

#define TREE_FALL_RADIUS 10.0f

#define QUICK_EXPLOSION_DANGER_OFF_FRAME 2
#define SOUND_EMITTER_DESTROY_FRAME 2

#define WATER_DROP_SIZE_MAX 0.5f
#define WATER_DROPS_USED 500
#define WATER_DROP_OPACITY 30 //0-255
#define WATER_DROP_SPEED 2.5f
#define RAIN_WIND_DIRECTION 4.0f //rad
#define RAIN_WIND_DIR_COS cos(RAIN_WIND_DIRECTION)
#define RAIN_WIND_DIR_SIN sin(RAIN_WIND_DIRECTION)
#define RAIN_WIND_SPEED 50.0f

HTEXTURE Effect::tex = NULL;
hgeSprite * Effect::bulletHitSprite1 = NULL;
hgeSprite * Effect::bulletHitSprite2 = NULL;
hgeSprite * Effect::bulletHitSprite3 = NULL;
hgeSprite * Effect::bloodSprite = NULL;
hgeSprite * Effect::bloodSprite2 = NULL;
hgeParticleSystem * Effect::bulletHitTankPS = NULL;
hgeParticleSystem * Effect::bulletHitManPS = NULL;
HEFFECT Effect::bulletHitGroundSound = NULL;
HEFFECT Effect::bulletHitTankSound = NULL;
hgeParticleSystem * Effect::shellHitGroundFire = NULL;
hgeParticleSystem * Effect::tankShellHitParticleSystem3 = NULL;
hgeSprite * Effect::tankShellHitSprite = NULL;
HEFFECT Effect::shellHitExplosionSound = NULL;
hgeParticleSystem * Effect::tankShellHitTankParticleSystem1 = NULL;
hgeParticleSystem * Effect::tankShellHitTankParticleSystem3 = NULL;
hgeParticleSystem * Effect::tankShellHitTankParticleSystem4 = NULL;
hgeParticleSystem * Effect::tankShellHitTankParticleSystem5 = NULL;
HEFFECT Effect::shellHitTankExplosionSound = NULL;

HEFFECT Effect::sounds[];

//Adding a shadow to an object:
//Give the object a height.
//If the object is round, give it shadowOutLineRadius and render shadows with GameObject::renderShadows().
//If the object is cubical or special, render the shadow with a custom function.

Effect::Effect(void)
{
	amount++;
}

Effect::Effect(EFFECTTYPE effectType, Location location, float direction, GameObject * targetObject, float scale, int info, Velocity speed)
:cpManager(NULL)
{
	set(effectType, location, direction, targetObject, scale, info, speed);
	amount++;
}

Effect::~Effect(void)
{
	amount--;

	for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
	{
		if (particleSystems[i]) { delete particleSystems[i]; particleSystems[i] = NULL; }	
	}

	if (targetObject) { targetObject->removeReference(); targetObject = NULL; }
 	if (cpManager) { delete cpManager; cpManager = NULL; }
}

void Effect::set(EFFECTTYPE effectType, Location location, float direction, GameObject * targetObject, float scale, int info, Velocity speed)
{
	GameObject::set(GameObject::EFFECT, location, direction, scale);

	amount++;
	this->effectType = effectType;

	this->timer1 = 0.0f;
	this->timer2 = 0.0f;
	this->collisionTimer = 0.0f;
	this->illuminationTimer = 0.0f;
	this->illuminationSize = 0.0f;
	this->fire = true;
	this->counter1 = 0.0f;
	this->intCounter1 = 0.0f;
	this->flag1 = true;
	this->frameCounter = 0;
	this->targetObject = targetObject;
	if (targetObject)
		targetObject->addReference();

	illuminatorIterator = game.objects.illuminators.end();

	bool big_light = false;;

	for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
	{
		particleSystems[i] = NULL;
	}

	switch (effectType)
	{
	case ASSAULT_RIFLE_MUZZLE_FLASH:
		{
			timer1 = ASSAULT_RIFLE_MUZZLE_FLASH_TIME;	
			illuminationTimer = ASSAULT_RIFLE_MUZZLE_FLASH_TIME;
			illuminationSize = 3.0f;
			break;
		}
	case TANK_CANNON_MUZZLE_FLASH:
		{
			timer1 = TANK_CANNON_MUZZLE_FLASH_TIME;
			illuminationTimer = TANK_CANNON_MUZZLE_FLASH_TIME;
			illuminationSize = 5.0f;
			break;
		}
	case MISSILE_FLY:
		{
			illuminationTimer = FLT_MAX;
			illuminationSize = 3.0f; 
			for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
			{
				particleSystems[i] =  new hgeParticleSystem("missile.psi", game.smokeParticleSprite, 20.0f);
				particleSystems[i]->info.nEmission = 500.0f;
			}
			counter1 = FLT_MAX;
			break;
		}
	case AT_WEAPON_BACKFIRE:
		{
			illuminationTimer = 0.1f;
			illuminationSize = 3.0f; 
			collisionTimer = FLT_MAX;
			particleSystems[0] = new hgeParticleSystem(*(game.particleSystems[Game::PARTICLE_SYSTEM_AT_FIRING_SMOKE]));	
			particleSystems[0]->info.fLifetime = 0.05f;
			particleSystems[0]->info.fDirection = direction;
			particleSystems[0]->FireAt(x, y);
			break;
		}
	case AT_WEAPON_FIRE:
		{
			illuminationTimer = 0.1;
			illuminationSize = 3.0f;
			particleSystems[0] = new hgeParticleSystem(*(game.particleSystems[Game::PARTICLE_SYSTEM_AT_FIRING_SMOKE]));	
			particleSystems[0]->info.fLifetime = 0.2f;
			particleSystems[0]->info.fDirection = direction + HALF_PI;
			particleSystems[0]->FireAt(x, y);
			break;
		}
	case TANK_MG_BULLET_HIT_GROUND:
	case ASSAULT_RIFLE_BULLET_HIT_GROUND:
		{
			timer1  = BULLET_DIRT_STOP_TIME;
			hgeSprite * sprite = NULL;
			int rand = randInt(0, 2);

			switch(rand)
			{
			case 0: sprite = bulletHitSprite1; break; 
			case 1: sprite = bulletHitSprite2; break; 
			case 2: sprite = bulletHitSprite3; break; 
			default: sprite = bulletHitSprite1; break;
			}

			particleSystems[0] = new hgeParticleSystem("bullet_hit_ground.psi", game.smokeParticleSprite);
			particleSystems[0]->info.fDirection = direction;
	
			if (effectType == TANK_MG_BULLET_HIT_GROUND)
			{
				game.objects.background->modify(sprite, x, y, direction);
				particleSystems[0]->FireAt(x, y);
			}
			else
			{
				particleSystems[0]->info.fLifetime = particleSystems[0]->info.fLifetime / 2.0f;
				game.objects.background->modify(sprite, x, y, direction, 0.5f);
				game.objects.fireParticleSystem(particleSystems[0], x, y, direction, true);
				destroy();
			}
			playSoundAtLocation(bulletHitGroundSound);
			break;
		}
	case PROJECTILE_HIT_TANK:
		{
			playSoundAtLocation(bulletHitTankSound);
			game.objects.fireParticleSystem(bulletHitTankPS, x, y);
			destroy();
			break;
		}
	case PROJECTILE_HIT_MAN:
		{
			playSoundAtLocation(sounds[randInt(SOUND_PROJECTILE_HIT_MAN_1, SOUND_PROJECTILE_HIT_MAN_MAX - 1)]);
			game.objects.fireParticleSystem(bulletHitManPS, x, y);
			if (randInt(0, 2) == 0)
				particleSystems[0] = new hgeParticleSystem("blood_burst2.psi", bloodSprite2);
			else
				particleSystems[0] = new hgeParticleSystem("blood_burst2.psi", bloodSprite);
			particleSystems[0]->info.fDirection = direction;
			particleSystems[0]->info.fSizeStart = 0.15f * size * randFloat(0.8f, 1.2f);
			particleSystems[0]->info.fSizeEnd = 0.15f * size * randFloat(0.8f, 1.2f);
			particleSystems[0]->info.fSpinStart = 100.0f;
			particleSystems[0]->info.fSpinStart = 100.0f;
			particleSystems[0]->info.fSpeedMax *= GameObject::objectSize;
			particleSystems[0]->info.fSpeedMin *= GameObject::objectSize;
			particleSystems[0]->info.nEmission *= (float)game.amountOfBlood / 100.0f;
			float rand = randFloat(5.0f, 7.0f);
			float ax = x + rand * cos(direction);
			float ay = y + rand * sin(direction);
			particleSystems[0]->FireAt(ax, ay);
			timer1 = 0.05f;
			break;
		}
	case LARGE_PROJECTILE_HIT_MAN:
		{
			if (game.currentLevel->night)
			{
				bloodSprite->SetColor(game.currentLevel->getObjectColor());
				bloodSprite2->SetColor(game.currentLevel->getObjectColor());
			}
			else
			{
				bloodSprite->SetColor(0xffffffff);
				bloodSprite2->SetColor(0xffffffff);
			}

			playSoundAtLocation(sounds[randInt(SOUND_PROJECTILE_HIT_MAN_1, SOUND_PROJECTILE_HIT_MAN_MAX - 1)]);
			game.objects.fireParticleSystem(bulletHitManPS, x, y);
			particleSystems[0] = new hgeParticleSystem("blood_burst2.psi", bloodSprite);
			particleSystems[0]->info.fDirection = direction;
			particleSystems[0]->info.fSizeStart *= scale;
			particleSystems[0]->info.fSizeEnd *= scale;
			particleSystems[0]->info.fSpeedMax *= GameObject::objectSize;
			particleSystems[0]->info.fSpeedMin *= GameObject::objectSize;
			particleSystems[0]->info.nEmission *= (float)game.amountOfBlood / 100.0f;
			float rand = randFloat(5.0f, 10.0f);
			float ax = x + rand * cos(direction);
			float ay = y + rand * sin(direction);
			particleSystems[0]->FireAt(ax, ay);
			timer1 = 0.05f;
			break;
		}
	case BLOOD_POND:
		{
			break;
		}
	case MISSILE_EXPLOSION:
	case TANKSHELL_EXPLOSION:
	case MORTARSHELL_EXPLOSION:
		{			
			setRenderOrder(3);
			collisionTimer = FLT_MAX;
			counter1 = 3.0f;

			timer1 = TANKSHELL_SMOKE_TIME;
			timer2 = TANKSHELL_EXPLOSION_DEBRIS_STOP_TIME;

			//piles of ground:
			DWORD col = getColor(255, 70, 50, 0);
			ed = new ExplosionEffectData();
			int nEmission = 50;
			if (effectType == MORTARSHELL_EXPLOSION) 
				nEmission = 25;
			nEmission *= scale;
			ExplosionEffectData * ted = static_cast<ExplosionEffectData *>(ed);
			ted->foParticleSystem.set(//&sprites, 1,
				50, nEmission, col, colorAdd(col, 10), true, 0.05f, 0.15f, 1.0f,
				2, 1.0f, 5.0f, 10.0f);
			ted->foParticleSystem.FireAt(x, y, 10.0f, 1.0f, 0.0f, M_PI / 4.0f, 
				PI_TIMES_TWO, HALF_PI, 100.0f, 100.0f);

			if (effectType == MISSILE_EXPLOSION)
			{
				if (missileExplosionCounter++ == 0)
				{
					big_light = true;
					illuminationTimer = 7.0f;
					illuminationSize = 14.0f;
					counter2 = illuminationSize;
					missileExplosionCounter++;

				}
				if (missileExplosionCounter == 4)
					missileExplosionCounter = 0;
				setRenderOrder(4);
				timer1 = 1.0f;
				playSoundAtLocation(game.sounds[randInt(Game::SOUND_MORTAR_SHELL_EXPLOSION_1, Game::SOUND_MORTAR_SHELL_EXPLOSION_LAST)]); 
				playSoundAtLocation(game.sounds[randInt(Game::SOUND_MISSILE_EXPLOSION_1, Game::SOUND_MISSILE_EXPLOSION_MAX)]); 
				setCollisionSphereRadius(MISSILE_EXPLOSION_NOTIFY_RADIUS);
				hgeParticleSystemInfo psInfo = shellHitGroundFire->info;
				psInfo.nEmission = 1000.0f;
				psInfo.fSizeStart = 1.0f;
				psInfo.fSizeEnd = 8.0f;
				game.objects.fireParticleSystem(psInfo, x, y);				
				game.objects.background->modify(tankShellHitSprite, x, y, randomDirection(), randFloat(1.3f, 1.7f));
			}
			if (effectType == TANKSHELL_EXPLOSION)
			{
				big_light = true;
				illuminationTimer = 0.4f;
				illuminationSize = 8.0f;
				playSoundAtLocation(shellHitExplosionSound, 50);
				setCollisionSphereRadius(TANKSHELL_EXPLOSION_DANGER_RADIUS);
				game.objects.fireParticleSystem(shellHitGroundFire, x, y);

				if (scale > 0.5f)
					game.objects.background->modify(tankShellHitSprite, x, y, randomDirection(), randFloat(0.7f, 1.3f));
			}
			else if (effectType == MORTARSHELL_EXPLOSION)
			{
				big_light = true;
				illuminationTimer = 0.2f;
				illuminationSize = 12.0f;
				playSoundAtLocation(game.sounds[randInt(Game::SOUND_MORTAR_SHELL_EXPLOSION_1, Game::SOUND_MORTAR_SHELL_EXPLOSION_LAST)]); 
				setCollisionSphereRadius(MORTARSHELL_EXPLOSION_NOTIFY_RADIUS_DEF);
				game.objects.fireParticleSystem(shellHitGroundFire, x, y);

				hgeParticleSystem shrapnel("particle2.psi", game.debrisParticleSprite2, 30.0f);
				shrapnel.info.nEmission = 500;
				for (int i = 0; i < 3; i++)
					game.objects.fireParticleSystem(&shrapnel, x, y, direction);

				game.objects.background->modify(tankShellHitSprite, x, y, randomDirection(), randFloat(0.7f, 1.3f));
			}
			break;
		}
	case NAPALM_EXPLOSION:
		{
			big_light = true;
			illuminationTimer = 12.0f;
			illuminationSize = 14.0f;
			counter2 = illuminationSize;
			timer1 = 20.0f;
			setRenderOrder(4);
			collisionTimer = NAPALM_EXPLOSION_DANGEROUS_TIME;
			counter1 = 0.0f; 
			cpManager = new CustomParticleManager();	
			playSoundAtLocation(game.sounds[randInt(Game::SOUND_NAPALM_EXPLOSION_1, Game::SOUND_NAPALM_EXPLOSION_MAX)]);
			setCollisionSphereRadius(NAPALM_EXPLOSION_NOTIFY_RADIUS);
			game.objects.background->modify(game.animations[Game::ANIMATION_HAND_GRENADE_EXPLOSION_CRATER], x, y, randomDirection(), randFloat(10.0f, 12.0f));
			DWORD col = tankShellHitSprite->GetColor();
			tankShellHitSprite->SetColor(getColor(255, 150, 150, 150));
			game.objects.background->modify(tankShellHitSprite, x, y, randomDirection(), randFloat(1.3f, 1.7f));
			tankShellHitSprite->SetColor(col);
			break;
		}
	case HAND_GRENADE_EXPLOSION:
		{
			illuminationTimer = 0.2f;
			illuminationSize = 2.5f;
			collisionTimer = FLT_MAX;
			setCollisionSphereRadius(HAND_GREMADE_EXPLOSION_RADIUS);
			game.objects.fireParticleSystem(game.particleSystems[Game::PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_DIRT], x, y, direction);
			game.objects.fireParticleSystem(game.particleSystems[Game::PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_SHRAPNEL], x, y, direction);
			game.objects.fireParticleSystem(game.particleSystems[Game::PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_SMOKE], x, y, direction);
			game.objects.fireParticleSystem(game.particleSystems[Game::PARTICLE_SYSTEM_HAND_GRENADE_EXPLOSION_FIRE], x, y, direction);
			game.objects.background->modify(game.animations[Game::ANIMATION_HAND_GRENADE_EXPLOSION_CRATER], x, y, randomDirection(), randFloat(0.7f, 1.3f));
			playSoundAtLocation(game.sounds[Game::SOUND_HAND_GRENADE_EXPLOSION]);
			break;
		}
	case TANK_EXPLOSION:
		{
			big_light = true;			
			setHeight(Tank::TANK_HEIGHT);
			illuminationTimer = 3.0f;
			illuminationSize = 12.0f;
			counter2 = illuminationSize ;
			collisionTimer = FLT_MAX;//SHELL_EXPLOSION_DANGEROUS_TIME;
			counter1 = 3.0f;
			timer1 = TANK_EXPLOSION_SMOKE_TIME;
			timer2 = TANKSHELL_EXPLOSION_DEBRIS_STOP_TIME;
			timer3 = TANK_EXPLOSION_FIRE_TIME;
			timer4 = FLT_MAX;

			//clouds of fire burst
			particleSystems[0] = new hgeParticleSystem("tank_explosion_fire_burst_2.psi", game.animations[Game::ANIMATION_PARTICLE_SPRITE_RUBBLE], 50.0f);
			particleSystems[0]->FireAt(x, y);			

			//chunks of debris flying around:
			ed = new TankExplosionEffectData();
			TankExplosionEffectData * ted = static_cast<TankExplosionEffectData *>(ed);
			
			ted->foParticleSystem.set(//sprites, nSprites,
				50, 50, 0xff000000, 0xff333333, true, 0.05f, 0.25f, 1.0f,
				2, 1.0f, 5.0f, 10.0f);
			ted->foParticleSystem.FireAt(x, y, 10.0f, 1.0f, 0.0f, M_PI / 4.0f, 
				PI_TIMES_TWO, HALF_PI, 100.0f, 100.0f);
			
			particleSystems[2] = new hgeParticleSystem("tank_explosion_fire_burst.psi", game.fireParticleSprite, 50.0f);
			particleSystems[2]->FireAt(x, y);
			game.objects.background->modify(tankShellHitSprite, x, y);
			playSoundAtLocation(shellHitTankExplosionSound, 50);
			setCollisionSphereRadius(TANK_EXPLOSION_DANGER_RADIUS);
			break;
		}
	case BASE_EXPLOSION:
		{
			collisionTimer = FLT_MAX;
			counter1 = 3.0f;
			timer1 = TANKSHELL_SMOKE_TIME;
			timer2 = TANKSHELL_EXPLOSION_DEBRIS_STOP_TIME;

			//sand
			DWORD col = getColor(255, 230, 180, 30);
			ed = new BaseExplosionData();
			BaseExplosionData * bed = static_cast<BaseExplosionData *>(ed);

			bed->sand.set(50.0f, 1000 * scale, col, colorAdd(col, 20), true, 0.005f, 0.025f, 1.0f,
				1, 1.0f, 5.0f, 10.0f);
			bed->sand.FireAt(x, y, 10.0f, 1.0f, 0.0f, M_PI / 4.0f, 
				PI_TIMES_TWO, HALF_PI, 50.0f, 110.0f);

			//pieces of camo net
			bed->camoNet.set(//&game.animations[Game::ANIMATION_PARTICLE_SPRITE_CAMO_NET_1], 1,
				50.0f, 10 * scale, 0xfff0f0f0, 0xffffffff, true, 1.0f, 1.0f, 1.0f,
				1, 1.0f, 5.0f, 10.0f);

			bed->camoNet.FireAt(x, y, 10.0f, 1.0f, 0.0f, M_PI / 4.0f, 
				PI_TIMES_TWO, HALF_PI, 20.0f, 30.0f);

			//sandbags moving
			col = getColor(255, 200, 200, 200);
			bed->sandbags.set(50.0f, 6 * scale, col, col, true, 1.0f, 1.0f, 5.0f,
				1, 0.0f, 5.0f, 10.0f);
			bed->sandbags.FireAt(
				x, //x
				y, //y
				10.0f, //z 
				randFloat(0.5f, 1.0f), //emissionRatio
				0.0f, //horizontalDirection
				M_PI / 4.0f, //verticalDirection
				PI_TIMES_TWO, //horizontalSpread
				HALF_PI, //verticalSpread
				10.0f, //speedMin
				20.0f, //speedMax
				0.0f, //startDirectionMin
				0.0f //startDirectionMax
				);

			playSoundAtLocation(game.sounds[randInt(Game::SOUND_SUPPRESSED_EXPLOSION_1, Game::SOUND_SUPPRESSED_EXPLOSION_MAX)], 100);
			setCollisionSphereRadius(TANKSHELL_EXPLOSION_DANGER_RADIUS);
			game.objects.fireParticleSystem(shellHitGroundFire, x, y);
			col = game.animations[Game::ANIMATION_HAND_GRENADE_EXPLOSION_CRATER]->GetColor();
			game.animations[Game::ANIMATION_HAND_GRENADE_EXPLOSION_CRATER]->SetColor(getColor(randInt(100, 150), 255, 255, 255));
			game.objects.background->modify(game.animations[Game::ANIMATION_HAND_GRENADE_EXPLOSION_CRATER], x, y, randomDirection(), randFloat(1.0f, 2.0f) * scale);			
			game.animations[Game::ANIMATION_HAND_GRENADE_EXPLOSION_CRATER]->SetColor(col);
			break;
		}
	case BULLET_SHELL_FLY:
		{
			particleSystems[0] = new hgeParticleSystem("bullet_shell.psi", game.debrisParticleSprite2, 50.0f);
			particleSystems[0]->info.fDirection = direction;
			particleSystems[0]->info.nEmission = 9;
			particleSystems[0]->info.fSizeStart = 0.03f;
			particleSystems[0]->info.fSizeEnd = 0.03f;
			particleSystems[0]->info.fSpeedMax = 4.0f;
			particleSystems[0]->info.fSpeedMin = 6.0f;
			particleSystems[0]->FireAt(x, y);
			timer1 = randFloat(0.2f, 0.3f);
			break;
		}
	case FLYING_SMOKING_DEBRIS:
		{
			setSpeed(speed);
			deacceleration = speed * 2.0f;
			particleSystems[0] = new hgeParticleSystem("particle5.psi", game.animations[Game::ANIMATION_PARTICLE_SPRITE_BOLT], 30.0f);
			particleSystems[0]->info.nEmission = 100;
			particleSystems[0]->info.fSizeStart = 0.2f;
			particleSystems[0]->info.fSizeEnd = 0.2f;
			particleSystems[1] = new hgeParticleSystem(*(particleSystems[0]));
			particleSystems[2] = new hgeParticleSystem(*(particleSystems[0]));
			particleSystems[0]->FireAt(x, y);
			particleSystems[1]->FireAt(x, y);
			break;
		}
	case FIRE:
		{
			big_light = true;
			illuminationTimer = FLT_MAX;
			counter2 = 3.5f * scale;
			illuminationSize = counter2;
			setRenderOrder(1);
			particleSystems[0] = new hgeParticleSystem("particle9.psi", game.animations[Game::ANIMATION_PARTICLE_SPRITE_BOLT], 30.0f);
			particleSystems[0]->info.colColorEnd = getColor(255, 255, 0, 0);
			particleSystems[0]->info.colColorStart = getColor(255, 255, 255, 0);
			particleSystems[0]->info.fParticleLifeMax = 0.8f * scale;
			particleSystems[0]->info.fParticleLifeMin = 0.5f * scale;
			particleSystems[0]->info.fRadialAccelMax = 0.0f;
			particleSystems[0]->info.fRadialAccelMin = 0.0f;
			particleSystems[0]->info.fSizeEnd = 0.0f;
			particleSystems[0]->info.fSizeStart = 1.0f * scale;
			particleSystems[0]->info.fSpeedMax = 0.6f;
			particleSystems[0]->info.fSpeedMin = 0.0f;
			particleSystems[0]->info.fTangentialAccelMax = -0.01f;
			particleSystems[0]->info.fTangentialAccelMin = 0.01f;
			particleSystems[1] = new hgeParticleSystem(*(particleSystems[0]));
			particleSystems[0]->FireAt(x, y);

			bool raining = false;

			if (game.currentLevel)
				raining = game.currentLevel->raining;

			if (!raining)
				timer1 = FIRE_BURNING_TIME * randomVariation(0.3f);
			else
				timer1 = FIRE_BURNING_TIME * randomVariation(0.3f) * 0.1f;
			counter1 = timer1;
			break;
		}
	case TREE_FALL:
		{
			collisionTimer = FLT_MAX;
			setCollisionSphereRadius(TREE_FALL_RADIUS * size);
			timer1 = 1.1f;
			break;
		}
	case DEBRIS_FALL_SOUND_EMITTER:
		{
			collisionTimer = FLT_MAX;
			break;
		}
	case SMOKE_GRENADE:
		{
			playSoundAtLocation(game.sounds[Game::SOUND_SMOKE_GRENADE_EXPLODE]);
			hgeParticleSystemInfo psi = tankShellHitTankParticleSystem3->info;
			psi.sprite = game.animations[Game::ANIMATION_PARTICLE_SPRITE_SMOKE_4_LIGHT];			
			psi.colColorStart = getColor(150, 255, 255, 255);
			psi.colColorEnd = getColor(0, 255, 255, 255);
			psi.fLifetime = 15.0f;
			psi.nEmission = 66;
			psi.fGravityMax = -0.01f;
			psi.fGravityMin = 0.0f;
			psi.fParticleLifeMax = 60.0f;
			psi.fParticleLifeMin = 30.0f;
			psi.fRadialAccelMax = 0.0f;
			psi.fRadialAccelMin = 0.0f;
			psi.fSizeEnd = 8.0f;
			psi.fSizeStart = 0.0f;
			psi.fSizeVar = 0.02f;
			psi.fSpeedMax = 0.25f;
			psi.fSpeedMin = 0.0f;
			psi.fSpinEnd = 0.5f;
			psi.fSpinStart = 0.5f;
			psi.fSpinVar;
			psi.fTangentialAccelMax = 0.005f;
			psi.fTangentialAccelMin = 0.0f;
			game.objects.fireParticleSystem(psi, x, y);	
			destroy();
			break;
		}
	case RAIN:
		{
			image = game.animations[Game::ANIMATION_WATER_DROP];
			image->SetColor(getColor(WATER_DROP_OPACITY, 255, 255, 255));
			ed = new RainEffectData();
			RainEffectData * red = static_cast<RainEffectData *>(ed);
			for (int i = 0; i < WATER_DROPS_USED; i++)
			{
				red->waterDropSizes[i] = randFloat(0.0f, WATER_DROP_SIZE_MAX);
				red->waterDropLocations[i] = Location(randFloat(0.0f, Game::SCREENWIDTH), randFloat(0.0f, Game::SCREENHEIGHT));
			}
			break;
		}
	case FLARE_LIGHT:
		{
			big_light = true;
			illuminationTimer = 12.0f;
			illuminationSize = 14.0f;
			counter2 = illuminationSize;
			break;
		}
	case SUN:
		{
			big_light = true;
			illuminationTimer = FLT_MAX;
			illuminationSize = 2000.0f;
			setHeight(80000.0f);
			setLocation(Location(40000, -30000));
			fire = false;
			break;
		}
	case TEST_EFFECT_1:
		{
			DWORD col = getColor(255, 230, 180, 30);
			ed = new TestEffectData();
			TestEffectData * ted = static_cast<TestEffectData *>(ed);
			ted->foParticleSystem.set(50.0f, 5000, col, col, true, 0.005f, 0.025f, 0.5f,
				1, 1.0f, 5.0f, 10.0f);
			ted->foParticleSystem.FireAt(x, y, 10.0f, 1.0f, 0.0f, M_PI / 4.0f, 
				PI_TIMES_TWO, HALF_PI, 50.0f, 110.0f);

			break;
		}
	case TEST_EFFECT_2:
		{
			image = game.animations[Game::ANIMATION_BUSH_1];
			image->Play();

			break;
		}
	}

	for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
	{
		if (particleSystems[i])
		{
			particleSystems[i]->info.fSizeEnd *= GameObject::objectSize;
			particleSystems[i]->info.fSizeStart *= GameObject::objectSize;
		}
	}

	if ((big_light && game.currentLevel->night) || effectType == SUN)
	{
		game.objects.illuminators.push_back(this);
		illuminatorIterator = --game.objects.illuminators.end();
	}
}

void Effect::init(void)
{
	game.renderLoadScreen("LOADING EFFECTS");
	tex = hge->Texture_Load("effects.png");

	//bullet hit sprites
	bulletHitSprite1 = new hgeSprite(tex, 0.0f, 81.0f, 13.0f, 8.0f);
    bulletHitSprite1->SetHotSpot(13.0f / 2.0f, 4.0f); 
	bulletHitSprite2 = new hgeSprite(tex, 0.0f, 90.0f, 13.0f, 8.0f);
    bulletHitSprite2->SetHotSpot(13.0f / 2.0f, 4.0f); 
	bulletHitSprite3 = new hgeSprite(tex, 15.0f, 81.0f, 13.0f, 8.0f);
    bulletHitSprite3->SetHotSpot(13.0f / 2.0f, 4.0f); 
	bulletHitTankPS = new hgeParticleSystem("bullet_hit_tank.psi", game.fireParticleSprite);
	bloodSprite = new hgeSprite(game.particletex, 3.0f * 32.0f, 0.0f * 32.0f, 32.0f, 32.0f);
	bloodSprite->SetHotSpot(16.0f, 16.0f);
	bloodSprite2 = new hgeSprite(game.particletex3, 1.0f * 32.0f, 0.0f * 32.0f, 32.0f, 32.0f);
	bloodSprite2->SetHotSpot(16.0f, 16.0f);
	bulletHitManPS = new hgeParticleSystem("blood_burst1.psi", bloodSprite);
	bulletHitGroundSound = hge->Effect_Load("473173_SOUNDDOGS_Bu.wav");
	bulletHitTankSound = hge->Effect_Load("437699_SOUNDDOGS_BU.wav");
	tankShellHitSprite = new hgeSprite(tex, 0.0f, 101.0f, 124.0f, 119.0f);
    tankShellHitSprite->SetHotSpot(62.0f, 58.0f); 
	shellHitGroundFire = new hgeParticleSystem("tank_shell_hit_ground_fire.psi", game.fireParticleSprite);
	shellHitGroundFire->info.nEmission *= (float)game.amountOfParticles / 100.0f;
	tankShellHitParticleSystem3 = new hgeParticleSystem("tank_shell_hit_ground_smoke.psi", game.smokeParticleSprite);
	tankShellHitParticleSystem3->info.nEmission *= (float)game.amountOfParticles / 100.0f;
	shellHitExplosionSound = hge->Effect_Load("a_exm1.wav");
	tankShellHitTankParticleSystem1 = new hgeParticleSystem("tank_explosion_fire_burst.psi", game.fireParticleSprite);
	tankShellHitTankParticleSystem1->info.nEmission *= (float)game.amountOfParticles / 100.0f;
	tankShellHitTankParticleSystem3 = new hgeParticleSystem("shell_hit_tank3.psi", game.smokeParticleSprite);
	tankShellHitTankParticleSystem3->info.nEmission *= (float)game.amountOfParticles / 100.0f;
	tankShellHitTankParticleSystem4 = new hgeParticleSystem("shell_hit_tank4.psi", game.smokeParticleSprite);
	tankShellHitTankParticleSystem4->info.nEmission *= (float)game.amountOfParticles / 100.0f;
	tankShellHitTankParticleSystem5 = new hgeParticleSystem("shell_hit_tank5.psi", game.smokeParticleSprite);
	tankShellHitTankParticleSystem5->info.nEmission *= (float)game.amountOfParticles / 100.0f;
	shellHitTankExplosionSound = hge->Effect_Load("a_exmetal_sand.wav");
	sounds[SOUND_PROJECTILE_HIT_MAN_1] = hge->Effect_Load("bullet hit body 1 134029_SOUNDDOGS_bu.mp3");
	sounds[SOUND_PROJECTILE_HIT_MAN_2] = hge->Effect_Load("bullet hit body 2 134031_SOUNDDOGS_bu.mp3");
	sounds[SOUND_PROJECTILE_HIT_MAN_3] = hge->Effect_Load("bullet hit body 3 134032_SOUNDDOGS_bu.mp3");
	sounds[SOUND_PROJECTILE_HIT_MAN_4] = hge->Effect_Load("bullet hit body 4 268425_SOUNDDOGS_Im.mp3");
}

void Effect::release(void)
{
	//delete textures
	hge->Texture_Free(tex);
	//delete sprites
	delete bulletHitSprite1, bulletHitSprite1 = NULL;
	delete bulletHitSprite2, bulletHitSprite2 = NULL;
	delete bulletHitSprite3, bulletHitSprite3 = NULL;
	delete tankShellHitSprite; tankShellHitSprite = NULL;
	//delete tankShellHitTankSprite; tankShellHitTankSprite = NULL;
	delete bloodSprite; bloodSprite = NULL;
	delete bloodSprite2; bloodSprite2 = NULL;
	//delete particle systems
	delete bulletHitTankPS; bulletHitTankPS = NULL;
	delete bulletHitManPS; bulletHitManPS = NULL;
	delete shellHitGroundFire; shellHitGroundFire = NULL;
	delete tankShellHitParticleSystem3; tankShellHitParticleSystem3 = NULL;
	delete tankShellHitTankParticleSystem1; tankShellHitTankParticleSystem1 = NULL;
	delete tankShellHitTankParticleSystem3; tankShellHitTankParticleSystem3 = NULL;
	delete tankShellHitTankParticleSystem4; tankShellHitTankParticleSystem4 = NULL;
	delete tankShellHitTankParticleSystem5; tankShellHitTankParticleSystem5 = NULL;
	//delete sounds
	hge->Effect_Free(bulletHitGroundSound);
	hge->Effect_Free(bulletHitTankSound);
	hge->Effect_Free(shellHitExplosionSound);
	hge->Effect_Free(shellHitTankExplosionSound);
	for (int i = 0; i < SOUND_MAX; i++)
	{
		hge->Effect_Free(sounds[i]);
	}
}

void Effect::processInteraction(void)
{
	timer1 -= timerDelta;
	frameCounter++;

	if (collisionTimer > 0.0f)
	{
		collisionTimer -= timerDelta;
	}
	if (illuminationTimer > 0.0f)
	{
		illuminationTimer -= timerDelta;		

		if (illuminationTimer <= 0.0f)
		{
			if (illuminatorIterator != game.objects.illuminators.end())
			{
				game.objects.illuminators.erase(illuminatorIterator);
				illuminatorIterator = game.objects.illuminators.end();
			}
		}
	}

	switch(effectType)
	{
	case ASSAULT_RIFLE_MUZZLE_FLASH:
		{
			if (timer1 < 0.0f)
			{
				game.objects.fireParticleSystem(game.particleSystems[Game::PARTICLE_SYSTEM_ASSAULT_RIFLE_FIRING_SMOKE_1], x, y, direction);
				destroy();
			}
			break;
		}
	case TANK_CANNON_MUZZLE_FLASH:
		{
			if (timer1 < 0.0f)
			{
				game.objects.fireParticleSystem(game.particleSystems[Game::PARTICLE_SYSTEM_CANNON_FIRING_SMOKE], x, y, direction);
				destroy();
			}
			break;
		}
	case MISSILE_FLY:
		{
			float l = particleSystems[0]->info.fLifetime;
			Distance dist = static_cast<Projectile *>(targetObject)->distance;
			setLocation(ArcPoint(targetObject->getLocation(), 45.0f, targetObject->getDirection() + M_PI).getLocation());

			//Algorithm needed to fire smoke particle systems in short
			//succession. This is needed to fill the gaps left because of the 
			//maximum particle creation rate, which is only 1000 per second.
			if (dist < 1000.0f))
			{
				//fire off particle systems
				if (dist < counter1)
				{
					if (intCounter1 < MAX_PARTICLE_SYSTEMS)
					{
						particleSystems[intCounter1]->FireAt(x, y);
						intCounter1++;
						counter1 = dist - MISSILE_FLY_DISTANCE_BETWEEN_PARTICLE_SYSTEMS_FIRING;
					}
				}

				for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
				{
					particleSystems[i]->MoveTo(x, y, false);
					particleSystems[i]->Update(timerDelta);
				}

				if (dist <= 0.0f || targetObject->isDestroyed())
				{
					int particles = 0;
					for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
					{
						particleSystems[i]->Stop(false);
						particles += particleSystems[i]->GetParticlesAlive();
					}	
					if (particles <= 0)
						destroy();
				}
			}

			if (flag1 && dist < MISSILE_FLY_WHISTLE_DISTANCE)
			{
				game.playSound(game.sounds[randInt(Game::SOUND_MISSILE_FLY_1, Game::SOUND_MISSILE_FLY_MAX)]); 
				flag1 = false;
			}
			break;
		}
	case AT_WEAPON_BACKFIRE:
		{
			particleSystems[0]->Update(timerDelta);
			if (timer1 < -3.0f && particleSystems[0]->hgeParticleSystem::GetParticlesAlive() <= 0)
			{
				destroy();
			}
			break;
		}
	case AT_WEAPON_FIRE:
		{
			particleSystems[0]->Update(timerDelta);
			if (timer1 > -0.2f)
			{
                float speed = 100.0f;
				x += speed * cos(direction) * timerDelta;
				y += speed * sin(direction) * timerDelta;
				particleSystems[0]->MoveTo(x, y);
			}
			else if (timer1 < -3.0f && particleSystems[0]->hgeParticleSystem::GetParticlesAlive() <= 0)
			{
				destroy();
			}
			break;
		}
	case TANK_MG_BULLET_HIT_GROUND:
		{
			particleSystems[0]->Update(timerDelta);
			if (timer1 <= 0.0f)
			{
				game.objects.background->modify(particleSystems[0]);
				destroy();
			}
			break;;
		}
	case PROJECTILE_HIT_MAN:
	case LARGE_PROJECTILE_HIT_MAN:
		{
			particleSystems[0]->Update(timerDelta);

			if (timer1 < 0.0f)
			{
				game.objects.background->modify(particleSystems[0]);
				destroy();
			}

			break;
		}
	case BLOOD_POND:
		{
			break;
		}
	case MISSILE_EXPLOSION:
	case TANKSHELL_EXPLOSION:
	case MORTARSHELL_EXPLOSION:
		{	
			if (effectType == MISSILE_EXPLOSION)
			{
				if (illuminationTimer < 2.0f)
				{
					illuminationSize = counter2 * illuminationTimer / 2.0f;
				}
			}
			if (frameCounter == QUICK_EXPLOSION_DANGER_OFF_FRAME)
				collisionTimer = -1.0f;
			ExplosionEffectData * eed = static_cast<ExplosionEffectData *>(ed);
			eed->foParticleSystem.Update(timerDelta);
			if (eed->foParticleSystem.GetParticlesAlive() == 0)
			{
				destroy();

				if (effectType == MISSILE_EXPLOSION)
				{
					missileExplosionCounter = 0;
				}
			}

			if (timer1 < 0.0f)
			{
				hgeParticleSystemInfo psi = tankShellHitParticleSystem3->hgeParticleSystem::info;
				psi.nEmission *= 2.0f;
				psi.fParticleLifeMax /= 2.0f;
				psi.fParticleLifeMin /= 2.0f;
				game.objects.fireParticleSystem(psi, x, y);		
				timer1 = 9999.0f;
			}
			timer2 -= timerDelta;
			break;
		}
	case BASE_EXPLOSION:
		{
			if (frameCounter == QUICK_EXPLOSION_DANGER_OFF_FRAME)
				collisionTimer = -1.0f;
			BaseExplosionData * bed = static_cast<BaseExplosionData *>(ed);
			bed->sand.Update(timerDelta);
			bed->camoNet.Update(timerDelta);
			bed->sandbags.Update(timerDelta);
			if (bed->sand.GetParticlesAlive() == 0 && 
				bed->camoNet.GetParticlesAlive() == 0 &&
				bed->sandbags.GetParticlesAlive() == 0)
				destroy();
			if (timer1 < 0.0f)
			{
				if (size >= 1.0f)
					game.objects.fireParticleSystem(tankShellHitParticleSystem3, x, y);		
				timer1 = 9999.0f;
			}
			timer2 -= timerDelta;
			break;
		}
	case NAPALM_EXPLOSION:
		{
			if (illuminationTimer < 2.0f)
			{
				illuminationSize = counter2 * illuminationTimer / 2.0f;
			}
			cpManager->update(timerDelta);
			if ((counter1 < NAPALM_EXPLOSION_SPREAD_RADIUS && randomEvent(0.03 * GameObject::objectSize)) || flag1)
			{
				//***spreading napalm explosion thunder***
				float scale = 1.0f;
				//cloudy fire burst
				hgeParticleSystem ps("tank_explosion_fire_burst_2.psi", game.animations[Game::ANIMATION_PARTICLE_SPRITE_RUBBLE], 50.0f);				
				hgeParticleSystemInfo psi1a = ps.info;
				psi1a.fLifetime = NAPALM_EXPLOSION_FIRE_CLOUD_LIFETIME / GameObject::objectSize;
				psi1a.nEmission = NAPALM_EXPLOSION_FIRE_CLOUD_EMISSION;
				psi1a.fParticleLifeMin = 1.9f * randomVariation(0.3f);
				psi1a.fParticleLifeMax = 2.4f * randomVariation(0.3f);
				psi1a.fSizeStart = NAPALM_EXPLOSION_FIRE_CLOUD_SIZE_START * scale * GameObject::objectSize;
				psi1a.fSizeEnd = NAPALM_EXPLOSION_FIRE_CLOUD_SIZE_END * scale * GameObject::objectSize;
				psi1a.colColorStart = getColor(255, 255, 255, 0);
				psi1a.colColorEnd = getColor(0, 255, 0, 0);
				psi1a.fSpinStart = randFloat(-0.4, 0.4);
				psi1a.fSpinEnd = randFloat(-0.4, 0.4);
				//clouds of fire turn to clouds of smoke
				hgeParticleSystemInfo psi1b = tankShellHitTankParticleSystem3->info;
				psi1b.nEmission = 10;
				psi1b.fLifetime = 0.5f;
				psi1b.fParticleLifeMin = 10.0f;
				psi1b.fParticleLifeMax = 12.0f;
				psi1b.fSizeEnd = 5.0f;
				psi1b.fGravityMin = -0.7f;
				psi1b.fGravityMax = -0.7f;
				psi1b.fSpeedMin = 2.0f;
				//first flame is bigger
				if (flag1)
                    scale *= 2.0f;
				//fire flames
				hgeParticleSystemInfo psi2 = tankShellHitTankParticleSystem1->info;				
				psi2.fLifetime = NAPALM_EXPLOSION_FIRE_FLAME_LIFETIME;//andFloat(0.1f, 0.3f);
				psi2.nEmission = NAPALM_EXPLOSION_FIRE_FLAME_EMISSION * scale;
				psi2.fParticleLifeMin = 1.0f * scale * randomVariation(0.3f);
				psi2.fParticleLifeMax = 2.5f * scale * randomVariation(0.3f);
				psi2.fSpinStart = randFloat(-2.0f, 2.0f);
				psi2.fSizeStart = NAPALM_EXPLOSION_FIRE_FLAME_SIZE_START * scale * GameObject::objectSize;
				psi2.fSizeEnd = NAPALM_EXPLOSION_FIRE_FLAME_SIZE_END * scale * GameObject::objectSize;
				psi2.colColorStart = getColor(255, 255, 255, 100);
				psi2.colColorEnd = getColor(0, 255, 0, 0);
				float dist = counter1;
				Location l = ArcPoint(getLocation(), dist * randomVariation(0.1f), randomDirection()).getLocation();
				if (cpManager->spawnPS(&psi2, l.x, l.y))
				{
					ComplexParticleSystem cps;
					cps.addInfo(&psi1a, NAPALM_EXPLOSION_FIRE_CLOUD_CHANGE_TIME);
					cps.addInfo(&psi1b, 20.0f);
					cpManager->spawnPS(&cps, l.x, l.y);	
				}
				if (flag1)
				{
					counter1 = 50.0f;
					flag1 = false;
				}
				counter1 += 100.0f * timerDelta;
			}		
			if (cpManager->areAllFinished())
			{
				destroy();
			}
			break;
		}
	case HAND_GRENADE_EXPLOSION:
		{
			if (frameCounter == QUICK_EXPLOSION_DANGER_OFF_FRAME)
			{
				collisionTimer = -1.0f;
				destroy();
			}

			break;
		}
	case TANK_EXPLOSION:
		{
			if (illuminationTimer < 2.0f)
			{
				illuminationSize = 6.0f + (counter2 - 6.0f) * illuminationTimer / 2.0f;

				if (illuminationTimer < 1.0f)
				{
					illuminationTimer += timerDelta;
				}
			}

			if (frameCounter == QUICK_EXPLOSION_DANGER_OFF_FRAME)
				collisionTimer = -1.0f;

			particleSystems[0]->Update(timerDelta);
			particleSystems[2]->Update(timerDelta);

			if (targetObject)
			{
				setLocation(targetObject->getLocation());
				particleSystems[0]->MoveTo(x, y);
				particleSystems[2]->MoveTo(x, y);
			}
			TankExplosionEffectData * teed = static_cast<TankExplosionEffectData *>(ed);
			teed->foParticleSystem.Update(timerDelta);
			timer2 -= timerDelta;
			timer3 -= timerDelta;
			timer4 -= timerDelta;
			if (timer1 < 0.0f)
			{
				//smoke time
				game.objects.fireParticleSystem(tankShellHitTankParticleSystem3, x, y);			
				game.objects.fireParticleSystem(tankShellHitTankParticleSystem4, x, y);	
				timer1 = 9999.0f;
			}
			if (timer3 < 0.0f)
			{
				//let tank burn & kill this effect
				hgeParticleSystemInfo psInfo = tankShellHitTankParticleSystem5->info;
				if (game.currentLevel->raining)
				{
					psInfo.fLifetime /= 2.0f;
				}
				timer4 = psInfo.fLifetime;
				illuminationTimer = 100.0f + timer4;
				game.objects.fireParticleSystem(psInfo, x, y);	
				timer3 = 9999.0f;
			}
			if (timer4 < 0.0f)
			{				
				destroy();
			}
			break;
		}
	case BULLET_SHELL_FLY:
		{
			particleSystems[0]->Update(timerDelta);
			if (timer1 < 0.0f)
			{
				game.objects.background->modify(particleSystems[0]);
				destroy();
			}
			break;
		}
	case FLYING_SMOKING_DEBRIS:
		{
			speed -= deacceleration * timerDelta;
			dx = speed * cos(direction);
			dy = speed * sin(direction);
			x += dx * timerDelta;
			y += dy * timerDelta;
			Distance dist = 0.0f;
			bool dest = true;
			for (int i = 0; i < 1; i++)
			{
				particleSystems[i]->Update(timerDelta);

				if (speed < 0.0f)
					particleSystems[i]->Stop(false);

				Location l = ArcPoint(getLocation(), dist, getDirection()).getLocation();
				particleSystems[i]->MoveTo(l.x, l.y, false);

				if (particleSystems[i]->GetParticlesAlive() > 0 ||
					particleSystems[i]->GetAge() != -2)
				{
					dest = false;
				}
				dist+= (speed * timerDelta) * 80.0f;
			}
			if (dest)
				destroy();
			break;
		}
	case FIRE:
		{
			particleSystems[0]->Update(timerDelta);
			if (targetObject)
			{
				Location l;
				if (targetObject->type == GameObject::FOOT_SOLDIER)
				{
					FootSoldier * fs = static_cast<FootSoldier *>(targetObject);
					l = fs->getBodyCenter();
				}
				else
					l = targetObject->getLocation();
				setLocation(l);				
			}
			particleSystems[0]->MoveTo(x, y, false);
			float relativeSize = timer1 / counter1;
			particleSystems[0]->info.fParticleLifeMax = particleSystems[1]->info.fParticleLifeMax * relativeSize;
			particleSystems[0]->info.fParticleLifeMin = particleSystems[1]->info.fParticleLifeMin * relativeSize;
			particleSystems[0]->info.fSizeStart = particleSystems[1]->info.fSizeStart * relativeSize;
			illuminationSize = counter2 * relativeSize;
			if (timer1 < 0.0f)
			{
				destroy();
			}
			break;
		}
	case TREE_FALL:
		{
			if (timer1 < 0.0f)
			{
				hgeParticleSystem ps(&(game.particleSystems[Game::PARTICLE_SYSTEM_DUST_RISE]->info), 30.0f);	
				ps.info.nEmission = 1000;
				int r = 255;
				int g = 244;
				int b = 233;
				ps.info.colColorStart = getColor(55, r, g, b);
				ps.info.colColorEnd = getColor(0, r, g, b);
				ps.info.fSizeEnd = 1.5f;
				ps.info.fSizeStart;
				ps.info.sprite = game.animations[Game::ANIMATION_PARTICLE_SPRITE_CLOUD_2];
				ps.info.fLifetime = 0.1f;
				ps.info.nEmission = 100;
				ps.info.fParticleLifeMax = 4.0f;
				ps.info.fParticleLifeMin = 2.0f;
				ps.info.fSpeedMin = 0.01f;
				ps.info.fSpeedMax = 0.3f;
				ps.info.fTangentialAccelMax = 0.0f;
				ps.info.fTangentialAccelMin = 0.0f;
				ps.info.fRadialAccelMax = 0.0f;
				ps.info.fRadialAccelMin = 0.0f;
				ps.info.fGravityMax = 0.0f;
				ps.info.fGravityMin = 0.0f;
				game.objects.fireParticleSystem(&ps, x, y, direction, true);
				destroy();
			}
			break;
		}
	case DEBRIS_FALL_SOUND_EMITTER:
		{
			if (frameCounter == SOUND_EMITTER_DESTROY_FRAME)
			{
				DebrisFallEffectData * dfed = static_cast<DebrisFallEffectData *>(ed);
				playSoundAtLocation(dfed->groundHitSounds[randInt(0, dfed->nGroundHitSounds - 1)], dfed->volume, randFloat(0.8f, 1.2f));
				destroy();
			}
			break;
		}
	case FLARE_LIGHT:
		{
			setLocation(targetObject->getLocation());
			setHeight(targetObject->height);
			{
				illuminationSize = counter2 * illuminationTimer / 3.0f;
			}
			break;
		}
	case TEST_EFFECT_1:
		{
			TestEffectData * ted = static_cast<TestEffectData *>(ed);
			ted->foParticleSystem.Update(timerDelta);

			if (ted->foParticleSystem.GetParticlesAlive() == 0)
				destroy();
			break;
		}
	case TEST_EFFECT_2:
		{
			image->Update(timerDelta);		
			image->GetWidth();
			break;
		}
	default: break;
	}	
}

void Effect::render(void)
{	
	for (int i = 0; i < MAX_PARTICLE_SYSTEMS; i++)
	{
		if (particleSystems[i])
			particleSystems[i]->Render();
	}
	if (illuminationTimer > 0.0f && game.currentLevel->night)
	{
		float var = randFloat(0.9f, 1.1f);
		float renderSize = illuminationSize * GameObject::objectSize * var;
		game.animations[Game::ANIMATION_LIGHT]->RenderEx(x, y, 0.0f, renderSize, renderSize);
	}
	switch(effectType)
	{
	case ASSAULT_RIFLE_MUZZLE_FLASH:
		{			
			game.animations[Game::ANIMATION_ASSAULT_RIFLE_MUZZLE_FLASH]->RenderEx(x, y, direction, GameObject::objectSize, GameObject::objectSize);
			break;
		}
	case TANK_CANNON_MUZZLE_FLASH:
		{
			game.animations[Game::ANIMATION_TANK_CANNON_MUZZLE_FLASH]->RenderEx(x, y, direction, GameObject::objectSize, GameObject::objectSize);
			break;
		}
	case MISSILE_FLY: break;
	case AT_WEAPON_BACKFIRE: break;
	case AT_WEAPON_FIRE: break;
	case TANK_MG_BULLET_HIT_GROUND: break;
	case PROJECTILE_HIT_MAN: break;
	case PROJECTILE_HIT_CORPSE: break;
	case LARGE_PROJECTILE_HIT_MAN: break;
	case LARGE_PROJECTILE_HIT_CORPSE: break;
	case BLOOD_POND:
		{
			if (game.currentLevel->night)
			{
				DWORD bs1col = bloodSprite->GetColor();
				DWORD bs2col = bloodSprite2->GetColor();
				bloodSprite->SetColor(game.currentLevel->getObjectColor());
				bloodSprite2->SetColor(game.currentLevel->getObjectColor());
				particleSystems[0]->Render();
				bloodSprite->SetColor(bs1col);
				bloodSprite2->SetColor(bs2col);
			}
			break;
		}
	case MISSILE_EXPLOSION: 
	case TANKSHELL_EXPLOSION:
	case MORTARSHELL_EXPLOSION:
		{
			if (illuminationTimer > 0.0f && !game.currentLevel->night)
			{
				float var = randFloat(0.9f, 1.1f);
				float renderSize = 4.0 * GameObject::objectSize * var;
				game.animations[Game::ANIMATION_LIGHT]->RenderEx(x, y, 0.0f, renderSize, renderSize);
				game.animations[Game::ANIMATION_LIGHT]->RenderEx(x, y, 0.0f, renderSize, renderSize);
			}
			ExplosionEffectData * eed = static_cast<ExplosionEffectData *>(ed);
			eed->foParticleSystem.Render();
			break;
		}
	case BASE_EXPLOSION:
		{
			BaseExplosionData * bed = static_cast<BaseExplosionData *>(ed);
			bed->sand.Render();
			bed->camoNet.Render();
			bed->sandbags.Render();
			break;
		}
	case NAPALM_EXPLOSION: 
		{
			cpManager->render(false); 
			break;
		}
	case TANK_EXPLOSION:
		{
			if (illuminationTimer > 100.0f && game.currentLevel->night)
			{
				illuminationSize = 6.0f;
				float var = randFloat(0.9f, 1.1f);
				float renderSize = illuminationSize * GameObject::objectSize * var;

				if (timer4 < 2.0f)
				{
					renderSize = counter2 * timer4 / 2.0f;
					illuminationSize = renderSize;
				}

				game.animations[Game::ANIMATION_LIGHT]->RenderEx(x, y, 0.0f, renderSize, renderSize);

			}

			if (illuminationTimer > 0.0f && !game.currentLevel->night)
			{
				float var = randFloat(0.9f, 1.1f);
				float renderSize = 7.0 * GameObject::objectSize * var;
				game.animations[Game::ANIMATION_LIGHT]->RenderEx(x, y, 0.0f, renderSize, renderSize);
				game.animations[Game::ANIMATION_LIGHT]->RenderEx(x, y, 0.0f, renderSize, renderSize);
			}
			TankExplosionEffectData * teed = static_cast<TankExplosionEffectData *>(ed);
			teed->foParticleSystem.Render();

			break;
		}
	case BULLET_SHELL_FLY: break;
	case FLYING_SMOKING_DEBRIS: break;
	case DEBRIS_FALL_SOUND_EMITTER:
		{
			break;
		}
	case RAIN:
		{
			RainEffectData * red = static_cast<RainEffectData *>(ed);
			float randSize = WATER_DROP_SIZE_MAX * randFloat(0.8f, 1.2f);
			float sizeChange = WATER_DROP_SPEED * timerDelta;
			float randLocationX = randFloat(-100.0f, 100.0f);
			float randLocationY = randFloat(-100.0f, 100.0f);
			float rainWindDX = RAIN_WIND_DIR_COS * RAIN_WIND_SPEED * timerDelta; 
			float rainWindDY = RAIN_WIND_DIR_SIN * RAIN_WIND_SPEED * timerDelta;
			for (int i = 0; i < WATER_DROPS_USED; i++)
			{				
				red->waterDropSizes[i] -= sizeChange;
				red->waterDropLocations[i].x += rainWindDX;
				red->waterDropLocations[i].y += rainWindDY;

				if (red->waterDropSizes[i] < 0.0f)
				{
					red->waterDropSizes[i] = randSize += 0.001;
					red->waterDropLocations[i] = Location(randFloat(0.0f, Game::SCREENWIDTH), randFloat(0.0f, Game::SCREENHEIGHT));
				}
				image->RenderEx(red->waterDropLocations[i].x, red->waterDropLocations[i].y, 0.0f, red->waterDropSizes[i],red->waterDropSizes[i]);
			}
			break;
		}
	case TEST_EFFECT_1:
		{
			TestEffectData * ted = static_cast<TestEffectData *>(ed);
			ted->foParticleSystem.Render();
			break;
		}
	case TEST_EFFECT_2:
		{
			image->RenderEx(x, y, 0.0f, 0.1f, 0.1f);
			break;
		}
	default: break;
	}
	GameObject::render();
}


void Effect::destroy(void)
{
	if (illuminatorIterator != game.objects.illuminators.end())
	{
		game.objects.illuminators.erase(illuminatorIterator);
		illuminatorIterator = game.objects.illuminators.end();
	}
	GameObject::destroy();
}

Shake_Effect::Shake_Effect(void)
{
}

Shake_Effect::Shake_Effect(float force, Location location)
:Effect(Effect::SHAKE, location, 0.0f) 
{
	set(force, location);
}

void Shake_Effect::set(float force, Location location)
{
	this->force = force;
	setLocation(location);
	origDpx = game.dpx;
	origDpy = game.dpy;
	float power = (float)getLocationVolume(100) / 100.0f;
	float xForce = (x - (Game::SCREENWIDTH + GameObject::SOUND_CLIPPING_DISTANCE));
	float yForce = (y - (Game::SCREENHEIGHT + GameObject::SOUND_CLIPPING_DISTANCE));
	xtrans = 5.0f; //10.0f * xForce * force * power;
	ytrans = 0.0f; //10.0f * yForce * force * power;
	bool right = true;//(xForce > 0.0f);
	bool up = false;//(yForce < 0.0f);
}

void Shake_Effect::processInteraction(void)
{
	shake();
}

void Shake_Effect::shake(void)
{
	if (right)
	{
		game.dpx += 1000.0f * timerDelta;
		if (game.dpx > xtrans)
		{
			xtrans -= 0.5f;
			right = false;
		}
	}
	else
	{
		game.dpx -= 1000.0f * timerDelta;
		if (game.dpx < -xtrans)
		{
			xtrans -= 0.5f;
			right = true;
		}
	}
	if (xtrans < 0.0f)
	{
		game.dpx = origDpx;
		game.dpy = origDpy;
		destroy();
	}
}

//Members of the hgeParticleSystemInfo structure:
//particleSystems[0]->info.bRelative;
//particleSystems[0]->info.colColorEnd;
//particleSystems[0]->info.colColorStart;
//particleSystems[0]->info.fAlphaVar;
//particleSystems[0]->info.fColorVar;
//particleSystems[0]->info.fDirection;
//particleSystems[0]->info.fGravityMax;
//particleSystems[0]->info.fGravityMin;
//particleSystems[0]->info.fLifetime;
//particleSystems[0]->info.fParticleLifeMax;
//particleSystems[0]->info.fParticleLifeMin;
//particleSystems[0]->info.fRadialAccelMax;
//particleSystems[0]->info.fRadialAccelMin;
//particleSystems[0]->info.fSizeEnd;
//particleSystems[0]->info.fSizeStart;
//particleSystems[0]->info.fSizeVar;
//particleSystems[0]->info.fSpeedMax;
//particleSystems[0]->info.fSpeedMin;
//particleSystems[0]->info.fSpinEnd;
//particleSystems[0]->info.fSpinStart;
//particleSystems[0]->info.fSpinVar;
//particleSystems[0]->info.fSpread;
//particleSystems[0]->info.fTangentialAccelMax;
//particleSystems[0]->info.fTangentialAccelMin;
//particleSystems[0]->info.nEmission;
//particleSystems[0]->info.sprite;