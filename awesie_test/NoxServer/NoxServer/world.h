#pragma once

#include "global.h"

class NoxMap;

class world : public boost::singleton<world>
{
public:
	world(boost::restricted);

	void run();
	void quit();

	bool is_quiting();
	NoxMap* get_map() { return m_map; }
private:
	bool m_is_quitting;

	boost::posix_time::ptime m_start_time;
	boost::posix_time::ptime m_current_time;

	NoxMap* m_map;

	// physics
	b2World* m_the_world;
	float32 m_time_step;
};