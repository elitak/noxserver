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

#ifndef __UNIT_H
#define __UNIT_H

#include "Common.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Mthread.h"
#include "Util.h"
#include "UpdateFields.h"
#include "SharedDefines.h"

#include <list>


// delay time next attack to privent client attack animanation problems
#define ATTACK_DISPLAY_DELAY 200

enum UnitActionType
{
	ACTION_IDLE = 0x0,
	ACTION_DIE,
	ACTION_DEAD,
	ACTION_JUMP,
	ACTION_WALK,
	ACTION_WALK_AND_DRAG,
	ACTION_RUN,
	ACTION_RUNNING_JUMP,
	ACTION_PICKUP,
	ACTION_DODGE_LEFT,
	ACTION_DODGE_RIGHT,
	ACTION_ELECTROCUTED,
	ACTION_FALL,
	ACTION_TRIP,
	ACTION_GET_UP,
	ACTION_LAUGH,
	ACTION_POINT,
	ACTION_SIT,
	ACTION_SLEEP,
	ACTION_TALK,
	ACTION_TAUNT,
	ACTION_CAST_SPELL,
	ACTION_CONCENTRATE,
	ACTION_PUNCH_LEFT,
	ACTION_PUNCH_RIGHT,
	ACTION_PUNCH_RIGHT_HOOK,
	ACTION_MACE_STRIKE,
	ACTION_SWORD_STRIKE,
	ACTION_LONG_SWORD_STRIKE,
	ACTION_STAFF_STRIKE,
	ACTION_STAFF_BLOCK,
	ACTION_STAFF_SPELL_BLAST,
	ACTION_STAFF_THRUST,
	ACTION_SHOOT_BOW,
	ACTION_SHOOT_CROSSBOW,
	ACTION_AXE_STRIKE,
	ACTION_GREAT_SWORD_PARRY,
	ACTION_GREAT_SWORD_STRIKE,
	ACTION_GREAT_SWORD_IDLE,
	ACTION_HAMMER_STRIKE,
	ACTION_RAISE_SHIELD,
	ACTION_RECOIL_FORWARD,
	ACTION_RECOIL_BACKWARD,
	ACTION_RECOIL_SHIELD,
	ACTION_CHAKRAM_STRIKE,
	ACTION_BERSERKER_CHARGE,
	ACTION_WARCRY,
	ACTION_GREAT_SWORD_BLOCK_LEFT,
	ACTION_GREAT_SWORD_BLOCK_RIGHT,
	ACTION_GREAT_SWORD_BLOCK_DOWN,
	ACTION_ELECTRIC_ZAP,
	ACTION_DUST,
	ACTION_RECOIL,
	ACTION_SNEAK,
	ACTION_HARPOONTHROW
};

enum UnitSlotType
{
	SLOT_HELMET = 0x0, //these correspond with SUBCLASS enum
	SLOT_BREASTPLATE = 0x1,
	SLOT_ARM_ARMOR = 0x2,
	SLOT_PANTS = 0x3,
	SLOT_BOOTS = 0x4,
	SLOT_SHIRT = 0x5,
	SLOT_LEG_ARMOR = 0x6,
	SLOT_BACK = 0x7,
	SLOT_SHIELD = 0x8,
	SLOT_FLAG = 0x9,
	SLOT_QUIVER = 0xA,
	SLOT_WEP_PRIMARY,// this and below does not directly correspond
	SLOT_WEP_SECONDARY,// however, everything with &0x40 is a weapon
	SLOT_SIZE //this works since enum goes 0, 1, ...
};

enum UnitArmorType
{
	STREET_SNEAKERS = 0x00000001,
	MEDIEVAL_CLOAK = 0x00000002,
	STREET_PANTS = 0x00000004,
	MEDIEVAL_PANTS = 0x00000008,
	LEATHER_LEGGINGS = 0x00000010,
	CHAIN_LEGGINGS = 0x00000020,
	LEATHER_BOOTS = 0x00000040,
	LEATHER_ARMORED_BOOTS = 0x00000080,
	PLATE_BOOTS = 0x00000100,
	PLATE_LEGGINGS = 0x00000200,
	STREET_SHIRT = 0x00000400,
	MEDIEVAL_SHIRT = 0x00000800,
	LEATHER_ARMBANDS = 0x00001000,
	PLATE_ARMS = 0x00002000,
	WIZARD_ROBE = 0x00004000,
	LEATHER_TUNIC = 0x00008000,
	CHAIN_TUNIC = 0x00010000,
	PLATE_BREAST = 0x00020000,
	CHAIN_COIF = 0x00040000,
	WIZARD_HELM = 0x00080000,
	CONJURER_HELM = 0x00100000,
	LEATHER_HELM = 0x00200000,
	PLATE_HELM = 0x00400000,
	ORNATE_HELM = 0x00800000,
	ROUND_SHIELD = 0x01000000,
	KITE_SHIELD = 0x02000000
};
enum UnitWeaponType
{
	FLAG = 0x00000001,
	QUIVER = 0x00000002,
	BOW = 0x00000004,
	CROSSBOW = 0x00000008,
	ARROW = 0x00000010,
	BOLT = 0x00000020,
	CHAKRAM = 0x00000040,
	SHURIKEN = 0x00000080,
	SWORD = 0x00000100,
	LONG_SWORD = 0x00000200,
	GREAT_SWORD = 0x00000400,
	MACE = 0x00000800,
	AXE = 0x00001000,
	OGRE_AXE = 0x00002000,
	HAMMER = 0x00004000,
	STAFF = 0x00008000,
	STAFF_SULPHOROUS_FLARE = 0x00010000,
	STAFF_SULPHOROUS_SHOWER = 0x00020000,
	STAFF_LIGHTNING = 0x00040000,
	STAFF_FIREBALL = 0x00080000,
	STAFF_TRIPLE_FIREBALL = 0x00100000,
	STAFF_FORCE_OF_NATURE = 0x00200000,
	STAFF_DEATH_RAY = 0x00400000,
	STAFF_OBLIVION_HALBERD = 0x00800000,
	STAFF_OBLIVION_HEART = 0x01000000,
	STAFF_OBLIVION_WIERDLING = 0x02000000,
	STAFF_OBLIVION_ORB = 0x04000000
};
class MANGOS_DLL_SPEC Unit : public WorldObject
{
    public:
		Unit(uint16 type, GridPair pos, uint16 extent);
        virtual ~Unit ( );
        virtual void Update( uint32 time );
		virtual void MoveToward( uint16 x, uint16 y );

		virtual void Laugh();
        virtual void Point();
        virtual void Taunt();

		virtual bool Equip( WorldObject* obj );
		virtual bool Equip( WorldObject* obj, uint32 slot );
		virtual bool Dequip( WorldObject* obj );

		void _BuildEquipPacket(WorldPacket& packet, bool armor, uint32 slot, uint32 modifier = 0);
		void _BuildDequipPacket(WorldPacket& packet, bool armor, uint32 slot);
		static uint32 ObjectToUnitArmor(Object* obj);
		
		void SetAngle (uint8 angle) { m_angle = angle; }
		uint8 GetAngle () { return m_angle; }
    protected:
        Unit ( WorldObject *instantiator );
		UnitActionType m_action;
		time_t m_action_time;

		uint8 m_angle;
		WorldObject* m_equipment[SLOT_SIZE];
   
    private:

        uint32 m_state;                                     // Even derived shouldn't modify
        uint32 m_CombatTimer;
};
#endif
