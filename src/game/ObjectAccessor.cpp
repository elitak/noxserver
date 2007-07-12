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

#include "Policies/SingletonImp.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "WorldSession.h"
#include "WorldPacket.h"
//#include "Opcodes.h"
//#include "ObjectDefines.h"

#include <cmath>

#define CLASS_LOCK MaNGOS::ClassLevelLockable<ObjectAccessor, ZThread::FastMutex>
INSTANTIATE_SINGLETON_2(ObjectAccessor, CLASS_LOCK);
INSTANTIATE_CLASS_MUTEX(ObjectAccessor, ZThread::FastMutex);

namespace MaNGOS
{

    struct MANGOS_DLL_DECL BuildUpdateForPlayer
    {
        Player &i_player;
        UpdateDataMapType &i_updatePlayers;

        BuildUpdateForPlayer(Player &player, UpdateDataMapType &data_map) : i_player(player), i_updatePlayers(data_map) {}

        void Visit(PlayerMapType &m)
        {
            for(PlayerMapType::iterator iter=m.begin(); iter != m.end(); ++iter)
            {
                if( iter->second == &i_player )
                    continue;

                UpdateDataMapType::iterator iter2 = i_updatePlayers.find(iter->second);
                if( iter2 == i_updatePlayers.end() )
                {
                    std::pair<UpdateDataMapType::iterator, bool> p = i_updatePlayers.insert( ObjectAccessor::UpdateDataValueType(iter->second, UpdateData()) );
                    assert(p.second);
                    iter2 = p.first;
                }

                //i_player.BuildValuesUpdateBlockForPlayer(&iter2->second, iter2->first);
            }
        }

        template<class SKIP> void Visit(std::map<OBJECT_HANDLE, SKIP *> &) {}
        template<class SKIP> void Visit(std::map<OBJECT_HANDLE, CountedPtr<SKIP> > &) {}
    };

}

ObjectAccessor::ObjectAccessor() {}
ObjectAccessor::~ObjectAccessor() {}

Player*
ObjectAccessor::FindPlayer(uint16 extent)
{
    PlayersMapType::iterator itr = i_players.find(extent);
    if (itr != i_players.end())
        return itr->second;
    return NULL;
}

Player*
ObjectAccessor::FindPlayerByName(const wchar_t *name)
{
    for(PlayersMapType::iterator iter=i_players.begin(); iter != i_players.end(); ++iter)
        if( ::wcscmp(name, iter->second->GetName()) == 0 )
            return iter->second;
    return NULL;
}

void
ObjectAccessor::InsertPlayer(Player *pl)
{
    i_players[pl->GetExtent()] = pl;
    _update();

	objmgr.AddObject(pl);
}

void
ObjectAccessor::RemovePlayer(Player *pl)
{
    Guard guard(i_playerGuard);
    PlayersMapType::iterator iter = i_players.find(pl->GetExtent());
    if( iter != i_players.end() )
        i_players.erase(iter);

    std::set<Object *>::iterator iter2 = std::find(i_objects.begin(), i_objects.end(), (Object *)pl);
    if( iter2 != i_objects.end() )
        i_objects.erase(iter2);

	objmgr.RemoveObject(pl);
}

void
ObjectAccessor::_update()
{
    //UpdateDataMapType update_players;
    {
        Guard guard(i_updateGuard);
		Guard guard2(i_playerGuard);

        for(std::set<Object *>::iterator iter=i_objects.begin(); iter != i_objects.end(); ++iter)
        {
            // check for valid pointer
            if (!*iter)
                continue;

			for(PlayersMapType::iterator iter2 =i_players.begin(); iter2 != i_players.end(); ++iter2)
			{
				if(iter2->second->CanSeePoint((*iter)->GetPositionX(), (*iter)->GetPositionY(), 0))
					iter2->second->AddUpdateObject(*iter);
			}
//            _buildUpdateObject(*iter, update_players);
//            (*iter)->ClearUpdateMask(false);
        }
        i_objects.clear();
    }

    /*WorldPacket packet; // here we allocate a std::vector with a size of 0x10000
    for(UpdateDataMapType::iterator iter = update_players.begin(); iter != update_players.end(); ++iter)
    {
        iter->second.BuildPacket(&packet);
        iter->first->GetSession()->SendPacket(&packet);
        packet.clear(); // clean the string
    }*/
}

void
ObjectAccessor::UpdateObject(Object* obj, Player* exceptPlayer)
{
    UpdateDataMapType update_players;
//    obj->BuildUpdate(update_players);

    WorldPacket packet;
    for(UpdateDataMapType::iterator iter = update_players.begin(); iter != update_players.end(); ++iter)
    {
        if(iter->first == exceptPlayer)
            continue;

        iter->second.BuildPacket(&packet);
        iter->first->GetSession()->SendPacket(&packet);
        packet.clear();
    }
}

void
ObjectAccessor::AddUpdateObject(Object *obj)
{
    Guard guard(i_updateGuard);
    i_objects.insert(obj);
}

void
ObjectAccessor::RemoveUpdateObject(Object *obj)
{
    Guard guard(i_updateGuard);
    std::set<Object *>::iterator iter = i_objects.find(obj);
    if( iter != i_objects.end() )
        i_objects.erase( iter );
}

void ObjectAccessor::AddObjectToRemoveList(WorldObject *obj)
{
    if(!obj) return;

    Guard guard(i_removeGuard);
    i_objectsToRemove.insert(obj);
    //sLog.outDebug("Object (GUID: %u TypeId: %u ) added to removing list.",obj->GetExtentLow(),obj->GetTypeId());
}

void ObjectAccessor::DoDelayedMovesAndRemoves()
{
//    MapManager::Instance().MoveAllCreaturesInMoveList();
    RemoveAllObjectsInRemoveList();
}

void ObjectAccessor::RemoveAllObjectsInRemoveList()
{
    if(i_objectsToRemove.empty())
        return;

    Guard guard(i_removeGuard);
    //sLog.outDebug("Object remover 1 check.");
    while(!i_objectsToRemove.empty())
    {
        WorldObject* obj = *i_objectsToRemove.begin();
        i_objectsToRemove.erase(i_objectsToRemove.begin());
        /*switch(obj->GetTypeId())
        {
            case TYPEID_CORPSE:
            {
                CorpsePtr corpse = MapManager::Instance().GetMap(obj->GetMapId(), obj)->GetObjectNear<Corpse>(*obj, obj->GetExtent());
                if (!corpse)
                {
                    sLog.outError("ERROR: Try delete corpse/bones %u that not in map", obj->GetExtentLow());
                }
                else
                {
                    MapManager::Instance().GetMap(obj->GetMapId(), obj)->Remove(corpse,true);
                }
                break;
            }
            case TYPEID_DYNAMICOBJECT:
                MapManager::Instance().GetMap(obj->GetMapId(), obj)->Remove((DynamicObject*)obj,true);
                break;
            case TYPEID_GAMEOBJECT:
                MapManager::Instance().GetMap(obj->GetMapId(), obj)->Remove((GameObject*)obj,true);
                break;
            case TYPEID_UNIT:
                MapManager::Instance().GetMap(obj->GetMapId(), obj)->Remove((Creature*)obj,true);
                break;
            default:
                sLog.outError("Non-grid object (TypeId: %u) in grid object removing list, ignored.",obj->GetTypeId());
                break;
        }*/
    }
    //sLog.outDebug("Object remover 2 check.");
}

void
ObjectAccessor::_buildUpdateObject(Object *obj, UpdateDataMapType &update_players)
{
    /*bool build_for_all = true;
    Player *pl = NULL;
    if( obj->isType(TYPE_ITEM ))
    {
        Item *item = static_cast<Item *>(obj);
        pl = item->GetOwner();
        build_for_all = false;
    }
    else if( obj->isType(TYPE_CONTAINER) )
    {
        Container *c = static_cast<Container *>(obj);
        assert( c != NULL );
        pl = c->GetOwner();
        build_for_all = false;
    }

    if( pl != NULL )
        _buildPacket(pl, obj, update_players);

    // Capt: okey for all those fools who think its a real fix
    //       THIS IS A TEMP FIX
    if( build_for_all )
    {
        WorldObject * temp = dynamic_cast<WorldObject*>(obj);

        //assert(dynamic_cast<WorldObject*>(obj)!=NULL);
        if (temp)
            _buildChangeObjectForPlayer(temp, update_players);
        else
            sLog.outDebug("ObjectAccessor: Ln 405 Temp bug fix");
    }*/
}

void
ObjectAccessor::_buildPacket(Player *pl, Object *obj, UpdateDataMapType &update_players)
{
    /*if(obj->isType(TYPE_UNIT) && !((Unit*)obj)->isVisibleFor(pl,false))
        return;

    UpdateDataMapType::iterator iter = update_players.find(pl);

    if( iter == update_players.end() )
    {
        std::pair<UpdateDataMapType::iterator, bool> p = update_players.insert( UpdateDataValueType(pl, UpdateData()) );
        assert(p.second);
        iter = p.first;
    }

    obj->BuildValuesUpdateBlockForPlayer(&iter->second, iter->first);*/
}

void
ObjectAccessor::_buildChangeObjectForPlayer(WorldObject *obj, UpdateDataMapType &update_players)
{
    /*CellPair p = MaNGOS::ComputeCellPair(obj->GetPositionX(), obj->GetPositionY());
    Cell cell = RedZone::GetZone(p);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();
    ObjectChangeAccumulator notifier(*obj, update_players);
    TypeContainerVisitor<ObjectChangeAccumulator, WorldTypeMapContainer > player_notifier(notifier);
    CellLock<GridReadGuard> cell_lock(cell, p);
    cell_lock->Visit(cell_lock, player_notifier, *MapManager::Instance().GetMap(obj->GetMapId(), obj));*/
}

void
ObjectAccessor::Update(const uint32  &diff)
{
	_update();
    {
        Guard guard(i_playerGuard);
        for(PlayersMapType::iterator iter=i_players.begin(); iter != i_players.end(); ++iter)
        {
            iter->second->Update(diff);
        }
    }
}

bool
ObjectAccessor::PlayersNearGrid(const uint32 &x, const uint32 &y) const
{
    /*CellPair cell_min(x*MAX_NUMBER_OF_CELLS, y*MAX_NUMBER_OF_CELLS);
    CellPair cell_max(cell_min.x_coord + MAX_NUMBER_OF_CELLS, cell_min.y_coord+MAX_NUMBER_OF_CELLS);
    cell_min << 2;
    cell_min -= 2;
    cell_max >> 2;
    cell_max += 2;

    Guard guard(const_cast<ObjectAccessor *>(this)->i_playerGuard);
    for(PlayersMapType::const_iterator iter=i_players.begin(); iter != i_players.end(); ++iter)
    {
        CellPair p = MaNGOS::ComputeCellPair(iter->second->GetPositionX(), iter->second->GetPositionY());
        if( (cell_min.x_coord <= p.x_coord && p.x_coord <= cell_max.x_coord) &&
            (cell_min.y_coord <= p.y_coord && p.y_coord <= cell_max.y_coord) )
            return true;
    }
	*/
    return false;
}

void
ObjectAccessor::ObjectChangeAccumulator::Visit(PlayerMapType &m)
{
    for(PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        ObjectAccessor::_buildPacket(iter->second, &i_object, i_updateDatas);
}


void
ObjectAccessor::SendPacketToAll(WorldPacket* packet)
{
	Guard guard(i_playerGuard);
	
	for(PlayersMapType::iterator iter = i_players.begin();iter != i_players.end();++iter)
		iter->second->GetSession()->SendPacket(packet);
}
void
ObjectAccessor::SendPacketToTeam(WorldPacket* packet, uint8 teamId)
{
	Guard guard(i_playerGuard);
	
	for(PlayersMapType::iterator iter = i_players.begin();iter != i_players.end();++iter)
		if(iter->second->GetTeam() == teamId)
			iter->second->GetSession()->SendPacket(packet);
}
void
ObjectAccessor::SendTextMessageToAll(wchar_t* message, uint16 x, uint16 y)
{
}