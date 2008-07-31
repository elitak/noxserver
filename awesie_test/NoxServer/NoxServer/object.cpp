#include "object.h"
#include "object_mgr.h"
#include "world.h"

#include <algorithm>

object::object(uint16 type) : m_type(type), m_shape(NULL), m_body(NULL), m_extent(0), m_flags(0), 
m_props1(0), m_props2(0), m_worth(0), m_max_health(0), m_health(0), m_max_mana(0), m_mana(0),
m_delta_health(0), m_combined_health(0), m_updated(true), m_parent(NULL)
{
	// for now, let's do a switch
	// later we may go back to the table syntax
	switch(get_object_info()->collide)
	{
	case COLLIDE_NONE:
		m_collide_handler = NULL;
		break;
	default:
	case COLLIDE_DEFAULT:
		m_collide_handler = boost::bind(&object::handle_default_collide, this, _1);
		break;
	}

	switch(get_object_info()->pickup)
	{
	case PICKUP_FOOD:
		m_pickup_handler = boost::bind(&object::handle_food_pickup, this, _1);
		break;
	case PICKUP_ARMOR:
		m_pickup_handler = boost::bind(&object::handle_armor_pickup, this, _1);
		break;
	case PICKUP_WEAPON:
		m_pickup_handler = boost::bind(&object::handle_weapon_pickup, this, _1);
		break;
	default:
		break;
	}
}

object::~object()
{
	destroy_body();
}

void object::use(player *plr)
{
}

void object::update(uint32 diff)
{
	if(m_updated)
		m_updated = false;
}

void object::create_body(float x, float y)
{
	// applies to static bodies only
	m_body = object_mgr::instance->get_static_body();
	x = x * SCALING_FACTOR;
	y = y * SCALING_FACTOR;

	bool is_sensor = false;
	if(get_object_info()->flags & (FLAG_ALLOW_OVERLAP | FLAG_NO_COLLIDE | FLAG_BELOW))
	{
		is_sensor = true;
	}
	
	switch(get_object_info()->extent.shape)
	{
	case EXTENT_BOX:
		{
		b2PolygonDef polygonDef;
		polygonDef.SetAsBox(get_object_info()->extent.x * SCALING_FACTOR, get_object_info()->extent.y * SCALING_FACTOR, b2Vec2(x, y), 0.25f * b2_pi);
		polygonDef.isSensor = is_sensor;

		cout << "NEW POLYGON (" << get_extent() << "): " << (int)get_object_info()->extent.x << ", " << (int)get_object_info()->extent.y << "; " << x << ", " << y << "\n";

		m_shape = m_body->CreateShape(&polygonDef);
		}
		break;
	case EXTENT_CIRCLE:
		{
		b2CircleDef circleDef;
		circleDef.radius = get_object_info()->extent.radius * SCALING_FACTOR;
		circleDef.localPosition.Set(x, y);	
		circleDef.isSensor = is_sensor;

		cout << "NEW CIRCLE (" << get_extent() << "): " << (int)get_object_info()->extent.radius << "; " << x << ", " << y << "\n";

		m_shape = m_body->CreateShape(&circleDef);
		}
		break;
	case EXTENT_CENTER:
	default:
		{
		b2CircleDef centerDef;
		centerDef.radius = 1 * SCALING_FACTOR;
		centerDef.localPosition.Set(x, y);
		centerDef.isSensor = is_sensor;

		m_shape = m_body->CreateShape(&centerDef);
		}
		break;
	}

	m_shape->SetUserData(this);

	m_position.Set(x, y);
}

void object::destroy_body()
{
	// applies to static bodies only
	if(m_shape)
		m_body->DestroyShape(m_shape);
	m_shape = NULL;
	m_body = NULL;
}

bool object::can_see_point(float x, float y, float* collision_x, float* collision_y)
{
	if(!m_body)
		return true;

	b2Segment segment;
	segment.p1 = m_body->GetPosition();
	segment.p2.Set(x*SCALING_FACTOR, y*SCALING_FACTOR);

	b2AABB aabb;
	aabb.lowerBound.Set(std::min(segment.p1.x, segment.p2.x) - 1, std::min(segment.p1.y, segment.p2.y) - 1);
	aabb.upperBound.Set(std::max(segment.p1.x, segment.p2.x) + 1, std::max(segment.p1.y, segment.p2.y) + 1);

	const int max_shapes = 512;
	b2Shape* shapes[512];
	int actual = world::instance->get_the_world().Query(aabb, shapes, max_shapes);

	// lambda [0, 1]->[p1, p2]
	float lambda = 1.0f;
	b2Vec2 normal(0, 0);
	bool collision = false;

	for(int i = 0; i < actual; i++)
	{
		object* o = (object*)shapes[i]->GetUserData();
		if(o == NULL || o->get_object_info()->subclass & CLASS_OBSTACLE)
		{
			// even if there is a collision, we have to continue testing other objects
			// in case one of the collides earlier (lambda)
			collision |= shapes[i]->TestSegment(shapes[i]->GetBody()->GetXForm(), &lambda, &normal, segment, lambda);
		}
	}

	*collision_x = ((1-lambda)*segment.p1.x + lambda*segment.p2.x) / SCALING_FACTOR;
	*collision_y = ((1-lambda)*segment.p1.y + lambda*segment.p2.y) / SCALING_FACTOR;
	return collision;
}
bool object::pickup(object *obj)
{
	if(obj->is_in_inventory())
		return false;

	m_inventory.insert(obj);
	obj->handle_pickup(this);

	return true;
}
bool object::drop(object *obj, float x, float y)
{
	InventoryType::iterator iter = m_inventory.find(obj);
	if(iter == m_inventory.end())
		return false;

	m_inventory.erase(iter);
	obj->set_position(x, y);
	obj->handle_drop(this);

	return true;
}
void object::handle_pickup(object *parent)
{
	m_parent = parent;
	this->destroy_body();

	if(m_pickup_handler != NULL)
		m_pickup_handler(parent, this);
}
void object::handle_drop(object *parent)
{
	m_parent = NULL;
}
// Should this object collide with the other object
// If other == null, then we hit a wall or something similar
bool object::should_collide(object* other)
{
	return m_collide_handler != NULL;
}

void object::handle_default_collide(object* other)
{

}

// maybe these should be called from the other object?
void object::handle_armor_pickup(object *parent)
{
	if(parent->is_unit())
	{
		unit* p = (unit*)parent;
		p->equip(this);
	}
}

void object::handle_food_pickup(object *parent)
{
	if(parent->get_health() < parent->get_max_health())
	{
		if(parent->is_unit())
			((unit*)parent)->use(this);
	}
}

void object::handle_weapon_pickup(object *parent)
{
	if(parent->is_unit())
	{
		unit* p = (unit*)parent;
		p->equip(this);
	}
}