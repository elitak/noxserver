/* 
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

#ifndef _SPELL_H
#define _SPELL_H

#include "Policies/Singleton.h"
#include "WorldSession.h"

struct SpellHandler
{
    SpellHandler() : classFlag(0), handler(NULL) {};
    SpellHandler( uint8 _classFlag, void (WorldSession::*_handler)(bool invert) ) : classFlag(_classFlag), handler(_handler) {};

	uint8 classFlag;
    void (WorldSession::*handler)(bool invert);
};

typedef HM_NAMESPACE::hash_map< uint8 , SpellHandler > SpellTableMap;

struct AbilityHandler
{
    AbilityHandler() : handler(NULL) {};
    AbilityHandler( void (WorldSession::*_handler)() ) : handler(_handler) {};

    void (WorldSession::*handler)();
};

typedef HM_NAMESPACE::hash_map< uint8 , AbilityHandler > AbilityTableMap;

enum AbilityType
{
    ABILITY_INVALID = 0,
    ABILITY_BERSERKER_CHARGE,
    ABILITY_WARCRY,
    ABILITY_HARPOON,
    ABILITY_TREAD_LIGHTLY,
    ABILITY_EYE_OF_THE_WOLF
};

enum SpellType
{
    SPELL_INVALID = 0x0,
    SPELL_ANCHOR,
    SPELL_ARACHNAPHOBIA,
    SPELL_BLIND,
    SPELL_BLINK,
    SPELL_BURN,
    SPELL_CANCEL,
    SPELL_CHAIN_LIGHTNING_BOLT,
    SPELL_CHANNEL_LIFE,
    SPELL_CHARM,
    SPELL_CLEANSING_FLAME,
    SPELL_CLEANSING_MANA_FLAME,
    SPELL_CONFUSE,
    SPELL_COUNTERSPELL,
    SPELL_CURE_POISON,
    SPELL_DEATH,
    SPELL_DEATH_RAY,
    SPELL_DETECT_MAGIC,
    SPELL_DETONATE,
    SPELL_DETONATE_GLYPHS,
    SPELL_DISENCHANT_ALL,
    SPELL_TURN_UNDEAD,
    SPELL_DRAIN_MANA,
    SPELL_EARTHQUAKE,
    SPELL_LIGHTNING,
    SPELL_EXPLOSION,
    SPELL_FEAR,
    SPELL_FIREBALL,
    SPELL_FIREWALK,
    SPELL_FIST,
    SPELL_FORCE_FIELD,
    SPELL_FORCE_OF_NATURE,
    SPELL_FREEZE,
    SPELL_FUMBLE,
    SPELL_GLYPH,
    SPELL_GREATER_HEAL,
    SPELL_HASTE,
    SPELL_INFRAVISION,
    SPELL_INVERSION,
    SPELL_INVISIBILITY,
    SPELL_INVULNERABILITY,
    SPELL_LESSER_HEAL,
    SPELL_LIGHT,
    SPELL_CHAIN_LIGHTNING,
    SPELL_LOCK,
    SPELL_MARK,
    SPELL_MARK_1,
    SPELL_MARK_2,
    SPELL_MARK_3,
    SPELL_MARK_4,
    SPELL_MAGIC_MISSILE,
    SPELL_SHIELD,
    SPELL_METEOR,
    SPELL_METEOR_SHOWER,
    SPELL_MOONGLOW,
    SPELL_NULLIFY,
    SPELL_MANA_BOMB,
    SPELL_PHANTOM,
    SPELL_PIXIE_SWARM,
    SPELL_PLASMA,
    SPELL_POISON,
    SPELL_PROTECTION_FROM_ELECTRICITY,
    SPELL_PROTECTION_FROM_FIRE,
    SPELL_PROTECTION_FROM_MAGIC,
    SPELL_PROTECTION_FROM_POISON,
    SPELL_PULL,
    SPELL_PUSH,
    SPELL_OVAL_SHIELD,
    SPELL_RESTORE_HEALTH,
    SPELL_RESTORE_MANA,
    SPELL_RUN,
    SPELL_SHOCK,
    SPELL_SLOW,
    SPELL_SMALL_ZAP,
    SPELL_STUN,
    SPELL_SUMMON_BAT,
    SPELL_SUMMON_BLACK_BEAR,
    SPELL_SUMMON_BEAR,
    SPELL_SUMMON_BEHOLDER,
    SPELL_SUMMON_BOMBER,
    SPELL_SUMMON_CARNIVOROUS_PLANT,
    SPELL_SUMMON_ALBINO_SPIDER,
    SPELL_SUMMON_SMALL_ALBINO_SPIDER,
    SPELL_SUMMON_EVIL_CHERUB,
    SPELL_SUMMON_EMBER_DEMON,
    SPELL_SUMMON_GHOST,
    SPELL_SUMMON_GIANT_LEECH,
    SPELL_SUMMON_IMP,
    SPELL_SUMMON_MECHANICAL_FLYER,
    SPELL_SUMMON_MECHANICAL_GOLEM,
    SPELL_SUMMON_MIMIC,
    SPELL_SUMMON_OGRE,
    SPELL_SUMMON_OGRE_BRUTE,
    SPELL_SUMMON_OGRE_WARLORD,
    SPELL_SUMMON_SCORPION,
    SPELL_SUMMON_SHADE,
    SPELL_SUMMON_SKELETON,
    SPELL_SUMMON_SKELETON_LORD,
    SPELL_SUMMON_SPIDER,
    SPELL_SUMMON_SMALL_SPIDER,
    SPELL_SUMMON_SPITTING_SPIDER,
    SPELL_SUMMON_STONE_GOLEM,
    SPELL_SUMMON_TROLL,
    SPELL_SUMMON_URCHIN,
    SPELL_SUMMON_WASP,
    SPELL_SUMMON_WILLOWISP,
    SPELL_SUMMON_WOLF,
    SPELL_SUMMON_BLACK_WOLF,
    SPELL_SUMMON_WHITE_WOLF,
    SPELL_SUMMON_ZOMBIE,
    SPELL_SUMMON_VILE_ZOMBIE,
    SPELL_SUMMON_DEMON,
    SPELL_SUMMON_LICH,
    SPELL_SUMMON_DRYAD,
    SPELL_SUMMON_URCHIN_SHAMAN,
    SPELL_SWAP,
    SPELL_TAG,
    SPELL_TELEPORT_OTHER_TO_MARK_1,
    SPELL_TELEPORT_OTHER_TO_MARK_2,
    SPELL_TELEPORT_OTHER_TO_MARK_3,
    SPELL_TELEPORT_OTHER_TO_MARK_4,
    SPELL_TELEPORT_POP,
    SPELL_TELEPORT_TO_MARK_1,
    SPELL_TELEPORT_TO_MARK_2,
    SPELL_TELEPORT_TO_MARK_3,
    SPELL_TELEPORT_TO_MARK_4,
    SPELL_TELEPORT_TO_TARGET,
    SPELL_TELEKINESIS,
    SPELL_TOXIC_CLOUD,
    SPELL_TRIGGER_GLYPH,
    SPELL_VAMPIRISM,
    SPELL_VILLAIN,
    SPELL_WALL,
    SPELL_WINK,
    SPELL_SUMMON_CREATURE,
    SPELL_MARK_LOCATION,
    SPELL_TELEPORT_TO_MARKER
};


class SpellMgr
{
public:
	SpellMgr();
	~SpellMgr();

	SpellTableMap spellTable;
	AbilityTableMap abilityTable;
};

#define spellmgr MaNGOS::Singleton<SpellMgr>::Instance()
#endif