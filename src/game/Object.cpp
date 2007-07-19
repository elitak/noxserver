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
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "Object.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "WorldSession.h"
#include "UpdateData.h"
#include "UpdateMask.h"
#include "Util.h"


/// Collisions
#include "flatland/flatland.hpp"

using namespace std;

Object::Object(uint16 type, GridPair pos, uint16 extent) : m_objectType(type), m_extent(extent), m_update_timer(0), m_health(0), m_max_health(0), m_delta_health(0), m_combined_health(0)
{
	Flatland::Geometry* g;
	switch(GetObjectInfo()->extent.shape)
	{
	case EXTENT_BOX:
		g = new Flatland::Block(Flatland::vec2(pos.x_coord, pos.y_coord), GetObjectInfo()->extent.x, GetObjectInfo()->extent.y);
		g->Rotate(45);
		break;
	case EXTENT_CIRCLE:
		g = new Flatland::Circle(Flatland::vec2(pos.x_coord, pos.y_coord), GetObjectInfo()->extent.radius);
		break;
	case EXTENT_CENTER:
	default:
		g = new Flatland::Circle(Flatland::vec2(pos.x_coord, pos.y_coord), 1);
		break;
	}
	if(GetType() != 0x2C9)
		body = new Flatland::Static<Flatland::Geometry>(g);
	else
		body = new Flatland::Dynamic<Flatland::Geometry>(g, objmgr.createBody());
	body->SetUserPointer(this);

	CollideTableMap::const_iterator iter = objmgr.collideTable.find(GetObjectInfo()->collide);
	if(iter != objmgr.collideTable.end())
	{
		body->Property().collisionMask = iter->second.mask;
		body->Property().callback = iter->second.handler;
	}

	if(GetObjectInfo()->flags & (FLAG_ALLOW_OVERLAP | FLAG_NO_COLLIDE | FLAG_BELOW))
	{
		body->Property().collisionMask = 0x00000000;
	}

	body->Property().bounceVelocity = 0.0f;
	objmgr.AddObject(this);

	if(GetObjectInfo()->update != UPDATE_NONE)
		objacc.AddUpdateObject(this);
}

Object::Object( )
{
 /*   m_objectTypeId      = TYPEID_OBJECT;
    m_objectType        = TYPE_OBJECT;

    m_uint32Values      = 0;
    m_uint32Values_mirror = 0;
    m_valuesCount       = 0;

    m_inWorld           = false;
    m_objectUpdated     = false;

    m_PackGUID.clear();
    _SetPackGUID(&m_PackGUID,0);*/
	m_extent = 0;
	m_objectType = 0;
}

Object::~Object( )
{
	objmgr.RemoveObject(this);
	objacc.RemoveUpdateObject(this);
}

bool Object::Pickup(WorldObject* obj, uint32 max_dist)
{
	if(!obj || obj->InAnInventory())
		return false;
	if(max_dist && GetPosition().distance(obj->GetPosition()) > max_dist)
		return false;
	return AddToInventory(obj);
}

bool Object::Drop(WorldObject* obj, uint32 max_dist, GridPair newPos)
{
     if(!obj || !InMyInventory(obj))
          return false;
     RemoveFromInventory(obj, newPos);
     return true;
}
WorldObject* Object::NewPickup(uint16 type, uint16 extent, uint32 modifier)
{
	WorldObject* obj = new WorldObject(type, GridPair(5880, 5880), extent);
	if(!Pickup(obj))
	{
		delete obj;
		obj = NULL;
	}
	return obj;
}
bool Object::CanSeePoint(uint16 x, uint16 y, uint32 size)
{
	GridPair m_position = GetPosition();
	return GridPair(x, y).in(GridPair(m_position.x_coord - 300, m_position.y_coord - 300), GridPair(m_position.x_coord + 300, m_position.y_coord + 300));
}

void Object::_BuildUpdatePacket(WorldPacket& packet)
{
	if(IsImmobile())
		return;
	packet.SetOpcode(MSG_UPDATE_STREAM);	
	packet << (uint8)0x00;  // This is going to be here until we figure a way to make efficient use of relative coords.
	packet << (uint8)0xFF;
	packet << (uint16)GetExtent();
	packet << (uint16)GetType();
	packet << (uint16)GetPosition().x_coord;
	packet << (uint16)GetPosition().y_coord;
}
bool Object::RemoveFromInventory(WorldObject* obj, GridPair newPos)
{
	obj->SetPosition(newPos);
	//objmgr.AddObject(obj);
	return m_inventory.erase(obj);
}

bool Object::AddToInventory(WorldObject* obj) 
{
	if(!obj || obj->InAnInventory())
		return false;
	obj->SetPosition(GridPair(5880, 5880));
	//objmgr.RemoveObject(obj, false);
	m_inventory.insert(obj);
	return true;
}

void Object::Update(uint32 time)
{
	UpdateTableMap::iterator iter = objmgr.updateTable.find(GetObjectInfo()->update);
	if(iter != objmgr.updateTable.end())
		(this->*iter->second.handler)(time);
}

void Object::Die()
{
	m_health = 0;
}
void Object::Damage(float damage)
{
	if(!IsDead())
		m_delta_health -= damage;
}
void Object::Heal(float heal)
{
	if(!IsDead())
		m_delta_health += heal;
}
void Object::_BuildHealthPacket(WorldPacket &packet)
{
	packet.SetOpcode(MSG_REPORT_ITEM_HEALTH);
	packet << GetExtent();
	packet << (uint16)m_health;
	packet << (uint16)m_max_health;
}
void Object::_BuildDeltaHealthPacket(WorldPacket &packet)
{
	if(((uint16)m_combined_health) > 0)
	{
		packet.SetOpcode(MSG_REPORT_HEALTH_DELTA);
		packet << GetExtent();
		packet << (uint16)m_combined_health;
	}
}
WorldObject::WorldObject(uint16 type, GridPair pos, uint16 extent) : Object(type, pos, extent), m_updateMask(0)
{
}

WorldObject::WorldObject( WorldObject *instantiator )
{
  /*  m_positionX         = 0.0f;
    m_positionY         = 0.0f;
    m_positionZ         = 0.0f;
    m_orientation       = 0.0f;

    m_mapId             = 0;
    m_InstanceId        = 0;

    m_name = "";

    mSemaphoreTeleport  = false;

    if (instantiator)
    {
        m_InstanceId = instantiator->GetInstanceId();
    }*/
}
void WorldObject::SendUpdatePacket(WorldSession* session)
{
	WorldPacket packet(MSG_UPDATE_STREAM);
	_BuildUpdatePacket(packet);
	packet << (uint8)0x00;
	session->SendPacket(&packet);
}

void WorldObject::SetPosition(GridPair position)
{	
	GridPair oldPos = GetPosition();
	std::set<Player*> oldPlayers = objacc.PlayersCanSeePoint(oldPos.x_coord, oldPos.y_coord, 1);
	std::set<Player*> newPlayers = objacc.PlayersCanSeePoint(position.x_coord, position.y_coord, 1);

	body->GetGeometry().SetCenter(Flatland::vec2(position.x_coord, position.y_coord));

	if(!InAnInventory())
	{
		for(std::set<Player*>::iterator iter = newPlayers.begin(); iter != newPlayers.end(); ++iter)
		{
			if(oldPlayers.find(*iter) != oldPlayers.end())
				oldPlayers.erase(*iter);
			(*iter)->AddUpdateObject(this);
		}
	}
	for(std::set<Player*>::iterator iter = oldPlayers.begin(); iter != oldPlayers.end(); ++iter)
		(*iter)->ObjectOutOfSight(this);
}

bool WorldObject::InAnInventory()
{
	return GetPosition() == GridPair(5880, 5880);
	//return !objmgr.ContainsObject(this);
}


void WorldObject::Use(Player* plr)
{
	UseTableMap::iterator iter = objmgr.useTable.find(GetObjectInfo()->use);
	if(iter != objmgr.useTable.end())
		(this->*iter->second.handler)(plr);
	
}
/// Use Handlers
void WorldObject::PotionUse(Player *plr)
{
	int amt = atoi(GetObjectInfo()->use_args[0]);
	plr->Heal( amt );
	plr->RemoveFromInventory(this, GridPair(5880, 5880));
	objacc.AddObjectToRemoveList(this);
}
void WorldObject::ConsumeUse(Player *plr)
{
     int amt = atoi(GetObjectInfo()->use_args[0]);
     plr->Heal( amt );
     plr->RemoveFromInventory(this, GridPair(5880, 5880));
     objacc.AddObjectToRemoveList(this);
}
void WorldObject::ConsumeConfuseUse(Player *plr)
{
     int amt = atoi(GetObjectInfo()->use_args[0]);
     plr->SetEnchant(ENCHANT_CONFUSED,90);
     plr->Heal( amt );
     plr->RemoveFromInventory(this, GridPair(5880, 5880));
     objacc.AddObjectToRemoveList(this);
}
void WorldObject::MushroomUse(Player *plr)
{
     plr->Poison( 0 );
     plr->SetEnchant(ENCHANT_CONFUSED,90);
     plr->RemoveFromInventory(this, GridPair(5880, 5880));
     objacc.AddObjectToRemoveList(this);
}
/// Update Handlers
void Object::OneSecondDieUpdate(time_t diff)
{
	if(!m_update_timer)
		m_update_timer = 1000;
	if(m_update_timer > diff)
		m_update_timer -= diff;
	else
		Die();
}