#include "global.h"
#include "cli.h"
#include "world.h"

#include <iostream>
#include <string>

cli::cli(boost::restricted)
{
}

void cli::run()
{
	while(true)
	{
		std::string line;

		std::cin >> line;
		if(line == "quit")
			break;
	}

	// we decided to quit, so tell the world this
	world::instance->quit();
}