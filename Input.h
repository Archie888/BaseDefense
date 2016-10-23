#pragma once
#include "tank.h"
#include <hge.h>

class Input
{
public:

	//debug:
	static int numberOfInputs;
	int number;

	//ínput hack flag:
	bool flag;

	static hgeInputEvent event;
	static bool isEvent;

	int tank_turn_left_key;
	int tank_turn_right_key;
	int tank_forward_key;
	int tank_backwards_key;
	int tank_turret_turn_left_key;
	int tank_turret_turn_right_key;
	int tank_turret_angle_up_key;
	int tank_turret_angle_down_key;
	int tank_machine_gun_turn_left_key;
	int tank_machine_gun_turn_right_key;
	int tank_fire_cannon_key1;
	int tank_fire_cannon_key2;
	int tank_fire_mg_key1;
	int tank_fire_mg_key2;
	int tank_toggle_firemode_key;
	int tank_mg_angle_up_key;
	int tank_mg_angle_down_key;

	Input(void);
	~Input(void);
	static void getEvent(void);
	void processInput(Tank * tank);
	void setKeys(char * playerName);

private:
	static int translate(char * keycode);
	static int getHex(char * numstring);
};