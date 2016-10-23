#include ".\input.h"
#include "main.h"
#include "tank.h"
#include <string.h>

int Input::numberOfInputs = 0;
hgeInputEvent Input::event;
bool Input::isEvent = false;

Input::Input(void)
{
	number = numberOfInputs++;
	flag = true;
}

Input::~Input(void)
{
}

void Input::processInput(Tank * tank)
{
	tank->tank_toggle_firemode = 
		isEvent &&
		event.type == INPUT_KEYDOWN &&
		event.key == tank_toggle_firemode_key;

	tank->tank_fire_cannon = 
		isEvent &&
		(event.type == INPUT_KEYDOWN || event.type == INPUT_MBUTTONDOWN) &&
		(event.key == tank_fire_cannon_key1 || event.key == tank_fire_cannon_key2);	

	if (isEvent &&
		event.type == INPUT_MBUTTONDOWN && event.type == INPUT_MBUTTONDOWN &&
		(event.key == tank_fire_cannon_key1 || event.key == tank_fire_cannon_key2) &&
		tank == game.player_2.tank)
		int u = 2;

	if (isEvent &&
		event.type == INPUT_MBUTTONDOWN&&
		(event.key == tank_fire_cannon_key1 || event.key == tank_fire_cannon_key2))// && event.type == INPUT_MBUTTONDOWN)
		int u = 2;

	tank->tank_turn_left = hge->Input_GetKeyState(tank_turn_left_key);
	tank->tank_turn_right = hge->Input_GetKeyState(tank_turn_right_key);
	tank->tank_forward = hge->Input_GetKeyState(tank_forward_key);
	tank->tank_backwards = hge->Input_GetKeyState(tank_backwards_key);
	tank->tank_turret_turn_left = hge->Input_GetKeyState(tank_turret_turn_left_key);
	tank->tank_turret_turn_right = hge->Input_GetKeyState(tank_turret_turn_right_key);
	tank->tank_machine_gun_turn_left = hge->Input_GetKeyState(tank_machine_gun_turn_left_key);
	tank->tank_machine_gun_turn_right = hge->Input_GetKeyState(tank_machine_gun_turn_right_key);
	tank->tank_fire_mg = hge->Input_GetKeyState(tank_fire_mg_key1) || hge->Input_GetKeyState(tank_fire_mg_key2);
	tank->tank_turret_angle_up = hge->Input_GetKeyState(tank_turret_angle_up_key);
	tank->tank_turret_angle_down = hge->Input_GetKeyState(tank_turret_angle_down_key);
	tank->tank_mg_angle_up = hge->Input_GetKeyState(tank_mg_angle_up_key);
	tank->tank_mg_angle_down = hge->Input_GetKeyState(tank_mg_angle_down_key);
}

void Input::setKeys(char * playerName)
{
	hge->System_SetState(HGE_INIFILE, "Input.ini");
	tank_turn_left_key = translate(hge->Ini_GetString(playerName, "TANK_TURN_LEFT_KEY", "vasuri"));
	tank_turn_right_key = translate(hge->Ini_GetString(playerName, "TANK_TURN_RIGHT_KEY", "oikuri"));
	tank_forward_key = translate(hge->Ini_GetString(playerName, "TANK_FORWARD_KEY", "etiapai"));
	tank_backwards_key = translate(hge->Ini_GetString(playerName, "TANK_BACKWARDS_KEY", "taakke"));
	tank_turret_turn_left_key = translate(hge->Ini_GetString(playerName, "TANK_TURRET_TURN_LEFT_KEY", "vasu"));
	tank_turret_turn_right_key = translate(hge->Ini_GetString(playerName, "TANK_TURRET_TURN_RIGHT_KEY", "oiku"));
	tank_machine_gun_turn_right_key = translate(hge->Ini_GetString(playerName, "TANK_MACHINE_GUN_TURN_RIGHT_KEY", "vasui"));
	tank_machine_gun_turn_left_key = translate(hge->Ini_GetString(playerName, "TANK_MACHINE_GUN_TURN_LEFT_KEY", "oikui"));
	string test = hge->Ini_GetString(playerName, "TANK_FIRE_CANNON_KEY1", "tuli");
	tank_fire_cannon_key1 = translate(hge->Ini_GetString(playerName, "TANK_FIRE_CANNON_KEY1", "tuli"));
	tank_fire_cannon_key2 = translate(hge->Ini_GetString(playerName, "TANK_FIRE_CANNON_KEY2", "tuli"));
	tank_fire_mg_key1 = translate(hge->Ini_GetString(playerName, "TANK_FIRE_MG_KEY1", "mgtuli"));
	tank_fire_mg_key2 = translate(hge->Ini_GetString(playerName, "TANK_FIRE_MG_KEY2", "mgtuli"));
	tank_toggle_firemode_key = translate(hge->Ini_GetString(playerName, "TANK_TOGGLE_FIREMODE_KEY", "tulimoodi"));
	tank_turret_angle_up_key = translate(hge->Ini_GetString(playerName, "TANK_TURRET_ANGLE_UP_KEY", "tykkikulma ylös"));
	tank_turret_angle_down_key = translate(hge->Ini_GetString(playerName, "TANK_TURRET_ANGLE_DOWN_KEY", "tykkikulma alas"));
	tank_mg_angle_up_key = translate(hge->Ini_GetString(playerName, "TANK_MG_ANGLE_UP_KEY", "kkkulma ylös"));
	tank_mg_angle_down_key = translate(hge->Ini_GetString(playerName, "TANK_MG_ANGLE_DOWN_KEY", "kkkulma alas"));
}

int Input::translate(char * keycode)
{
	char * mjonot[] =
	{
	  "HGEK_LBUTTON"	//x01"
	, "HGEK_RBUTTON"	//x02"
	, "HGEK_MBUTTON"	//x04"

	, "HGEK_ESCAPE"		//x1B"
	, "HGEK_BACKSPACE"	//x08"
	, "HGEK_TAB"		//x09"
	, "HGEK_ENTER"		//x0D"
	, "HGEK_SPACE"		//x20"

	, "HGEK_SHIFT"		//x10"
	, "HGEK_CTRL"		//x11"
	, "HGEK_ALT"		//x12"

	, "HGEK_LWIN"		//x5B"
	, "HGEK_RWIN"		//x5C"
	, "HGEK_APPS"		//x5D"

	, "HGEK_PAUSE"		//x13"
	, "HGEK_CAPSLOCK"	//x14"
	, "HGEK_NUMLOCK"	//x90"
	, "HGEK_SCROLLLOCK"	//x91"

	, "HGEK_PGUP"		//x21"
	, "HGEK_PGDN"		//x22"
	, "HGEK_HOME"		//x24"
	, "HGEK_END"		//x23"
	, "HGEK_INSERT"		//x2D"
	, "HGEK_DELETE"		//x2E"

	, "HGEK_LEFT"		//x25"
	, "HGEK_UP"			//x26"
	, "HGEK_RIGHT"		//x27"
	, "HGEK_DOWN"		//x28"

	, "HGEK_0"			//x30"
	, "HGEK_1"			//x31"
	, "HGEK_2"			//x32"
	, "HGEK_3"			//x33"
	, "HGEK_4"			//x34"
	, "HGEK_5"			//x35"
	, "HGEK_6"			//x36"
	, "HGEK_7"			//x37"
	, "HGEK_8"			//x38"
	, "HGEK_9"			//x39"

	, "HGEK_A"			//x41"
	, "HGEK_B"			//x42"
	, "HGEK_C"			//x43"
	, "HGEK_D"			//x44"
	, "HGEK_E"			//x45"
	, "HGEK_F"			//x46"
	, "HGEK_G"			//x47"
	, "HGEK_H"			//x48"
	, "HGEK_I"			//x49"
	, "HGEK_J"			//x4A"
	, "HGEK_K"			//x4B"
	, "HGEK_L"			//x4C"
	, "HGEK_M"			//x4D"
	, "HGEK_N"			//x4E"
	, "HGEK_O"			//x4F"
	, "HGEK_P"			//x50"
	, "HGEK_Q"			//x51"
	, "HGEK_R"			//x52"
	, "HGEK_S"			//x53"
	, "HGEK_T"			//x54"
	, "HGEK_U"			//x55"
	, "HGEK_V"			//x56"
	, "HGEK_W"			//x57"
	, "HGEK_X"			//x58"
	, "HGEK_Y"			//x59"
	, "HGEK_Z"			//x5A"

	, "HGEK_GRAVE"		//xC0"
	, "HGEK_MINUS"		//xBD"
	, "HGEK_EQUALS"		//xBB"
	, "HGEK_BACKSLASH"	//xDC"
	, "HGEK_LBRACKET"	//xDB"
	, "HGEK_RBRACKET"	//xDD"
	, "HGEK_SEMICOLON"	//xBA"
	, "HGEK_APOSTROPHE"	//xDE"
	, "HGEK_COMMA"		//xBC"
	, "HGEK_PERIOD"		//xBE"
	, "HGEK_SLASH"		//xBF"

	, "HGEK_NUMPAD0"	//x60"
	, "HGEK_NUMPAD1"	//x61"
	, "HGEK_NUMPAD2"	//x62"
	, "HGEK_NUMPAD3"	//x63"
	, "HGEK_NUMPAD4"	//x64"
	, "HGEK_NUMPAD5"	//x65"
	, "HGEK_NUMPAD6"	//x66"
	, "HGEK_NUMPAD7"	//x67"
	, "HGEK_NUMPAD8"	//x68"
	, "HGEK_NUMPAD9"	//x69"

	, "HGEK_MULTIPLY"	//x6A"
	, "HGEK_DIVIDE"		//x6F"
	, "HGEK_ADD"		//x6B"
	, "HGEK_SUBTRACT"	//x6D"
	, "HGEK_DECIMAL"	//x6E"

	, "HGEK_F1"			//x70"
	, "HGEK_F2"			//x71"
	, "HGEK_F3"			//x72"
	, "HGEK_F4"			//x73"
	, "HGEK_F5"			//x74"
	, "HGEK_F6"			//x75"
	, "HGEK_F7"			//x76"
	, "HGEK_F8"			//x77"
	, "HGEK_F9"			//x78"
	, "HGEK_F10"		//x79"
	, "HGEK_F11"		//x7A"
	, "HGEK_F12"		//x7B"
	};

	int keykoodit[102] = 
	{
	  HGEK_LBUTTON		//x01
	, HGEK_RBUTTON		//x02
	, HGEK_MBUTTON		//x04

	, HGEK_ESCAPE		//x1B
	, HGEK_BACKSPACE	//x08
	, HGEK_TAB			//x09
	, HGEK_ENTER		//x0D
	, HGEK_SPACE		//x20

	, HGEK_SHIFT		//x10
	, HGEK_CTRL			//x11
	, HGEK_ALT			//x12

	, HGEK_LWIN			//x5B
	, HGEK_RWIN			//x5C
	, HGEK_APPS			//x5D

	, HGEK_PAUSE		//x13
	, HGEK_CAPSLOCK		//x14
	, HGEK_NUMLOCK		//x90
	, HGEK_SCROLLLOCK	//x91

	, HGEK_PGUP			//x21
	, HGEK_PGDN			//x22
	, HGEK_HOME			//x24
	, HGEK_END			//x23
	, HGEK_INSERT		//x2D
	, HGEK_DELETE		//x2E

	, HGEK_LEFT			//x25
	, HGEK_UP			//x26
	, HGEK_RIGHT		//x27
	, HGEK_DOWN			//x28

	, HGEK_0			//x30
	, HGEK_1			//x31
	, HGEK_2			//x32
	, HGEK_3			//x33
	, HGEK_4			//x34
	, HGEK_5			//x35
	, HGEK_6			//x36
	, HGEK_7			//x37
	, HGEK_8			//x38
	, HGEK_9			//x39

	, HGEK_A			//x41
	, HGEK_B			//x42
	, HGEK_C			//x43
	, HGEK_D			//x44
	, HGEK_E			//x45
	, HGEK_F			//x46
	, HGEK_G			//x47
	, HGEK_H			//x48
	, HGEK_I			//x49
	, HGEK_J			//x4A
	, HGEK_K			//x4B
	, HGEK_L			//x4C
	, HGEK_M			//x4D
	, HGEK_N			//x4E
	, HGEK_O			//x4F
	, HGEK_P			//x50
	, HGEK_Q			//x51
	, HGEK_R			//x52
	, HGEK_S			//x53
	, HGEK_T			//x54
	, HGEK_U			//x55
	, HGEK_V			//x56
	, HGEK_W			//x57
	, HGEK_X			//x58
	, HGEK_Y			//x59
	, HGEK_Z			//x5A

	, HGEK_GRAVE		//xC0
	, HGEK_MINUS		//xBD
	, HGEK_EQUALS		//xBB
	, HGEK_BACKSLASH	//xDC
	, HGEK_LBRACKET		//xDB
	, HGEK_RBRACKET		//xDD
	, HGEK_SEMICOLON	//xBA
	, HGEK_APOSTROPHE	//xDE
	, HGEK_COMMA		//xBC
	, HGEK_PERIOD		//xBE
	, HGEK_SLASH		//xBF

	, HGEK_NUMPAD0		//x60
	, HGEK_NUMPAD1		//x61
	, HGEK_NUMPAD2		//x62
	, HGEK_NUMPAD3		//x63
	, HGEK_NUMPAD4		//x64
	, HGEK_NUMPAD5		//x65
	, HGEK_NUMPAD6		//x66
	, HGEK_NUMPAD7		//x67
	, HGEK_NUMPAD8		//x68
	, HGEK_NUMPAD9		//x69

	, HGEK_MULTIPLY		//x6A
	, HGEK_DIVIDE		//x6F
	, HGEK_ADD			//x6B
	, HGEK_SUBTRACT		//x6D
	, HGEK_DECIMAL		//x6E

	, HGEK_F1		//x70
	, HGEK_F2		//x71
	, HGEK_F3		//x72
	, HGEK_F4		//x73
	, HGEK_F5		//x74
	, HGEK_F6		//x75
	, HGEK_F7		//x76
	, HGEK_F8		//x77
	, HGEK_F9		//x78
	, HGEK_F10		//x79
	, HGEK_F11		//x7A
	, HGEK_F12		//x7B
	};

	for (int i = 0; i < 102; i ++)
	{
		if (strcmp(keycode, mjonot[i]) == 0)
		{
			//MessageBox(NULL, keycode, "i", MB_OK);
			return keykoodit[i];
		}
	}
	//Heitetään poikkeus
	MessageBox(NULL, keycode, "i", MB_OK);
	return 0;
}

int Input::getHex(char * numstring)
{
	int number = 0;
	int i = 0;
	while (numstring[i] != '\0')
		i++;
	int mul = 1;

	for (i = i - 1; i > 1; i--)
	{
		number += (numstring[i] - '0') * mul;
		mul *= 16;
	}

	return number;
}

void Input::getEvent(void)
{
	isEvent = hge->Input_GetEvent(&event);
}