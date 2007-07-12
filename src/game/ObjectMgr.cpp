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
#include "ObjectMgr.h"
#include "NoxMap.h"
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

	uint16 type = rdr->read<uint16>();
	if(!type)
		return NULL;
	rdr->skip();
	sLog.outDebug((*toc)[type].c_str()); // get string object name

	Object* obj = new Object(0x50);
	size_t finish = rdr->read<int64>() + rdr->rpos();

	rdr->read<uint16>(); // properties
	uint16 type2 = rdr->read<uint16>(); // type2
	obj->m_extent = rdr->read<uint16>(); // extent
	rdr->read<uint16>();
	rdr->read<uint32>();
	
	obj->SetPosition(GridPair(rdr->read<float>(), rdr->read<float>())); // x, y are floats
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
			obj->m_inventory.push_back(child);
		else
			break;
	}

	AddObject(obj);
	return obj;
}