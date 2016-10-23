#include ".\Background.h"
#include "main.h"

HTARGET Background::target = NULL;

Background::Background(const char * filename)
{
	if (filename == NULL)
		MessageBox(NULL, "Background::Background(const char * filename): Unable to load Background image", "i", MB_OK);
	this->type = GameObject::BACKGROUND;
	x = 0.0f;
	y = 0.0f;
	direction = 0.0f;
	target = hge->Target_Create(800, 600, false);
	if (tex = hge->Texture_Load(filename))
	{
		image = new hgeAnimation(tex, 1, 0.0f, 0.0f, 0.0f, 800.0f, 600.0f);
		hge->Gfx_BeginScene(target);
		image->Render(0, 0);
		hge->Gfx_EndScene();
		image->SetTexture(hge->Target_GetTexture(target));
	}
	else
	{
		MessageBox(NULL, "No background texture.", "i", MB_OK);
	}
}

Background::~Background(void)
{
	delete image;
	image = NULL;
	hge->Texture_Free(tex);
	hge->Target_Free(target);
}

void Background::modify(hgeSprite * modification, float mx, float my, float mdirection, float scale, bool dbg)
{
	if (game.modify_background)
	{
		if (modification)
		{
			modification->RenderEx(mx, my, mdirection, scale * GameObject::objectSize, scale * GameObject::objectSize);
		}
		if (dbg && game.show_locations)
		{
			hge->Gfx_RenderLine(0.0f,0.0f,mx, my);
			hge->Gfx_RenderLine((float)game.SCREENWIDTH,(float)game.SCREENHEIGHT,mx, my);
		}
	}
}

void Background::modify(hgeParticleSystem * modification)
{
	if (game.modify_background)
	{
		modification->Render();	
		int u = 2;
	}
}

void Background::render(void)
{
	image->SetTexture(hge->Target_GetTexture(target));
	if (game.currentLevel->night)
	{
		DWORD col = image->GetColor();
		image->SetColor(colorAdd(game.currentLevel->getObjectColor(), -70));
		image->RenderEx(x, y, direction);
		image->SetColor(col);
	}
	else
		image->RenderEx(x, y, direction);
}

void Background::startBatch(void)
{
	hge->Gfx_BeginScene(target);
	image->Render(x, y);
}

void Background::batchModify(hgeSprite * modification, float mx, float my, float mdirection, float scale)
{
	modification->RenderEx(mx, my, mdirection, scale, scale);
}

void Background::finishBatch(void)
{
	hge->Gfx_EndScene();
}

