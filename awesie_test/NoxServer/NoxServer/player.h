#pragma once

#include "global.h"
#include "unit.h"

#include <set>

class world;
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

typedef std::set<object*> update_queue;

class player : public unit
{
public:
	player(world_session& session);

	void update_player();
	virtual void respawn();
	virtual void update(uint32 diff);
	virtual void set_position(float x, float y)
	{
		unit::set_position(x, y);
		update_view();
	}
	
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
	void move_towards(float x, float y, float speed);
	void running_jump();
	void run();
	void stop();

	bool is_ability_ready(uint8 ability);

	world_session& get_session() { return _session; }

protected:
	world_session& _session;
	player_info m_player_info;
	update_queue m_update_queue;

	uint32 m_moving_timer;

	void update_view();

	virtual void SendUpdatePacket();
	virtual void _BuildUpdatePacket(world_packet& packet);
	virtual void _BuildNewPlayerPacket(world_packet& packet);
	virtual void _BuildClientStatusPacket(world_packet& packet);
	virtual void _BuildHealthPacket(world_packet& packet);
	virtual void _BuildMyHealthPacket(world_packet& packet);
	virtual void _BuildTotalHealthPacket(world_packet& packet);
	virtual void _BuildManaPacket(world_packet& packet);
	virtual void _BuildTotalManaPacket(world_packet& packet);
	virtual void _BuildStatsPacket(world_packet& packet);
	virtual void _BuildResetAbilityPacket(world_packet& packet, uint8 ability);
	virtual void _BuildAbilityStatePacket(world_packet& packet, uint8 ability);

	friend world;
	friend world_session;
};