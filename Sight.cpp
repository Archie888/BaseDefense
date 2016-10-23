#include ".\sight.h"
#include "main.h"

HTEXTURE Sight::tex = NULL;
hgeAnimation * Sight::cannonSightImage = NULL;
hgeAnimation * Sight::mgSightImage = NULL;


Sight::Sight()
{
	GameObject::set(GameObject::SIGHT, Location(0.0f, 0.0f), 0.0f);
}

Sight::~Sight(void)
{
}

void Sight::init(void)
{
	game.renderLoadScreen("LOADING SIGHT CLASS");
	float x1 = 0.0f;
	float x2 = 60.0f;
	float y1 = 400.0f;
	float y2 = 430.0f;
	float height = y2 - y1;
	tex = hge->Texture_Load("tank.png");
	cannonSightImage = new hgeAnimation(tex, 1, 0.0f, x1, y1, x2, height);
	cannonSightImage->SetHotSpot(x2 / 2.0f, height / 2.0f);

	x1 = 0.0f;
	x2 = 50.0f;
	y1 = 445.0f;
	y2 = 494.0f;
	height = y2 - y1;
	mgSightImage = new hgeAnimation(tex, 1, 0.0f, x1, y1, x2, height);
	mgSightImage->SetHotSpot(x2 / 2.0f, height / 2.0f);
}

void Sight::release()
{
	hge->Texture_Free(tex);
}

void Sight::setPosition(float px, float py, float pDirection, float pAngle)
{
	float d = pAngle * pAngle * 10000.0f + 100.0f;
	x = px + d * cos(pDirection);
	y = py + d * sin(pDirection);
}

void Sight::setType(int sightType)
{
	this->sightType = sightType;

	switch (sightType)
	{
	case SIGHT_TYPE_CANNON : image = cannonSightImage; break;
	case SIGHT_TYPE_MG: image = mgSightImage; break;
	default :  image = mgSightImage; break;
	}
}