#include "global.h"
#include "object.h"

class world_packet;

class world_object : public object
{
public:
	world_object(uint16 type);

	virtual float get_position_x() 
	{ 
		if(m_body)
			return m_body->GetPosition().x / SCALING_FACTOR; 
		else
			return 0;
	}
	virtual float get_position_y() 
	{ 
		if(m_body)
			return m_body->GetPosition().y / SCALING_FACTOR; 
		else
			return 0;
	}
	virtual void set_position(float x, float y)
	{
		b2Vec2 pos(x, y);
		if(m_body)
			m_body->SetXForm(pos, 0);
		else
			create_body(x, y);
	}

	
	virtual bool is_static() { return false; }
	virtual void _BuildUpdatePacket(world_packet& packet);
	
protected:

	b2BodyDef* m_body_def;

	virtual void create_body(float x, float y);
	virtual void destroy_body();
};