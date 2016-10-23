#pragma once

#include <math.h>
#include <cfloat>
#include "game.h"
#include <hge.h>
#include <hgefont.h>

#define DEBUGC 1 //dbg compile
  
extern HGE * hge;
extern Game game;
extern float timerDelta;
extern hgeFont * fnt;
extern hgeFont * titlefont;
extern hgeFont * hudfont;
extern sinTable[628];
extern cosTable[628];
extern tanTable[628];
extern const float PI_TIMES_TWO;
extern const float HALF_PI;
extern const float ONE_AND_A_HALF_PI;
extern const float EIGHT_OF_PI;
extern int checkRange(int value, int min, int max);
extern float checkRange(float value, float min, float max);
extern float getRelativeFigure(float value, float valueRangeMin, float valueRangeMax, float relativeFigureRangeMin, float relativeFigureRangeMax);
extern int randInt(int min, int max);
extern float randFloat(float min, float max);
extern float randomVariation(float variation);
extern float randomDirection(void);
extern float distance(float x1, float y1, float x2, float y2);
extern float directionCheck(float direction);
extern hgeAnimation * CopyAnimation(hgeAnimation * aniAnimation);
extern float smallestAngleBetween(float angle1, float angle2);
extern float directionalSmallestAngleBetween(float angle1, float angle2);
extern float actualAngleBetween(float angle1, float angle2);
extern DWORD getColor(BYTE a, BYTE r, BYTE g, BYTE b);
extern void getColorComponents(DWORD color, BYTE *a, BYTE *r, BYTE *g, BYTE *b);
extern DWORD colorAdd(DWORD color, int value);
extern DWORD colorMul(DWORD color1, DWORD color2);
extern DWORD modifyColor(const DWORD color, BYTE a, BYTE r, BYTE g, BYTE b);
extern void renderBoundedString(hgeFont * fnt, float x, float y, int align, float width, const char * fmt, ...);
extern void renderSquare(float x1, float y1, float x2, float y2, DWORD color = 0xffffffff);
extern void renderBox(float x1, float y1, float width, float height, DWORD color = 0xffffffff);
extern void renderCenteredText(float x, float y, char * text, hgeFont * fnt);