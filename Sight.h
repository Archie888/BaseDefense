#pragma once
#include "GameObject.h"


class Sight :
	public GameObject
{
public:
	static HTEXTURE tex;
	static hgeAnimation * cannonSightImage;
	static hgeAnimation * mgSightImage;

	enum SIGHTTYPE
	{
		SIGHT_TYPE_CANNON,
		SIGHT_TYPE_MG,
		SIGHT_TYPE_FOOT_SOLDIER
	};

	int sightType;
		
	Sight();
	~Sight(void);
	static void init(void);
	static void release(void);
	void setPosition(float px, float py, float pDirection, float pAngle);
	void setType(int sightType);
};