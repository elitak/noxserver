#include "global.h"
#include "world.h"
#include "world_session.h"
#include "world_packet.h"
#include "player.h"

using namespace boost::posix_time;

// loop resolution in milliseconds
#define LOOP_TIME 33
// physics defines
#define STEPS_PER_SECOND 30
#define ITERATIONS 10

world::world(boost::restricted) : m_is_quitting(false)
{
	// right now the world has a well-defined size of 588, 588
	b2AABB worldSize;
	worldSize.lowerBound.Set(0, 0);
	worldSize.upperBound.Set(588, 588);

	// the only non-free id is 0, which is our id
	m_free_ids.set();
	m_free_ids[0] = false;

	// we don't have gravity in n0x
	b2Vec2 gravity(0.0f, 0.0f);

	m_the_world = new b2World(worldSize, gravity, true);
	m_time_step = 1.0f / STEPS_PER_SECOND;
}

void world::run()
{
	m_start_time = microsec_clock::local_time();

	// connect to WOL
	socket_mgr::instance->get_wol_socket().connect("zoaedk", "computer");

	// enable listening for requests
	socket_mgr::instance->get_world_socket().listen();

	while(!m_is_quitting)
	{
		m_current_time = microsec_clock::local_time();
		
		// step physics
		m_the_world->Step( m_time_step, ITERATIONS );

		// step network
		socket_mgr::instance->run();

		// sleep till next cycle
		time_duration duration = m_current_time - microsec_clock::local_time();
		boost::this_thread::sleep( milliseconds( LOOP_TIME - duration.total_milliseconds() ) );
	}

	// we are quitting, so destroy stuff as necessary
}

void world::quit()
{
	m_is_quitting = true;
}

bool world::is_quiting()
{
	return m_is_quitting;
}

uint8 world::get_free_id()
{
	if(m_free_ids.none())
		return 0xFF;

	uint8 i;
	for(i = 1; i < 32; i++)
		if(m_free_ids[i])
			break;

	m_free_ids[i] = false;
	return i;
}

void world::return_free_id(uint8 id)
{
	if(id > 32)
		return;

	m_free_ids.set(id);
}

void world::add_player(player *plr)
{
	m_players[plr->get_session().get_player_id()] = plr;
}

void world::remove_player(player* plr)
{
	m_players.erase(plr->get_session().get_player_id());
}

void world::send_player_info(world_session& session, world_session& packet)
{
    for(player_map::iterator iter=m_players.begin(); iter != m_players.end(); ++iter)
    {
        if(iter->second != session->get_player())
		{
			iter->second->_BuildNewPlayerPacket(packet);
			session->send_packet(packet);
			iter->second->_BuildClientStatusPacket(packet);
			session->send_packet(packet);
			iter->second->_BuildTotalHealthPacket(packet);
			session->send_packet(packet);
			iter->second->_BuildStatsPacket(packet);
			session->send_packet(packet);
		}
    }
}