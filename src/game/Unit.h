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

struct DamageShield
{
    uint32 m_spellId;
    uint32 m_damage;
    uint64 m_caster_guid;
};

struct SpellImmune
{
    uint32 type;
    uint32 spellId;
};

typedef std::list<SpellImmune> SpellImmuneList;

enum DeathState
{
    ALIVE       = 0,
    JUST_DIED   = 1,
    CORPSE      = 2,
    DEAD        = 3
};

enum UnitState
{
    UNIT_STAT_STOPPED       = 0,
    UNIT_STAT_DIED          = 1,
    UNIT_STAT_ATTACKING     = 2,                            // player is attacking someone
    UNIT_STAT_ATTACK_BY     = 4,                            // player is attack by someone
                                                            // player is in combat mode
    UNIT_STAT_IN_COMBAT     = (UNIT_STAT_ATTACKING | UNIT_STAT_ATTACK_BY),
    UNIT_STAT_STUNDED       = 8,
    UNIT_STAT_ROAMING       = 16,
    UNIT_STAT_CHASE         = 32,
    UNIT_STAT_SEARCHING     = 64,
    UNIT_STAT_FLEEING       = 128,
    UNIT_STAT_MOVING        = (UNIT_STAT_ROAMING | UNIT_STAT_CHASE | UNIT_STAT_SEARCHING | UNIT_STAT_FLEEING),
    UNIT_STAT_IN_FLIGHT     = 256,                          // player is in flight mode
    UNIT_STAT_FOLLOW        = 512,
    UNIT_STAT_ROOT          = 1024,
    UNIT_STAT_CONFUSED      = 2048,
    UNIT_STAT_ALL_STATE     = 0xffff                        //(UNIT_STAT_STOPPED | UNIT_STAT_MOVING | UNIT_STAT_IN_COMBAT | UNIT_STAT_IN_FLIGHT)
};

enum UnitMoveType
{
    MOVE_WALK       = 0,
    MOVE_RUN        = 1,
    MOVE_WALKBACK   = 2,
    MOVE_SWIM       = 3,
    MOVE_SWIMBACK   = 4,
    MOVE_TURN       = 5,
    MOVE_FLY        = 6,
    MOVE_FLYBACK    = 7
};

#define MAX_MOVE_TYPE 8

extern float baseMoveSpeed[MAX_MOVE_TYPE];

enum WeaponAttackType
{
    BASE_ATTACK   = 0,
    OFF_ATTACK    = 1,
    RANGED_ATTACK = 2
};

enum DamageEffectType
{
    DIRECT_DAMAGE           = 0,
    SPELL_DIRECT_DAMAGE     = 1,
    DOT                     = 2,
    HEAL                    = 3,
    NODAMAGE                = 4,                            // used also in case when damage applied to health but not applied to spell channelInterruptFlags/etc
    SELF_DAMAGE             = 5
};

enum UnitVisibilityUpdate
{
    VISIBLE_NOCHANGES                 = 0,
    VISIBLE_SET_VISIBLE               = 1,
    VISIBLE_SET_INVISIBLE             = 2,
    VISIBLE_SET_INVISIBLE_FOR_GROUP   = 3
};

enum UnitVisibility
{
    VISIBILITY_OFF         = 0,
    VISIBILITY_ON          = 1,
    VISIBILITY_GROUP       = 2
};

// Value masks for UNIT_FIELD_FLAGS
enum UnitFlags
{
    UNIT_FLAG_NONE           = 0x00000000,
    UNIT_FLAG_DISABLE_MOVE   = 0x00000004,
    UNIT_FLAG_UNKNOWN1       = 0x00000008,                  // essential for all units..
    UNIT_FLAG_RENAME         = 0x00000010,                  // rename creature, not working in 2.0.8
    UNIT_FLAG_RESTING        = 0x00000020,
    UNIT_FLAG_UNKNOWN2       = 0x00000100,                  // 2.0.8
    UNIT_FLAG_UNKNOWN3       = 0x00000800,                  // in combat ?2.0.8
    UNIT_FLAG_PVP            = 0x00001000,
    UNIT_FLAG_MOUNT          = 0x00002000,
    UNIT_FLAG_UNKNOWN4       = 0x00004000,                  // 2.0.8
    UNIT_FLAG_PACIFIED       = 0x00020000,
    UNIT_FLAG_DISABLE_ROTATE = 0x00040000,                  // may be it's stunned flag?
    UNIT_FLAG_IN_COMBAT      = 0x00080000,
    UNIT_FLAG_DISARMED       = 0x00200000,                  // disable melee spells casting..., "Required melee weapon" added to melee spells tooltip.
    UNIT_FLAG_CONFUSED       = 0x00400000,
    UNIT_FLAG_FLEEING        = 0x00800000,
    UNIT_FLAG_UNKNOWN5       = 0x01000000,					// used in spell Eyes of the Beast for pet...
    UNIT_FLAG_NOT_SELECTABLE = 0x02000000,
    UNIT_FLAG_SKINNABLE      = 0x04000000,
    UNIT_FLAG_UNKNOWN6       = 0x20000000,                  // used in Feing Death spell
    UNIT_FLAG_SHEATHE        = 0x40000000
};

enum TempSummonType
{
    TEMPSUMMON_TIMED_OR_DEAD_DESPAWN       = 1,             // despawns after a specified time OR when the creature disappears
    TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN     = 2,             // despawns after a specified time OR when the creature dies
    TEMPSUMMON_TIMED_DESPAWN               = 3,             // despawns after a specified time
    TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT = 4,             // despawns after a specified time after the creature is out of combat
    TEMPSUMMON_CORPSE_DESPAWN              = 5,             // despawns instantly after death
    TEMPSUMMON_CORPSE_TIMED_DESPAWN        = 6,             // despawns after a specified time after death
    TEMPSUMMON_DEAD_DESPAWN                = 7,             // despawns when the creature disappears
    TEMPSUMMON_MANUAL_DESPAWN              = 8              // despawns when UnSummon() is called        
};

enum ProcFlags
{
    PROC_FLAG_NONE               = 0x00000000,              // None
    PROC_FLAG_HIT_MELEE          = 0x00000001,              // On melee hit
    PROC_FLAG_STRUCK_MELEE       = 0x00000002,              // On being struck melee
    PROC_FLAG_KILL_XP_GIVER      = 0x00000004,              // On kill target giving XP or honor
    PROC_FLAG_SPECIAL_DROP       = 0x00000008,              //
    PROC_FLAG_DODGE              = 0x00000010,              // On dodge melee attack
    PROC_FLAG_PARRY              = 0x00000020,              // On parry melee attack
    PROC_FLAG_BLOCK              = 0x00000040,              // On block attack
    PROC_FLAG_TOUCH              = 0x00000080,              // On being touched (for bombs, probably?)
    PROC_FLAG_TARGET_LOW_HEALTH  = 0x00000100,              // On deal damage to enemy with 20% or less health
    PROC_FLAG_LOW_HEALTH         = 0x00000200,              // On health dropped below 20%
    PROC_FLAG_STRUCK_RANGED      = 0x00000400,              // On being struck ranged
    PROC_FLAG_HIT_SPECIAL        = 0x00000800,              // (!)Removed, may be reassigned in future
    PROC_FLAG_CRIT_MELEE         = 0x00001000,              // On crit melee
    PROC_FLAG_STRUCK_CRIT_MELEE  = 0x00002000,              // On being critically struck in melee
    PROC_FLAG_CAST_SPELL         = 0x00004000,              // On cast spell (and broken Aspect of Hawk)
    PROC_FLAG_TAKE_DAMAGE        = 0x00008000,              // On take damage
    PROC_FLAG_CRIT_SPELL         = 0x00010000,              // On crit spell
    PROC_FLAG_HIT_SPELL          = 0x00020000,              // On hit spell
    PROC_FLAG_STRUCK_CRIT_SPELL  = 0x00040000,              // On being critically struck by a spell
    PROC_FLAG_HIT_RANGED         = 0x00080000,              // On getting ranged hit
    PROC_FLAG_STRUCK_SPELL       = 0x00100000,              // On being struck by a spell
    PROC_FLAG_TRAP               = 0x00200000,              // On trap activation (?)
    PROC_FLAG_CRIT_RANGED        = 0x00400000,              // On getting ranged crit
    PROC_FLAG_STRUCK_CRIT_RANGED = 0x00800000,              // On being critically struck by a ranged attack
    PROC_FLAG_RESIST_SPELL       = 0x01000000,              // On resist enemy spell
    PROC_FLAG_TARGET_RESISTS     = 0x02000000,              // On enemy resisted spell
    PROC_FLAG_TARGET_AVOID_ATTACK= 0x04000000,              // On enemy blocks/dodges/parries
    PROC_FLAG_HEAL               = 0x08000000,              // On heal
    PROC_FLAG_CRIT_HEAL          = 0x10000000               // On critical healing effect
};

enum AuraState
{
    AURA_STATE_DODGE          = 1,
    AURA_STATE_HEALTHLESS     = 2,
    AURA_STATE_RACE           = 3,
    AURA_STATE_UNKNOWN1       = 4,
    AURA_STATE_JUDGEMENT      = 5,
    AURA_STATE_UNKNOWN2       = 6,
    AURA_STATE_PARRY          = 7                           // unsure.
};

enum Mechanics
{
    MECHANIC_CHARM            = 1,
    MECHANIC_CONFUSED         = 2,
    MECHANIC_DISARM           = 3,
    MECHANIC_ATTRACT          = 4,
    MECHANIC_FEAR             = 5,
    MECHANIC_STUPID           = 6,
    MECHANIC_ROOT             = 7,
    MECHANIC_PEACE            = 8,
    MECHANIC_SILENCE          = 9,
    MECHANIC_SLEEP            = 10,
    MECHANIC_CHASE            = 11,
    MECHANIC_STUNDED          = 12,
    MECHANIC_FREEZE           = 13,
    MECHANIC_KNOCKOUT         = 14,
    MECHANIC_BLEED            = 15,
    MECHANIC_HEAL             = 16,
    MECHANIC_POLYMORPH        = 17,
    MECHANIC_BANISH           = 18,
    MECHANIC_SHIELDED         = 19,
    MECHANIC_DURANCED         = 20,
    MECHANIC_MOUNT            = 21,
    MECHANIC_PERSUADED        = 22,
    MECHANIC_TURNED           = 23,
    MECHANIC_HORROR           = 24,
    MECHANIC_INVULNERABILITY  = 25,
    MECHANIC_INTERRUPTED      = 26,
    MECHANIC_DAZED            = 27
};

//To all Immune system,if target has immunes,
//some spell that related to ImmuneToDispel or ImmuneToSchool or ImmuneToDamage type can't cast to it,
//some spell_effects that related to ImmuneToEffect<effect>(only this effect in the spell) can't cast to it,
//some aura(related to Mechanics or ImmuneToState<aura>) can't apply to it.
enum SpellImmunity
{
    IMMUNITY_EFFECT                = 0,
    IMMUNITY_STATE                 = 1,
    IMMUNITY_SCHOOL                = 2,
    IMMUNITY_DAMAGE                = 3,
    IMMUNITY_DISPEL                = 4,
    IMMUNITY_MECHANIC              = 5
};

enum DiminishingMechanics
{
    DIMINISHING_NONE                = 0,
    DIMINISHING_MECHANIC_CONFUSE    = 1,                    // incapacitate, confuse
    DIMINISHING_MECHANIC_CHARM      = 2,                    // fear, mind control, sleep
    DIMINISHING_MECHANIC_STUN       = 3,                    // stun
    DIMINISHING_MECHANIC_ROOT       = 4,                    // roots, freeze
    DIMINISHING_MECHANIC_SPEED      = 5                     // speed reduction
};

enum DiminishingLevels
{
    DIMINISHING_LEVEL_1             = 0,
    DIMINISHING_LEVEL_2             = 1,
    DIMINISHING_LEVEL_3             = 2,
    DIMINISHING_LEVEL_IMMUNE        = 3
};

struct DiminishingReturn
{
    DiminishingReturn(DiminishingMechanics mech, uint32 t, uint32 count) : Mechanic(mech), hitTime(t), hitCount(count) {}

    DiminishingMechanics    Mechanic;
    uint32                  hitTime;
    uint32                  hitCount;
};

enum ImmuneToDispel
{
    IMMUNE_DISPEL_MAGIC        = 1,
    IMMUNE_DISPEL_CURSE        = 2,
    IMMUNE_DISPEL_DISEASE      = 3,
    IMMUNE_DISPEL_POISON       = 4,
    IMMUNE_DISPEL_STEALTH      = 5,
    IMMUNE_DISPEL_INVISIBILITY = 6,
    IMMUNE_DISPEL_ALL          = 7,
    IMMUNE_DISPEL_SPE_NPC_ONLY = 8,
    IMMUNE_DISPEL_CRAZY        = 9,
    IMMUNE_DISPEL_ZG_TICKET    = 10
};

enum ImmuneToDamage
{
    IMMUNE_DAMAGE_PHYSICAL     = 1,
    IMMUNE_DAMAGE_MAGIC        = 126
};

enum ImmuneToSchool
{
    IMMUNE_SCHOOL_PHYSICAL     = 1,
    IMMUNE_SCHOOL_HOLY         = 2,
    IMMUNE_SCHOOL_FIRE         = 4,
    IMMUNE_SCHOOL_NATURE       = 8,
    IMMUNE_SCHOOL_FROST        = 16,
    IMMUNE_SCHOOL_SHADOW       = 32,
    IMMUNE_SCHOOL_ARCANE       = 64,
    IMMUNE_SCHOOL_MAGIC        = 126
};

inline SpellSchools immuneToSchool(ImmuneToSchool immune)
{
    switch(immune)
    {
        case IMMUNE_SCHOOL_PHYSICAL: return SPELL_SCHOOL_NORMAL;
        case IMMUNE_SCHOOL_HOLY:     return SPELL_SCHOOL_HOLY;
        case IMMUNE_SCHOOL_FIRE:     return SPELL_SCHOOL_FIRE;
        case IMMUNE_SCHOOL_NATURE:   return SPELL_SCHOOL_NATURE;
        case IMMUNE_SCHOOL_FROST:    return SPELL_SCHOOL_FROST;
        case IMMUNE_SCHOOL_SHADOW:   return SPELL_SCHOOL_SHADOW;
        case IMMUNE_SCHOOL_ARCANE:   return SPELL_SCHOOL_ARCANE;
        case IMMUNE_SCHOOL_MAGIC:
        default:
            break;
    }
    assert(false);
    return SPELL_SCHOOL_NORMAL;
}

struct Hostil
{
    Hostil(uint64 _UnitGuid, float _Threat) : UnitGuid(_UnitGuid), Threat(_Threat) {}

    uint64 UnitGuid;
    float Threat;
    bool operator <(Hostil item)
    {
        if(Threat < item.Threat)
            return true;
        else
            return false;
    };
};

typedef std::list<Hostil> ThreatList;
typedef std::list<Hostil> HateOfflineList;

enum MeleeHitOutcome
{
    MELEE_HIT_MISS, MELEE_HIT_DODGE, MELEE_HIT_BLOCK, MELEE_HIT_PARRY, MELEE_HIT_GLANCING,
    MELEE_HIT_CRIT, MELEE_HIT_CRUSHING, MELEE_HIT_NORMAL
};

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
        virtual ~Unit ( );
        virtual void Update( uint32 time );
		virtual void MoveToward( uint16 x, uint16 y );
		virtual void Move( int16 deltax, int16 deltay );

          virtual void Laugh();
          virtual void Point();
          virtual void Taunt();

		virtual bool Equip( Object* obj );
		virtual bool Equip( Object* obj, uint32 slot );
		virtual bool Dequip( Object* obj );
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
		Object* m_equipment[SLOT_SIZE];
   
    private:

        uint32 m_state;                                     // Even derived shouldn't modify
        uint32 m_CombatTimer;
};
#endif
