#pragma once
#include "hgesprite.h"
#include "Background.h"
#include "player.h"
#include "projectile.h"
#include "sight.h"
#include "effect.h"
#include "footsoldier.h"
#include "base.h"
#include "backgroundobject.h"
#include "firesupport.h"
#include <hgeparticle.h>
#include <list>
using namespace std;


class GameObjectCollection
{
public:
	
	enum ints
	{
		NUMBER_OF_RENDER_LISTS = 6,
		LAST_RENDER_LIST_INDEX = NUMBER_OF_RENDER_LISTS - 1
	};

	list<GameObject *> objects; //all objects except Background, sights and player arrows
	list<GameObject *> AIObjects; //footsoldiers and non-player tanks
	list<GameObject *> actionObjects; //moving objects
	list<GameObject *> footSoldiers;
	list<GameObject *> tanks;
	list<GameObject *> renderLists[NUMBER_OF_RENDER_LISTS];
	list<GameObject *> killList;
	list<Effect *> illuminators;
	list<GameObject *>::iterator it;

	hgeParticleManager * hgePartMan; //particles rendered early
	hgeParticleManager * preRenderPartMan; //particles rendered late

	int projectilesBeginIndex;
	
	Background * background;
	Sight * player1_cannonSight;
	Sight * player2_cannonSight;
	Sight * player1_MGSight;
	Sight * player2_MGSight;

	int amountOfObjects;

	float smallestEnemyDist;

	GameObjectCollection(void);
	~GameObjectCollection(void);
	void render(void);
	void processAction(void);
	void processAI(void);
	void processInteraction(void);
	inline void testCollision(GameObject * object1, GameObject * object2);
	void fireParticleSystem(hgeParticleSystem * particleSystem, float x, float y, float direction = 0.0f, bool prerender = false);
	void fireParticleSystem(hgeParticleSystemInfo psInfo, float x, float y, float direction = 0.0f, bool prerender = false);
	void addGameObject(GameObject * object, int renderOrder = -1);
	void deleteGameObject(list<GameObject *>::iterator &);
	void clear(void);
	void release(void);
	void listAll(void);
	bool isAIObject(GameObject * object);
	bool isActionObject(GameObject * object);

	//mehtods for iterating through objects:
	void startIterating(list<GameObject *> & listOf);
	bool reachedEnd(void);
	GameObject * getNext(void);

	GameObject * getNearestEnemy(CombatObject * thisObject, GameObject::TYPE enemyType, float minObservingAngle = -1.0f, float maxObservingAngle = -1.0f);

	void setOnTop(GameObject * setUpper, GameObject * setLower);

	//dbg:
	bool render_type_location[GameObject::NUMBER_OF_TYPES];

private:
	float clearTimer;
};