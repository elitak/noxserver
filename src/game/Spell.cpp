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

bool WorldSession::ExecuteSpell(uint8 spellId, bool dontinvert)
{
	SpellTableMap::const_iterator iter = spellmgr.spellTable.find( spellId );
	if(iter == spellmgr.spellTable.end())
	{
		sLog.outError( "SESSION: received unhandled spell (0x%.2X)",
			spellId);
		return false;
	}
	else
	{
	    (spellmgr.*iter->second.handler)(GetPlayer(), dontinvert);
	}

	return true;
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
	abilityTable[ ABILITY_WARCRY ]				= AbilityHandler( &SpellMgr::HandleAbilityUnknown );
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
	
}
void SpellMgr::HandleTreadLightlyAbility(Player *plr)
{
     int duration = sGameConfig.GetFloatDefault("TreadLightlyDuration",99999);
     plr->SetEnchant(ENCHANT_SNEAK,duration);
     int delay = sGameConfig.GetFloatDefault("TreadLightlyDelay",30);
     plr->SetAbilityDelay(ABILITY_TREAD_LIGHTLY,delay);
}

void SpellMgr::HandleEyeOfWolfAbility(Player *plr)
{
     int duration = sGameConfig.GetFloatDefault("EyeOfTheWolfDuration",300);
     plr->SetEnchant(ENCHANT_INFRAVISION,duration);
	int delay = sGameConfig.GetFloatDefault("EyeOfTheWolfDelay",600);
     plr->SetAbilityDelay(ABILITY_EYE_OF_THE_WOLF, delay);
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
      //code to say "Don't have enough mana for that spell. don't forget the noise
      return false;
}
void SpellMgr::HandleSpellCurePoison(Player *plr, bool dontinvert)
{
     int cost = 30;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          plr->Poison(0,0);
     }
}
void SpellMgr::HandleSpellForceField(Player *plr, bool dontinvert)
{
     int cost = 80;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          int duration = 1200;
          plr->SetEnchant(ENCHANT_SHIELD,duration);
          plr->ManaDrain(cost);
     }
}

void SpellMgr::HandleSpellHaste(Player *plr, bool dontinvert)
{
     //will still need to handle hasting other players.
     //for now, too bad so sad.
     int cost = 10;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          int duration = sGameConfig.GetFloatDefault("HasteEnchantDuration",600);
          plr->SetEnchant(ENCHANT_HASTED,duration);
          plr->ManaDrain(cost);
          //25 for spell power 3. power of haste is loaded in thing.bin
          //Where can mana cost be found? (other than the spellbook)
     }
}
void SpellMgr::HandleSpellInfravision(Player *plr, bool dontinvert)
{
     int cost = 30;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          int duration = sGameConfig.GetFloatDefault("InfravisionEnchantDuration",900);
          plr->SetEnchant(ENCHANT_INFRAVISION,duration);
          plr->ManaDrain(cost);
     }
}
void SpellMgr::HandleSpellLesserHeal(Player *plr, bool dontinvert)
{
     int cost = 30;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          int amount = sGameConfig.GetFloatDefault("LesserHealAmount", 15);
          plr->Heal(amount);
          plr->ManaDrain(cost);
     }
}
void SpellMgr::HandleSpellProtectFromFire(Player *plr, bool dontinvert)
{
     int cost = 30;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          int duration = sGameConfig.GetFloatDefault("ProtectFireEnchantDuration",1800);
          plr->SetEnchant(ENCHANT_PROTECT_FROM_FIRE,duration);
          plr->ManaDrain(cost);
     }
}
void SpellMgr::HandleSpellProtectFromElectricity(Player *plr, bool dontinvert)
{
     int cost = 30;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          int duration = sGameConfig.GetFloatDefault("ProtectElectricityEnchantDuration",1800);
          plr->SetEnchant(ENCHANT_PROTECT_FROM_ELECTRICITY,duration);
          plr->ManaDrain(cost);
     }
}
void SpellMgr::HandleSpellProtectFromPoison(Player *plr, bool dontinvert)
{
     int cost = 30;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          int duration = sGameConfig.GetFloatDefault("ProtectPoisonEnchantDuration",1800);
          plr->SetEnchant(ENCHANT_PROTECT_FROM_POISON,duration);
          plr->ManaDrain(cost);
          plr->GetSession()->_SendAudioPlayerEvent(SOUND_PROTECTIONFROMELECTRICITYEFFECT);
     }
}
void SpellMgr::HandleSpellShock(Player* plr, bool dontinvert)
{
     int cost = 30;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          int duration = sGameConfig.GetFloatDefault("ShockPoisonEnchantDuration",1800);
          plr->SetEnchant(ENCHANT_SHOCK,duration);
          plr->ManaDrain(cost);
     }
}

void SpellMgr::HandleSpellTeleportToTarget(Player* plr, bool dontinvert)
{
     int cost = 20;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          GridPair mouse = plr->GetSession()->GetCursor();
          if(plr->CanSeePoint((uint16)(mouse.x_coord),(uint16)(mouse.y_coord),0))
          {
               plr->SetPosition(mouse);
               plr->ManaDrain(cost);
          }
     }
}
void SpellMgr::HandleSpellVampirism(Player *plr, bool dontinvert)
{
     int cost = 20;
     if(!HasEnoughMana(plr,cost))
          return;
     if(dontinvert)
     {
          int duration = sGameConfig.GetFloatDefault("VampirismDuration",900);
          plr->SetEnchant(ENCHANT_VAMPIRISM,duration);
          plr->ManaDrain(cost);
     }
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
