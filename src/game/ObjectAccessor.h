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

#ifndef MANGOS_OBJECTACCESSOR_H
#define MANGOS_OBJECTACCESSOR_H

#include "Platform/Define.h"
#include "Policies/Singleton.h"
#include "zthread/FastMutex.h"

#include "ByteBuffer.h"
#include "UpdateData.h"

#include "GridDefines.h"
#include "Object.h"

#include <set>

class Player;

class MANGOS_DLL_DECL ObjectAccessor : public MaNGOS::Singleton<ObjectAccessor, MaNGOS::ClassLevelLockable<ObjectAccessor, ZThread::FastMutex> >
{

    friend class MaNGOS::OperatorNew<ObjectAccessor>;
    ObjectAccessor();
    ~ObjectAccessor();
    ObjectAccessor(const ObjectAccessor &);
    ObjectAccessor& operator=(const ObjectAccessor &);

    public:

        typedef HM_NAMESPACE::hash_map<uint16, Player* > PlayersMapType;
        typedef HM_NAMESPACE::hash_map<Player*, UpdateData>::value_type UpdateDataValueType;

//        Player* GetPlayer(Unit const &, uint64 guid) { return FindPlayer(guid); }

        Player* FindPlayer(uint16);
        Player* FindPlayerByName(const wchar_t *name) ;

        PlayersMapType& GetPlayers() { return i_players; }
        void InsertPlayer(Player *);
        void RemovePlayer(Player *);

        void AddUpdateObject(Object *obj);
        void RemoveUpdateObject(Object *obj);

        void AddObjectToRemoveList(WorldObject   *obj);

        void DoDelayedMovesAndRemoves();

        void Update(const uint32 &diff);

        std::set<Player*> PlayersCanSeePoint(uint16 x, uint16 y, uint32 size);
		void SendPacketToAll(WorldPacket* packet);
		void SendTextMessageToAll(wchar_t* message, uint16 x, uint16 y);
		void SendPacketToTeam(WorldPacket* packet, uint8 teamId);
		void SendPlayerInfo(WorldSession* session);

        static void UpdateObject(Object* obj, Player* exceptPlayer);
        static void _buildUpdateObject(Object *, UpdateDataMapType &);
    private:
        void RemoveAllObjectsInRemoveList();

        struct ObjectChangeAccumulator
        {
            UpdateDataMapType &i_updateDatas;
            Object &i_object;
            ObjectChangeAccumulator(Object &obj, UpdateDataMapType &d) : i_updateDatas(d), i_object(obj) {}
            void Visit(PlayerMapType &);
            template<class SKIP> void Visit(std::map<OBJECT_HANDLE, SKIP *> &) {}
            template<class SKIP> void Visit(std::map<OBJECT_HANDLE, CountedPtr<SKIP> > &) {}
        };

        friend struct ObjectChangeAccumulator;
        PlayersMapType        i_players;

        typedef ZThread::FastMutex LockType;
        typedef MaNGOS::GeneralLock<LockType > Guard;

        static void _buildChangeObjectForPlayer(WorldObject *, UpdateDataMapType &);
        static void _buildPacket(Player *, Object *, UpdateDataMapType &);
        void _update(void);
        std::set<Object *> i_objects;
        std::set<WorldObject *> i_objectsToRemove;
        LockType i_playerGuard;
        LockType i_updateGuard;
        LockType i_removeGuard;
};

#define objacc ObjectAccessor::Instance()
#endif
