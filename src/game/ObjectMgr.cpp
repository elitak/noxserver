/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
 * Copyright (C) 2007 NoxForum.net <http://www.noxforum.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Database/SQLStorage.h"

#include "Log.h"
#include "WorldLog.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "NoxMap.h"
#include "NoxThinglib.h"
#include "UpdateMask.h"
#include "World.h"
#include "WorldSession.h"
#include "ProgressBar.h"
#include "Policies/SingletonImp.h"

/// Collisions
#include "flatland/flatland.hpp"

INSTANTIATE_SINGLETON_1(ObjectMgr);

ObjectMgr::ObjectMgr()
{
	memset(objectExtents, 0, MAX_EXTENT);
	World.SetContactMaxCorrectingVel(0.0f);

	collideTable[ COLLIDE_DAMAGE ]	= CollideHandler( &CollisionResponse::DamageCollideCallback );
}

ObjectMgr::~ObjectMgr()
{
}

Object* ObjectMgr::CreateObjectFromFile(NoxBuffer* rdr, NoxObjectTOC* toc)
{
	char buffer[256];
	uint8 inventory = 0;

	uint16 type = rdr->read<uint16>(), type2 = 0;
	if(toc->find(type) == toc->end())
		return NULL;
	rdr->skip();
	
	type = sThingBin.Thing.Object.GetIndex((*toc)[type].c_str(), (*toc)[type].size());
	ASSERT(type != 0);

	size_t finish = rdr->read<int64>() + rdr->rpos();

	rdr->read<uint16>(); // properties
	type2 = rdr->read<uint16>(); // type2
	uint16 extent = rdr->read<uint16>(); // extent
	rdr->read<uint16>();
	rdr->read<uint32>();

	float x = rdr->read<float>();
	float y = rdr->read<float>();
	
	sWorldLog.Log("\tX: %f Y: %f\n", x, y);

	if(rdr->read<uint8>() == 0xFF)
	{
		rdr->read<uint8>();
		rdr->read<uint8>();
		rdr->read<uint16>();
		rdr->readstring<uint8>(buffer, 256);
		rdr->read<uint8>();
		inventory = rdr->read<uint8>();
		for (int i = rdr->read<uint16>(); i > 0; i--)
			rdr->read<uint32>();
		rdr->read<uint16>();
		rdr->read<uint32>();
		if (type2 == 0x40)
		{
			rdr->readstring<uint32>(buffer, 256);
		}
		rdr->read<uint64>();
	}

	Object* obj = NULL;
	if(sThingBin.Thing.Object.Objects.Get(type)->classes & CLASS_IMMOBILE)
		obj = new Object(type, GridPair(x, y), extent);
	else
		obj = new WorldObject(type, GridPair(x, y), extent);

	if(rdr->rpos() < finish)
		rdr->rpos(finish); // ignore modifiers for now
	else
		ASSERT(rdr->rpos() == finish);
	
	for(;inventory > 0; inventory--)
	{
		WorldObject* child = (WorldObject*)CreateObjectFromFile(rdr, toc);
		if(child != NULL)
			obj->m_inventory.insert(child);
		else
			break;
	}

	return obj;
}

std::vector<WorldObject*> ObjectMgr::GetObjectsInRect(GridPair leftTop, GridPair rightBottom)
{
	std::vector<WorldObject*> v;
	for(ObjectTableMap::iterator iter = objectTable.begin(); iter != objectTable.end(); ++iter)
	{
		if(!(*iter)->IsImmobile() && (*iter)->GetPosition().in(leftTop, rightBottom))
			v.push_back((WorldObject*)*iter);
	}
	return v;
}

void ObjectMgr::AddObject(Object *obj)
{
	ASSERT(obj);
	if(GetObj(obj->GetExtent()) == obj)
		return;
	if(!obj->GetExtent() || !IsExtentAvailable(obj->GetExtent()))
		obj->m_extent = RequestExtent();
	objectExtents[obj->GetExtent()] = obj;
	objectTable.insert(obj);

	if(obj->GetType() == 0x2C9)
		playerList.insert(obj);
}
bool ObjectMgr::RemoveObject(Object *obj)
{
	ObjectTableMap::iterator i = objectTable.find(obj);
	if(i == objectTable.end())
		return false;
	objectTable.erase(i);
	if(obj->GetType() == 0x2C9)
		playerList.erase(obj);
	objectExtents[obj->GetExtent()] = 0;
	return true;
}

void ObjectMgr::Update(float diff)
{
	if(!objectTable.size())
		return;
	uint32 mstime = getMSTime();

	//Having our own list of dynamic players cuts time by ALOT
	World.GenerateContacts(objectTable, playerList);
	sLog.outDebug("ContactTime: %u", getMSTime() - mstime);
	World.QuickStep(diff);
	//ASSERT(!World.IsCorrupt(objectTable));
}

NoxWallObject::NoxWallObject() : Object()
{
	shape = new Flatland::Composite(Flatland::vec2(0.0f,0.0f));
	body = new Flatland::Static<Flatland::Composite>(shape);

	body->Property().bounceVelocity = 0.0f;
	body->Property().collisionMask = 0x00000001;


}
NoxWallObject::~NoxWallObject()
{
	delete body;
	delete shape;
}
void NoxWallObject::AddWall(CoordPair<255> pos, uint8 facing)
{
	switch(facing)
	{
		case WALL_NORTH:
			shape->push_back(CreateWall(pos, WALL_NORTH_FLAG | WALL_SOUTH_FLAG));
		break;
		case WALL_WEST:
			shape->push_back(CreateWall(pos, WALL_EAST_FLAG | WALL_WEST_FLAG));
		break;
		case WALL_SW_CORNER:
			shape->push_back(CreateWall(pos, WALL_WEST_FLAG | WALL_NORTH_FLAG));
		break;
		case WALL_NW_CORNER:
			shape->push_back(CreateWall(pos, WALL_NORTH_FLAG | WALL_EAST_FLAG));
		break;
		case WALL_NE_CORNER:
			shape->push_back(CreateWall(pos, WALL_SOUTH_FLAG | WALL_EAST_FLAG));
		break;
		case WALL_SE_CORNER:
			shape->push_back(CreateWall(pos, WALL_WEST_FLAG | WALL_SOUTH_FLAG));
		break;
		case WALL_SOUTH_T:
			shape->push_back(CreateWall(pos, WALL_WEST_FLAG | WALL_NORTH_FLAG | WALL_SOUTH_FLAG));
			break;
		case WALL_EAST_T:
			shape->push_back(CreateWall(pos, WALL_NORTH_FLAG | WALL_EAST_FLAG | WALL_WEST_FLAG));
			break;
		case WALL_NORTH_T:
			shape->push_back(CreateWall(pos, WALL_SOUTH_FLAG | WALL_EAST_FLAG | WALL_NORTH_FLAG));
			break;
		case WALL_WEST_T:
			shape->push_back(CreateWall(pos, WALL_WEST_FLAG | WALL_EAST_FLAG | WALL_SOUTH_FLAG));
			break;
		default:
			shape->push_back(CreateWall(pos, WALL_NORTH_FLAG | WALL_SOUTH_FLAG));
			break;
	}
}
Flatland::Composite* NoxWallObject::CreateWall(CoordPair<255> pos, uint8 flags)
{
	const float wallSize = 23.0;
	const float halfSize = 11.5;
	Flatland::vec2 center(pos.x_coord * wallSize + halfSize, pos.y_coord * wallSize + halfSize);
	Flatland::Composite* comp = new Flatland::Composite(center);
	NoxWallLine* line;
	if(flags & WALL_NORTH_FLAG)
	{
		line = new NoxWallLine(center, Flatland::vec2(center.x + halfSize, center.y - halfSize));
		comp->push_back(line);
	}
	if(flags & WALL_SOUTH_FLAG)
	{
		line = new NoxWallLine(center, Flatland::vec2(center.x - halfSize, center.y + halfSize));
		comp->push_back(line);
	}
	if(flags & WALL_EAST_FLAG)
	{
		line = new NoxWallLine(center, Flatland::vec2(center.x + halfSize, center.y + halfSize));
		comp->push_back(line);
	}
	if(flags & WALL_WEST_FLAG)
	{
		line = new NoxWallLine(center, Flatland::vec2(center.x - halfSize, center.y - halfSize));
		comp->push_back(line);
	}

	return comp;
}

NoxWallLine::NoxWallLine(Flatland::vec2 a, Flatland::vec2 b) : Flatland::Line(a, b)
{
	extent[1] = 5;
}