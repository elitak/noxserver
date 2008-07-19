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
#include "Config/GameConfig.h"
//#include "World.h"

#include "flatland/flatland.hpp"

Player::Player (WorldSession *session): Unit(0x2C9, GridPair(2285, 2600), -1)
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

void Player::Update(uint32 time)
{
	if(updateAll)
	{
		std::vector<WorldObject*> v = objmgr.GetObjectsInRect(GridPair(GetPositionX() - 300, GetPositionY() - 300), GridPair(GetPositionX() + 300, GetPositionY() + 300));
		updateQueue.insert(v.begin(), v.end());

		updateAll = false;
	}
	else
		UpdateView();

	WorldPacket packet;
	if(m_delta_health)
	{
		m_health += m_delta_health;
		if(m_health > m_max_health)
			m_health = m_max_health;
		if(m_health <= 0)
			Die();

		m_combined_health += m_delta_health;
		m_delta_health = 0;

		_BuildMyHealthPacket(packet);
		GetSession()->SendPacket(&packet);
	}
	else if(m_combined_health)
	{
		_BuildDeltaHealthPacket(packet);
		objacc.SendPacketToAll(&packet);
		m_combined_health = 0;
	}

     if(m_mana)
     {
          _BuildManaPacket(packet);
          objacc.SendPacketToAll(&packet);
     }

	// Update ability delays
	if(plrInfo.pclass == PLAYER_CLASS_WARRIOR)
	{
		for(int i = 0; i < 5; i++)
			if(m_ability_delays[i] > 0)
				if(--m_ability_delays[i] == 0)
					_BuildResetAbilityPacket(packet, (AbilityType)(i+1));
	}

	// TODO: health/mana regen, death stuff, etc.
	SendUpdatePacket();

	Unit::Update(time);
}
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
	packet << (uint8)GetAngle(); // we need to OR this with 0x80 depending on the m_action
	packet << (uint8)m_action;
}

void Player::_BuildNewPlayerPacket(WorldPacket& packet)
{
	packet.Initialize(MSG_NEW_PLAYER, 0x80, 0, 0x80);
	packet << (uint16)GetExtent();
	packet.append((uint8*)(&plrInfo), 0x7E);
}
void Player::_BuildClientStatusPacket(WorldPacket& packet)
{
	packet.Initialize(MSG_REPORT_CLIENT_STATUS, 0x0, 0, 6);
	packet << (uint16)GetExtent();
	packet << (uint32)m_session->IsObserving();
}

void Player::_BuildResetAbilityPacket(WorldPacket& packet, uint8 ability)
{
	packet.Initialize(MSG_RESET_ABILITIES);
	packet << (uint8)ability;
}
void Player::_BuildAbilityStatePacket(WorldPacket& packet, uint8 ability)
{
	packet.Initialize(MSG_REPORT_ABILITY_STATE);
	packet << (uint8)ability;
	packet << (uint8)(IsAbilityReady(ability));
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

void Player::Attack()
{
	Unit::Attack();
	if(GetSession()->IsObserving())
		GetSession()->SetObserving(false);
	if(IsDead())
		Respawn();
}
void Player::Jump()
{
     if(!(m_action==ACTION_IDLE||m_action==ACTION_WALK||m_action==ACTION_RUN||m_action==ACTION_SNEAK))
          return;
     GridPair cursor = GetSession()->GetCursor();
     SetActionAnim(ACTION_JUMP, 8);
	MoveToward(cursor.x_coord, cursor.y_coord, 0.42);
}
void Player::RunningJump()
{
     if(!(m_action==ACTION_IDLE||m_action==ACTION_WALK||m_action==ACTION_RUN||m_action==ACTION_SNEAK))
          return;
     GridPair cursor = GetSession()->GetCursor();
     SetActionAnim(ACTION_JUMP, 8);
	MoveToward(cursor.x_coord, cursor.y_coord, 0.42);
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

        return obj->HandlePickup((Player*)this);
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
	Dequip(newSecondary);
	m_equipment[SLOT_WEP_SECONDARY] = newSecondary;

	WorldPacket packet(MSG_REPORT_SECONDARY_WEAPON);
	packet << (uint16)(newSecondary ? newSecondary->GetExtent() : 0);
	m_session->SendPacket(&packet);

	if(!obj) // NULL object (0 extent) means swap weps.
		Equip(oldSecondary);
}
void Player::PlayerCollideCallback(Flatland::ContactList &contacts)
{
	Player* plr = (Player*)contacts.Self()->GetUserPointer();
	Object* obj = (Object*)contacts.Other()->GetUserPointer();

	if(!plr)
		return;
	if(!obj || obj->GetFlatlandObject()->Property().collisionMask & 0x00000001) // Hit an obstacle
	{
		if(plr->m_action == ACTION_BERSERKER_CHARGE)
		{
			plr->ResetActionAnim();
			//plr->SetActionAnim(ACTION_RECOIL, 60);
			plr->Damage(plr->GetHealth() * 0.2, NULL);
			plr->SetEnchant( ENCHANT_HELD, 45 );
		}
	}
	else if(obj->GetType() == 0x2C9)
	{
		if(plr->m_action == ACTION_BERSERKER_CHARGE)
		{
			plr->ResetActionAnim();
			//plr->SetActionAnim(ACTION_RECOIL, 60);
			if(obj->GetHealth() <= 100)
				plr->SetAbilityDelay(ABILITY_BERSERKER_CHARGE, 0);
			((Player*)obj)->Damage(100, plr);
		}
	}
     if(plr->HasEnchant(ENCHANT_SHOCK)&&obj->GetType() == 0x2C9)
     {
          obj->Damage(45, plr);
          plr->UnsetEnchant(ENCHANT_SHOCK);
     }
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

void Player::Poison( byte poisoned, uint16 poisoner )
{
    Unit::Poison(poisoned, poisoner);
    WorldPacket packet;
    packet.Initialize(MSG_REPORT_POISON);
    packet << GetExtent();
	packet << m_poison;
	m_session->SendPacket(&packet);

	packet.Initialize(MSG_INFORM, 0, 0, 5);
	packet << (uint8)0x0D;
	packet << (uint32)2;
	GetSession()->SendPacket(&packet);
}

void Player::_BuildHealthPacket(WorldPacket &packet)
{
	packet.Initialize(MSG_REPORT_HEALTH);
	packet << GetExtent();
	packet << (uint8)(m_health);
}
void Player::_BuildMyHealthPacket(WorldPacket &packet)
{
	packet.Initialize(MSG_REPORT_PLAYER_HEALTH);
	packet << (uint16)m_health;
}
void Player::_BuildTotalHealthPacket(WorldPacket &packet)
{
	packet.Initialize(MSG_REPORT_TOTAL_HEALTH);
	packet << GetExtent();
	packet << (uint16)m_health;
	packet << (uint16)m_max_health;
}
void Player::_BuildManaPacket(WorldPacket &packet)
{
     packet.Initialize(MSG_REPORT_MANA);
     packet << GetExtent();
     packet << (uint16)(m_mana);
}
void Player::_BuildTotalManaPacket(WorldPacket &packet)
{
     packet.Initialize(MSG_REPORT_TOTAL_MANA);
     packet << GetExtent();
     packet << (uint16)m_mana;
     packet << (uint16)m_max_mana;
}
void Player::_BuildStatsPacket(WorldPacket &packet)
{
	packet.Initialize(MSG_REPORT_STATS);
	packet << GetExtent();
	packet << (uint16)m_health;
	packet << (uint16)m_mana; //mana
	packet << (uint16)m_weight; //weight
	packet << (uint16)m_speed; //speed
	packet << (uint16)m_strength; //strength
	packet << (uint8)10; //level
}
bool Player::CanMove( )
{
     if(Unit::CanMove())
          return GetSession()->IsObserving();
     return false;
}
void Player::RunTowards(uint16 x, uint16 y)
{
	if(SetActionAnim(ACTION_RUN, 3))
		MoveToward(x, y, m_speed*.00001);
}
void Player::WalkTowards(uint16 x, uint16 y)
{
	if(SetActionAnim(ACTION_WALK, 3))
		MoveToward(x, y, m_speed*.000005);
}
void Player::TreadTowards(uint16 x, uint16 y)
{
     if(SetActionAnim(ACTION_SNEAK, 3))
          MoveToward(x, y, m_speed*.000005);
}
void Player::MoveTowards(uint16 x, uint16 y)
{
	if(IsDead() || HasEnchant(ENCHANT_HELD))
		return;

	GridPair pos = GetPosition();
	int _x = pos.x_coord - x;
	int _y = pos.y_coord - y;
	int sqlength = (_x*_x) + (_y*_y);
     if(HasEnchant(ENCHANT_SNEAK))
          TreadTowards(x, y);
	else if(sqlength < 1600)
		WalkTowards(x, y);
	else if(sqlength < 2500 && m_action == ACTION_WALK)
		WalkTowards(x, y);
	else
		RunTowards(x, y);
}
void Player::Respawn()
{
	// Set respawn states for the player
     uint16 type;
	switch(plrInfo.pclass)
	{
	case PLAYER_CLASS_WARRIOR:
          {
					ResetAbilityDelays();
					m_health = sGameConfig.GetFloatDefault("WarriorMaxHealth",150);
                         m_mana = sGameConfig.GetFloatDefault("WarriorMaxMana",0);
					m_speed = sGameConfig.GetFloatDefault("WarriorMaxSpeed",21000);
                         m_strength = sGameConfig.GetFloatDefault("WarriorMaxStrength",125);
                         m_weight = 3000;

                         type = sThingBin.Thing.Object.GetIndex("Longsword");
                         Pickup(NewPickup(type));
					break;
          }

	case PLAYER_CLASS_CONJURER: 
					m_health = sGameConfig.GetFloatDefault("ConjurerMaxHealth",100);
                         m_mana = sGameConfig.GetFloatDefault("ConjurerMaxMana",150);
					m_speed = sGameConfig.GetFloatDefault("ConjurerMaxSpeed",18500);
                         m_strength = sGameConfig.GetFloatDefault("ConjurerMaxStrength",125);
                         m_weight = 3000;
                         type = sThingBin.Thing.Object.GetIndex("StreetShirt");
                         Pickup(NewPickup(type));
					break;

	case PLAYER_CLASS_WIZARD: 
					m_health = sGameConfig.GetFloatDefault("WizardMaxHealth",75);
                         m_mana = sGameConfig.GetFloatDefault("WizardMaxMana",125);
					m_speed = sGameConfig.GetFloatDefault("WizardMaxSpeed",17500);
                         m_strength = sGameConfig.GetFloatDefault("WizardMaxStrength",125);
                         m_weight = 3000;

                         type = sThingBin.Thing.Object.GetIndex("StreetShirt");
                         if(type)
                              Pickup(NewPickup(type));
                         type = sThingBin.Thing.Object.GetIndex("WizardRobe");
                         if(type)
                              Pickup(NewPickup(type));
					break;
	default:break;
	}
     type = sThingBin.Thing.Object.GetIndex("StreetPants");
     if(type)
          Pickup(NewPickup(type));
     type = sThingBin.Thing.Object.GetIndex("StreetSneakers");
     if(type)
          Pickup(NewPickup(type));
     m_max_speed = m_speed;
     m_max_mana = m_mana;
	m_max_health = m_health;

	// Get a random spawn point and set position to that
	SetPosition(GridPair(3000, 2900));
	ForceUpdateAll();

	ResetEnchants();
	SetEnchant(ENCHANT_INVULNERABLE);

	GetSession()->_SendPlayerRespawnOpcode();
	WorldPacket packet;
	_BuildTotalHealthPacket(packet);
	ObjectAccessor::Instance().SendPacketToAll(&packet);
     _BuildTotalManaPacket(packet);
     ObjectAccessor::Instance().SendPacketToAll(&packet);
	_BuildStatsPacket(packet);
	ObjectAccessor::Instance().SendPacketToAll(&packet);

	_BuildDequipPacket(packet, true, 0xFFFFFFFF);
	GetSession()->SendPacket(&packet);
	_BuildDequipPacket(packet, false, 0xFFFFFFFF);
	GetSession()->SendPacket(&packet);
}
void Player::ResetAbilityDelays()
{
	memset(m_ability_delays, 0, sizeof(uint16) * 5);

	WorldPacket packet;
	_BuildResetAbilityPacket(packet, ABILITY_ALL);
	GetSession()->SendPacket(&packet);
}
bool Player::IsAbilityReady(uint8 ability)
{
	if(ability < 1 || ability > 5)
		return false;
	return !(m_ability_delays[ ((uint8)ability) - 1 ]);
}
void Player::SetAbilityDelay(uint8 ability, uint16 frames)
{
	if(ability < 1 || ability > 5)
		return;
	m_ability_delays[ ((uint8)ability) - 1 ] = frames;
	
	WorldPacket packet;
	if(frames)
	{
		_BuildAbilityStatePacket(packet, ability);
		GetSession()->SendPacket(&packet);
	}
	else
	{
		_BuildResetAbilityPacket(packet, ability);
		GetSession()->SendPacket(&packet);
	}
}
void Player::SetEnchant(UnitEnchantType enchant, int16 frames)
{
	Unit::SetEnchant(enchant, frames);
	WorldPacket packet(MSG_INFORM, 0, 0, 5);
	if(enchant == ENCHANT_CONFUSED)
	{
		packet << (uint8)0x0D;
		packet << (uint32)1;
		GetSession()->SendPacket(&packet);
	}
	else if(enchant == ENCHANT_HELD)
	{
		packet << (uint8)0x0D;
		packet << (uint32)0;
		GetSession()->SendPacket(&packet);
	}
}