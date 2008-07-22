#pragma once

#include "global.h"

class cli : public boost::singleton<cli>
{
public:
	cli(boost::restricted);

	void run();
};