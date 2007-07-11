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
#include "Log.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Unit.h"
#include "Player.h"

#include <math.h>


Unit::Unit( WorldObject *instantiator ) : WorldObject( instantiator )
{
 //   m_objectType |= TYPE_UNIT;
 //   m_objectTypeId = TYPEID_UNIT;
 //   m_updateFlag = (UPDATEFLAG_HIGHGUID | UPDATEFLAG_ALL | UPDATEFLAG_LIVING | UPDATEFLAG_HASPOSITION);
	m_action = ACTION_IDLE;
	m_angle = 0;
}

Unit::~Unit()
{
    // remove references to unit
 /*   std::list<GameObject*>::iterator i;
    for (i = m_gameObj.begin(); i != m_gameObj.end();)
    {
        (*i)->SetOwnerGUID(0);
        (*i)->SetRespawnTime(0);
        (*i)->Delete();
        i = m_gameObj.erase(i);
    }*/
}

void Unit::Update( uint32 p_time )
{
    /*if(p_time > m_AurasCheck)
    {
    m_AurasCheck = 2000;
    _UpdateAura();
    }else
    m_AurasCheck -= p_time;*/
	if(m_action)
	{
		if((m_action_time - p_time) <= 0)
		{
			m_action = ACTION_IDLE;
		}
		else
			m_action_time -= p_time;
	}
}

void Unit::MoveToward(uint16 _x, uint16 _y)
{
	float x = (int32)_x - (int32)m_position.x_coord;
	float y = (int32)_y - (int32)m_position.y_coord;
	double len = sqrt(x*x+y*y);
	double unit_x = x/len;
	double unit_y = y/len;

	float SPEED;
	if(len > 10.0)
	{
		SPEED = 5.0;
		m_action = ACTION_RUN;
		m_action_time = 50;
	}
	else
	{
		SPEED = 2.5;
		m_action = ACTION_WALK;
		m_action_time = 50;
	}

	m_position >> unit_x*SPEED;
	m_position += unit_y*SPEED;
}