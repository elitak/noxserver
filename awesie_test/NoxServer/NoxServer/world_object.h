#include "global.h"
#include "object.h"

class world_object : public object
{
public:
	world_object(uint16 extent) : object(extent) { }
};