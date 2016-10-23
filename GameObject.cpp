#include ".\GameObject.h"
#include "main.h"

/*
Paliobjektin lisääminen peliin, tarvittavat toimenpiteet:
-headerin nimi lisättävä GameObjectCollectionin alkuun.
-tyypin omaan konstruktoriin pohjaluokan konstruktorin kutsu tyyppinimellä
-Tänne enumiksi uusi TYPE-nimi
-typeNames-taulukkoon nimi lisättävä.
Mikäli objektin renderöintiin liittyy erikoisuuksia, taiobjekti on
palimaailmassa liikkuva objekti (actionObject):
//-GameObject::Gameobject::TYPE type)-konstruktorissa tyyppinimi asettettava.
-GameObjectCollectioniin addGameObjectiin saatettava ottaa kantaa ja tällöin myös deletegameObjectiin
*/

Distance GameObject::SOUND_CLIPPING_DISTANCE = 200.0f;

const Direction GameObject::DIRECTION_FORWARD = 0.0f;
const Direction GameObject::DIRECTION_BACKWARDS = M_PI;
const Direction GameObject::DIRECTION_LEFT = ONE_AND_A_HALF_PI;
const Direction GameObject::DIRECTION_RIGHT = HALF_PI;

int GameObject::numberOfObjects = 0;
float GameObject::objectSize = 1.36f; //default object size
char * GameObject::typeNames[NUMBER_OF_TYPES] = 
{
		"BACKGROUND",
		"TANK",
		"TURRET",
		"TANKMAGHINEGUN",
		"FOOT_SOLDIER",
		"SIGHT",
		"PROJECTILE",
		"EFFECT",
		"TANK_CARCASS",
		"CORPSE",
		"BASE",
		"TREE",
		"BUSH",
		"FIRE_SUPPORT",
		"SEPARATED_BODY_PART",
		"FLYING_SMOKING_DEBRIS"
};
int GameObject::renderOrders[];

GameObject::GameObject(void)
:
 number(numberOfObjects++)
,x(FLT_MAX)
,y(FLT_MAX)
,size(1.0f)
,height(0.0f)
,shadowOutLineRadius(0.0f)
,destroyed(false)
,visible(true)
,computer_controlled(false)
,referenceCount(0)
,derivedClass(0)
,dbgString(NULL)
,collisionSphereCount(1)
,collisionSphereRadius(0.0f)
,image(NULL)
,spawnCounter(1)
,startChecks(true)
{
}

void GameObject::set(TYPE type, Location location, Direction direction, Size size)
{
	setType(type);
	setLocation(location);
	setDirection(direction);
	setSize(size);

	dbgString = typeNames[type];
	renderOrder = renderOrders[type];

	renderListIterator = game.objects.renderLists[0].end();
}

//image should not be deleted here, as it can be a static one.
GameObject::~GameObject(void)
{
	numberOfObjects--;
}

void GameObject::init(void)
{
	game.renderLoadScreen("LOADING GAME OBJECT CLASS");

	hge->System_SetState(HGE_INIFILE, "Settings.ini");
	for (int i = 0; i < NUMBER_OF_TYPES; i++)
	{
		renderOrders[i] = hge->Ini_GetInt("RENDER_ORDER", typeNames[i], 1);
	}
}

void GameObject::render(void)
{
	float renderSize = size * objectSize;
	if (image && visible)
	{
		if (game.currentLevel->night && 
			type != PROJECTILE &&
			type != SIGHT &&
			type != POINTER)
		{
			DWORD col = image->GetColor();
			image->SetColor(game.currentLevel->getObjectColor());			
			image->RenderEx(x, y, direction, renderSize, renderSize);
			image->SetColor(col);
		}
		else
			image->RenderEx(x, y, direction, renderSize, renderSize);
	}
}

void GameObject::processAction(void)
{
}

void GameObject::processAI(void)
{
}

void GameObject::processInteraction(void)
{
}

void GameObject::startCheckingObjects(void)
{
}

void GameObject::checkObject(GameObject * otherObject)
{
}

void GameObject::stopCheckingObjects(void)
{
}

void GameObject::playSound(void)
{
}

HCHANNEL GameObject::playSoundAtLocation(HEFFECT effect, int volume, float pitch, bool loop)
{
	//Set defender firing started for level
	if (!game.currentLevel->levelCombatStarted)
	{
		if (effect == Tank::tank_fire_cannon_sound ||
			effect == Tank::tank_fire_mg_sound ||
			effect == game.sounds[Game::SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_1] ||
			effect == game.sounds[Game::SOUND_FOOT_SOLDIER_ASSAULT_RIFLE_FIRING_2] ||
			(effect >= game.sounds[Game::SOUND_MORTAR_SHELL_EXPLOSION_1] &&
			effect <= game.sounds[Game::SOUND_MORTAR_SHELL_EXPLOSION_LAST]) ||
			effect == Effect::shellHitExplosionSound ||
			(effect >= game.sounds[Game::SOUND_BASE_MG_FIRE_1] &&
			effect <= game.sounds[Game::SOUND_BASE_MG_FIRE_LAST]))
		{
			game.currentLevel->levelCombatStarted = true;
		}
	}

	int pan = getLocationPanning();
	volume = getLocationVolume(volume);
	
	if (!game.play_sounds)
		volume = 0;

	return hge->Effect_PlayEx(effect, volume, pan, pitch, loop);

}

int GameObject::getLocationPanning(void)
{
	//setting the pan
	int pan = (int)((x - 400.0f) / 4.0f);

	if (x < 0)
	{
		pan = -100;
	}
	else if (x > Game::SCREENWIDTH)
	{
		pan = 100;
	}

	return pan;
}

int GameObject::getLocationVolume(int volume)
{
	//setting the volume (pumpupthevolume!)

	if ( x > 850.0f )
		int u = 2;

	//by x:
	if (x < - SOUND_CLIPPING_DISTANCE || x > Game::SCREENWIDTH + SOUND_CLIPPING_DISTANCE)
		volume = 0;
	else if (x < 0)
		volume = int(volume * ((x + SOUND_CLIPPING_DISTANCE)/SOUND_CLIPPING_DISTANCE));
	else if (x > Game::SCREENWIDTH)
		volume = int(volume * (fabs(Game::SCREENWIDTH + SOUND_CLIPPING_DISTANCE - x)/SOUND_CLIPPING_DISTANCE));

	//by y:
	if (y < -SOUND_CLIPPING_DISTANCE || y > Game::SCREENHEIGHT + SOUND_CLIPPING_DISTANCE)
		volume = 0;
	else if (y < 0)
		volume = int(volume * ((y + SOUND_CLIPPING_DISTANCE)/SOUND_CLIPPING_DISTANCE));
	else if (y > Game::SCREENHEIGHT)
		volume = int(volume * (fabs(Game::SCREENHEIGHT + SOUND_CLIPPING_DISTANCE - y)/SOUND_CLIPPING_DISTANCE));

	volume = (int)((float)volume * ((float)game.fxVolume / 100.0f));

	return volume;
}

void GameObject::playAnimation(void)
{
}

void GameObject::renderShadows(void)
{
	float maxLighting = 0.0f;
    list<Effect *>::iterator it = game.objects.illuminators.begin();

	int i = 0;
	for (; it != game.objects.illuminators.end() && i < game.maxShadows; it++, i++)
	{
		float lightRadiusUnit = 50.0f; //64.0f; //'Effective' radius of light object in light.png
		float illuminationSize = (*it)->illuminationSize;
		float illuminatorRadius = illuminationSize * lightRadiusUnit * GameObject::objectSize;		
		Location lightLocation = (*it)->getLocation();
		Distance lightDistance = objectDistance(lightLocation);
		if (lightDistance < illuminatorRadius)
		{
			//Get shadow length:
			Distance shadowLength = 0.0f;
			Height objectHeight = getHeight();
			Height lightHeight = (*it)->getHeight();
			if (objectHeight > lightHeight)
			{
				//Set shadow length to the length of the light:
				shadowLength = illuminatorRadius;
			}
			else
			{
				float d = objectHeight;
				float a = lightHeight - objectHeight;
				float ratio = d / a;
				shadowLength = lightDistance * ratio;
			}

			//Get distances to shadow edges:
			Distance distFromLightToObjectEdge = sqrt(lightDistance * lightDistance + shadowOutLineRadius * shadowOutLineRadius);
			Distance distFromLightToShadowEdge = distFromLightToObjectEdge + distFromLightToObjectEdge * (shadowLength / lightDistance);

			//Get angle to shadow edge:
			Angle shadowAngle = atan(shadowOutLineRadius / lightDistance);
			Angle alpha = lightLocation.angle(Location(x, y));

			bool fire = (*it)->fire;
			Location l0 = ArcPoint(lightLocation, distFromLightToObjectEdge, alpha + shadowAngle).getLocation();
			Location l1 = ArcPoint(lightLocation, distFromLightToShadowEdge, (alpha + shadowAngle) + ((fire)?randFloat(-0.01, 0.01):0)).getLocation();
			Location l2 = ArcPoint(lightLocation, distFromLightToShadowEdge, (alpha - shadowAngle) + ((fire)?randFloat(-0.01, 0.01):0)).getLocation();
			Location l3 = ArcPoint(lightLocation, distFromLightToObjectEdge, alpha - shadowAngle).getLocation();

			game.animations[Game::ANIMATION_SHADOW]->Render4V(l0.x, l0.y, l1.x, l1.y, l2.x, l2.y, l3.x, l3.y);
		}
	}
}

void GameObject::release(void)
{
}

void GameObject::destroy(void)
{
	destroyed = true;
}

void GameObject::renderCollisionSpheres(void)
{
	if (this->type == BASE)
		int u = 2;
	int end = getCollisionSphereCount();
	for (float a = 0.0f; a < 2 * M_PI; a += 0.1f)
	{
		for (int i = 0; i < end; i++)
		{
			hge->Gfx_RenderLine(
				getCollisionSphere(i).location.x + getCollisionSphereRadius(i) * cos(a),
				getCollisionSphere(i).location.y + getCollisionSphereRadius(i) * sin(a),
				getCollisionSphere(i).location.x + getCollisionSphereRadius(i) * cos(a),
				getCollisionSphere(i).location.y + getCollisionSphereRadius(i) * sin(a) + 1);
		}
	}
	fnt->printf(x + 10.0f, y - 5.0f, "%i", number);
}

void GameObject::renderCross(void)
{
	hge->Gfx_RenderLine(0.0f,0.0f,x,y);
	hge->Gfx_RenderLine((float)game.SCREENWIDTH,0.0f,x,y);
	hge->Gfx_RenderLine((float)game.SCREENWIDTH,(float)game.SCREENHEIGHT,x,y);
	hge->Gfx_RenderLine(0.0f,(float)game.SCREENHEIGHT,x,y);
}

int GameObject::getVertex(ArcPoint arcPoint)
{
	Angle a = direction + arcPoint.angle;

	if (a > PI_TIMES_TWO)
		a -= PI_TIMES_TWO;

	if (a < HALF_PI)
	{
		return 3;
	}
	else if (a < M_PI)
	{
		return 0;
	}
	else if (a < ONE_AND_A_HALF_PI)
	{
		return 1;
	}
	else if (a < PI_TIMES_TWO)
	{
		return 2;
	}

	return -1;
}

bool GameObject::randomEvent(Time seconds)
{
	return (randFloat(0.0f, seconds / timerDelta) < 1.0f);
}

void GameObject::rl()
{
	hge->Gfx_RenderLine(0.0f, 0.0f, x, y);
}

void GameObject::rl(float x2, float y2)
{
	hge->Gfx_RenderLine(x, y, x2, y2);
}

void GameObject::rl(float x1, float y1, float x2, float y2)
{
	hge->Gfx_RenderLine(x1, y1, x2, y2);
}

void GameObject::rl(ArcPoint ap)
{
	Location l = ap.getLocation();
	rl(l);
}

void GameObject::rl(Location l)
{
	hge->Gfx_RenderLine(x, y, l.x, l.y);
}

void GameObject::rt(Location l, char * msg)
{
	fnt->printf(l.x, l.y, msg);
}

void GameObject::changeRenderOrder(int order)
{
	game.objects.renderLists[renderOrders[type]].erase(renderListIterator);
	game.objects.renderLists[order].push_back(this);
	renderListIterator = --(game.objects.renderLists[order].end());
	renderOrder = order;
}
void GameObject::checkSpawn(void)
{
	if (spawnCounter > 0)
	{	
		spawnCounter++;
		Direction dir = 0.0f;
		if (randInt(0, 1) == 0)
			dir = M_PI;
		Location l = ArcPoint(getLocation(), 50.0f, dir).getLocation();
		setLocation(l);
	}
}