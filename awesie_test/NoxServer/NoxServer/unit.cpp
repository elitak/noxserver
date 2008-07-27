#include "unit.h"
#include "world_packet.h"

unit::unit(uint16 type) : world_object(type), m_angle(0), m_action(ACTION_IDLE)
{
	m_body_def->allowSleep = false;
}

void unit::_BuildEquipPacket(world_packet& packet, bool armor, uint32 slot, uint32 modifier)
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

	packet.initialize(opcode);
	packet << (uint16) (get_extent() | 0x8000); //some reason you need to add this bit
	packet << slot;
	if(modifier)
		packet << modifier;
}

void unit::_BuildDequipPacket(world_packet& packet, bool armor, uint32 slot)
{
	uint8 opcode = 0;
	if(armor)
		opcode = MSG_REPORT_ARMOR_DEQUIP;
	else
		opcode = MSG_REPORT_WEAPON_DEQUIP;

	packet.initialize(opcode);
	packet << (uint16) get_extent();
	packet << slot;
}

void unit::_BuildEnchantsPacket(world_packet& packet)
{
	packet.initialize(MSG_REPORT_ENCHANTMENT);
	packet << (uint16)get_extent();
	packet << (uint32)m_auras;
}