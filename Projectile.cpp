#include ".\projectile.h"
#include "main.h"
//#include <math.h>
#include <cstdlib>
#include "effect.h"

int Projectile::amount = 0;
HTEXTURE Projectile::tex = NULL;
hgeAnimation * Projectile::tankCannonProjectileImage = NULL;
hgeAnimation * Projectile::ATProjectileImage = NULL;
hgeAnimation * Projectile::bulletImage = NULL;
hgeAnimation * Projectile::grenadeImage = NULL;

const Time Projectile::MAX_GAME_AGE = 2.0f; //projectile max age in game is 5 seconds.
const Velocity Projectile::MISSILE_SPEED = 2000.0f;
const Velocity Projectile::TANK_CANNON_PROJECTILE_SPEED = 1500.0f;
const Velocity Projectile::AT_ROCKET_SPEED = 800.0f;
const Velocity Projectile::TANK_MG_BULLET_SPEED = 600.0f;
const Velocity Projectile::ASSAULT_RIFLE_BULLET_SPEED = 1000.0f;
const Velocity Projectile::HAND_GRENADE_SPEED = 300.0f;	
const Velocity Projectile::MISSILE_COLLISIONSPHERE_RADIUS = 2.0f;
const Distance Projectile::TANK_SHELL_COLLISIONSPHERE_RADIUS = 1.0f;
const Distance Projectile::AT_ROCKET_COLLISIONSPHERE_RADIUS = 1.0f;
const Distance Projectile::TANK_MG_BULLET_COLLISIONSPHERE_RADIUS = 0.02f;
const Distance Projectile::ASSAULT_RIFLE_BULLET_COLLISIONSPHERE_RADIUS = 0.01f;
const Distance Projectile::HAND_GRENADE_NOTIFICATION_RADIUS = 20.0f;
const Time Projectile::HAND_GRENADE_TIME = 3.0f;

#define MISSILE_COLLISIONSPHERE_RADIUS_DEF 25.0f
#define AT_ROCKET_SPEED_DEF 1300
#define MISSILE_SPEED_DEF 3000.0f
#define TANK_MG_BULLET_SPEED_DEF 1500.0f
#define ASSAULT_RIFLE_BULLET_SPEED_DEF 1500.0f
#define TANK_CANNON_PROJECTILE_SPEED_DEF 2000.0f
#define FLARE_SPEED_DEF 800.0f
#define FLARE_FIRING_ANGLE 1.25f
#define FLARE_RISING_TIME 5.0f

#define SMALL_PROJECTILE_GLOOM_SIZE 2.0f
#define BIG_PROJECTILE_GLOOM_SIZE 3.0f


Projectile::Projectile(void)
{
}

Projectile::Projectile(CombatObject * shooter, PROJECTILETYPE projectileType, Location firingLocation, Height firingHeight, Location destination)
{
	Projectile::amount++;
	set(shooter, projectileType, firingLocation, firingHeight, destination);
}

Projectile::~Projectile(void)
{
	amount -= 1;
}

void Projectile::set(CombatObject * shooter, PROJECTILETYPE projectileType, Location firingLocation, Height firingHeight, Location destination)
{
	if (shooter == NULL)
	{
		char mj[255];
		sprintf(mj, "(PROJECTILETYPE)%i: Projectile::Shooter can't be NULL", projectileType);
		MessageBox(NULL, mj, "i", MB_OK);
	}
	CombatObject::set(GameObject::PROJECTILE, firingLocation, 0.0f, shooter->side);
	this->shooter = shooter;
	this->projectileType = projectileType;
	setDestination(destination);
	this->startDistance = objectDistance(getDestination());
	this->startHeight = firingHeight;
	this->distance = startDistance;
	this->frameDistance = 0.0f;
	this->direction = objectAngle(getDestination());
	speed = 0.0f;
	age = 0.0f;
	previousLocation = Location(x, y);

	switch (projectileType)
	{
	case TANK_CANNON_PROJECTILE:
		{
			speed = TANK_CANNON_PROJECTILE_SPEED_DEF;
			if (1.0f / timerDelta < 30.0f)
				speed /= 3.0f;
			image = tankCannonProjectileImage; 
			setCollisionSphereRadius(TANK_SHELL_COLLISIONSPHERE_RADIUS);
			break;
		}
	case AT_ROCKET:
		{
			speed = AT_ROCKET_SPEED_DEF; 
			if (1.0f / timerDelta < 30.0f)
				speed /= 2.0f;
			image = ATProjectileImage; 
			setCollisionSphereRadius(AT_ROCKET_COLLISIONSPHERE_RADIUS);
			break;
		}
	case AIR_TO_GROUND_MISSILE:
		{
			speed = MISSILE_SPEED_DEF;
			image = tankCannonProjectileImage;
			setCollisionSphereRadius(MISSILE_COLLISIONSPHERE_RADIUS_DEF);	
			game.objects.addGameObject(new Effect(Effect::MISSILE_FLY, getLocation(), getDirection(), this), GameObjectCollection::LAST_RENDER_LIST_INDEX);
			break;
		}
	case TANK_MG_BULLET : 
		{
			setSize(0.7f);
			speed = TANK_MG_BULLET_SPEED_DEF;
			image = bulletImage;
			setCollisionSphereRadius(TANK_MG_BULLET_COLLISIONSPHERE_RADIUS);
			break;
		}
	case ASSAULT_RIFLE_BULLET :
		{
			setSize(0.5f);
			speed = ASSAULT_RIFLE_BULLET_SPEED_DEF;
			image = bulletImage;
			setCollisionSphereRadius(ASSAULT_RIFLE_BULLET_COLLISIONSPHERE_RADIUS);
			break;
		}
	case FLARE :
		{
			game.objects.addGameObject(new Effect(Effect::FLARE_LIGHT, getLocation(), getDirection(), this));
			speed = FLARE_SPEED_DEF * cos(FLARE_FIRING_ANGLE);
			timer = FLARE_RISING_TIME;
			setHeight(0.0f);
			break;
		}
	default : break;
	}

	this->dx = speed * cos(direction);
	this->dy = speed * sin(direction);

	reachedGoal = false;
}

void Projectile::init(void)
{
	game.renderLoadScreen("LOADING PROJECTILE CLASS");
	tex = hge->Texture_Load("effects.png");
	tankCannonProjectileImage = new hgeAnimation(tex, 1, 0.0f, 0.0f, 9.0f, 47.0f, 11.0f);
	tankCannonProjectileImage->SetHotSpot(38.0f, 11.0f / 2.0f);
	tankCannonProjectileImage->SetBlendMode(BLEND_COLORMUL| BLEND_ALPHAADD | BLEND_NOZWRITE);

	ATProjectileImage = new hgeAnimation(tex, 1, 0.0f, 64.0f, 0.0f, 96.0f - 64.0f, 4.0f);
	ATProjectileImage->SetHotSpot((96.0f - 64.0f) / 2.0f, 2.0f);
	ATProjectileImage->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE);

	bulletImage = new hgeAnimation(tex, 1, 0.0f, 0.0f, 0.0f, 25.0f, 3.0f);
	bulletImage->SetHotSpot(22.0f, 1.5f);
	bulletImage->SetBlendMode(BLEND_COLORMUL| BLEND_ALPHAADD | BLEND_NOZWRITE);
	
	grenadeImage = new hgeAnimation(tex, 1, 0.0f, 53.0f, 0.0f, 8.0f, 8.0f);
	grenadeImage->SetHotSpot(4.0f, 4.0f);
}

void Projectile::release(void)
{
	hge->Texture_Free(tex);
	delete tankCannonProjectileImage; tankCannonProjectileImage = NULL;
	delete ATProjectileImage; ATProjectileImage = NULL;
	delete bulletImage; bulletImage = NULL;
	delete grenadeImage; grenadeImage = NULL;
}

void Projectile::processAction(void)
{
	previousLocation = Location(x, y);
	frameDistance = speed * timerDelta;
	if (distance < frameDistance)
	{
		setLocation(getDestination());
		frameDistance = distance;
		distance = 0;
		reachedGoal = true;
	}
	else
	{
		distance -= frameDistance;
		x += dx * timerDelta;
		y += dy * timerDelta;

		if (projectileType == FLARE)
		{
			if (timer > 0.0f)
			{
				height += 100.0f * timerDelta;

				if (speed > 100.0f)
					speed -= 75.0f * timerDelta;
			}
			else
			{
				height -= 100.0f * timerDelta;
			}
			this->dx = speed * cos(direction);
			this->dy = speed * sin(direction);
		}
	}
}

void Projectile::processInteraction(void)
{
	//If this hasn't hit anything else

	if (projectileType == FLARE)
	{
		timer -= timerDelta;

		if (reachedGoal)
			hit(GameObject::BACKGROUND, getLocation());
	}
	else
	{
		if (reachedGoal)
			hit(GameObject::BACKGROUND, getLocation());
	}
}

void Projectile::render(void)
{
	if (projectileType == ASSAULT_RIFLE_BULLET ||
		projectileType == TANK_MG_BULLET)
	{
		if (game.show_mg_tracers)
		{
			if (game.currentLevel->night)
			{
				game.animations[Game::ANIMATION_LIGHT]->RenderEx(x, y, 0.0f, SMALL_PROJECTILE_GLOOM_SIZE * GameObject::objectSize, SMALL_PROJECTILE_GLOOM_SIZE * GameObject::objectSize);
			}

			GameObject::render();
		}

		if (game.show_locations)
		{
			hge->Gfx_RenderLine(x, y, 0.0f, 0.0f, 0x0000FF00);
			float dist = objectDistance(getDestination());
			float ang = objectAngle(getDestination());
			fnt->printf(dist / 2.0f * cos(ang), dist / 2.0f * sin(ang), "DIST=%i", dist);
		}
	}
	else if (projectileType == TANK_CANNON_PROJECTILE)
	{
		if (game.currentLevel->night)
		{
			game.animations[Game::ANIMATION_LIGHT]->RenderEx(x, y, 0.0f, BIG_PROJECTILE_GLOOM_SIZE * GameObject::objectSize, BIG_PROJECTILE_GLOOM_SIZE * GameObject::objectSize);
		}

		if (game.show_cannon_tracers)
			GameObject::render();
	}
	else if (!reachedGoal)
	{
		GameObject::render();
	}
}

void Projectile::hit(GameObject::TYPE hitObjectType, Location hittingLocation)
{
	//ground hit
	if (hitObjectType == GameObject::BACKGROUND)
	{
		if (projectileType == TANK_CANNON_PROJECTILE)
			game.objects.addGameObject(new Effect(Effect::TANKSHELL_EXPLOSION, hittingLocation, direction));

		else if (projectileType == AT_ROCKET)
			game.objects.addGameObject(new Effect(Effect::TANKSHELL_EXPLOSION, hittingLocation, direction));

		else if (projectileType == AIR_TO_GROUND_MISSILE)
			game.objects.addGameObject(new Effect(Effect::MISSILE_EXPLOSION, hittingLocation, direction));

		else if (projectileType == TANK_MG_BULLET)
			game.objects.addGameObject(new Effect(Effect::TANK_MG_BULLET_HIT_GROUND, hittingLocation, direction));
			
		else if (projectileType == ASSAULT_RIFLE_BULLET)		
			game.objects.addGameObject(new Effect(Effect::ASSAULT_RIFLE_BULLET_HIT_GROUND, hittingLocation, direction));
	}
	else if (hitObjectType == GameObject::FOOT_SOLDIER)
	{
		if (projectileType == ASSAULT_RIFLE_BULLET)
			game.objects.addGameObject(new Effect(Effect::PROJECTILE_HIT_MAN, hittingLocation, direction));			
		else if (projectileType == TANK_MG_BULLET)
			game.objects.addGameObject(new Effect(Effect::LARGE_PROJECTILE_HIT_MAN, hittingLocation, direction));		
	}
	else if (hitObjectType == GameObject::CORPSE)
	{
		if (projectileType == ASSAULT_RIFLE_BULLET)
			game.objects.addGameObject(new Effect(Effect::PROJECTILE_HIT_CORPSE, hittingLocation, direction));						
		else if (projectileType == TANK_MG_BULLET)		
			game.objects.addGameObject(new Effect(Effect::LARGE_PROJECTILE_HIT_CORPSE, hittingLocation, direction));
	}
	else if (hitObjectType == GameObject::TANK)
	{
		if (projectileType == TANK_MG_BULLET || projectileType == ASSAULT_RIFLE_BULLET)
			game.objects.addGameObject(new Effect(Effect::PROJECTILE_HIT_TANK, hittingLocation, direction));	
		else if (projectileType == TANK_CANNON_PROJECTILE)
			game.objects.addGameObject(new Effect(Effect::TANK_EXPLOSION, hittingLocation, direction));
		else if (projectileType == AT_ROCKET)
			game.objects.addGameObject(new Effect(Effect::TANK_EXPLOSION, hittingLocation, direction));
	}
	else if (hitObjectType == GameObject::TANK_CARCASS)
	{
		if (projectileType == TANK_MG_BULLET || projectileType == ASSAULT_RIFLE_BULLET)
			game.objects.addGameObject(new Effect(Effect::PROJECTILE_HIT_TANK, hittingLocation, direction));	
		else if (projectileType == TANK_CANNON_PROJECTILE)
			game.objects.addGameObject(new Effect(Effect::TANKSHELL_EXPLOSION, hittingLocation, direction));
		else if (projectileType == AT_ROCKET)
			game.objects.addGameObject(new Effect(Effect::TANKSHELL_EXPLOSION, hittingLocation, direction));
		else if (projectileType == AIR_TO_GROUND_MISSILE)
			game.objects.addGameObject(new Effect(Effect::MISSILE_EXPLOSION, hittingLocation, direction));
	}
	destroy();
}

void Projectile::hit(void)
{
	destroy();
}

void Projectile::explode(void)
{
	game.objects.addGameObject(new Effect(Effect::HAND_GRENADE_EXPLOSION, getLocation(), direction));
	destroy();
}

void Projectile::smoke(void)
{
	game.objects.addGameObject(new Effect(Effect::SMOKE_GRENADE, getLocation(), direction));
}

bool Projectile::checkPenetrationCourse(CollisionSphere cs)
{
	//Todo: ...
	return false;
}

ArcPoint Projectile::getPenetrationPoint(CollisionSphere cs)
{
	return ArcPoint();
}

//Simplified ballistics in this game: projectiles fly straight.
Height Projectile::getHeight(void)
{
	return distance / startDistance * startHeight;
}

#define SMOKE_GRENADE_TIMER 2.5f
#define SMOKE_GRENADE_SPEED 350.0f

Grenade::Grenade(CombatObject * shooter, PROJECTILETYPE projectileType, Location firingLocation, Height firingHeight, Location destination)
:angle(0.0f)
{
	Projectile::set(shooter, projectileType, firingLocation, firingHeight, destination);

	switch (projectileType)
	{
	case HAND_GRENADE :
		{
			timer = HAND_GRENADE_TIME;
			speed = HAND_GRENADE_SPEED;
			image = grenadeImage;
			setCollisionSphereRadius(HAND_GRENADE_NOTIFICATION_RADIUS);
			break;
		}
	case SMOKE_GRENADE:
		{
			timer = SMOKE_GRENADE_TIMER;
			speed = SMOKE_GRENADE_SPEED;
			image = grenadeImage;
			break;
		}
	default: break;
	}
}

void Grenade::processAction(void)
{
	previousLocation = Location(x, y);

	if (speed > 0.0f)
		speed -= 150.0f * timerDelta;

	frameDistance = speed * timerDelta;
	if (distance < frameDistance)
	{
		setLocation(getDestination());
		frameDistance = distance;
		distance = 0;
		reachedGoal = true;
	}
	else
	{
		dx = speed * cos(direction);
		dy = speed * sin(direction);
		distance -= frameDistance;
		x += dx * timerDelta;
		y += dy * timerDelta;
	}
}

void Grenade::processInteraction(void)
{
	if (projectileType == HAND_GRENADE)
	{
		timer -= timerDelta;

		if (timer < 0.0f)
		{
			explode();
		}
	}
	else if (projectileType == SMOKE_GRENADE)
	{
		timer -= timerDelta;

		if (timer < 0.0f)
		{
			smoke();
			timer = 1000.0f;
		}
		if (timer > 900 && timer < 995.0f)
			destroy();
	}
}

void Grenade::render(void)
{
	GameObject::render();
}