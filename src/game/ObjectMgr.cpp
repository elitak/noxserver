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
#include "ObjectMgr.h"
#include "NoxMap.h"
#include "NoxThinglib.h"
#include "UpdateMask.h"
#include "World.h"
#include "WorldSession.h"
#include "ProgressBar.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1(ObjectMgr);

ObjectMgr::ObjectMgr()
{
	memset(freeExtents, 0, MAX_EXTENT);
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
	
	type2 = sThingBin.Thing.Object.GetIndex((*toc)[type].c_str(), (*toc)[type].size());
	sWorldLog.Log("Object: %s %s (0x%.4X)",(*toc)[type].c_str(), sThingBin.Thing.Object.Objects.Get(type2-1)->Name, type2); // get string object name
	ASSERT(type2 != 0);

	Object* obj = new Object(type2);
	size_t finish = rdr->read<int64>() + rdr->rpos();

	rdr->read<uint16>(); // properties
	type2 = rdr->read<uint16>(); // type2
	ChangeObjectExtent(obj, rdr->read<uint16>()); // extent
	rdr->read<uint16>();
	rdr->read<uint32>();

	float x = rdr->read<float>();
	float y = rdr->read<float>();
	
	sWorldLog.Log("\tX: %f Y: %f\n", x, y);

	obj->SetPosition(GridPair(x, y)); // x, y are floats
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

	if(rdr->rpos() < finish)
		rdr->rpos(finish); // ignore modifiers for now
	else
		ASSERT(rdr->rpos() == finish);
	
	for(;inventory > 0; inventory--)
	{
		Object* child = CreateObjectFromFile(rdr, toc);
		if(child != NULL)
			obj->m_inventory.insert(child);
		else
			break;
	}

	return obj;
}

std::vector<Object*> ObjectMgr::GetObjectsInRect(GridPair leftTop, GridPair rightBottom)
{
	std::vector<Object*> v;
	for(ObjectTableMap::iterator iter = objectTable.begin(); iter != objectTable.end(); ++iter)
	{
		if(iter->second->GetPosition().in(leftTop, rightBottom))
			v.push_back(iter->second);
	}
	return v;
}