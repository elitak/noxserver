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

#ifndef _OBJECTMGR_H
#define _OBJECTMGR_H

#include "Log.h"
#include "Database/DatabaseEnv.h"
#include "Object.h"
#include "ObjectDefines.h"
#include "NoxMap.h"
#include "Policies/Singleton.h"
#include "Database/SQLStorage.h"

class WorldSession;
class WorldPacket;

/// Player state
enum SessionStatus
{
    STATUS_AUTHED = 0,                                      ///< Player authenticated
	STATUS_CONFIRMED,
    STATUS_LOGGEDIN                                         ///< Player in game
};

struct OpcodeHandler
{
    OpcodeHandler() : status(STATUS_AUTHED), handler(NULL) {};
    OpcodeHandler( SessionStatus _status, void (WorldSession::*_handler)(WorldPacket& recvPacket) ) : status(_status), handler(_handler) {};

    SessionStatus status;
    void (WorldSession::*handler)(WorldPacket& recvPacket);
};

#define INVALID_EXTENT	0x0000
#define MAX_EXTENT		0xFFFF

typedef HM_NAMESPACE::hash_map< uint16 , OpcodeHandler > OpcodeTableMap;
typedef HM_NAMESPACE::hash_map< uint16 , Object* > ObjectTableMap;

class ObjectMgr
{
    public:
        ObjectMgr();
        ~ObjectMgr();

        ObjectTableMap::iterator
        Begin() { return objectTable.begin(); }
        ObjectTableMap::iterator
        End() { return objectTable.end(); }
        Object* GetObject(const uint16 extent)
		{
			ObjectTableMap::const_iterator itr =
				objectTable.find(extent);
			if(itr == objectTable.end() || itr->second->GetExtent() != extent)
				return NULL;
			else
				return itr->second;
		}
        void AddObject(Object *obj)
		{
			ASSERT(obj);
			if(!obj->GetExtent() || !RequestExtent(obj->GetExtent()))
				obj->m_extent = RequestExtent();
			objectTable[obj->GetExtent()] = obj;
		}
        bool RemoveObject(Object *obj)
		{
			ObjectTableMap::iterator i = objectTable.find(obj->GetExtent());
			if(i == objectTable.end())
				return false;
			ReturnExtent(obj->GetExtent());
			obj->m_extent = 0;
			objectTable.erase(i);
			return true;
		}
		bool ChangeObjectExtent(Object *obj, uint16 newExtent)
		{
			if(!IsExtentAvailable(newExtent))
				return false;
			RemoveObject(obj);
			obj->m_extent = newExtent;
			AddObject(obj);
			return true;
		}
		uint16 RequestExtent()
		{
			uint8* ptr = (uint8*)memchr(freeExtents, 0, 0xFFFF);
			if(ptr == NULL)
				return INVALID_EXTENT;
			
			uint16 extent = (uint16)((ptr - freeExtents + 1));
			if(RequestExtent(extent))
				return extent;
			else
				return INVALID_EXTENT;
		}

		bool RequestExtent(uint16 extent)
		{
			if(extent > MAX_EXTENT || !IsExtentAvailable(extent))
				return false;
			freeExtents[extent-1] = 1;
			return true;
		}

		void ReturnExtent(uint16 extent)
		{
			if(extent > 0 && extent <= MAX_EXTENT)
				freeExtents[extent-1] = 0;
		}

		bool IsExtentAvailable(uint16 extent)
		{
			if(extent == 0 || objectTable.find(extent) != objectTable.end())
				return false;
			return true;
		}

		std::vector<Object*> GetObjectsInRect(GridPair rightCorner, GridPair leftCorner);

		Object* CreateObjectFromFile(NoxBuffer* rdr, NoxObjectTOC* toc);

        OpcodeTableMap opcodeTable;
    protected:
        //template<class T> HM_NAMESPACE::hash_map<uint32,T*>& _GetContainer();
        //template<class T> TYPEID _GetTypeId() const;

		ObjectTableMap	objectTable;
		uint8			freeExtents[MAX_EXTENT];
    private:
};

#define objmgr MaNGOS::Singleton<ObjectMgr>::Instance()
#endif
