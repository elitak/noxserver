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
#include "CollisionResponse.h"
#include "Database/DatabaseEnv.h"
#include "Object.h"
#include "ObjectDefines.h"
#include "NoxMap.h"
#include "Policies/Singleton.h"
#include "Database/SQLStorage.h"

/// Collision Library
#include "flatland/flatland.hpp"

class ObjectMgr;
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
struct CollideHandler
{
	CollideHandler() : mask(0xFFFFFFFF), handler(NULL) {};
	CollideHandler( Flatland::Callback _handler, uint32 _mask = 0xFFFFFFFF ) : mask(_mask), handler(_handler) {};

	uint32 mask;
	Flatland::Callback handler;
};
struct UpdateHandler
{
	UpdateHandler() : handler(NULL) {};
	UpdateHandler( void (Object::*_handler)(time_t diff) ) : handler(_handler) {};

	void (Object::*handler)(time_t diff);
};
struct UseHandler
{
	UseHandler() : handler(NULL) {};
	UseHandler( void (WorldObject::*_handler)(Player* plr) ) : handler(_handler) {};

	void (WorldObject::*handler)(Player* plr);
};
struct PickupHandler
{
	PickupHandler() : handler(NULL) {};
	PickupHandler( bool (WorldObject::*_handler)(Player* plr) ) : handler(_handler) {};

	bool (WorldObject::*handler)(Player* plr);
};
struct XferHandler
{
	XferHandler() : handler(NULL) {};
	XferHandler( int (ObjectMgr::*_handler)(Object* obj, NoxBuffer* rdr) ) : handler(_handler) {};

	int (ObjectMgr::*handler)(Object* obj, NoxBuffer* rdr);
};
struct EnchantEntry
{
	EnchantEntry() : name(NULL), frames(0), enchantsoundon(0), enchantsoundoff(0) {};
	EnchantEntry( char* _name, int16 _frames = 0, int8 _enchantsoundon = 0, int8 _enchantsoundoff = 0 ) : name(_name), frames(_frames), enchantsoundon(_enchantsoundon), enchantsoundoff(_enchantsoundoff) {};

	char* name;
	int16 frames;
	int8 enchantsoundon;
	int8 enchantsoundoff;
};

#define INVALID_EXTENT	0x0000
#define MAX_EXTENT		0x7FFF //extents are actually signed, lol

typedef HM_NAMESPACE::hash_map< uint16 , OpcodeHandler > OpcodeTableMap;
typedef HM_NAMESPACE::hash_map< uint16 , CollideHandler > CollideTableMap;
typedef HM_NAMESPACE::hash_map< uint16 , UpdateHandler > UpdateTableMap;
typedef HM_NAMESPACE::hash_map< uint16 , UseHandler > UseTableMap;
typedef HM_NAMESPACE::hash_map< uint16 , XferHandler > XferTableMap;
typedef HM_NAMESPACE::hash_map< uint16 , PickupHandler > PickupTableMap;
typedef HM_NAMESPACE::hash_map< uint32 , EnchantEntry > EnchantTableMap;
typedef std::set< Object* > ObjectTableMap;

class NoxWallLine : public Flatland::Line
{
public:
	NoxWallLine(Flatland::vec2 a, Flatland::vec2 b);
};

// This is here in order to add walls to the object list
class NoxWallObject : Object
{
	friend ObjectMgr;
public:
	void AddWall(CoordPair<0xFF> pos, uint8 facing);
	bool IsImmobile() { return true; }
protected:
	NoxWallObject();
	~NoxWallObject();

	Flatland::Composite* CreateWall(CoordPair<0xFF> pos, uint8 flags);

	Flatland::Composite* shape;
};

class ObjectMgr
{
	friend NoxWallMap;
    public:
        ObjectMgr();
        ~ObjectMgr();

        ObjectTableMap::iterator
        Begin() { return objectTable.begin(); }
        ObjectTableMap::iterator
        End() { return objectTable.end(); }
        WorldObject* GetObj(const uint16 extent)
		{
			Object* obj = GetImmobileObj(extent);
			if(obj && !obj->IsImmobile())
				return (WorldObject*)obj;
			else
				return NULL;
		}
       /* WorldObject* GetObjByType(const uint16 extent)
		{
			//this->objectTable.
			Object* obj = GetImmobileObj(extent);
			if(obj && !obj->IsImmobile())
				return (WorldObject*)obj;
			else
				return NULL;
		}*/
		Object* GetImmobileObj(const uint16 extent)
		{
			if(extent > MAX_EXTENT)
				return NULL;
			return objectExtents[extent];
		}
        void AddObject(Object *obj);
        bool RemoveObject(Object *obj);
		bool ChangeObjectExtent(WorldObject *obj, uint16 newExtent)
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
			uint32* ptr = (uint32*)objectExtents;
			do {
				ptr++;
			} while(ptr < (uint32*)(objectExtents+MAX_EXTENT+1) && *ptr != 0);
			if(ptr >= (uint32*)(objectExtents+MAX_EXTENT+1))
				return INVALID_EXTENT;
			
			uint16 extent = (uint16)((ptr - (uint32*)objectExtents));

			return extent;
		}

		bool IsExtentAvailable(uint16 extent)
		{
			if(extent == 0 || extent > MAX_EXTENT || objectExtents[extent])
				return false;
			return true;
		}

		bool ContainsObject(Object* obj)
		{
			return objectExtents[obj->GetExtent()] != NULL;
		}

		std::vector<WorldObject*> GetObjectsInRect(GridPair rightCorner, GridPair leftCorner);
		void Update(float diff);

		Object* CreateObjectFromFile(NoxBuffer* rdr, NoxObjectTOC* toc);
		Flatland::Body createBody() { return World.BodyCreate(); }

		void AddCollisionWalls() 
		{ 
			collisionWalls.shape->UpdateBounds();
			objectTable.insert(&collisionWalls);
		}

		NoxWallObject collisionWalls;
		OpcodeTableMap opcodeTable;
		CollideTableMap collideTable;
		UpdateTableMap updateTable;
		EnchantTableMap enchantTable;
		UseTableMap useTable;
          PickupTableMap pickupTable;
		XferTableMap xferTable;
    protected:
        //template<class T> HM_NAMESPACE::hash_map<uint32,T*>& _GetContainer();
        //template<class T> TYPEID _GetTypeId() const;

		ObjectTableMap	objectTable;
		Object*			objectExtents[MAX_EXTENT+1];
		std::set<Object*> playerList;

		Flatland::World World;


		/// XFER Handlers (used to create object from map file)
		int DefaultXferHandler(Object* obj, NoxBuffer* rdr);
		int GoldXferHandler(Object* obj, NoxBuffer* rdr);
		int ObeliskXferHandler(Object* obj, NoxBuffer* rdr);
		int ToxicCloudXferHandler(Object* obj, NoxBuffer* rdr);
		int RewardMarkerXferHandler(Object* obj, NoxBuffer* rdr);
		int AmmoXferHandler(Object* obj, NoxBuffer* rdr);
		int AbilityRewardXferHandler(Object* obj, NoxBuffer* rdr);
		int SpellPagePedestalXferHandler(Object* obj, NoxBuffer* rdr);
		int SpellRewardXferHandler(Object* obj, NoxBuffer* rdr);
		int FieldGuideXferHandler(Object* obj, NoxBuffer* rdr);
		int ReadableXferHandler(Object* obj, NoxBuffer* rdr);
		int ExitXferHandler(Object* obj, NoxBuffer* rdr);
		int DoorXferHandler(Object* obj, NoxBuffer* rdr);
		int TriggerXferHandler(Object* obj, NoxBuffer* rdr);
		int HoleXferHandler(Object* obj, NoxBuffer* rdr);
		int TransporterXferHandler(Object* obj, NoxBuffer* rdr);
		int ElevatorXferHandler(Object* obj, NoxBuffer* rdr);
		int ElevatorShaftXferHandler(Object* obj, NoxBuffer* rdr);
		int MoverXferHandler(Object* obj, NoxBuffer* rdr);
		int GlyphXferHandler(Object* obj, NoxBuffer* rdr);
		int InvisibleLightXferHandler(Object* obj, NoxBuffer* rdr);
		int SentryXferHandler(Object* obj, NoxBuffer* rdr);
		int WeaponXferHandler(Object* obj, NoxBuffer* rdr);
		int ArmorXferHandler(Object* obj, NoxBuffer* rdr);
		int TeamXferHandler(Object* obj, NoxBuffer* rdr);
    private:
};

#define objmgr MaNGOS::Singleton<ObjectMgr>::Instance()
#endif
