#pragma once

#include "global.h"

class master : public boost::singleton<master>
{
public:
	master(boost::restricted);

	void run();
};