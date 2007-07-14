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
#include "ObjectAccessor.h"
#include "Opcodes.h"
#include "NoxThinglib.h"
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

	memset(m_equipment, 0, SLOT_SIZE * sizeof(Object*));
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
	float x = (int32)_x - (int32)GetPositionX();
	float y = (int32)_y - (int32)GetPositionY();
	double len = sqrt(x*x+y*y);
	double unit_x = x/len;
	double unit_y = y/len;

	float SPEED;
	if(len > 30.0)
	{
		SPEED = 10.0;
		m_action = ACTION_RUN;
		m_action_time = 50;
	}
	else
	{
		SPEED = 5.0;
		m_action = ACTION_WALK;
		m_action_time = 50;
	}

	Move(unit_x*SPEED, unit_y*SPEED);
}

void Unit::Move(int16 deltax, int16 deltay)
{
	Object::SetPosition(GridPair(m_position.x_coord+deltax, m_position.y_coord+deltay));
}

void Unit::Laugh()
{
     m_action = ACTION_LAUGH;
     m_action_time = 1000;
}
void Unit::Point()
{
     m_action = ACTION_POINT;
     m_action_time = 1000;
}

void Unit::Taunt()
{
     m_action = ACTION_TAUNT;
     m_action_time = 1000;
}

bool Unit::Equip(Object *obj)
{
	if(!obj)
		return false;
	uint32 slot = obj->GetObjectInfo()->subclass;
	if(slot & SUBCLASS_WEAPON) //this can only be true for weapons
	{
		if(slot == SUBCLASS_QUIVER)
			slot = SLOT_QUIVER;
		else if(slot == SUBCLASS_FLAG)
			slot = SLOT_FLAG;
		else
			slot = SLOT_WEP_PRIMARY;
	}
	if(!Equip(obj, slot))
		return false;

	WorldPacket packet;
	if(obj->GetObjectInfo()->subclass & SUBCLASS_WEAPON)
		_BuildEquipPacket(packet, false, 1 << (obj->GetObjectInfo()->subclass & 0x1F), 0xFFFFFFFF);
	else
		_BuildEquipPacket(packet, true, Unit::ObjectToUnitArmor(obj), 0xFFFFFFFF);
	objacc.SendPacketToAll(&packet);	
	return true;
}

bool Unit::Equip(Object *obj, uint32 slot)
{
	if(!obj || !InMyInventory(obj) || slot >= SLOT_SIZE)
		return false;
	
	Dequip(m_equipment[slot]); //Dequip will check for NULL object
	m_equipment[slot] = obj;
	return true;
}

bool Unit::Dequip(Object *obj)
{
	if(!obj)
		return false;
	for(int i = 0; i < SLOT_SIZE; i++)
	{
		if(m_equipment[i] == obj)
		{
			WorldPacket packet;
			if(obj->GetObjectInfo()->subclass & SUBCLASS_WEAPON)
				_BuildDequipPacket(packet, false, 1 << (obj->GetObjectInfo()->subclass & 0x1F));
			else
				_BuildDequipPacket(packet, true, Unit::ObjectToUnitArmor(obj));
			objacc.SendPacketToAll(&packet);

			m_equipment[i] = 0;
			return true;
		}
	}
	return false;
}

void Unit::_BuildEquipPacket(WorldPacket& packet, bool armor, uint32 slot, uint32 modifier)
{
	uint8 opcode = 0;
	if(armor && modifier)
		opcode = MSG_REPORT_MODIFIABLE_ARMOR_EQUIP;
	else if(armor)
		opcode = MSG_REPORT_MUNDANE_ARMOR_EQUIP;
	else if(modifier)
		opcode = MSG_REPORT_MODIFIABLE_WEAPON_EQUIP;
	else
		opcode = MSG_REPORT_MUNDANE_WEAPON_EQUIP;

	packet.Initialize(opcode);
	packet << (uint16) (GetExtent() | 0x8000); //some reason you need to add this bit
	packet << slot;
	if(modifier)
		packet << modifier;
}

void Unit::_BuildDequipPacket(WorldPacket& packet, bool armor, uint32 slot)
{
	uint8 opcode = 0;
	if(armor)
		opcode = MSG_REPORT_ARMOR_DEQUIP;
	else
		opcode = MSG_REPORT_WEAPON_DEQUIP;

	packet.Initialize(opcode);
	packet << (uint16) GetExtent();
	packet << slot;
}

NoxEnumNamePair g_noxUnitArmorNames[] =
{
	STREET_SNEAKERS, "StreetSneakers",
	MEDIEVAL_CLOAK, "MedievalCloak",
	STREET_PANTS, "StreetPants",
	MEDIEVAL_PANTS, "MedievalPants",
	LEATHER_LEGGINGS, "LeatherLeggings",
	CHAIN_LEGGINGS, "ChainLeggings",
	LEATHER_BOOTS, "LeatherBoots",
	LEATHER_ARMORED_BOOTS, "LeatherArmoredBoots",
	PLATE_BOOTS, "PlateBoots",
	PLATE_LEGGINGS, "PlateLeggings",
	STREET_SHIRT, "StreetShirt",
	MEDIEVAL_SHIRT, "MedievalShirt",
	LEATHER_ARMBANDS, "LeatherArmbands",
	PLATE_ARMS, "PlateArms",
	WIZARD_ROBE, "WizardRobe",
	LEATHER_TUNIC, "LeatherArmor",
	CHAIN_TUNIC, "ChainTunic",
	PLATE_BREAST, "Breastplate",
	CHAIN_COIF, "ChainCoif",
	WIZARD_HELM, "WizardHelm",
	CONJURER_HELM, "ConjurerHelm",
	LEATHER_HELM, "LeatherHelm",
	PLATE_HELM, "SteelHelm",
	ORNATE_HELM, "OrnateHelm",
	ROUND_SHIELD, "RoundShield",
	KITE_SHIELD, "SteelShield",
	0, NULL
};

uint32 Unit::ObjectToUnitArmor(Object* obj)
{
	return ThingBin::noxNameToEnum(obj->GetObjectInfo()->Name, g_noxUnitArmorNames);
}