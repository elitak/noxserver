#include "world_object.h"
#include "world.h"
#include "world_packet.h"

world_object::world_object(uint16 type) : object(type)
{
	m_body_def = new b2BodyDef();

	// a mass of 0 means that object is static
	// if the mass of the object is >999999 that is close
	// enough to static i think
	m_body_def->massData.mass = get_object_info()->mass > 99999 ? 0 : get_object_info()->mass;
	m_body_def->linearDamping = 1.0f;
}

void world_object::create_body(float x, float y)
{
	x = x * SCALING_FACTOR;
	y = y * SCALING_FACTOR;

	// non-static objects only
	m_body_def->position.Set(x, y);
	m_body_def->angle = 0.25 * b2_pi;
	m_body = world::instance->get_the_world().CreateBody(m_body_def);

	b2PolygonDef polygonDef;
	b2CircleDef circleDef;
	b2CircleDef centerDef;
	b2ShapeDef* shapeDef;

	switch(get_object_info()->extent.shape)
	{
	case EXTENT_BOX:
		polygonDef.SetAsBox(get_object_info()->extent.x * SCALING_FACTOR, get_object_info()->extent.y * SCALING_FACTOR);
		shapeDef = &polygonDef;
		break;
	case EXTENT_CIRCLE:
		circleDef.radius = get_object_info()->extent.radius * SCALING_FACTOR;
		circleDef.localPosition.SetZero();
		shapeDef = &circleDef;
		break;
	case EXTENT_CENTER:
	default:
		circleDef.radius = 1 * SCALING_FACTOR;
		shapeDef = &centerDef;
		break;
	}

	shapeDef->filter.categoryBits = 0x0002;
	if(get_object_info()->flags & (FLAG_ALLOW_OVERLAP | FLAG_NO_COLLIDE | FLAG_BELOW))
	{
		shapeDef->filter.maskBits = 0x0000;
	}
	else
		shapeDef->filter.maskBits = 0xFFFF;

	shapeDef->userData = this;
	shapeDef->restitution = 0;
	shapeDef->friction = 1;
	m_body->CreateShape(shapeDef);
}

void world_object::destroy_body()
{
	// non-static objects only
	if(m_body)
		world::instance->get_the_world().DestroyBody(m_body);
	m_body = NULL;
	m_shape = NULL;
}
void world_object::_BuildUpdatePacket(world_packet& packet)
{
	packet.set_opcode(MSG_UPDATE_STREAM);	
	packet << (uint8)0x00;  // This is going to be here until we figure a way to make efficient use of relative coords.
	packet << (uint8)0xFF;
	packet << (uint16)get_extent();
	packet << (uint16)get_type();
	packet << (uint16)get_position_x();
	packet << (uint16)get_position_y();
}