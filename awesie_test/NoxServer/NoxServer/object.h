#pragma once

#include "global.h"
#include <set>

#include "NoxThinglib.h"

class object_mgr;
class player;

class object : boost::noncopyable
{
public:
	object(uint16 type);

	GNHT* get_object_info() 
	{ 
		return ThingBin::instance->Thing.Object.GetObject(m_type);
	}

	uint16 get_extent() { return m_extent; }
	uint16 get_type() { return m_type; }
	float get_position_x() 
	{ 
		if(!is_in_inventory())
			return m_position.y;
		else
			return 0;
	}
	float get_position_y() 
	{ 
		if(!is_in_inventory())
			return m_position.y;
		else
			return 0;
	}

	void set_position(float x, float y)
	{
		destroy_body();
		create_body(x, y);
	}

	bool is_in_inventory() { return m_body == NULL; }

	void use(player* plr);
protected:
	boost::posix_time::ptime m_start_time;
	uint16 m_extent;
	uint16 m_type;
	uint32 m_flags;
	uint16 m_props1;
	uint16 m_props2;
	uint32 m_worth;

	float m_max_health;
	float m_health;
	float m_delta_health;
	float m_combined_health;
    float m_max_mana;
    float m_mana;

	typedef std::set<object*> InventoryType;
	InventoryType m_inventory;

	// physics
	b2Body* m_body;
	b2Shape* m_shape;

	void create_body(float x, float y);
	void destroy_body();

	friend class object_mgr;
private:
	// there is no easy way to remember position for static objects
	b2Vec2 m_position;
};