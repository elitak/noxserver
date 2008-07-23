#include "global.h"
#include "object.h"

class world_object : public object
{
public:
	world_object(uint16 type);

	float get_position_x() 
	{ 
		if(m_body)
			return m_body->GetPosition().x; 
		else
			return 0;
	}
	float get_position_y() 
	{ 
		if(m_body)
			return m_body->GetPosition().y; 
		else
			return 0;
	}
protected:
	b2BodyDef* m_body_def;

	void create_body(float x, float y);
	void destroy_body();
};