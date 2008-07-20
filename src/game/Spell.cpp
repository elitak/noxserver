/* 
* Copyright (C) 2007 NoxForum.net <http://www.noxforum.net/>
*00 05 27 72 36 A4 94 DE 05 26 12 50 00    00 30 7B 06 F3 12 20 06 00 00 00       39 A5 81 09 // before try spell
00 05 27 73 36 A4 94 DE 05 26 12 50 00    00 30 80 06 ED 12 20 06 00 00 00       39 A5 81 09 
00 05 27 74 36 A4 94 DE 05 26 12 D0 00 15 00 30 85 06 E7 12 20 06 00 00 00 70 33 39 A5 81 08 A6 14 13 71 // spell start 33
00 05 27 76 36 A4 94 DE 05 28 12 D0 00 15 00 30 8F 06 DA 12 20 06 00 00 00       39 A5 81 07
00 05 27 77 36 A4 94 DE 05 28 12 D0 01 15 00 30 94 06 D3 12 20 06 00 FF 98 12 C9 02 92 0A 57 09 20 06 00 00 00 D7 94 08 DA 00 39 A5 81 07 5E 98 12 00
00 05 27 78 36 A4 94 DE 05 28 12 D0 01 15 00 30 99 06 CD 12 20 06 00 00 00       39 A5 81 06 
00 05 27 79 36 A4 94 DE 05 28 12 D0 01 15 00 30 9E 06 C6 12 20 06 00 00 00       39 A5 81 06 
00 05 27 7E 36 A4 94 DE 05 28 12 D0 03 15 00 30 BB 06 A9 12 40 06 00 00 00       39 A5 81 03 
00 05 27 7F 36 A4 94 DE 05 28 12 D0 03 15 00 30 BD 06 A8 12 40 06 00 FF 0B 12 C9 02 B8 0B 5F 0E 10 06 00 00 00                39 A5 81 03 5E 0B 12 00 AA CC EC 00 07 5A 0B 12 00 20 80 00 // enchantment
00 05 27 80 36 A4 94 DE 05 28 12 D0 04 15 00 30 C0 06 A8 12 40 06 00 00 00       39 A5 81 02 A6 1B 13 71 
00 05 27 81 36 A4 94 DE 05 28 12 D0 04 15 00 30 C4 06 A6 12 40 06 00 00 00       39 A5 81 02 5E 30 0D 0C 
00 05 27 82 36 A4 94 DE 05 28 12 30 04    00 30 CA 06 A4 12 40 06 00 00 00       39 A5 81 01 5E 30 0D 0A 
// ??
00 05 27 8C 36 A4 94 B0 05 0C 12 00 06    00 30 DE 06 9F 12 C0 00 28 00 00 00 | A9 01 33 00 00 00 | A7 00 81 C4 | A7 00 81 C4 | AA | 45 94 08 3D 00 | CC ED 00 07 | 5A 94 08 00 00 80 04 // enchantment // inform spell cast success // report mana


// duration spell example
00 05 27 0B 60 A4 94 A2 0B AE 07 70 00 00 00 00 AA CC 83 0B 07 //9E 07 00 37 02 F7 03 //
// couple packets later
00 05 27 0E 60 A4 94 A2 0B AE 07 70 00 00 00 00 AA CC 88 0B 07 //9E 0E 00 37 02 F7 03 //
// start again
00 05 27 34 60 A4 94 A2 0B AE 07 50 00 00 00 00 AA CC 8D 0B 07 //9E 07 00 30 0D 7F 04 //
//
00 05 27 5C 60 A4 94 A2 0B AE 07 30 00 00 00 00 A9 0E 37 02 00 00 30 0D 02 00 02 AA CC 90 0B 07 //9E 0E 00 30 0D 7F 04// CC 91 0B 0B 4E 37 02 0A 00 


//blink
00 05 27 61 36 A4 94 48 0B 12 0B 50 06 00 02 52 0B B9 0B 40 00 00                               98 F1 08 24 0B 20 06 00 00 00  //before
00 05 27 62 36 A4 94 42 0B 14 0B 50 06 00 02 52 0B B9 0B 40 00 00 FF 76 0E D4 03 D0 0B FB 0C 00 98 F5 08 20 0B 20 04 00 00 00 
00 05 27 63 36 A4 94 3C 0B 18 0B 50 06 00 02 52 0B B9 0B 40 00 00                               98 F8 08 1D 0B 20 06 00 00 00 
00 05 27 64 36 A4 94 38 0B 1A 0B 50 04 00 02 52 0B B9 0B 40 00 00 FF FF 0A 77 04 9F 0A C9 0C 00 98 FC 08 19 0B 20 06 00 00 00 | 70 04 // spell start 04
00 05 27 65 36 A4 94 34 0B 1C 0B 50 06 00 02 52 0B B9 0B 40 00 00                               98 01 09 14 0B 20 06 00 00 00 | A7 00 0D C9 
00 05 27 66 36 A4 94 30 0B 1E 0B 50 06 00 02 52 0B B9 0B 40 00 00                               98 05 09 0F 0B 20 06 00 00 00 | 39 C8 80 0F 
00 05 27 67 36 A4 94 2A 0B 22 0B 50 06 00 02 52 0B B9 0B 40 00 00 FF 57 0F 93 04 B0 0A B2 0C 00 98 09 09 0A 0B 20 06 00 FF 17 0C C9 02 04 02 93 12 60 00 00 00 00 | D7 EA 0E 27 00 39 C8 80 0F 5E 17 0C 00 
00 05 27 68 36 A4 94 24 0B 24 0B 50 06 00 02 52 0B B9 0B 40 00 00                               98 0E 09 05 0B 20 06 00 00 00                                     |                39 C8 80 0E A7 00 42 C9 
00 05 27 69 36 A4 94 1E 0B 28 0B 50 06 00 02 52 0B B9 0B 40 00 00                               98 13 09 00 0B 20 06 00 00 00                                     |                39 C8 80 0E 
00 05 27 6A 36 A4 94 18 0B 2A 0B 50 06 00 02 52 0B B9 0B 40 00 00                               98 17 09 FB 0A 20 06 00 00 00 39 C8 80 0D 
00 05 27 6B 36 A4 94 12 0B 2E 0B 50 00 00 02 52 0B B9 0B 40 00 00                               98 1C 09 F5 0A 20 06 00 00 00 5E 94 08 00 39 C8 80 0D 
00 05 27 6C 36 A4 94 0E 0B 30 0B 50 00 00 FF 02 00 C9 02 52 0B B9 0B 40 00 00 98 20 09 F0 0A 20 06 00 00 00 39 C8 80 0C 
00 05 27 6D 36 A4 94 E2 05 26 12 50 00 00 00 00 A9 01 04 00 00 00                                                             89 0B 0B 32 0B | 89 0B 0B 32 0B | 89 E2 05 26 12 | A7 00 07 C8 | AA CC E4 00 05 | EA 6D 36 96 01 | DF 04 00 00 00 0F | 45 94 08 8C 00 
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


#include "Policies/SingletonImp.h"
#include "Spell.h"
#include "WorldSession.h"
#include "Config/GameConfig.h"

INSTANTIATE_SINGLETON_1(SpellMgr);

SpellMgr::SpellMgr()
{
	FillSpellHandlerHashTable();
	FillAbilityHandlerHashTable();
}

SpellMgr::~SpellMgr()
{
}
// TODO: Make a manacost table
bool WorldSession::ExecuteSpell(uint8 spellId, bool dontinvert)
{
    //We need to calculate wait time by counting phenomes
	if(spellmgr.HasEnoughMana(GetPlayer(), 30)) // change this to represent the actual mana for spell
	{
		if(dontinvert) // execute spell now, also check if the spell should be on others only, maybe change this to an xor
			spellmgr.DoSpellEffect(spellId, GetPlayer(), NULL);
		else
			spellmgr.DoSpellProjectile(spellId, GetPlayer(), false); // last argument should be whether to target friendlies
		GetPlayer()->ManaDrain(30); // again replace with actual mana
	}
	else
		return false;

	return true;
}

void SpellMgr::DoSpellEffect(uint8 spellId, Player* caster, Player* target)
{
	SpellTableMap::const_iterator iter = spellmgr.spellTable.find( spellId );
	if(iter == spellmgr.spellTable.end())
	{
		sLog.outError( "SESSION: received unhandled spell (0x%.2X)",
			spellId);
	}
	else
	{
		(spellmgr.*iter->second.handler)(caster, target);
	}
}

void SpellMgr::DoSpellProjectile(uint8 spellId, Player* caster, bool friendly)
{
	Magic* projectile = new Magic(spellId, caster, friendly);
}

bool WorldSession::ExecuteAbility(uint8 abilityId)
{
	AbilityTableMap::const_iterator iter = spellmgr.abilityTable.find( abilityId );
	if(iter == spellmgr.abilityTable.end())
	{
		sLog.outError( "SESSION: received unhandled ability (0x%.2X)",
			abilityId);
		return false;
	}
	else
	{
		(spellmgr.*iter->second.handler)(GetPlayer());
	}

	return true;
}

void SpellMgr::FillSpellHandlerHashTable()
{
	if(!spellTable.empty())
		return;

	spellTable[ SPELL_ANCHOR ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_ARACHNAPHOBIA ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_BLIND ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_BLINK ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_BURN ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_CANCEL ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_CHAIN_LIGHTNING_BOLT ]	= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_CHANNEL_LIFE ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_CHARM ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_CLEANSING_FLAME ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_CLEANSING_MANA_FLAME ]	= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_CONFUSE ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_COUNTERSPELL ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_CURE_POISON ]			= SpellHandler( 0, &SpellMgr::HandleSpellCurePoison );
	spellTable[ SPELL_DEATH ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_DEATH_RAY ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_DETECT_MAGIC ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_DETONATE ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_DETONATE_GLYPHS ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_DISENCHANT_ALL ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TURN_UNDEAD ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_DRAIN_MANA ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_EARTHQUAKE ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_LIGHTNING ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_EXPLOSION ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_FEAR ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_FIREBALL ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_FIREWALK ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_FIST ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_FORCE_FIELD ]			= SpellHandler( 0, &SpellMgr::HandleSpellForceField );
	spellTable[ SPELL_FORCE_OF_NATURE ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_FREEZE ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_FUMBLE ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_GLYPH ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_GREATER_HEAL ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_HASTE ]					= SpellHandler( 0, &SpellMgr::HandleSpellHaste );
	spellTable[ SPELL_INFRAVISION ]			= SpellHandler( 0, &SpellMgr::HandleSpellInfravision );
	spellTable[ SPELL_INVERSION ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_INVISIBILITY ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_INVULNERABILITY ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_LESSER_HEAL ]			= SpellHandler( 0, &SpellMgr::HandleSpellLesserHeal );
	spellTable[ SPELL_LIGHT ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_CHAIN_LIGHTNING ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_LOCK ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_MARK ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_MARK_1 ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_MARK_2 ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_MARK_3 ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_MARK_4 ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_MAGIC_MISSILE ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SHIELD ]					= SpellHandler( 0, &SpellMgr::HandleSpellForceField );
	spellTable[ SPELL_METEOR ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_METEOR_SHOWER ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_MOONGLOW ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_NULLIFY ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_MANA_BOMB ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_PHANTOM ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_PIXIE_SWARM ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_PLASMA ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_POISON ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_PROTECTION_FROM_ELECTRICITY ]	= SpellHandler( 0, &SpellMgr::HandleSpellProtectFromElectricity );
	spellTable[ SPELL_PROTECTION_FROM_FIRE ]	= SpellHandler( 0, &SpellMgr::HandleSpellProtectFromFire );
	spellTable[ SPELL_PROTECTION_FROM_MAGIC ]	= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_PROTECTION_FROM_POISON ]	= SpellHandler( 0, &SpellMgr::HandleSpellProtectFromPoison );
	spellTable[ SPELL_PULL ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_PUSH ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_OVAL_SHIELD ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_RESTORE_HEALTH ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_RESTORE_MANA ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_RUN ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SHOCK ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SLOW ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SMALL_ZAP ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_STUN ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_BAT ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_BLACK_BEAR ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_BEAR ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_BEHOLDER ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_BOMBER ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_CARNIVOROUS_PLANT ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_ALBINO_SPIDER ]	= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_SMALL_ALBINO_SPIDER ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_EVIL_CHERUB ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_EMBER_DEMON ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_GHOST ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_GIANT_LEECH ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_IMP ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_MECHANICAL_FLYER ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_MECHANICAL_GOLEM ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_MIMIC ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_OGRE ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_OGRE_BRUTE ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_OGRE_WARLORD ]	= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_SCORPION ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_SHADE ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_SKELETON ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_SKELETON_LORD ]	= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_SPIDER ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_SMALL_SPIDER ]	= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_SPITTING_SPIDER ]	= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_STONE_GOLEM ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_TROLL ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_URCHIN ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_WASP ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_WILLOWISP ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_WOLF ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_BLACK_WOLF ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_WHITE_WOLF ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_ZOMBIE ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_VILE_ZOMBIE ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_DEMON ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_LICH ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_DRYAD ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_URCHIN_SHAMAN ]	= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SWAP ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TAG ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_OTHER_TO_MARK_1 ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_OTHER_TO_MARK_2 ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_OTHER_TO_MARK_3 ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_OTHER_TO_MARK_4 ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_POP ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_TO_MARK_1 ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_TO_MARK_2 ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_TO_MARK_3 ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_TO_MARK_4 ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_TO_TARGET ]		= SpellHandler( 0, &SpellMgr::HandleSpellTeleportToTarget );
	spellTable[ SPELL_TELEKINESIS ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TOXIC_CLOUD ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TRIGGER_GLYPH ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_VAMPIRISM ]				= SpellHandler( 0, &SpellMgr::HandleSpellVampirism );
	spellTable[ SPELL_VILLAIN ]				= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_WALL ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_WINK ]					= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_SUMMON_CREATURE ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_MARK_LOCATION ]			= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );
	spellTable[ SPELL_TELEPORT_TO_MARKER ]		= SpellHandler( 0, &SpellMgr::HandleSpellUnknown );}
void SpellMgr::FillAbilityHandlerHashTable()
{
	if(!abilityTable.empty())
		return;

	abilityTable[ ABILITY_BERSERKER_CHARGE ]	= AbilityHandler( &SpellMgr::HandleBerserkerChargeAbility );
	abilityTable[ ABILITY_WARCRY ]				= AbilityHandler( &SpellMgr::HandleWarcryAbility );
	abilityTable[ ABILITY_HARPOON ]			= AbilityHandler( &SpellMgr::HandleAbilityUnknown );
	abilityTable[ ABILITY_TREAD_LIGHTLY ]		= AbilityHandler( &SpellMgr::HandleTreadLightlyAbility );
	abilityTable[ ABILITY_EYE_OF_THE_WOLF ]	= AbilityHandler( &SpellMgr::HandleEyeOfWolfAbility );
}
/***********************************************
*    Abilities    Abilities    Abilities    Abilities
*    Abilities    Abilities    Abilities    Abilities
*    Abilities    Abilities    Abilities    Abilities
*    Abilities    Abilities    Abilities    Abilities
*    Abilities    Abilities    Abilities    Abilities
***********************************************/
void SpellMgr::HandleBerserkerChargeAbility(Player *plr)
{
	GridPair cursor = plr->GetSession()->GetCursor();
	int duration = sGameConfig.GetFloatDefault("BerserkerChargeDuration",90);
	plr->SetActionAnim(ACTION_BERSERKER_CHARGE, duration); //frames is from gamedata.bin
	plr->MoveToward(cursor.x_coord, cursor.y_coord, 0.42);
	int delay = sGameConfig.GetFloatDefault("BerserkerChargeDelay",300);
	plr->SetAbilityDelay(ABILITY_BERSERKER_CHARGE, delay);	
	plr->EmitSound(SOUND_BERSERKERCHARGEINVOKE);
	plr->EmitSound(SOUND_BERSERKERCHARGEON);

}
void SpellMgr::HandleTreadLightlyAbility(Player *plr)
{
	int duration = sGameConfig.GetFloatDefault("TreadLightlyDuration",99999);
	plr->SetEnchant(ENCHANT_SNEAK,duration);
	int delay = sGameConfig.GetFloatDefault("TreadLightlyDelay",30);
	plr->SetAbilityDelay(ABILITY_TREAD_LIGHTLY,delay);	
	plr->SetTreadLightlyMarker(plr->GetPosition());
	plr->EmitSound(SOUND_TREADLIGHTLYINVOKE);
	plr->EmitSound(SOUND_TREADLIGHTLYON);
}

void SpellMgr::HandleWarcryAbility(Player *plr)
{
	int duration = sGameConfig.GetFloatDefault("WarcryDuration",120);
	int delay = sGameConfig.GetFloatDefault("WarcryDelay",300);
	plr->SetAbilityDelay(ABILITY_WARCRY,delay);	
	plr->SetActionAnim(ACTION_WARCRY, 15);
	plr->EmitSound(SOUND_WARCRYINVOKE);
	plr->EmitSound(SOUND_WARCRYON);
}

void SpellMgr::HandleEyeOfWolfAbility(Player *plr)
{
	int duration = sGameConfig.GetFloatDefault("EyeOfTheWolfDuration",300);
	plr->SetEnchant(ENCHANT_INFRAVISION,duration);
	int delay = sGameConfig.GetFloatDefault("EyeOfTheWolfDelay",600);
	plr->SetAbilityDelay(ABILITY_EYE_OF_THE_WOLF, delay);	
	plr->EmitSound(SOUND_TROLLFLATUS);
    //SOUND_EYEOFTHEWOLFINVOKE
}
/***********************************************
*    Spells    Spells    Spells    Spells
*    Spells    Spells    Spells    Spells
*    Spells    Spells    Spells    Spells
*    Spells    Spells    Spells    Spells
************************************************/

bool SpellMgr::HasEnoughMana(Player *plr, int cost)
{
	if(plr->GetMana()>=cost)
	{
		return true;
	}
	//code to say "Don't have enough mana for that spell
    //This makes it so it only sends the audio event to the player, and not
    //the whole server, like EmitSound does
	plr->GetSession()->_SendAudioPlayerEvent(SOUND_MANAEMPTY,100,0);
	return false;
}
void SpellMgr::HandleSpellCurePoison(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	target->Poison(0,0);
}
void SpellMgr::HandleSpellDeathRay(Player* caster, Player* target)
{
    //Doesn't work due to spell projectiles at the moment.
    if(target == NULL)
		target = caster;
	GridPair mouse = target->GetSession()->GetCursor();
	if(target->CanSeePoint((uint16)(mouse.x_coord),(uint16)(mouse.y_coord),0))
	{
        /*DeathRayOutRadius = 15;
        DeathRayInRadius = 10;
        SOLO DeathRayDamage = 90;
        ARENA DeathRayDamage = 100;*/
        caster->GetSession()->_SendFxDeathRaySpell(caster->GetPositionX(),
                                                    caster->GetPositionY(),
                                                    mouse.x_coord,
                                                    mouse.y_coord);
        caster->EmitSound(SOUND_DEATHRAYCAST);
	}
}
void SpellMgr::HandleSpellForceField(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	int duration = 1200;
	target->SetEnchant(ENCHANT_SHIELD,duration);
}

void SpellMgr::HandleSpellHaste(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	int duration = sGameConfig.GetFloatDefault("HasteEnchantDuration",600);
	target->SetEnchant(ENCHANT_HASTED,duration);

	//25 for spell power 3. power of haste is loaded in thing.bin
	//Where can mana cost be found? (other than the spellbook)
}
void SpellMgr::HandleSpellInfravision(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	int duration = sGameConfig.GetFloatDefault("InfravisionEnchantDuration",900);
	target->SetEnchant(ENCHANT_INFRAVISION,duration);


}
void SpellMgr::HandleSpellLesserHeal(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	float amount = sGameConfig.GetFloatDefault("LesserHealAmount", 15);
	target->Heal(amount);
    target->EmitSound(SOUND_LESSERHEALEFFECT);
}
void SpellMgr::HandleSpellProtectFromFire(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	int duration = sGameConfig.GetFloatDefault("ProtectFireEnchantDuration",1800);
	target->SetEnchant(ENCHANT_PROTECT_FROM_FIRE,duration);

}
void SpellMgr::HandleSpellProtectFromElectricity(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	int duration = sGameConfig.GetFloatDefault("ProtectElectricityEnchantDuration",1800);
	target->SetEnchant(ENCHANT_PROTECT_FROM_ELECTRICITY,duration);


}
void SpellMgr::HandleSpellProtectFromPoison(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	int duration = sGameConfig.GetFloatDefault("ProtectPoisonEnchantDuration",1800);
	target->SetEnchant(ENCHANT_PROTECT_FROM_POISON,duration);
}
void SpellMgr::HandleSpellShock(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	int duration = sGameConfig.GetFloatDefault("ShockPoisonEnchantDuration",1800);
	target->SetEnchant(ENCHANT_SHOCK,duration);
}

void SpellMgr::HandleSpellTeleportToTarget(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	GridPair mouse = target->GetSession()->GetCursor();
	if(target->CanSeePoint((uint16)(mouse.x_coord),(uint16)(mouse.y_coord),0))
	{
	   target->SetPosition(mouse);
	}
}
void SpellMgr::HandleSpellVampirism(Player* caster, Player* target)
{
	if(target == NULL)
		target = caster;
	int duration = sGameConfig.GetFloatDefault("VampirismDuration",900);
	target->SetEnchant(ENCHANT_VAMPIRISM,duration);
}

Magic::Magic(uint8 spell, Player* owner, bool friendly) : WorldObject(0x3BB, owner->GetPosition(), -1), m_spell (spell), m_owner (owner), m_friendly (friendly)
{
	//uint16 type = sThingBin.Thing.Object.GetIndex("Magic");
}

// Magic object has just hit player
void Magic::Collide(Player* player)
{
	spellmgr.DoSpellEffect(m_spell, m_owner, player);
}

void Magic::Update(uint32 time)
{
	SetPosition(GetPosition());
}

const char *g_spellNames[] =
{
	"SPELL_INVALID",
	"SPELL_ANCHOR",
	"SPELL_ARACHNAPHOBIA",
	"SPELL_BLIND",
	"SPELL_BLINK",
	"SPELL_BURN",
	"SPELL_CANCEL",
	"SPELL_CHAIN_LIGHTNING_BOLT",
	"SPELL_CHANNEL_LIFE",
	"SPELL_CHARM",
	"SPELL_CLEANSING_FLAME",
	"SPELL_CLEANSING_MANA_FLAME",
	"SPELL_CONFUSE",
	"SPELL_COUNTERSPELL",
	"SPELL_CURE_POISON",
	"SPELL_DEATH",
	"SPELL_DEATH_RAY",
	"SPELL_DETECT_MAGIC",
	"SPELL_DETONATE",
	"SPELL_DETONATE_GLYPHS",
	"SPELL_DISENCHANT_ALL",
	"SPELL_TURN_UNDEAD",
	"SPELL_DRAIN_MANA",
	"SPELL_EARTHQUAKE",
	"SPELL_LIGHTNING",
	"SPELL_EXPLOSION",
	"SPELL_FEAR",
	"SPELL_FIREBALL",
	"SPELL_FIREWALK",
	"SPELL_FIST",
	"SPELL_FORCE_FIELD",
	"SPELL_FORCE_OF_NATURE",
	"SPELL_FREEZE",
	"SPELL_FUMBLE",
	"SPELL_GLYPH",
	"SPELL_GREATER_HEAL",
	"SPELL_HASTE",
	"SPELL_INFRAVISION",
	"SPELL_INVERSION",
	"SPELL_INVISIBILITY",
	"SPELL_INVULNERABILITY",
	"SPELL_LESSER_HEAL",
	"SPELL_LIGHT",
	"SPELL_CHAIN_LIGHTNING",
	"SPELL_LOCK",
	"SPELL_MARK",
	"SPELL_MARK_1",
	"SPELL_MARK_2",
	"SPELL_MARK_3",
	"SPELL_MARK_4",
	"SPELL_MAGIC_MISSILE",
	"SPELL_SHIELD",
	"SPELL_METEOR",
	"SPELL_METEOR_SHOWER",
	"SPELL_MOONGLOW",
	"SPELL_NULLIFY",
	"SPELL_MANA_BOMB",
	"SPELL_PHANTOM",
	"SPELL_PIXIE_SWARM",
	"SPELL_PLASMA",
	"SPELL_POISON",
	"SPELL_PROTECTION_FROM_ELECTRICITY",
	"SPELL_PROTECTION_FROM_FIRE",
	"SPELL_PROTECTION_FROM_MAGIC",
	"SPELL_PROTECTION_FROM_POISON",
	"SPELL_PULL",
	"SPELL_PUSH",
	"SPELL_OVAL_SHIELD",
	"SPELL_RESTORE_HEALTH",
	"SPELL_RESTORE_MANA",
	"SPELL_RUN",
	"SPELL_SHOCK",
	"SPELL_SLOW",
	"SPELL_SMALL_ZAP",
	"SPELL_STUN",
	"SPELL_SUMMON_BAT",
	"SPELL_SUMMON_BLACK_BEAR",
	"SPELL_SUMMON_BEAR",
	"SPELL_SUMMON_BEHOLDER",
	"SPELL_SUMMON_BOMBER",
	"SPELL_SUMMON_CARNIVOROUS_PLANT",
	"SPELL_SUMMON_ALBINO_SPIDER",
	"SPELL_SUMMON_SMALL_ALBINO_SPIDER",
	"SPELL_SUMMON_EVIL_CHERUB",
	"SPELL_SUMMON_EMBER_DEMON",
	"SPELL_SUMMON_GHOST",
	"SPELL_SUMMON_GIANT_LEECH",
	"SPELL_SUMMON_IMP",
	"SPELL_SUMMON_MECHANICAL_FLYER",
	"SPELL_SUMMON_MECHANICAL_GOLEM",
	"SPELL_SUMMON_MIMIC",
	"SPELL_SUMMON_OGRE",
	"SPELL_SUMMON_OGRE_BRUTE",
	"SPELL_SUMMON_OGRE_WARLORD",
	"SPELL_SUMMON_SCORPION",
	"SPELL_SUMMON_SHADE",
	"SPELL_SUMMON_SKELETON",
	"SPELL_SUMMON_SKELETON_LORD",
	"SPELL_SUMMON_SPIDER",
	"SPELL_SUMMON_SMALL_SPIDER",
	"SPELL_SUMMON_SPITTING_SPIDER",
	"SPELL_SUMMON_STONE_GOLEM",
	"SPELL_SUMMON_TROLL",
	"SPELL_SUMMON_URCHIN",
	"SPELL_SUMMON_WASP",
	"SPELL_SUMMON_WILLOWISP",
	"SPELL_SUMMON_WOLF",
	"SPELL_SUMMON_BLACK_WOLF",
	"SPELL_SUMMON_WHITE_WOLF",
	"SPELL_SUMMON_ZOMBIE",
	"SPELL_SUMMON_VILE_ZOMBIE",
	"SPELL_SUMMON_DEMON",
	"SPELL_SUMMON_LICH",
	"SPELL_SUMMON_DRYAD",
	"SPELL_SUMMON_URCHIN_SHAMAN",
	"SPELL_SWAP",
	"SPELL_TAG",
	"SPELL_TELEPORT_OTHER_TO_MARK_1",
	"SPELL_TELEPORT_OTHER_TO_MARK_2",
	"SPELL_TELEPORT_OTHER_TO_MARK_3",
	"SPELL_TELEPORT_OTHER_TO_MARK_4",
	"SPELL_TELEPORT_POP",
	"SPELL_TELEPORT_TO_MARK_1",
	"SPELL_TELEPORT_TO_MARK_2",
	"SPELL_TELEPORT_TO_MARK_3",
	"SPELL_TELEPORT_TO_MARK_4",
	"SPELL_TELEPORT_TO_TARGET",
	"SPELL_TELEKINESIS",
	"SPELL_TOXIC_CLOUD",
	"SPELL_TRIGGER_GLYPH",
	"SPELL_VAMPIRISM",
	"SPELL_VILLAIN",
	"SPELL_WALL",
	"SPELL_WINK",
	"SPELL_SUMMON_CREATURE",
	"SPELL_MARK_LOCATION",
	"SPELL_TELEPORT_TO_MARKER"
};

const char* g_abilityNames[] =
{
	"ABILITY_INVALID",
	"ABILITY_BERSERKER_CHARGE",
	"ABILITY_WARCRY",
	"ABILITY_HARPOON",
	"ABILITY_TREAD_LIGHTLY",
	"ABILITY_EYE_OF_THE_WOLF"
};
