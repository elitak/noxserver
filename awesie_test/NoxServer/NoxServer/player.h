#pragma once

#include "global.h"
#include "unit.h"

class world_session;

struct player_info //size = 0x7E
{
	player_info()
	{
		unk0 = 0;
		unk2 = 0;
		unk4 = 0;
		flag = 0x10;
	}
	wchar_t name[0x21]; //0x19
	uint8  pclass;
	uint8 unk3;
	uint8 info[0x1D];
	uint8 unk0;
	uint8 unk1[0x0C];
	uint8 unk4;
	uint32 unk2;
	char username[9];
	uint8 flag;
};

enum player_class_type
{
	PLAYER_CLASS_WARRIOR	= 0x0,
	PLAYER_CLASS_WIZARD		= 0x1,
	PLAYER_CLASS_CONJURER	= 0x2
};

class player : public unit
{
public:
	player(world_session& session);

	void respawn();
	
	bool drop(object* obj, uint32 max_dist, float x, float y);
	bool pickup(object* obj, uint32 max_dist);
	bool equip(object* obj);
	bool dequip(object* obj);
	void equip_secondary(object* obj);
	void swap_weapon();

	void attack();
	void laugh();
	void jump();
	void taunt();
	void point();
	void move_towards(float x, float y);
	void running_jump();

protected:
	world_session& _session;
	player_info m_player_info;

	friend world_session;
};