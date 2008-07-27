#pragma once

#include "global.h"

class contact_listener : public b2ContactListener
{
public:
	// physics stuff
	void Add(const b2ContactPoint* point);
	void Persist(const b2ContactPoint* point);
	void Remove(const b2ContactPoint* point);
    void Result(const b2ContactResult* point);
};