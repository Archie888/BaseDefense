#include "main.h"

//System
HGE * hge = NULL;
hgeFont * fnt = NULL;
hgeFont * titlefont = NULL;
hgeFont * hudfont = NULL;
float timerDelta; //Time of last frame function call.

const float PI_TIMES_TWO = 2.0f * M_PI;
const float HALF_PI = M_PI / 2.0f;
const float ONE_AND_A_HALF_PI = M_PI * 1.5f;
const float EIGHT_OF_PI = M_PI / 8.0f;

int checkRange(int value, int min, int max)
{
	if (value < min)
		value = min;
	else if (value > max)
		value = max;

	return value;
}

float checkRange(float value, float min, float max)
{
	if (value < min)
		value = min;
	else if (value > max)
		value = max;

	return value;
}

float getRelativeFigure(float value, float valueRangeMin, float valueRangeMax, float relativeFigureRangeMin, float relativeFigureRangeMax)
{
	float relativeMultiplier = (value - valueRangeMin) / (valueRangeMax - valueRangeMin);
	float relativeFigure = relativeMultiplier * (relativeFigureRangeMax - relativeFigureRangeMin) + relativeFigureRangeMin;
	return relativeFigure;
}

int randInt(int min, int max)
{
	return hge->Random_Int(min, max);
}

float randFloat(float min, float max)
{
	float ret = hge->Random_Float(min, max);
	return ret;
}

float randomVariation(float variation)
{
	return hge->Random_Float(1.0f - variation, 1.0f + variation);
}

float randomDirection(void)
{
	return (Direction)hge->Random_Float(0.0f, 6.283185f);
}

float distance(float x1, float y1, float x2, float y2)
{
	float a = x2 - x1;
	float b = y2 - y1;
	float pow1 = pow(a, 2.0f);
	float pow2 = pow(b, 2.0f);
	float distance = sqrt(pow1 + pow2);
	return distance;
}

float directionCheck(float direction)
{
	if (direction > PI_TIMES_TWO)
		direction -= PI_TIMES_TWO;
	if (direction < 0.0f)
		direction += PI_TIMES_TWO;
	return direction;
}

hgeAnimation * CopyAnimation(hgeAnimation * aniAnimation)
{
   float fX, fY, fWidth, fHeight;
   aniAnimation->GetTextureRect(&fX, &fY, &fWidth, &fHeight);
   hgeAnimation * pRet = new hgeAnimation(aniAnimation->GetTexture(), aniAnimation->GetFrames(), aniAnimation->GetSpeed(),
      fX, fY, fWidth, fHeight);
   float hx, hy;
   aniAnimation->GetHotSpot(&hx, &hy);
   pRet->SetHotSpot(hx, hy);
   bool bx, by;
   aniAnimation->GetFlip(&bx, &by);
   pRet->SetFlip(bx, by);
   return pRet;
}

float smallestAngleBetween(float angle1, float angle2)
{
	float angleBetween = angle1 - angle2;
	angleBetween = fabs(angleBetween);
	float shortestAngle = 0.0f;

	//Get the shortest angle.
	if (angleBetween > M_PI)
		shortestAngle = PI_TIMES_TWO - angleBetween;
	else
		shortestAngle = angleBetween;

	return fabs(shortestAngle);
}

float directionalSmallestAngleBetween(float angle1, float angle2)
{
	float shortestAngle = 0.0f;

	//case 1
	if (angle1 >= M_PI && angle1 <= PI_TIMES_TWO &&
		angle2 > 0 && angle2 < M_PI)
	{
		if (angle2 < angle1 - M_PI)
		{
			shortestAngle = angle2 + (PI_TIMES_TWO - angle1);
		}
		else
		{
			shortestAngle = angle2 - angle1;
		}
	}

	//case 2
	else if (angle2 >= M_PI && angle2 <= PI_TIMES_TWO &&
		angle1 > 0 && angle1 < M_PI)
	{
		if (angle1 < angle2 - M_PI)
		{
			shortestAngle = -(angle1 + (PI_TIMES_TWO - angle2));
		}
		else
		{
			shortestAngle = angle2 - angle1;
		}
	}

	//case 3
	else
	{
		shortestAngle = angle2 - angle1;
	}
	return shortestAngle;
}

float actualAngleBetween(float angle1, float angle2)
{
	angle1 += PI_TIMES_TWO;
	angle2 += PI_TIMES_TWO;

	return angle1 - angle2;
}

//Max arvo 255 per kanava.
DWORD getColor(BYTE a, BYTE r, BYTE g, BYTE b)
{
	DWORD color = ((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b));
	return color;
}

void getColorComponents(DWORD color, BYTE *a, BYTE *r, BYTE *g, BYTE *b)
{
	*a = (BYTE)(color >> 24);
	*r = (BYTE)(color >> 16);
	*g = (BYTE)(color >> 8);
	*b = (BYTE)color;
}

DWORD colorAdd(DWORD color, int value)
{
	int a = (BYTE)(color >> 24);
	int r = (BYTE)(color >> 16);
	int g = (BYTE)(color >> 8);
	int b = (BYTE)color;

	//test values
	r = r + value;
	if (r < 0)
		r = 0;
	else if (r > 255)
		r = 255;

	g = g + value;
	if (g < 0)
		g = 0;
	else if (g > 255)
		g = 255;

	b = b + value;
	if (b < 0)
		b = 0;
	else if (b > 255)
		b = 255;

	color = ((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b));
	int * i = (int *)color;
	return color;
}

DWORD colorMul(DWORD color1, DWORD color2)
{
	BYTE a = (float)((BYTE)(color1 >> 24)) / 255.0f * (float)((BYTE)(color2 >> 24)) / 255.0f;
	BYTE r = (float)((BYTE)(color1 >> 16)) / 255.0f * (float)((BYTE)(color2 >> 16)) / 255.0f;
	BYTE g = (float)((BYTE)(color1 >> 8)) / 255.0f * (float)((BYTE)(color2 >> 8)) / 255.0f;
	BYTE b = (float)((BYTE)(color1)) / 255.0f * (float)((BYTE)(color2)) / 255.0f;
	DWORD color = ((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b));
	return color;
}

DWORD modifyColor(const DWORD color, BYTE _a, BYTE _r, BYTE _g, BYTE _b)
{
	BYTE a = (_a > -1)? a = _a:(BYTE)(color >> 24);
	BYTE r = (_r > -1)? r = _r:(BYTE)(color >> 16);
	BYTE g = (_g > -1)? g = _g:(BYTE)(color >> 8);
	BYTE b = (_b > -1)? b = _b:(BYTE)color;
	DWORD col = ((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b));
	return col;
}

void renderBoundedString(hgeFont * fnt, float x, float y, int align, float width, const char * fmt, ...) {
   static char txt[1000];
   txt[0] = 0;
   
   // Print everything onto a temporary buffer:
   va_list va;
   va_start(va, fmt);
   vsprintf(txt, fmt, va);
   va_end(va);
   
   // Account for font scaling:
   width /= fnt->GetScale();
   
   // Transfer to a temporary string with line breaks to keep it within the
   // specified width:
   static std::string strFinal, strWord;
   strFinal.clear();
   strWord.clear();
   size_t len = strlen(txt);
   float currLineWidth = 0;
   float currWordWidth = 0;
   
   for(size_t i = 0; i <= len; i++) {
      // Calculate this character's width:
      float charWidth = 0;
      if(fnt->GetSprite(txt[i]))
         charWidth += fnt->GetSprite(txt[i])->GetWidth() + fnt->GetTracking();
      else if(fnt->GetSprite('?'))
         charWidth += fnt->GetSprite('?')->GetWidth() + fnt->GetTracking();
      
      if(txt[i] == ' ' || txt[i] == '\n' || txt[i] == '\0') {
         // Word delimiter found.
         // Word too big for the remaining space on this line?
         if(currLineWidth + currWordWidth - fnt->GetTracking() > width) {
            // Eliminate trailing spaces:
            while(strFinal[strFinal.size() - 1] == ' ')
               strFinal.resize(strFinal.size() - 1);
            
            // Break the line:
            strFinal += '\n';
            currLineWidth = 0;
            
            // Start new line with that word:
            strFinal += strWord;
            strFinal += txt[i];
            currLineWidth += currWordWidth;
            currLineWidth += charWidth;
         }
         else {
            // Word fits within this line.
            strFinal += strWord;
            strFinal += txt[i];
            currLineWidth += currWordWidth;
            currLineWidth += charWidth;
         }
         
         // Start new word:
         strWord.clear();
         currWordWidth = 0;
         
         // Delimiter was a line break?
         if(txt[i] == '\n') {
            // Eliminate trailing spaces:
            while(strFinal[strFinal.size() - 1] == ' ')
               strFinal.resize(strFinal.size() - 1);
            
            // Start new line.
            currLineWidth = 0;
         }
      }
      else {
         // Word will become too big for the line width?
         if(currWordWidth + charWidth - fnt->GetTracking() > width) {
            // Start a new line with the current word fragment:
            if(currLineWidth > 0) {
               // Eliminate trailing spaces:
               while(strFinal[strFinal.size() - 1] == ' ')
                  strFinal.resize(strFinal.size() - 1);
               
               // Break the line:
               strFinal += '\n';
            }
            strFinal += strWord;
            strFinal += '\n';
            currLineWidth = 0;
            
            // Reset word:
            strWord.clear();
            currWordWidth = 0;
         }
         
         // Add to word:
         strWord += txt[i];
         currWordWidth += charWidth;
      }
   }
   
   fnt->Render(x, y, strFinal.c_str(), align);
} 


void renderSquare(float x1, float y1, float x2, float y2, DWORD color)
{
	hge->Gfx_RenderLine(x1, y1, x2, y1, color);
	hge->Gfx_RenderLine(x2, y1, x2, y2, color);
	hge->Gfx_RenderLine(x2, y2, x1, y2, color);
	hge->Gfx_RenderLine(x1, y2, x1, y1, color);
}
extern void renderBox(float x1, float y1, float width, float height, DWORD color)
{
	float x2 = width + x1;
	float y2 = height + y1;
	hge->Gfx_RenderLine(x1, y1, x2, y1, color);
	hge->Gfx_RenderLine(x2, y1, x2, y2, color);
	hge->Gfx_RenderLine(x2, y2, x1, y2, color);
	hge->Gfx_RenderLine(x1, y2, x1, y1, color);
}

void renderCenteredText(float x, float y, char * text, hgeFont * fnt)
{
	float stringWidth = fnt->GetStringWidth(text);
	float stringHeight = fnt->GetHeight()* fnt->GetScale();

	x = x - stringWidth / 2.0f;
	y = y - stringHeight / 2.0f;

	fnt->printf(x, y, text);
}

//Game
Game game;

bool frameFunc()
{
	return game.run();
}

bool focusLostFunc()
{
	hge->Gfx_EndScene();
	hge->Channel_StopAll();
	return false; 
}

bool focusGainFunc()
{
	if (game.objects.background)
	{
		hge->Gfx_BeginScene();
		game.objects.background->image->SetTexture(game.objects.background->tex);
		game.objects.background->modify(game.objects.background->image, game.objects.background->x, game.objects.background->y);
		hge->Gfx_EndScene();
	}
	return false;
}

void init()
{
	//HGE inits
	hge = hgeCreate(HGE_VERSION);

	hge->System_SetState(HGE_FRAMEFUNC, frameFunc);
	hge->System_SetState(HGE_FOCUSGAINFUNC, focusGainFunc);
	hge->System_SetState(HGE_FOCUSLOSTFUNC, focusLostFunc);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_LOGFILE, "log.txt");
	hge->System_SetState(HGE_TITLE, "Base Defense");
	hge->System_SetState(HGE_SCREENWIDTH, game.SCREENWIDTH);
	hge->System_SetState(HGE_SCREENHEIGHT, game.SCREENHEIGHT);
	hge->System_SetState(HGE_HIDEMOUSE, false);

	if (!(hge->System_Initiate()))
	{
		return;
	}

	//fonts
	{
		//game gui font
		fnt = new hgeFont("impact_green_smoked.fnt");
		titlefont = new hgeFont("titlefont.fnt");
		hudfont = new hgeFont("font1.fnt");
		hudfont->SetScale(0.5f);
	}
}

void shutdown()
{
	//Release font
	delete fnt, fnt = NULL;
	delete titlefont, titlefont = NULL;
	delete hudfont, hudfont = NULL;
	hge->System_Shutdown();
	hge->Release();
	hge = NULL;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	init();
	bool b = hge->System_Start();
	shutdown();
	return 0;
}

//Alpha-kanava on l‰pin‰kyvyysarvona t‰ss‰ kontekstissa. vain Arvolla 0xFF toinen objekti ei n‰y toisen l‰pi.
/*
//Alpha = l‰pin‰kyvyys.
//verteksiv‰rivaikuttimet:
#define	BLEND_COLORADD    1	nollasta ylˆsp‰in vaalenee normaalista t‰ysin vaaleaan. Kaikki v‰rit t‰ysill‰ = valkoiseen.						Jos verteksill‰ on v‰ri, se lis‰t‰‰n tekseleihin. Musta v‰ri eli 0x00000000 ei vaikuta mitenk‰‰n. Vain tekstuurin oma v‰ri on havaittavissa ilman efektej‰. Alfa-kanavaan vaikuttamalla t‰llˆinkin voi vaikuttaa tekstuurin l‰pin‰kyvyyteen: 0xFF: t‰ysin solidi, 0x00: n‰kym‰tˆn.
#define	BLEND_COLORMUL    0 ylh‰‰lt‰ alasp‰in tummenee normaalista mustaan. Kaikki v‰rit nollassa = musta.							T‰ss‰ voit saada aikaan hyvin tummia s‰vyj‰. Jos v‰ri on 0xFFFFFFFF, ei vaikutusta.

//alfa-kanavan vaikuttimet:
#define	BLEND_ALPHABLEND  2 Tavallinen rendaus. p‰‰ll‰ n‰kyv‰t objektit n‰kyv‰t toisten p‰‰ll‰ ja sill‰ sipuli, kun alpha-kanava huutaa 0xFF:aa. Jos alpha on pienempi, pinta n‰ytt‰‰ l‰pi ilman kirkastuksia. Pikseleiden v‰reist‰ tehd‰‰n l‰pi n‰kyess‰ jonkinlainen sovintoratkaisu ilmeisestikin.
#define	BLEND_ALPHAADD    0 Objektin pikseliv‰rit lis‰t‰‰n toisiinsa, jolloin tuloksena on hyvin kirkas lopputulos. Sit‰ kirkkaampi, mit‰  arvokkaampi alfa-kanava on. Lopputulos on valkoinen, kun alfa-kanava n‰ytt‰‰ 0xFF:aa.

#define	BLEND_ZWRITE      4
#define	BLEND_NOZWRITE    0
*/
