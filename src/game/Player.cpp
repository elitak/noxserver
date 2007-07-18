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
#include "Log.h"
#include "Opcodes.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "UpdateMask.h"
#include "Player.h"

#include "flatland/flatland.hpp"

Player::Player (WorldSession *session): Unit(0x2C9, GridPair(2285, 2500), 0)
{
	//body->setCollisionFlags(body->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	body->Property().collisionMask = 0xFFFFFFFF; //collide with everything
	body->Property().callback = PlayerCollideCallback;
	m_session = session;

	memset(&plrInfo, 0, sizeof(plrInfo));
	plrInfo.flag = 0x20;
	updateAll = false;
}

Player::~Player ()
{
}

void Player::SetName(const wchar_t* name)
{
	//memcpy(m_plrname, name, 0x30*sizeof(wchar_t));
}

void Player::Update(time_t time)
{
	uint32 mstime = getMSTime();
	if(updateAll)
	{
		std::vector<WorldObject*> v = objmgr.GetObjectsInRect(GridPair(GetPositionX() - 300, GetPositionY() - 300), GridPair(GetPositionX() + 300, GetPositionY() + 300));
		updateQueue.insert(v.begin(), v.end());

		updateAll = false;
	}
	else
		UpdateView();

	sLog.outDebug("UpdateViewTime: %u", getMSTime() - mstime);
	// TODO: health/mana regen, death stuff, etc.
	mstime = getMSTime();
	SendUpdatePacket();
	sLog.outDebug("PlayerSendTime: %u", getMSTime() - mstime);

	Unit::Update(time);
}

const uint8 rot_data[] = { 0x40, 0x70, 0x60, 0x50, 0x30, 0x00, 0x10, 0x20 }; 

void Player::_BuildUpdatePacket(WorldPacket& packet)
{
	packet.SetOpcode(MSG_UPDATE_STREAM);
	if(packet.size()) // If we are the first in the stream, then we can't/don't need this first byte
		packet << (uint8)0x00;  // This is going to be here until we figure a way to make efficient use of relative coords.
	packet << (uint8)0xFF;
	packet << (uint16)GetExtent();
	packet << (uint16)GetType();
	packet << (uint16)GetPositionX();
	packet << (uint16)GetPositionY();
	packet << (uint8)( rot_data[(uint8)floor((double)GetAngle() / 0x20 + 0.5) & 0x7] );
	packet << (uint8)m_action;
}

void Player::_BuildNewPlayerPacket(WorldPacket& packet)
{
	packet.Initialize(MSG_NEW_PLAYER, 0x0, 0, 0x80);
	packet << (uint16)GetExtent();
	packet.append((uint8*)(&plrInfo), 0x7E);
}
void Player::_BuildClientStatusPacket(WorldPacket& packet)
{
	packet.Initialize(MSG_REPORT_CLIENT_STATUS, 0x0, 0, 6);
	packet << (uint16)GetExtent();
	packet << (uint32)m_session->IsObserving();
}

void Player::SendUpdatePacket()
{
	WorldPacket packet(MSG_UPDATE_STREAM);
	// every object that starts stream must have the two bytes after the x,y
	/*packet << (uint8)0xFF;
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	packet << (uint16)3000;
	packet << (uint16)3000;
	packet << (uint8)0;
	packet << (uint8)0;*/

	updateQueue.erase(this);
	// Always update ourselves, we have to be first due to reasons above
	_BuildUpdatePacket(packet);

	for(UpdateQueueType::iterator iter = updateQueue.begin(); iter != updateQueue.end(); ++iter)
		if(*iter)
			(*iter)->_BuildUpdatePacket(packet);
	updateQueue.clear();

	// Three zeroes denotes end of MSG_UPDATE_STREAM
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	packet << (uint8)0x00;

	GetSession()->SendPacket(&packet);
}

void Player::Laugh()
{
     Unit::Laugh();
     WorldPacket packet(MSG_SOCIAL);
     //packet << (uint16)30;
     m_session->SendPacket(&packet);
}

void Player::Point()
{
     Unit::Point();
     WorldPacket packet(MSG_SOCIAL);
     //packet << (uint16)30;
     m_session->SendPacket(&packet);
}

void Player::Taunt()
{
     Unit::Taunt();
     WorldPacket packet(MSG_SOCIAL);
     //packet << (uint16)30;
     m_session->SendPacket(&packet);
}

bool Player::Equip(WorldObject* obj)
{
	if(Unit::Equip(obj))
	{
		WorldPacket packet(MSG_REPORT_EQUIP);
		packet << (uint16)obj->GetExtent();
		m_session->SendPacket(&packet);

		return true;
	}
	else
		return false;
}
bool Player::Dequip(WorldObject* obj)
{
	if(Unit::Dequip(obj))
	{
		WorldPacket packet(MSG_REPORT_DEQUIP);
		packet << (uint16)obj->GetExtent();
		m_session->SendPacket(&packet);

		return true;
	}
	else
		return false;
}
bool Player::Pickup(WorldObject* obj, uint32 max_dist)
{
	if(Unit::Pickup(obj, max_dist))
	{
		WorldPacket packet(MSG_REPORT_MODIFIABLE_PICKUP);
		packet << obj->GetExtent();
		packet << obj->GetType();
		packet << (uint32)0xFFFFFFFF;

		m_session->SendPacket(&packet);
		return true;
	}
	else
		return false;
}
bool Player::RemoveFromInventory(WorldObject* obj, GridPair newPos)
{
     if(InMyInventory(obj))
     {
          Dequip(obj);
          Unit::RemoveFromInventory(obj,newPos);

          WorldPacket packet(MSG_REPORT_DROP);
          packet << (uint16)obj->GetExtent();
          packet << (uint16) 0;

          m_session->SendPacket(&packet);
          return true;
     }
     return false;
}
void Player::ObjectOutOfSight(WorldObject* obj)
{
	updateQueue.erase(obj);

	WorldPacket packet(MSG_OBJECT_OUT_OF_SIGHT);
	packet << (uint16)obj->GetExtent();
	GetSession()->SendPacket(&packet);
}
void Player::ObjectDestroyed(WorldObject* obj)
{
	updateQueue.erase(obj);
	WorldPacket packet(MSG_DESTROY_OBJECT);
	packet << (uint16)obj->GetExtent();
	GetSession()->SendPacket(&packet);
}
void Player::EquipSecondary(WorldObject* obj)
{
	if(!obj && !InMyInventory(obj))
		return;
	WorldObject* newSecondary = obj ? obj : m_equipment[SLOT_WEP_PRIMARY];
	WorldObject* oldSecondary = m_equipment[SLOT_WEP_SECONDARY];
	m_equipment[SLOT_WEP_SECONDARY] = newSecondary;

	Dequip(newSecondary);

	WorldPacket packet(MSG_REPORT_SECONDARY_WEAPON);
	packet << (uint16)(newSecondary ? newSecondary->GetExtent() : 0);
	m_session->SendPacket(&packet);

	if(!obj) // NULL object (0 extent) means swap weps.
		Equip(oldSecondary);
}
void Player::PlayerCollideCallback(Flatland::ContactList &contacts)
{

}
void Player::UpdateView()
{
	GridPair pos = GetPosition();
	if(m_oldposition.x_coord && m_oldposition.y_coord)
	{
		int16 deltax = pos.x_coord - m_oldposition.x_coord;
		int16 deltay = pos.y_coord - m_oldposition.y_coord;
		if(deltax > 10 || deltay > 10)
		{
			if(deltax)
			{
				GridPair leftTop(deltax < 0 ? pos.x_coord - 300 : pos.x_coord - deltax + 300, pos.y_coord - 300);
				GridPair rightBottom(leftTop.x_coord + abs(deltax), pos.y_coord + 300);
				std::vector<WorldObject*> v = objmgr.GetObjectsInRect(leftTop, rightBottom);
				updateQueue.insert(v.begin(), v.end());
			}
			if(deltay)
			{
				GridPair leftTop(pos.x_coord - 300, deltay < 0 ? pos.y_coord - 300 : pos.y_coord - deltay + 300);
				GridPair rightBottom(pos.x_coord + 300, leftTop.y_coord + abs(deltay));
				std::vector<WorldObject*> v = objmgr.GetObjectsInRect(leftTop, rightBottom);
				updateQueue.insert(v.begin(), v.end());
			}
			m_oldposition = pos;
		}
	}
	else
		m_oldposition = pos;	
}