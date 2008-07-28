#include "contact_filter.h"
#include "object.h"

bool contact_filter::ShouldCollide(b2Shape *shape1, b2Shape *shape2)
{
	object* o1 = (object*)shape1->GetUserData();
	object* o2 = (object*)shape2->GetUserData();

	bool ret = false;

	if(o1 != NULL)
		ret |= o1->should_collide(o2);
	if(o2 != NULL)
		ret |= o2->should_collide(o1);

	return ret;
}