#include "global.h"
#include "world.h"

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