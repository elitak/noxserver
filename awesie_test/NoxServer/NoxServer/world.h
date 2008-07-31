#pragma once

#include "global.h"
#include "contact_listener.h"
#include "NoxMap.h"
#include <queue>
#include <map>

class player;
class world_packet;
class world_session;

typedef std::map<uint8, player*> player_map;

class world : public boost::singleton<world>
{
public:
	world(boost::restricted);

	void run();
	void quit();

	bool is_quiting();
	NoxMap& get_map() { return m_map; }

	uint8 get_free_id();
	void return_free_id(uint8 id);

	void add_player(player* plr);
	void remove_player(player* plr);
	player_map::const_iterator get_players() { return m_players.begin(); }

	void send_player_info(world_session& session, world_packet& packet);

	// physics
	b2World& get_the_world() { return *m_the_world; }
private:
	bool m_is_quitting;
	std::queue<uint8> m_free_ids;

	// map players ids to player objects
	std::map<uint8, player*> m_players;

	boost::posix_time::ptime m_start_time;
	boost::posix_time::ptime m_current_time;

	NoxMap m_map;

	// physics
	b2World* m_the_world;
	float32 m_time_step;
	contact_listener m_contact_listener;
};