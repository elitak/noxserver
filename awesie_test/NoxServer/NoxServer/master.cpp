/*
*
*/

#include "global.h"
#include "master.h"
#include "world.h"
#include "cli.h"

master::master(boost::restricted)
{
}

void master::run()
{
	boost::thread* cli_thread;
	boost::thread* world_thread;

	// start our threads
	world_thread = new boost::thread( boost::bind( &world::run, ref(world::instance) ) );
	cli_thread = new boost::thread( boost::bind( &cli::run, ref(cli::instance) ) );

	// we are done, so let's just wait until the world ends
	world_thread->join();
}