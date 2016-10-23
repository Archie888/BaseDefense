#include ".\Backgroundobject.h"
#include "main.h"

char * BackgroundObject::names[];

void BackgroundObject::init(void)
{
	names[BG_TREE1] = "TREE1";
	names[BG_BUSH1] = "BUSH1";
	names[BG_BUSH2] = "BUSH2";
	names[BG_BUSH3] = "BUSH3";
	names[BG_BUSH4] = "BUSH4";
}

void BackgroundObject::release(void)
{
}

BackgroundObject::BackgroundObject(void)
{
}

void BackgroundObject::set(GameObject::TYPE type, Location location, Size size, Direction direction)
{
	setDerivedClass(getDerivedClass() | (int)BACKGROUND_OBJECT);

	if (direction < 0.0f)
		direction = randFloat(0.0f, PI_TIMES_TWO);

	GameObject::set(type, location, direction, size);
}


BackgroundObject::~BackgroundObject(void)
{
}

Bush::Bush(void)
{
}

Bush::Bush(BackgroundObject::BACKGROUNDOBJECT_TYPE bushtype, Location location, Size size, Direction direction)
{	
	set(bushtype, location, size, direction);
}

void Bush::set(BackgroundObject::BACKGROUNDOBJECT_TYPE bushtype, Location location, Size size, Direction direction)
{
	BackgroundObject::set(GameObject::BUSH, location, size, direction);

	image = game.animations[Game::ANIMATION_BUSH_1 + (bushtype - BackgroundObject::BG_BUSH1)]; //new hgeAnimation(game.textures[Game::TEXTURE_BUSH_1 + (bushtype - BackgroundObject::BG_BUSH1)], 1, 0.0f, 0.0f, 0.0f, 64.0f, 64.0f);
}


Bush::~Bush(void)
{
}

void Bush::processInteraction(void)
{
	game.objects.background->modify(image, x, y, direction, size);
	destroy();
}

Size Tree::TANK_TURRET_BARREL_TURNING_SIZE = 1.0f;
Height Tree::AVERAGE_TREE_HEIGHT = 64.0f;

HTEXTURE Tree::treetex = NULL;
#define TREE_BURNING_LENGTH 48.0f

Tree::Tree(void)
{
}

Tree::Tree(BackgroundObject::BACKGROUNDOBJECT_TYPE treetype, Location location, Size size, Direction direction)
{
	set(treetype, location, size, direction);
}

void Tree::set(BackgroundObject::BACKGROUNDOBJECT_TYPE treetype, Location location, Size size, Direction direction)
{
	BackgroundObject::set(GameObject::TREE, location, size, direction);

	setHeight(64.0f * GameObject::objectSize * size); //53 is the tree length in tree1.png.
	shadowOutLineRadius = 16.0f * GameObject::objectSize * size/* * 1.6f*/;

	if (treetype == BackgroundObject::BACKGROUNDOBJECT_TYPE::BG_TREE1)
	{
		treeFallSpeed = 5.0f;
		image = new hgeAnimation(treetex, 10, treeFallSpeed, 0.0f, 0.0f, (float)TEXTURE_WIDTH, (float)TEXTURE_HEIGHT);
		image->SetHotSpot(32.0 * 3.0f, (float)TEXTURE_HEIGHT / 2.0f);
		image->SetMode(HGEANIM_NOLOOP);

		setCollisionSphereRadius(10.0f);
	}

	falling = false;
	fallen = false;
	set_order = false;
	burning = false;
	burningTimer = 0.0f;
	timer1 = 0.0f;

	for (int i = 0; i < MAX_FIRES; i++)
	{
		effects[i] = NULL;
	}
}

Tree::~Tree(void)
{
	for (int i = 0; i < MAX_FIRES; i++)
	{
		if (effects[i])
		{
			if (!effects[i]->isDestroyed())
				effects[i]->destroy();
			delete effects[i];
			effects[i] = NULL;
		}
	}
}

void Tree::init(void)
{
	treetex = hge->Texture_Load("tree1.png");
}

void Tree::release(void)
{
	hge->Texture_Free(treetex);
}

void Tree::processAction(void)
{
	if (falling && !fallen)
	{
		image->SetSpeed(treeFallSpeed += 0.02f);
		image->Update(timerDelta);
	}
}
void Tree::processInteraction(void)
{
	if (falling && !fallen)
	{
		if (image->GetFrame() == 9 && timer1 <= 0.0f)
		{		
			timer1 = 1.0f;
		}

		if (image->GetFrame() < 4 && !set_order)
		{
			changeRenderOrder(1);
			set_order = true;
		}

		//Effect locations change when tree falls
		if (image->GetFrame() < 9)
		{
			Angle a = (HALF_PI / 8) * (float)(8 - image->GetFrame());
			


			for (int i = 0; i < MAX_FIRES; i++)
			{
				if (effects[i])
				{
					float effectX = burningLocations[i].x * cos(a);
					Location l0(0.0f, 0.0f);
					Location l1(effectX, burningLocations[i].y);
					Distance d = l0.distance(l1);
					Angle a = l0.angle(l1);
					Location fireLocation = ArcPoint(getLocation(), d, a + direction + M_PI).getLocation();
	
					effects[i]->setLocation(fireLocation);
					effects[i]->processInteraction();
				}
			}
		}

		if (timer1 > 0.0f)
		{
			timer1 -= timerDelta;

			if (timer1 <= 0.0f)
			{
				setRenderOrder(0);
				fallen = true;
			}
		}
	}

	if (burningTimer > 0.0f)
	{		
		burningTimer -= timerDelta;

		if (burningTimer <= 0.0f)
		{
			int frame = image->GetFrame();
			image->SetTextureRect(0.0f, 64.0f, 128.0f, 64.0f);
			image->SetFrame(frame);
			burning = false;
		}
	}

	for (int i = 0; i < MAX_FIRES; i++)
	{
		bool stillBurning = false;
		if (effects[i])
		{
			if (effects[i]->timer1 <= 0.0f)
			{
				if (!effects[i]->isDestroyed())
					effects[i]->destroy();
				delete effects[i];
				effects[i] = NULL;
			}
			else
			{
				effects[i]->processInteraction();
				stillBurning = true;
			}
			burning = stillBurning;
		}
	}
}

void Tree::render(void)
{
	bool render_shadows = true;

	if (game.currentLevel->night)
	{
		render_shadows = (!burning);
	}

	render_shadows = render_shadows && (!fallen);

	if (render_shadows)
	{
		renderShadows();
	}

	float renderSize = size * GameObject::objectSize;
	if (image && visible)
	{
		if (game.currentLevel->night)
		{
			DWORD col = image->GetColor();
			image->SetColor(colorAdd(game.currentLevel->getObjectColor(), -50));
			image->RenderEx(x, y, direction, renderSize, renderSize);
			image->SetColor(col);
		}
		else
			image->RenderEx(x, y, direction, renderSize, renderSize);
	}

	for (int i = 0; i < MAX_FIRES; i++)
		if (effects[i])
		{
			effects[i]->render();
		}
	}
}

void Tree::fall(Direction direction)
{
	this->direction = direction;
	image->Play();
	playSoundAtLocation(game.sounds[randInt(Game::SOUND_TREE_FALL_1, Game::SOUND_TREE_FALL_MAX)]);

	Location l1 = ArcPoint(getLocation(), AVERAGE_TREE_HEIGHT * size * 0.25f, getDirection() + M_PI).getLocation();
	Location l2 = ArcPoint(getLocation(), AVERAGE_TREE_HEIGHT * size * 0.5f, getDirection() + M_PI).getLocation();
	Location l3 = ArcPoint(getLocation(), AVERAGE_TREE_HEIGHT * size * 0.75f, getDirection() + M_PI).getLocation();
	game.objects.addGameObject(new Effect(Effect::TREE_FALL, l1, getDirection(), NULL, size * 1.3f));
	game.objects.addGameObject(new Effect(Effect::TREE_FALL, l2, getDirection(), NULL, size));
	game.objects.addGameObject(new Effect(Effect::TREE_FALL, l3, getDirection(), NULL, size * 0.75f));

	falling = true;
}

void Tree::collide(GameObject * otherObject, ArcPoint hittingPoint)
{
	if (otherObject->type == GameObject::TANK)
		collide(static_cast<Tank *>(otherObject), hittingPoint);
	else if (otherObject->type == GameObject::PROJECTILE)
		collide(static_cast<Projectile *>(otherObject), hittingPoint);
	else if (otherObject->type == GameObject::FOOT_SOLDIER)
		otherObject->collide(this, hittingPoint);
	else if (otherObject->type == GameObject::EFFECT)
	{
		Effect * e = static_cast<Effect *>(otherObject);
		if (e->collisionTimer > 0.0f)
			collide(e, hittingPoint);
	}
}

void Tree::collide(Tank * tank, ArcPoint hittingPoint)
{
	if (objectDistance(tank->getCollisionSphere1().location) < getCollisionSphereRadius() + Tank::TANK_COLLISIONSPHERE_RADIUS ||
		objectDistance(tank->getCollisionSphere2().location) < getCollisionSphereRadius() + Tank::TANK_COLLISIONSPHERE_RADIUS)
	{
		if (!falling)
			fall(objectAngle(tank));
		else if (fallen)
		{
			tank->collide(this, hittingPoint);
		}
	}
	tank->collide(this, hittingPoint);
}

void Tree::collide(Projectile * projectile, ArcPoint hittingPoint)
{
	if (!falling && !fallen)
	{
		if (projectile->projectileType == Projectile::PROJECTILETYPE::TANK_CANNON_PROJECTILE ||
			projectile->projectileType == Projectile::PROJECTILETYPE::AT_ROCKET)
		{
			if (hittingPoint.radius > 2.0f)
				return; 

			game.objects.addGameObject(new Effect(Effect::TANKSHELL_EXPLOSION, hittingPoint.getLocation(), direction));
			projectile->hit();
			fall(projectile->direction + M_PI);

 			DWORD color = 0xFF000000;
								
			image->SetColor(color, 1);
			image->SetColor(color, 2);
			if (randFloat(0.0f, 0.1f) > 0.5f)
				image->SetColor(color, 0);
			if (randFloat(0.0f, 0.1f) > 0.5f)
				image->SetColor(color, 3);
				
		}
		else if (projectile->projectileType == Projectile::HAND_GRENADE)
		{
			projectile->speed = 0;
		}
		else
		{
			if (hittingPoint.radius > 2.0f)
				return; 

			if (randFloat(0.0f, 1.0f) > 0.2f)
				return;

			bulletHit(projectile, hittingPoint);
		}
	}
	else
	{
		if (hittingPoint.radius > 2.0f)
			return; 

		if (randFloat(0.0f, 1.0f) > 0.5f)
			return;

		if (projectile->getHeight() < 4.0f)
		{
			if (projectile->projectileType == Projectile::ASSAULT_RIFLE_BULLET ||
				projectile->projectileType == Projectile::TANK_MG_BULLET)
				bulletHit(projectile, hittingPoint);
		}
	}
}

void Tree::collide(Effect * effect, ArcPoint hittingPoint)
{
	Distance dist = squaredObjectDistance(effect);
	Distance fallDistance = 0.0f;
	Distance blackeningDistance = 0.0f;
	bool burn = false;
	if (effect->effectType == Effect::TANKSHELL_EXPLOSION)
	{
		fallDistance = 5.0f;
		blackeningDistance = 10.0f;
		fallDistance *= fallDistance;
	}
	else if (effect->effectType == Effect::MORTARSHELL_EXPLOSION)
	{
		fallDistance = 5.0f;
		blackeningDistance = 10.0f;
		fallDistance *= fallDistance;
	}
	if (effect->effectType == Effect::MISSILE_EXPLOSION)
	{
		fallDistance = 5.0f;
		blackeningDistance = 10.0f;
		fallDistance *= fallDistance;
	}
	if (effect->effectType == Effect::NAPALM_EXPLOSION)
	{
		fallDistance = -1.0f; //no falling

		if (compareDistance(dist, effect->counter1) < 0.0f)
			burn = true;
	}


	if (compareDistance(dist, blackeningDistance) < 0.0f)
	{
		image->SetColor(colorAdd(image->GetColor(), -20));

		if (!falling && !fallen)
		{
			if (compareDistance(dist, fallDistance) < 0.0f)
			{
				fall(objectAngle(effect));
			}
		}
	}

	if (!burning && burn && !fallen)
	{
		int max = MAX_FIRES;
		if (game.currentLevel->night) max = 1;
		for (int i = 0; i < max; i++)
		{
			if (effects[i])
			{
				if (!effects[i]->isDestroyed())
					effects[i]->destroy();
				delete effects[i];
			}
			effects[i] = new Effect(Effect::FIRE, getLocation(), effect->direction, NULL, 2.0f * getSize());
			Height burningHeight = randFloat(0.0f, TREE_BURNING_LENGTH);
			float rel = 1 - burningHeight / TREE_BURNING_LENGTH;
			burningHeight += 16.0f;
			float maxVertical = rel * 8.0f;
			float horizontal = randFloat(-maxVertical, maxVertical);
			//Location as seen on the texture
			burningLocations[i] = Location(burningHeight, horizontal);
		}
		burning = true;
		burningTimer = 10.0f;
	}
}

void Tree::bulletHit(Projectile * projectile, ArcPoint hittingPoint)
{
	//Play some multimedia here.
	int number = randInt(Game::SOUND_PROJECTILE_HIT_TREE_1, Game::SOUND_PROJECTILE_HIT_TREE_LAST);
	playSoundAtLocation(game.sounds[number], 15);
	projectile->hit();
}
