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

Player::Player (WorldSession *session): Unit( 0 )
{
	//m_extent = 100;
	m_objectType = 0x2C9;
	m_position.x_coord = 3000;
	m_position.y_coord = 3000;
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
	if(updateAll)
	{
		std::vector<Object*> v = objmgr.GetObjectsInRect(GridPair(GetPositionX() - 300, GetPositionY() - 300), GridPair(GetPositionX() + 300, GetPositionY() + 300));
		updateQueue.insert(v.begin(), v.end());

		updateAll = false;
	}
	
	// TODO: health/mana regen, death stuff, etc.
	SendUpdatePacket();

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
	packet << (uint16)GetPosition().x_coord;
	packet << (uint16)GetPosition().y_coord;
	packet << (uint8)( rot_data[(uint8)floor((double)GetAngle() / 0x20 + 0.5) & 0x7] );
	packet << (uint8)m_action;
}

void Player::_BuildNewPlayerPacket(WorldPacket& packet)
{
	packet.Initialize(MSG_NEW_PLAYER, 0x0, 0, 0x80);
	packet << (uint16)GetExtent();
	packet.append((uint8*)(&plrInfo), 0x7E);
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

void Player::Move(int16 deltax, int16 deltay)
{
	Unit::Move(deltax, deltay);
	if(deltax)
	{
		GridPair leftTop(deltax < 0 ? GetPositionX() - 300 : GetPositionX() - deltax + 300, GetPositionY() - 300);
		GridPair rightBottom(leftTop.x_coord + abs(deltax), GetPositionY() + 300);
		std::vector<Object*> v = objmgr.GetObjectsInRect(leftTop, rightBottom);
		updateQueue.insert(v.begin(), v.end());
	}
	if(deltay)
	{
		GridPair leftTop(GetPositionX() - 300, deltay < 0 ? GetPositionY() - 300 : GetPositionY() - deltay + 300);
		GridPair rightBottom(GetPositionX() + 300, leftTop.y_coord + abs(deltay));
		std::vector<Object*> v = objmgr.GetObjectsInRect(leftTop, rightBottom);
		updateQueue.insert(v.begin(), v.end());
	}
}
void Player::SetPosition(GridPair position)
{
	Unit::SetPosition(position);

}
void Player::Pickup(Object* obj)
{
	AddToInventory(obj);

	WorldPacket packet(MSG_REPORT_PICKUP);
	packet << obj->GetExtent();
	packet << this->GetExtent();

	objacc.SendPacketToAll(&packet);
}
void Player::NewPickup(uint16 type, uint16 extent, uint32 modifier)
{
	Object* obj = new Object(type, extent);

	WorldPacket packet(MSG_REPORT_MODIFIABLE_PICKUP);
	packet << obj->GetExtent();
	packet << obj->GetType();
	packet << modifier;

	m_session->SendPacket(&packet);
}