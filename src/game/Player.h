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

#ifndef _PLAYER_H
#define _PLAYER_H

#include "Common.h"

#include "Database/DatabaseEnv.h"
#include "Unit.h"
#include "WorldSession.h"

///Collision
#include "flatland/flatland.hpp"

class DynamicObject;
class UpdateMask;

struct PlayerInfo //size = 0x7E
{
	PlayerInfo()
	{
		unk0 = 0;
		unk2 = 0;
		unk4 = 0;
		flag = 0x10;
	}
	wchar_t name[0x21]; //0x19
	uint8  pclass;
	uint8 unk3;
	uint8 info[0x1D];
	uint8 unk0;
	uint8 unk1[0x0C];
	uint8 unk4;
	uint32 unk2;
	char username[9];
	uint8 flag;
};

enum PlayerClassType
{
	PLAYER_CLASS_WARRIOR	= 0x0,
	PLAYER_CLASS_WIZARD		= 0x1,
	PLAYER_CLASS_CONJURER	= 0x2
};

enum PlayerStateType
{
    /*
        PLAYER_STATE_DANCE
        PLAYER_STATE_SLEEP
        PLAYER_STATE_SIT
        PLAYER_STATE_STAND
        PLAYER_STATE_READYUNARMED
        PLAYER_STATE_WORK
        PLAYER_STATE_POINT(DNR)
        PLAYER_STATE_NONE // not used or just no state, just standing there?
        PLAYER_STATE_STUN
        PLAYER_STATE_DEAD
        PLAYER_STATE_KNEEL
        PLAYER_STATE_USESTANDING
        PLAYER_STATE_STUN_NOSHEATHE
        PLAYER_STATE_USESTANDING_NOSHEATHE
        PLAYER_STATE_WORK_NOSHEATHE
        PLAYER_STATE_SPELLPRECAST
        PLAYER_STATE_READYRIFLE
        PLAYER_STATE_WORK_NOSHEATHE_MINING
        PLAYER_STATE_WORK_NOSHEATHE_CHOPWOOD
        PLAYER_STATE_AT_EASE
        PLAYER_STATE_READY1H
        PLAYER_STATE_SPELLKNEELSTART
        PLAYER_STATE_SUBMERGED
    */

    PLAYER_STATE_NONE             = 0,
    PLAYER_STATE_SIT              = 1,
    PLAYER_STATE_SIT_CHAIR        = 2,
    PLAYER_STATE_SLEEP            = 3,
    PLAYER_STATE_SIT_LOW_CHAIR    = 4,
    PLAYER_STATE_SIT_MEDIUM_CHAIR = 5,
    PLAYER_STATE_SIT_HIGH_CHAIR   = 6,
    PLAYER_STATE_DEAD             = 7,
    PLAYER_STATE_KNEEL            = 8,

    PLAYER_STATE_FORM_ALL          = 0x00FF0000,

    PLAYER_STATE_FLAG_ALWAYS_STAND = 0x01000000,
    PLAYER_STATE_FLAG_STEALTH      = 0x02000000,
    PLAYER_STATE_FLAG_ALL          = 0xFF000000,
};

typedef std::set<WorldObject*> UpdateQueueType;

class MANGOS_DLL_SPEC Player : public Unit
{
    friend class WorldSession;
	friend class ObjectAccessor;

    public:
        explicit Player (WorldSession *session);
        ~Player ( );
		wchar_t* const GetName() { return plrInfo.name; }
		void SetName( const wchar_t* name );
		WorldSession* GetSession() { return m_session; }

		virtual void Update(uint32 time);

		void AddUpdateObject(WorldObject* obj) { updateQueue.insert(obj); }
		void RemoveUpdateObject(WorldObject* obj) { }

		void RunTowards( uint16 x, uint16 y);
		void WalkTowards( uint16 x, uint16 y);
		void MoveTowards( uint16 x, uint16 y);
		void Laugh();
		void Point();
		void Taunt();

		void SetPosition(GridPair position);
		void ForceUpdateAll() { updateAll = true; };
		void UpdateView();

		bool Equip(WorldObject* obj);
		void EquipSecondary(WorldObject* obj);
		bool Dequip(WorldObject* obj);
		bool Pickup(WorldObject* obj, uint32 max_dist = 0);
		bool RemoveFromInventory(WorldObject* obj, GridPair newPos);
		void ObjectOutOfSight(WorldObject* obj);
		void ObjectDestroyed(WorldObject* obj);

          virtual void Poison( byte poisoned );
		virtual void _BuildHealthPacket(WorldPacket& packet);

		static void PlayerCollideCallback(Flatland::ContactList &contacts);
    protected:
		PlayerInfo plrInfo;
		WorldSession* m_session;
		
		bool updateAll;
		GridPair m_oldposition;
		
		UpdateQueueType updateQueue;

		virtual void SendUpdatePacket();
		virtual void _BuildUpdatePacket(WorldPacket& packet);
		virtual void _BuildNewPlayerPacket(WorldPacket& packet);
		virtual void _BuildClientStatusPacket(WorldPacket& packet);
		virtual void _BuildMyHealthPacket(WorldPacket& packet);
		virtual void _BuildTotalHealthPacket(WorldPacket& packet);
		virtual void _BuildStatsPacket(WorldPacket& packet);
};
#endif
