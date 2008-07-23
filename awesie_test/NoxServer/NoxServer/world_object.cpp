#include "global.h"
#include "world_object.h"

world_object::world_object(uint16 type) : object(type)
{
	m_body_def = new b2BodyDef();
	m_body_def->massData.mass = get_object_info()->Properties.
}

void world_object::create_body(float x, float y)
{
	// non-static objects only
	m_body_def->position.Set(x, y);
	m_body = world::instance->get_the_world().CreateBody(m_body_def);

	switch(get_object_info()->extent.shape)
	{
	case EXTENT_BOX:
		b2PolygonDef polygonDef;
		polygonDef.SetAsBox(get_object_info()->extent.x, get_object_info()->extent.y);

		m_shape = m_body->CreateShape(&polygonDef);
		break;
	case EXTENT_CIRCLE:
		b2CircleDef circleDef;
		circleDef.radius = get_object_info()->extent.radius;

		m_shape = m_body->CreateShape(&circleDef);
		break;
	case EXTENT_CENTER:
	default:
		b2CircleDef centerDef;
		circleDef.radius = 1;

		m_shape = m_body->CreateShape(&centerDef);
		break;
	}
}

void world_object::destroy_body()
{
	// non-static objects only
	world::instance->get_the_world().DestoryBody(m_body);
	m_body = m_shape = NULL;
}