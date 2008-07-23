#include "object.h"

object::object(uint16 type)
{
	m_type = type;
}

void object::use(player *plr)
{
}

void object::create_body(float x, float y)
{
	// applies to static bodies only
	m_body = object_mgr::instance->get_static_body();
	
	switch(get_object_info()->extent.shape)
	{
	case EXTENT_BOX:
		b2PolygonDef polygonDef;
		polygonDef.SetAsBox(get_object_info()->extent.x, get_object_info()->extent.y, b2Vec2(x, y), 0);

		m_shape = m_body->CreateShape(&polygonDef);
		break;
	case EXTENT_CIRCLE:
		b2CircleDef circleDef;
		circleDef.radius = get_object_info()->extent.radius;
		circleDef.localPosition.Set(x, y);	

		m_shape = m_body->CreateShape(&circleDef);
		break;
	case EXTENT_CENTER:
	default:
		b2CircleDef centerDef;
		circleDef.radius = 1;
		circleDef.localPosition.Set(x, y);

		m_shape = m_body->CreateShape(&centerDef);
		break;
	}

	m_position.Set(x, y);
}

void object::destroy_body()
{
	// applies to static bodies only
	m_body->DestroyShape(m_shape);
	m_shape = NULL;
	m_body = NULL;
}