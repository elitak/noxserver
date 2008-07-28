#pragma once

#include "global.h"

class contact_filter : public b2ContactFilter
{
public:
	bool ShouldCollide(b2Shape *shape1, b2Shape *shape2);
};