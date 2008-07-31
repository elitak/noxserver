#pragma once

#include "global.h"
#include <set>

#include "NoxThinglib.h"

class object_mgr;
class player;
class world_packet;

struct CollideHandler;

class object : boost::noncopyable
{
public:
	object(uint16 type);
	virtual ~object();

	GNHT* get_object_info() 
	{ 
		return ThingBin::instance->Thing.Object.GetObject(m_type - 1);
	}

	uint16 get_extent() { return m_extent; }
	uint16 get_type() { return m_type; }
	virtual float get_position_x() 
	{ 
		if(!is_in_inventory())
			return m_position.x / SCALING_FACTOR;
		else
			return 0;
	}
	virtual float get_position_y() 
	{ 
		if(!is_in_inventory())
			return m_position.y / SCALING_FACTOR;
		else
			return 0;
	}

	virtual void set_position(float x, float y)
	{
		destroy_body();
		create_body(x, y);
	}
	float get_health() { return m_health; }
	float get_max_health() { return m_max_health; }

	bool is_in_inventory() { return m_parent != NULL; }
	bool is_dead() { return m_health == 0; }
	bool is_dirty() { return m_updated; }
	virtual bool is_static() { return true; }
	virtual bool is_unit() { return false; }

	object* get_parent() { return m_parent; }

	// tests if this object can see pt using a ray cast
	// if it can't, then it sets collision to the point of collision
	bool can_see_point(float x, float y, float* collision_x, float* collision_y);

	virtual void update(uint32 diff);
	bool should_collide(object* other);

	void use(player* plr);
	virtual void _BuildUpdatePacket(world_packet& packet) {}
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

	// the object whose inventory we are in
	object* m_parent;

	// tells players when this object has moved, etc.
	bool m_updated;

	typedef std::set<object*> InventoryType;
	InventoryType m_inventory;

	// physics
	b2Body* m_body;
	b2Shape* m_shape;

	virtual bool pickup(object* obj);
	virtual bool drop(object* obj, float x, float y);
	void handle_pickup(object* parent);
	void handle_drop(object* parent);

	virtual void create_body(float x, float y);
	virtual void destroy_body();

	friend class object_mgr;
private:
	// there is no easy way to remember position for static objects
	b2Vec2 m_position;

	// function pointes would be faster, but i think we would lose OOP
	boost::function<void (object* me, object* other)> m_collide_handler;
	boost::function<void (object* me, object* parent)> m_pickup_handler;

	// collision handlers
	void handle_default_collide(object* other);
	void handle_pickup_collide(object* other);
	void handle_sign_collide(object* other);

	// pickup handlers
	void handle_food_pickup(object* parent);
	void handle_armor_pickup(object* parent);
	void handle_weapon_pickup(object* parent);
};