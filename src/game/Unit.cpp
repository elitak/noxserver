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

#include "ode/ode.h"

Unit::Unit( uint16 type, GridPair pos, uint16 extent) : WorldObject(type, pos, extent)
{
	InitRespawn();
}
Unit::Unit( WorldObject *instantiator ) : WorldObject( instantiator )
{
	InitRespawn();
}

void Unit::InitRespawn()
{
	m_action = ACTION_IDLE;
	m_action_time = 0;
	m_angle = 0;
     m_poison = 0;
	
	memset(m_equipment, 0, SLOT_SIZE * sizeof(Object*));
	ResetEnchants();
	m_animFrames = -1;
	m_animTimer = 0;
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
	WorldPacket packet;

	if(IsDead())
	{
		m_action = ACTION_DEAD;
		m_health = 0;
	}
	else if(m_action || m_action != ACTION_RAISE_SHIELD)
	{
		if(p_time > m_action_time)
		{
			dBodySetLinearVel(body->GetBody(), 0.0f, 0.0f, 0.0f);
			if(m_equipment[SLOT_SHIELD])
				m_action = ACTION_RAISE_SHIELD;
			else
				m_action = ACTION_IDLE;
			m_action_time = 0;
		}
		else
			m_action_time -= p_time;
	}

     if(m_poison == 1)
     {
          Damage(.03, 0);
     }

	//Update Enchants
	for(int i = 0; i < ENCHANT_SIZE; i++)
	{
		if(m_aura_times[i] > 0)
			m_aura_times[i]--;
		else if(!m_aura_times[i] && (m_auras & (1 << i)))
			UnsetEnchant((UnitEnchantType)i);
	}

	//Update Anim. Frame
	if(m_animFrames >= 0 && m_animFrames < 256)
	{	
		packet.Initialize(MSG_DRAW_FRAME);
		packet << (uint16)GetExtent();
		packet << (uint8)(m_animFrames);
		objacc.SendPacketToAll(&packet); // save on bandwidth and only send to those in range?
		if(m_animTimer < p_time)
		{
			m_animFrames++;
			m_animTimer = 40;
		}
		else
			m_animTimer -= p_time;
	}

	if(m_updateMask & MASK_ENCHANTMENTS)
	{
		m_updateMask &= ~MASK_ENCHANTMENTS;
		_BuildEnchantsPacket(packet);
		objacc.SendPacketToAll(&packet);
	}

}

void Unit::Poison( byte poisoned )
{
     m_poison = poisoned;
}
void Unit::MoveToward(uint16 _x, uint16 _y, float speed)
{
	if(IsDead())
		return;

	float x = (int32)_x - (int32)GetPositionX();
	float y = (int32)_y - (int32)GetPositionY();
	double len = sqrt(x*x+y*y);
	double unit_x = x/len;
	double unit_y = y/len;

	dBodySetLinearVel(body->GetBody(), unit_x*speed, unit_y*speed, 0.0f);	
}

void Unit::Laugh()
{
     SetActionAnim(ACTION_LAUGH, 40);
}
void Unit::Point()
{
     SetActionAnim(ACTION_POINT, 40);
}

void Unit::Taunt()
{
     SetActionAnim(ACTION_TAUNT, 40);
}

void Unit::Attack()
{
	SetActionAnim(ACTION_LONG_SWORD_STRIKE, 30);
	if(HasEnchant(ENCHANT_INVULNERABLE))
		UnsetEnchant(ENCHANT_INVULNERABLE);
}
bool Unit::Equip(WorldObject *obj)
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

bool Unit::Equip(WorldObject *obj, uint32 slot)
{
	if(!obj || !InMyInventory(obj) || slot >= SLOT_SIZE)
		return false;
	
	Dequip(m_equipment[slot]); //Dequip will check for NULL object
	m_equipment[slot] = obj;
	return true;
}

bool Unit::Dequip(WorldObject *obj)
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

void Unit::_BuildEnchantsPacket(WorldPacket& packet)
{
	packet.Initialize(MSG_REPORT_ENCHANTMENT);
	packet << (uint16)GetExtent();
	packet << (uint32)m_auras;
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
bool Unit::SetActionAnim(UnitActionType anim, uint32 frames)
{
	if(m_action_time && m_action != anim && m_action != ACTION_WALK && m_action != ACTION_RUN)
		return false;

	if(m_action != anim)
		ResetActionAnim();

	m_action = anim;
	m_action_time = frames*30;
	m_animFrames = 0;

	return true;
}
void Unit::UnsetEnchant( UnitEnchantType enchant )
{
	m_auras &= ~(1 << (uint8)enchant);
	m_aura_times[enchant] = 0;

	m_updateMask |= MASK_ENCHANTMENTS;
}
void Unit::SetEnchant( UnitEnchantType enchant, int16 frames )
{
	m_auras |= (1 << (uint8)enchant);
	if(frames)
		m_aura_times[enchant] = frames;
	else
	{
		EnchantTableMap::iterator iter = objmgr.enchantTable.find(enchant);
		if(iter != objmgr.enchantTable.end())
			m_aura_times[enchant] = iter->second.frames;
		else
			m_aura_times[enchant] = 300;
	}
	m_updateMask |= MASK_ENCHANTMENTS;
}
void Unit::ResetEnchants()
{
	m_auras = 0;
	memset(m_aura_times, 0, sizeof(int16)*ENCHANT_SIZE);
	m_updateMask |= MASK_ENCHANTMENTS;
}