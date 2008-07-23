#include "player.h"

player::player(world_session& session) : _session(session), unit(0x2c9)
{
}

void player::attack()
{
}

bool player::dequip(object *obj)
{
	return true;
}

bool player::equip(object *obj)
{
	return true;
}

bool player::drop(object *obj, uint32 max_dist, float x, float y)
{
	return true;
}

bool player::pickup(object *obj, uint32 max_dist)
{
	return true;
}

void player::respawn()
{
}

void player::jump()
{
}

void player::laugh()
{
}

void player::running_jump()
{
}

void player::move_towards(float x, float y)
{
}

void player::taunt()
{
}

void player::point()
{
}

void player::equip_secondary(object *obj)
{
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

void Player::respawn()
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