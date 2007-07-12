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
#include "ObjectMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "UpdateMask.h"
#include "Player.h"

Player::Player (WorldSession *session): Unit( 0 )
{
	//m_extent = 1000;
	m_objectType = 0x2C9;
	m_position.x_coord = 3000;
	m_position.y_coord = 3000;
	m_session = session;
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
	// TODO: health/mana regen, death stuff, etc.
	SendUpdatePacket();

	Unit::Update(time);
}

const uint8 rot_data[] = { 0x40, 0x70, 0x60, 0x50, 0x30, 0x00, 0x10, 0x20 }; 

void Player::_BuildUpdatePacket(WorldPacket& packet)
{
	packet.SetOpcode(MSG_UPDATE_STREAM);
	packet << (uint8)0xFF;
	packet << (uint16)GetExtent();
	packet << (uint16)GetType();
	packet << (uint16)GetPosition().x_coord;
	packet << (uint16)GetPosition().y_coord;
	packet << (uint8)( rot_data[(uint8)floor((double)GetAngle() / 0x20 + 0.5) & 0x7] );
	packet << (uint8)m_action;
	packet << (uint8)0x00;
	packet << (uint8)0x00;  // This is going to be here until we figure a way to make efficient use of relative coords.
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
	
	_BuildUpdatePacket(packet);
	while(!updateQueue.empty())
	{
		updateQueue.front()->_BuildUpdatePacket(packet);
		updateQueue.pop();
	}

	GetSession()->SendPacket(&packet);
}

bool Player::CanSeePoint(uint16 x, uint16 y, uint32 size)
{
	int newx = x - m_position.x_coord;
	int newy = y - m_position.y_coord;
	int len = newy*newy + newx*newx - size*size;
	return (len < 10000);
}