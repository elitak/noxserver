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

#ifndef _OBJECT_H
#define _OBJECT_H

#include "Common.h"
#include "Log.h"
#include "ByteBuffer.h"
#include "GridDefines.h"
#include "NoxThinglib.h"
#include "World.h"
#include "UpdateFields.h"
#include "UpdateData.h"
#include <set>

/// Collision
#include "ode/ode.h"


#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

#define OBJECT_CONTACT_DISTANCE 0.5
#define OBJECT_ITERACTION_DISTANCE 5

enum TYPE
{
    TYPE_OBJECT         = 1,
    TYPE_ITEM           = 2,
    TYPE_CONTAINER      = 6,
    TYPE_UNIT           = 8,
    TYPE_PLAYER         = 16,
    TYPE_GAMEOBJECT     = 32,
    TYPE_DYNAMICOBJECT  = 64,
    TYPE_CORPSE         = 128,
    TYPE_AIGROUP        = 256,
    TYPE_AREATRIGGER    = 512
};

enum TYPEID
{
    TYPEID_OBJECT        = 0,
    TYPEID_ITEM          = 1,
    TYPEID_CONTAINER     = 2,
    TYPEID_UNIT          = 3,
    TYPEID_PLAYER        = 4,
    TYPEID_GAMEOBJECT    = 5,
    TYPEID_DYNAMICOBJECT = 6,
    TYPEID_CORPSE        = 7,
    TYPEID_AIGROUP       = 8,
    TYPEID_AREATRIGGER   = 9
};

struct POSITION
{
	uint16 x_cor;
	uint16 y_cor;
};

class WorldObject;
class WorldPacket;
class UpdateData;
class ByteBuffer;
class WorldSession;
class Player;
class MapCell;
class UpdateMask;
class ObjectMgr;

typedef HM_NAMESPACE::hash_map<Player*, UpdateData> UpdateDataMapType;

class MANGOS_DLL_SPEC Object
{
	friend ObjectMgr;
    public:
		Object (uint16 type, GridPair pos, uint16 extent = 0);
        virtual ~Object ( );

        virtual void Update ( float time ) { }

		uint16 GetExtent() { return m_extent; }
		uint16 GetType() { return m_objectType; }
		GridPair GetPosition() 
		{ 
			Flatland::vec2 oldPos = body->GetGeometry().Center();
			return GridPair(oldPos.x, oldPos.y);
		}
		uint16 GetPositionX() { return body->GetGeometry().Center().x; }
		uint16 GetPositionY() { return body->GetGeometry().Center().y; }
		GNHT* GetObjectInfo() 
		{ 
			return sThingBin.Thing.Object.Objects.Get(GetType()-1); 
		}

		bool IsImmobile() { return (GetObjectInfo()->classes)&CLASS_IMMOBILE; }
		bool AddToInventory(WorldObject* obj);
		bool InMyInventory(WorldObject* obj)
		{
			return m_inventory.find(obj) != m_inventory.end();
		}

		virtual void RemoveFromInventory(WorldObject* obj, GridPair newPos);
		virtual bool Pickup(WorldObject* obj, uint32 max_dist = 0);
		virtual bool Drop(WorldObject* obj, uint32 max_dist, GridPair newPos);
		WorldObject* NewPickup(uint16 type, uint16 extent = 0, uint32 modifier = 0xFFFFFFFF);

		virtual void _BuildUpdatePacket(WorldPacket& packet);

		/// Collisions
		int joint;
		Flatland::Object* GetFlatlandObject() {return body;}

    protected:
        Object ( );

		uint16 m_extent;
		uint16 m_objectType;
		//GridPair m_position; /// this can only be initialized for static objects

		bool CanSeePoint(uint16 x, uint16 y, uint32 size);

		Flatland::Object* body;

    private:
        /*ByteBuffer m_PackGUID;

        // for output helpfull error messages from asserts
        bool PrintIndexError(uint32 index, bool set) const;*/

        Object(const Object&);                              // prevent generation copy constructor
        Object& operator=(Object const&);                   // prevent generation assigment operator

		typedef std::set<WorldObject*> InventoryType;
		InventoryType m_inventory;
};

class MANGOS_DLL_SPEC WorldObject : public Object
{
    public:
		WorldObject(uint16 type, GridPair pos, uint16 extent);
        virtual ~WorldObject ( ) {}

		void SetPosition(GridPair position);

		uint8 GetTeam() { return m_teamId; }
		void SetTeam(uint8 team) { m_teamId = team; }

		bool InAnInventory();
    protected:
        WorldObject( WorldObject *instantiator );
		WorldObject( ) {};

		virtual void SendUpdatePacket(WorldSession* session);

        /*std::string m_name;*/

    private:
        /*uint32 m_mapId;

        float m_positionX;
        float m_positionY;
        float m_positionZ;
        float m_orientation;

        bool mSemaphoreTeleport;

        uint32 m_InstanceId;*/
		uint8 m_teamId;
};
#endif
