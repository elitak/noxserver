#include "unit.h"
#include "socket_mgr.h"
#include "world_packet.h"

unit::unit(uint16 type) : world_object(type), m_angle(0), m_action(ACTION_IDLE)
{
	m_body_def->allowSleep = false;

	// clear equipment
	memset(m_equipment, 0, sizeof(m_equipment));
}

bool unit::drop(object* obj, float x, float y)
{
	dequip(obj);
	return world_object::drop(obj, x, y);
}

NoxEnumNamePair nox_unit_armor_names[] =
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

bool unit::dequip(object* obj)
{
	if(!obj || obj->get_parent() != this)
		return false;
	for(int i = 0; i < SLOT_SIZE; i++)
	{
		if(m_equipment[i] == obj)
		{
			if(i != SLOT_WEP_SECONDARY)
			{
				world_packet packet;
				if(obj->get_object_info()->subclass & SUBCLASS_WEAPON)
					_BuildDequipPacket(packet, false, 1 << (obj->get_object_info()->subclass & 0x1F));
				else
					_BuildDequipPacket(packet, true, ThingBin::instance->noxNameToEnum(obj->get_object_info()->Name, nox_unit_armor_names));
				socket_mgr::instance->get_world_socket().send_to_all(packet);
			}

			m_equipment[i] = NULL;
			return true;
		}
	}

	return false;
}

bool unit::equip(object* obj)
{
	if(!obj || obj->get_parent() != this)
		return false;

	uint32 slot = obj->get_object_info()->subclass;
	if(slot & SUBCLASS_WEAPON) //this can only be true for weapons
	{
		if(slot == SUBCLASS_QUIVER)
			slot = SLOT_QUIVER;
		else if(slot == SUBCLASS_FLAG)
			slot = SLOT_FLAG;
		else
			slot = SLOT_WEP_PRIMARY;
	}

	if(slot >= SLOT_SIZE)
		return false;

	// just in case? do we really need this?
	dequip(obj);
	
	if(m_equipment[slot] != NULL)
		dequip(m_equipment[slot]);
	m_equipment[slot] = obj;

	world_packet packet;
	if(obj->get_object_info()->subclass & SUBCLASS_WEAPON)
		_BuildEquipPacket(packet, false, 1 << (obj->get_object_info()->subclass & 0x1F), 0xFFFFFFFF);
	else
		_BuildEquipPacket(packet, true, ThingBin::instance->noxNameToEnum(obj->get_object_info()->Name, nox_unit_armor_names), 0xFFFFFFFF);
	socket_mgr::instance->get_world_socket().send_to_all(packet);

	return true;
}

bool unit::use(object* obj)
{
	return true;
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