#pragma once
#include "GameObject.h"
#include <hgeparticle.h>

class Background :
	public GameObject
{
public:
	static HTARGET target;

	HTEXTURE tex;

	Background(const char * filename);
	~Background(void);
	void render(void);
	void modify(hgeSprite * modification, float mx, float my, float mdirection = 0.0f, float scale = 1.0f, bool dbg = false);
	void modify(hgeParticleSystem * modification);
	void startBatch(void);
	void batchModify(hgeSprite * modification, float mx, float my, float mdirection = 0.0f, float scale = 1.0f);
	void finishBatch(void);
};