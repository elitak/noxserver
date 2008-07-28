#include "object.h"
#include "object_mgr.h"

object::object(uint16 type) : m_type(type), m_shape(NULL), m_body(NULL), m_extent(0), m_flags(0), 
m_props1(0), m_props2(0), m_worth(0), m_max_health(0), m_health(0), m_max_mana(0), m_mana(0),
m_delta_health(0), m_combined_health(0), m_updated(true)
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

// Should this object collide with the other object
// If other == null, then we hit a wall or something similar
bool object::should_collide(object* other)
{
	return m_collide_handler != NULL;
}

void object::handle_default_collide(object* other)
{

}