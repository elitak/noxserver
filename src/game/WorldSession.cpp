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

/** \file
    \ingroup u2w
*/

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Log.h"
#include "Opcodes.h"
#include "WorldSocket.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Player.h"
#include "NameTables.h"
#include "Language.h"   // for CMSG_DISMOUNT handler
#include "ObjectAccessor.h"
#include "Spell.h"
#include "WorldLog.h"
#include "NoxThinglib.h"

#include <cmath>

#define TIMEOUT_MS 5000

/// WorldSession constructor
WorldSession::WorldSession(uint32 id, WorldSocket *sock, uint32 sec) : _player(NULL), _socket(sock),
_security(sec), _accountId(id), _logoutTime(0), m_playerLoading(false), m_playerRecentlyLogout(false),
m_status(STATUS_AUTHED), xorKey(0), m_timestamp(1000), m_unk(0), m_playerObserving(true), m_playerDownloading(0),
m_connTimer(TIMEOUT_MS)
{
    FillOpcodeHandlerHashTable();
}

/// WorldSession destructor
WorldSession::~WorldSession()
{
    ///- unload player if not unloaded
    if(_player)
        LogoutPlayer(true);

    ///- empty incoming packet queue
    while(!_recvQueue.empty())
    {
        WorldPacket *packet = _recvQueue.next();
        delete packet;
    }
}

void WorldSession::FillOpcodeHandlerHashTable()
{
	//if table is already filled
	if (!objmgr.opcodeTable.empty())
		return;

	//fill table if empty
	objmgr.opcodeTable[ 0x06 ]							 = OpcodeHandler( STATUS_AUTHED, &WorldSession::HandleUnknown06Opcode	);
	objmgr.opcodeTable[ 0x1F ]							 = OpcodeHandler( STATUS_AUTHED, &WorldSession::HandlePlayerJoinOpcode	);
	objmgr.opcodeTable[ MSG_PARTIAL_TIMESTAMP ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FULL_TIMESTAMP ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_NEED_TIMESTAMP ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNeedTimestampOpcode	);
	objmgr.opcodeTable[ MSG_SIMULATED_TIMESTAMP ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_USE_MAP ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_JOIN_DATA ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_NEW_PLAYER ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_PLAYER_QUIT ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_SIMPLE_OBJ ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_COMPLEX_OBJ ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_DESTROY_OBJECT ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_OBJECT_OUT_OF_SIGHT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_OBJECT_IN_SHADOWS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_OBJECT_FRIEND_ADD ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_OBJECT_FRIEND_REMOVE ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_RESET_FRIENDS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_ENABLE_OBJECT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_DISABLE_OBJECT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_DRAW_FRAME ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_DESTROY_WALL ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_OPEN_WALL ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_CLOSE_WALL ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_CHANGE_OR_ADD_WALL_MAGIC ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REMOVE_WALL_MAGIC ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_PLAYER_INPUT ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandlePlayerInputOpcode	);
	objmgr.opcodeTable[ MSG_PLAYER_SET_WAYPOINT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandlePlayerSetWaypointOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_HEALTH ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_HEALTH_DELTA ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_PLAYER_HEALTH ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ITEM_HEALTH ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_MANA ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_POISON ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_STAMINA ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_STATS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ARMOR_VALUE ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_GOLD ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_PICKUP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_MODIFIABLE_PICKUP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_DROP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_LESSON ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_MUNDANE_ARMOR_EQUIP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_MUNDANE_WEAPON_EQUIP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_MODIFIABLE_WEAPON_EQUIP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_MODIFIABLE_ARMOR_EQUIP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ARMOR_DEQUIP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_WEAPON_DEQUIP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_TREASURE_COUNT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_FLAG_BALL_WINNER ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_FLAG_WINNER ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_DEATHMATCH_WINNER ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_DEATHMATCH_TEAM_WINNER ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ENCHANTMENT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ITEM_ENCHANTMENT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_LIGHT_COLOR ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_LIGHT_INTENSITY ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_Z_PLUS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_Z_MINUS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_EQUIP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_DEQUIP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ACQUIRE_SPELL ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_TARGET ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_CHARGES ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_X_STATUS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_PLAYER_STATUS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_MODIFIER ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_STAT_MODIFIER ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_NPC ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_CLIENT_STATUS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ANIMATION_FRAME ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ACQUIRE_CREATURE ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_LOSE_CREATURE ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_EXPERIENCE ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_SPELL_AWARD ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_SPELL_START ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_INVENTORY_LOADED ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_TRY_DROP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTryDropOpcode	);
	objmgr.opcodeTable[ MSG_TRY_GET ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTryGetOpcode	);
	objmgr.opcodeTable[ MSG_TRY_USE ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTryUseOpcode	);
	objmgr.opcodeTable[ MSG_TRY_EQUIP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTryEquipOpcode	);
	objmgr.opcodeTable[ MSG_TRY_DEQUIP ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTryDequipOpcode	);
	objmgr.opcodeTable[ MSG_TRY_TARGET ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_TRY_CREATURE_COMMAND ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTryCreatureCommandOpcode	);
	objmgr.opcodeTable[ MSG_TRY_SPELL ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTrySpellOpcode	);
	objmgr.opcodeTable[ MSG_TRY_ABILITY ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTryAbilityOpcode	);
	objmgr.opcodeTable[ MSG_TRY_COLLIDE ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTryCollideOpcode	);
	objmgr.opcodeTable[ MSG_FX_PARTICLEFX ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_PLASMA ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_SUMMON ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_SUMMON_CANCEL ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_SHIELD ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_BLUE_SPARKS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_YELLOW_SPARKS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_CYAN_SPARKS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_VIOLET_SPARKS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_EXPLOSION ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_LESSER_EXPLOSION ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_COUNTERSPELL_EXPLOSION ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_THIN_EXPLOSION ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_TELEPORT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_SMOKE_BLAST ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_DAMAGE_POOF ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_LIGHTNING ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_ENERGY_BOLT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_CHAIN_LIGHTNING_BOLT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_DRAIN_MANA ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_CHARM ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_GREATER_HEAL ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_MAGIC ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_SPARK_EXPLOSION ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_DEATH_RAY ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_SENTRY_RAY ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_RICOCHET ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_JIGGLE ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_GREEN_BOLT ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_GREEN_EXPLOSION ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_WHITE_FLASH ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_GENERATING_MAP ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_ASSEMBLING_MAP ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_POPULATING_MAP ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_DURATION_SPELL ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_DELTAZ_SPELL_START ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_TURN_UNDEAD ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_ARROW_TRAP ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_VAMPIRISM ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FX_MANA_BOMB_CANCEL ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_UPDATE_STREAM ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_NEW_ALIAS ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewAliasOpcode	);
	objmgr.opcodeTable[ MSG_AUDIO_EVENT ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_AUDIO_PLAYER_EVENT ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_TEXT_MESSAGE ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTextMessageOpcode	);
	objmgr.opcodeTable[ MSG_INFORM ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_IMPORTANT ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleImportantOpcode	);
	objmgr.opcodeTable[ MSG_IMPORTANT_ACK ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleImportantAckOpcode	);
	objmgr.opcodeTable[ MSG_MOUSE ]						 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleMouseOpcode	);
	objmgr.opcodeTable[ MSG_INCOMING_CLIENT ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleIncomingClientOpcode	);
	objmgr.opcodeTable[ MSG_OUTGOING_CLIENT ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleOutgoingClientOpcode	);
	objmgr.opcodeTable[ MSG_GAME_SETTINGS ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_GAME_SETTINGS_2 ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_UPDATE_GUI_GAME_SETTINGS ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_DOOR_ANGLE ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_OBELISK_CHARGE ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_PENTAGRAM_ACTIVATE ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_CLIENT_PREDICT_LINEAR ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REQUEST_MAP ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleRequestMapOpcode	);
	objmgr.opcodeTable[ MSG_CANCEL_MAP ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleCancelMapOpcode	);
	objmgr.opcodeTable[ MSG_MAP_SEND_START ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_MAP_SEND_PACKET ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_MAP_SEND_ABORT ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_SERVER_CMD ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleServerCmdOpcode	);
	objmgr.opcodeTable[ MSG_SYSOP_PW ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleSysopPwOpcode	);
	objmgr.opcodeTable[ MSG_SYSOP_RESULT ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_KEEP_ALIVE ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleKeepAliveOpcode	);
	objmgr.opcodeTable[ MSG_RECEIVED_MAP ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleReceivedMapOpcode	);
	objmgr.opcodeTable[ MSG_CLIENT_READY ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleClientReadyOpcode	);
	objmgr.opcodeTable[ MSG_REQUEST_SAVE_PLAYER ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleRequestSavePlayerOpcode	);
	objmgr.opcodeTable[ MSG_XFER_MSG ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleXferMsgOpcode	);
	objmgr.opcodeTable[ MSG_PLAYER_OBJ ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_TEAM_MSG ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTeamMsgOpcode	);
	objmgr.opcodeTable[ MSG_KICK_NOTIFICATION ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_TIMEOUT_NOTIFICATION ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_SERVER_QUIT ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_SERVER_QUIT_ACK ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleServerQuitAckOpcode	);
	objmgr.opcodeTable[ MSG_TRADE ]						 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleTradeOpcode	);
	objmgr.opcodeTable[ MSG_CHAT_KILL ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_MESSAGES_KILL ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_SEQ_IMPORTANT ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ABILITY_AWARD ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ABILITY_STATE ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ACTIVE_ABILITIES ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_DIALOG ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleDialogOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_GUIDE_AWARD ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_INTERESTING_ID ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_TIMER_STATUS ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REQUEST_TIMER_STATUS ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleRequestTimerStatusOpcode	);
	objmgr.opcodeTable[ MSG_JOURNAL_MSG ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_CHAPTER_END ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_ALL_LATENCY ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_FLAG_STATUS ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_BALL_STATUS ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_OBJECT_POISON ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_MONITOR_CREATURE ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_UNMONITOR_CREATURE ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_TOTAL_HEALTH ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_TOTAL_MANA ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_SPELL_STAT ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_SECONDARY_WEAPON ]	 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleReportSecondaryWeaponOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_LAST_QUIVER ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_INFO_BOOK_DATA ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleInfoBookDataOpcode	);
	objmgr.opcodeTable[ MSG_SOCIAL ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleSocialOpcode	);
	objmgr.opcodeTable[ MSG_FADE_BEGIN ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_MUSIC_EVENT ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_MUSIC_PUSH_EVENT ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_MUSIC_POP_EVENT ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_PLAYER_DIED ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_PLAYER_RESPAWN ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_FORGET_DRAWABLES ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_RESET_ABILITIES ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_RATE_CHANGE ]				 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_REPORT_CREATURE_CMD ]		 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_VOTE ]						 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleVoteOpcode	);
	objmgr.opcodeTable[ MSG_STAT_MULTIPLIERS ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleNewUnknownOpcode	);
	objmgr.opcodeTable[ MSG_GAUNTLET ]					 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleGauntletOpcode	);
	objmgr.opcodeTable[ MSG_INVENTORY_FAIL ]			 = OpcodeHandler( STATUS_LOGGEDIN, &WorldSession::HandleInventoryFailOpcode	);
}

void WorldSession::SizeError(WorldPacket const& packet, uint32 size) const
{
    sLog.outError("Client (account %u) send packet %s (%u) with size %u but expected %u (attempt crash server?), skipped",
        GetAccountId(),LookupName(packet.GetOpcode(),g_worldOpcodeNames),packet.GetOpcode(),packet.size(),size);
}

/// Get the player name
wchar_t const* WorldSession::GetPlayerName() const
{
	return GetPlayer() ? GetPlayer()->GetName() : L"<none>";
}

/// Set the WorldSocket associated with this session
void WorldSession::SetSocket(WorldSocket *sock)
{
    _socket = sock;
}

/// Send a packet to the client
void WorldSession::SendPacket(WorldPacket* packet, bool ifReady, bool changeUnk)
{
	if (_socket && (!ifReady || !PlayerLoading()))
	{	
		if(changeUnk)
			packet->SetUnk(m_unk);

		if(sendPacket.GetOpcode() && (packet->GetUnk() != sendPacket.GetUnk() || packet->GetAccountId() != sendPacket.GetAccountId())) // || packet->GetAccountId() != sendPacket.GetAccountId()
			_SendPacket(&sendPacket);
		if(!sendPacket.GetOpcode())
		{
			sendPacket.SetOpcode(packet->GetOpcode());
			sendPacket.SetAccountId(packet->GetAccountId());
			sendPacket.SetUnk(packet->GetUnk());
		}
		else
			sendPacket << (uint8)packet->GetOpcode();
		
		if(packet->size() > 0)
			sendPacket.append(packet->contents(), packet->size());
	}
}

void WorldSession::_SendPacket(WorldPacket* packet)
{
	if (_socket)
	{
		packet->SetClient(_client);
		packet->SetXor(xorKey);
		_socket->SendPacket(packet);
	}
	packet->Initialize(0);
}

/// Add an incoming packet to the queue
void WorldSession::QueuePacket(WorldPacket& packet)
{
    WorldPacket *pck = new WorldPacket(packet);
    _recvQueue.add(pck);
}

/// Update the WorldSession (triggered by World update)
bool WorldSession::Update(uint32 diff)
{
    WorldPacket *packet = NULL;

	// Always send new timestamp
	if(m_status == STATUS_LOGGEDIN)
	{
		m_timestamp++;
		_SendPartialTimestampOpcode();
	}

    ///- Retrieve packets from the receive queue and call the appropriate handlers
    /// \todo Is there a way to consolidate the OpcondeHandlerTable and the g_worldOpcodeNames to only maintain 1 list?
    /// answer : there is a way, but this is better, because it would use redundand RAM
	if(!_recvQueue.empty() && m_connTimer < TIMEOUT_MS)
		m_connTimer = TIMEOUT_MS;
	else
	{
		if(m_connTimer < diff)
			LogoutRequest(1);
		else
			m_connTimer -= diff;
	}

    while ( (packet || !_recvQueue.empty()) && _socket)
    {
		if(!packet)
			packet = _recvQueue.next();

        /*#if 1
        sLog.outError( "MOEP: %s (0x%.4X)",
                        LookupName(packet->GetOpcode(), g_worldOpcodeNames),
                        packet->GetOpcode());
        #endif*/

		try {
			OpcodeTableMap::const_iterator iter = objmgr.opcodeTable.find( packet->GetOpcode() );
			if (packet->GetOpcode() == 0 && packet->GetAccountId() == 0xFF) //let's assume that this will always be FF 00 00
			{
				switch(m_status)
				{
					case STATUS_AUTHED:
						HandleJoinConfirmation();
						break;
					case STATUS_CONFIRMED:

						break;
				}
			}
			else if (iter == objmgr.opcodeTable.end())
			{
				sLog.outError( "SESSION: received unhandled opcode %s (0x%.4X)",
					LookupName(packet->GetOpcode(), g_worldOpcodeNames),
					packet->GetOpcode());
			}
			else
			{
				if (iter->second.status == STATUS_LOGGEDIN && _player)
				{
					(this->*iter->second.handler)(*packet);
				}
				else if (iter->second.status == STATUS_AUTHED)
				{
					m_playerRecentlyLogout = false;
					(this->*iter->second.handler)(*packet);
				}
				else
					// skip STATUS_LOGGEDIN opcode unexpected errors if player logout sometime ago - this can be network lag delayed packets
				if(!m_playerRecentlyLogout)
				{
					sLog.outError( "SESSION: received unexpected opcode %s (0x%.4X)",
						LookupName(packet->GetOpcode(), g_worldOpcodeNames),
						packet->GetOpcode());
				}
			}
			
			if(packet->rpos() == packet->size())
			{
				delete packet;
				packet = NULL;
			}
			else
				packet->SetOpcode(packet->read<uint8>());
		} catch(ByteBuffer::error &) {
			delete packet;
			packet = NULL;
		}
    }

	if(sendPacket.GetOpcode())
		_SendPacket(&sendPacket);
	if(m_playerDownloading > 0)
		_SendMapSendPacket();

    ///- If necessary, log the player out
    time_t currTime = time(NULL);
    if (!_socket || (ShouldLogOut(currTime) && !m_playerLoading))
        LogoutPlayer(true);

    if (!_socket)
        return false;                                       //Will remove this session from the world session map

    return true;
}

void WorldSession::HandleJoinConfirmation()
{
	/// Generate XOR key
	//if(!xorKey)
	//	xorKey = (uint8)urand(0, 0xFF);
	WorldPacket packet(0x1F, 0x80, _client);
	packet << (uint8)0;
	packet << (uint8)1;
	packet << (uint8)_accountId;
	packet << (uint8)0;
	packet << (uint8)0;
	packet << (uint8)0;
	packet << (uint8)xorKey;
	_socket->SendPacket(&packet);
	m_status = STATUS_CONFIRMED;
}

void WorldSession::HandlePlayerJoinOpcode(WorldPacket &recvPacket)
{
	uint8 data[0x20];	
	if(!_player)
		_player = new Player(this);

	try
    {
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read((uint8*)(&_player->plrInfo), 0x61);
		_player->plrInfo.name[0x18] = 0;
		recvPacket.read(data, 0x1F);
		recvPacket.read((uint8*)_player->plrInfo.username, 9);
		_player->plrInfo.username[8] = 0;
		recvPacket.read(_player->plrInfo.unk1, 0x0C);
		recvPacket.read(data, 0x03);
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete join packet");
        return;
    }
	m_status = STATUS_LOGGEDIN;

	ObjectAccessor::Instance().InsertPlayer(_player);
	m_unk++;

	//_player->SetName(name);

	WorldPacket packet(0xA9, 0x80, _client, 0x5);
	packet << (uint8)0x0C;
	packet << (uint32)1;
	SendPacket(&packet, false);

	_SendFullTimestampOpcode();
	_SendJoinDataOpcode();
	_SendGameSettingsOpcode();
	_SendGameSettings2Opcode();
	_SendNewPlayerOpcode();
	m_unk++;
	_SendUseMapOpcode();

	packet.SetOpcode(0x04);
	packet.SetAccountId(0x00);
	packet.SetUnk(0x00);
	packet.clear();
	SendPacket(&packet, false, false);
	_SendPacket(&sendPacket); //clear the send buffer

	m_timestamp++;
	_SendPartialTimestampOpcode();
}


void WorldSession::HandlePlayerInputOpcode(WorldPacket &recvPacket)
{
	try
    {
		//0A 01 00 00 00 55 01 00 00 00 55
		switch(recvPacket.read<uint8>())
		{
		case 0x0A: //default
			break;
		case 0x0E: //attack/swing: 0E 06 00 00 00
			switch(recvPacket.read<uint8>())
			{
			case 0x06:
				sLog.outDebug("Player attacked.");
				//if observer
				_player->Attack();
				break;
			case 0x07:
				//jumped
				sLog.outDebug("Player jumped.");
				break;
            case 0x30:
                 sLog.outDebug("Player laughed.");
                 _player->Laugh();
                 break;
            case 0x31:
                 sLog.outDebug("Player pointed.");
                 _player->Point();
                 break;
            case 0x2F:
                 sLog.outDebug("Player taunted.");
                 _player->Taunt();
                 break;
			default:
				recvPacket.rpos(recvPacket.rpos()-1);
				sLog.outDebug("Player input: 0x0E 0x%2X", recvPacket.read<uint8>());
				break;
			}
			recvPacket.read<uint8>();
			recvPacket.read<uint8>();
			recvPacket.read<uint8>();
			break;
		case 0x0F: //move: 0F 02 00 00 00 01
			_player->MoveTowards(_mouse.x_coord, _mouse.y_coord);
			recvPacket.read<uint32>();
			recvPacket.read<uint8>();
			break;
		case 0x13: //running jump: 13 07 00 00 00 02 01 00 00 01
				   //13 06 00 00 00 02 01 00 00 01
               switch(recvPacket.read<uint32>())
               {
                    case 0x07:
                         sLog.outDebug("Player did a running jump.");
                         break;
               }
			recvPacket.read<uint8>();
			recvPacket.read<uint8>();
			recvPacket.read<uint16>();
			recvPacket.read<uint8>();
			break;
		default:
			recvPacket.rpos(recvPacket.rpos()-1);
				sLog.outDebug("Player input: 0x%2X", recvPacket.read<uint8>());
			break;
		}
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		_player->SetAngle(recvPacket.read<uint8>());
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete player input packet");
        return;
    }
}
void WorldSession::HandleMouseOpcode(WorldPacket &recvPacket)
{
	try
    {
		_mouse.x_coord = recvPacket.read<uint16>(); //x
		_mouse.y_coord = recvPacket.read<uint16>(); //y
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete packet");
        return;
    }
}
void WorldSession::HandleUnknown06Opcode(WorldPacket &recvPacket)
{
	try
    {
		recvPacket.read<uint32>();
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete packet");
        return;
    }
}
void WorldSession::HandleImportantOpcode(WorldPacket &recvPacket)
{
	try
    {
		_SendImportantAckOpcode(recvPacket.read<uint32>());
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete packet");
        return;
    }
}
void WorldSession::HandleNeedTimestampOpcode(WorldPacket &recvPacket)
{
	try
    {
		_SendFullTimestampOpcode();
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete packet");
        return;
    }
}
void WorldSession::HandleServerCmdOpcode(WorldPacket &recvPacket)
{
	try
    {
		switch(recvPacket.read<uint8>())
		{
		case 0:
			//E8 03 03 0D F0 AD F0 00 00 00 40
			//E8 03 00 00 00
			recvPacket.read<uint16>(); //extent
			for(int i = recvPacket.read<uint8>(); i > 0; i--)
				recvPacket.read<uint16>();
			recvPacket.read<uint8>();
			recvPacket.read<uint8>();
			break;
		case 2:
			//E8 03 0A 20 00 73 00 61 00 79 00 20 00 53 00 54 00 46 00 55 00 00 00 00 00
			wchar_t *buffer;
			uint8 length;

			recvPacket.read<uint16>(); //extent
			length = recvPacket.read<uint8>();
			buffer = new wchar_t[length];
			recvPacket.read((uint8*)buffer, length*sizeof(wchar_t));
			buffer[length-1] = 0x0;

			recvPacket.read<uint8>();
			recvPacket.read<uint8>();

			delete[] buffer;
			break;
		}
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete packet");
        return;
    }
}


void WorldSession::HandleClientReadyOpcode(WorldPacket &recvPacket)
{
	m_unk = recvPacket.GetUnk() + 1;
	if(m_playerLoading)
	{
		m_playerLoading = false;
		m_timestamp++;
		_SendPartialTimestampOpcode();
		_player->SendUpdatePacket();
		_SendFadeBeginOpcode();
		_player->Respawn();
		ObjectAccessor::Instance().SendPlayerInfo(this);
	}
}

void WorldSession::HandleNewAliasOpcode(WorldPacket &recvPacket)
{
	// Not sure what this packet is for
	// A5 E8 E8 03 C9 02 FF FF FF FF
	try
    {
		recvPacket.read<uint8>();
		recvPacket.read<uint16>(); //extent
		recvPacket.read<uint16>(); //type
		recvPacket.read<uint32>();
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete packet");
        return;
    }
}
void WorldSession::HandleTextMessageOpcode(WorldPacket &recvPacket)
{
	try
    {
		char* buffer;
		uint16 length;
		size_t pstart, pend;

		pstart = recvPacket.rpos();
		recvPacket.read<uint16>(); //extent
		recvPacket.read<uint8>(); //flag?
		recvPacket.read<uint16>(); //x
		recvPacket.read<uint16>(); //y
		length = recvPacket.read<uint16>();
		recvPacket.read<uint8>(); //always zero?
		buffer = new char[length];
		recvPacket.read((uint8*)buffer, length*sizeof(char));
		buffer[length-1] = 0x0;
		pend = recvPacket.rpos();

		sLog.outDebug("Player say: %s", buffer);

		delete[] buffer;

		// Send out to all players
		WorldPacket packet(MSG_TEXT_MESSAGE, 0, 0, pend-pstart);
		packet.append(recvPacket.contents() + pstart, pend-pstart);
		ObjectAccessor::Instance().SendPacketToAll(&packet);
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete packet");
        return;
    }
}
void WorldSession::HandleOutgoingClientOpcode(WorldPacket &recvPacket)
{
	try
    {
		LogoutRequest(time(NULL));
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete packet");
        return;
    }
}
void WorldSession::HandleIncomingClientOpcode(WorldPacket &recvPacket)
{
	try
    {
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete packet");
        return;
    }
}
void WorldSession::HandleTryAbilityOpcode(WorldPacket& recv_data)
{
	try
    {
		uint8 ability = recv_data.read<uint8>();
		sLog.outDebug("TryAbility: 0x%2X", ability);

		if(GetPlayer()->IsAbilityReady(ability))
			ExecuteAbility(ability);
    }
    catch(ByteBuffer::error &)
    {
        sLog.outDetail("Incomplete packet");
        return;
    }
}


void WorldSession::HandlePlayerSetWaypointOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	
	recv_data.read<uint16>();
	recv_data.read<uint16>();
	recv_data.read<uint16>();
}
void WorldSession::HandleTryDropOpcode(WorldPacket& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
    WorldObject* obj = objmgr.GetObj(recv_data.read<uint16>());
    uint16 x = recv_data.read<uint16>();
    uint16 y = recv_data.read<uint16>();
    if(!(_player->Drop(obj,75, GridPair(x,y))))
             sLog.outDebug("Couldn't drop object.");

}
void WorldSession::HandleTryGetOpcode(WorldPacket& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
	WorldObject* obj = objmgr.GetObj(recv_data.read<uint16>());
	_player->Pickup(obj, 75);
}
void WorldSession::HandleTryUseOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	WorldObject* obj = objmgr.GetObj(recv_data.read<uint16>());
	if(obj)
		obj->Use(GetPlayer());
}
void WorldSession::HandleTryEquipOpcode(WorldPacket& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
	WorldObject* obj = objmgr.GetObj(recv_data.read<uint16>());
	_player->Equip(obj);
}
void WorldSession::HandleTryDequipOpcode(WorldPacket& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
	WorldObject* obj = objmgr.GetObj(recv_data.read<uint16>());
	_player->Dequip(obj);
}
void WorldSession::HandleTryCreatureCommandOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint16>();
	recv_data.read<uint8>();
}
void WorldSession::HandleTrySpellOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	uint8 spellId = recv_data.read<uint8>();

	recv_data.rpos(recv_data.rpos() + 19);
	uint8 dontinvert = recv_data.read<uint8>();

     //inversion was backwards.
	ExecuteSpell(spellId, (bool)dontinvert)

	//				(sp. id)													(invert)
	//summon ghost: 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	//summon bear:  4D 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01
	//meteor:		34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
}
void WorldSession::HandleTryCollideOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint16>();
}
void WorldSession::HandleImportantAckOpcode(WorldPacket& recv_data)
{
	recv_data.read<uint32>(); // should check this if we sent out an important packet
}
void WorldSession::HandleRequestMapOpcode(WorldPacket& recv_data)
{
	m_unk = recv_data.GetUnk();
	_SendMapSendStart();
}
void WorldSession::HandleCancelMapOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
}
void WorldSession::HandleSysopPwOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.rpos(recv_data.rpos()+20);
}
void WorldSession::HandleKeepAliveOpcode(WorldPacket& recv_data)
{
	// this opcode just keeps the connection open
}
void WorldSession::HandleReceivedMapOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();

	m_playerDownloading = 0;
}
void WorldSession::HandleRequestSavePlayerOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint16>();
}
void WorldSession::HandleXferMsgOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
	recv_data.read<uint8>();
	recv_data.read<uint8>();
	recv_data.read<uint32>();
	recv_data.rpos(recv_data.rpos()+0x84);
}
void WorldSession::HandleTeamMsgOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.rpos(recv_data.size());
	recv_data.read<uint8>();
	recv_data.read<uint32>();
	recv_data.read<uint16>();
	recv_data.read<uint16>();
}
void WorldSession::HandleServerQuitAckOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
}
void WorldSession::HandleTradeOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
	recv_data.read<uint16>();
}
void WorldSession::HandleDialogOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
	recv_data.read<uint8>();
}
void WorldSession::HandleRequestTimerStatusOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
}
void WorldSession::HandleReportSecondaryWeaponOpcode(WorldPacket& recv_data)
{
	WorldObject* obj = objmgr.GetObj(recv_data.read<uint16>());
	_player->EquipSecondary(obj);
}
void WorldSession::HandleInfoBookDataOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint16>();
	recv_data.read<uint8>();
}
void WorldSession::HandleSocialOpcode(WorldPacket& recv_data)
{
     //30 = laugh 31 = point
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
}
void WorldSession::HandleVoteOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
	recv_data.rpos(recv_data.rpos()+0x32);
}
void WorldSession::HandleGauntletOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
}
void WorldSession::HandleInventoryFailOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint16>();
}

/// %Log the player out
void WorldSession::LogoutPlayer(bool Save)
{
	if(_player)
	{
		ObjectAccessor::Instance().RemovePlayer(_player);

		///- Delete the player object
        delete _player;
        _player = 0;
	}

	_socket = 0;
}

/// Kick a player out of the World
void WorldSession::KickPlayer()
{
    if(!_socket)
        return;

    // player will be  logout and session will removed in next update tick
    _socket->SetCloseAndDelete(true);
    _socket = NULL;
}

/// Cancel channeling handler
/// \todo Complete HandleCancelChanneling function
void WorldSession::HandleNewUnknownOpcode( WorldPacket & recv_data )
{
    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.rpos(recv_data.size());
}

void WorldSession::CryptData(uint8* data, uint32 datalen)
{
	if(xorKey == 0)
		return;
	_socket->CryptData(xorKey, data, datalen);
}


void WorldSession::_SendFullTimestampOpcode()
{
	WorldPacket packet(MSG_FULL_TIMESTAMP, 0x80, _client, 4);
	packet << m_timestamp;
	SendPacket(&packet, false);
}
void WorldSession::_SendJoinDataOpcode()
{
	WorldPacket packet(MSG_JOIN_DATA, 0x80, _client, 6);
	packet << _player->GetExtent();
	packet << (uint32)0;
	SendPacket(&packet, false);
}
void WorldSession::_SendGameSettings2Opcode()
{
	WorldPacket packet(MSG_GAME_SETTINGS_2, 0x80, _client, 0x30);
	packet.append("ZoaEDK's Game\0\0\0", 0x10); // 0x10
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0x0;
	SendPacket(&packet, false);
}
void WorldSession::_SendGameSettingsOpcode()
{
	WorldPacket packet(MSG_GAME_SETTINGS, 0x80, _client, 0x13);
	packet << m_timestamp;
	packet << (uint32)0x0001039A;
	packet << (uint32)0x00032080;
	packet << (uint32)0x00002002;
	packet << (uint8)0x20;
	packet << (uint8)0x0F;
	packet << (uint8)0x00;
	SendPacket(&packet, false);
}
void WorldSession::_SendNewPlayerOpcode()
{
	WorldPacket packet;
	_player->_BuildNewPlayerPacket(packet);
	ObjectAccessor::Instance().SendPacketToAll(&packet);
	//SendPacket(&packet, false); // to all won't send to the current player due to playerLoading variable, so we to do this ourselves
}
void WorldSession::_SendUseMapOpcode()
{
	m_playerLoading = true;

	WorldPacket packet(MSG_USE_MAP, 0x80, _client, 0x28);
	packet.append("manamine.map\0", 0xD); //0xD // 0x20?
	packet << (uint16)0x0;
	packet << (uint32)0x0;
	packet << (uint32)0x0;
	packet << (uint32)0x0;
	packet << (uint16)0x0;
	packet << (uint16)0x0;
	packet << (uint8)0;
	packet << (uint32)sWorld.GetMap()->GetChecksum(); //checksum
	packet << (uint32)m_timestamp;
	SendPacket(&packet, false);
}
void WorldSession::_SendPartialTimestampOpcode()
{
	WorldPacket packet(MSG_PARTIAL_TIMESTAMP, 0x0, _client, 2);
	packet << (uint16)m_timestamp;
	SendPacket(&packet, false);
}
void WorldSession::_SendFadeBeginOpcode()
{
	WorldPacket packet(MSG_FADE_BEGIN, 0x0, _client, 2);
	packet << (uint8)1;
	packet << (uint8)1;
	SendPacket(&packet, false);
}
void WorldSession::_SendImportantAckOpcode(uint32 timestamp)
{
	WorldPacket packet(MSG_IMPORTANT_ACK, 0x0, _client, 4);
	packet << (uint32)timestamp;
	SendPacket(&packet, false);
}
void WorldSession::_SendPlayerRespawnOpcode()
{
	WorldPacket packet(MSG_PLAYER_RESPAWN, 0x0, _client, 10);
	packet << (uint16)_player->GetExtent();
	packet << (uint32)m_timestamp;
	packet << (uint8)0xFF;
	packet << (uint8)0x01;
	objacc.SendPacketToAll(&packet);
	packet.Initialize(MSG_SIMPLE_OBJ);
	packet << (uint16)_player->GetExtent();
	packet << (uint16)_player->GetType();
	packet << (uint16)_player->GetPositionX();
	packet << (uint16)_player->GetPositionY();
	objacc.SendPacketToAll(&packet);
}
/*void WorldSession::_SendUpdateStreamOpcode()
{
	WorldPacket packet(MSG_UPDATE_STREAM, 0x0, _client, 11);
	packet << (uint8)0xFF;
	packet << (uint16)_player->GetExtent();
	packet << (uint16)_player->GetType();
	packet << (uint16)_player->GetPosition().x_coord;
	packet << (uint16)_player->GetPosition().y_coord;
	packet << (uint8)( rot_data[(uint8)floor((double)_player->GetAngle() / 0x20 + 0.5) & 0x7] );
	packet << (uint8)_player->m_action;
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	SendPacket(&packet);
}*/

void WorldSession::_SendClientStatusOpcode()
{
	WorldPacket packet(MSG_REPORT_CLIENT_STATUS, 0x0, _client, 2);
	packet << (uint16)_player->GetExtent();
	packet << (uint32)m_playerObserving;
	objacc.SendPacketToAll(&packet);
	
	if(m_playerObserving) //should a different function send this?
	{
		packet.Initialize(MSG_INFORM);
		packet << (uint8)0x0C;
		packet << (uint32)m_playerObserving;
		SendPacket(&packet);
	}
}
void WorldSession::_SendMapSendStart()
{
	if(!m_playerDownloading)
	{
		WorldPacket packet(MSG_MAP_SEND_START, 0x80, _client, 87);
		packet << (uint8)0xF8; //unknown
		packet << (uint16)0x0013; //unknown
		packet << (uint32)sWorld.GetMap()->GetNxzSize();
		packet.append(sWorld.GetMap()->GetNxzName(), 0x50);

		SendPacket(&packet, false);
		m_playerDownloading = 1;
		m_unk2 = m_unk;
	}
}
void WorldSession::_SendMapSendPacket()
{
	m_unk2++;
	int PACKET_SIZE = 0x200;
	uint8 data[0x200];

	//put data in data and update size
	PACKET_SIZE = sWorld.GetMap()->ReadNxzBytes( (m_playerDownloading - 1) * PACKET_SIZE, data, PACKET_SIZE);

	WorldPacket packet(MSG_MAP_SEND_PACKET, 0x80, _client, PACKET_SIZE + 0x5);
	packet << (uint8)0xF8; //unknown/not used
	packet << (uint16)m_playerDownloading; //packet #
	packet << (uint16)PACKET_SIZE; //data size
	packet.append(data, PACKET_SIZE);
	packet.SetUnk(m_unk2);
	SendPacket(&packet, false, false);

	m_playerDownloading++;
	if((m_playerDownloading - 1) * PACKET_SIZE > sWorld.GetMap()->GetNxzSize())
		m_playerDownloading = 0;
}
void WorldSession::_SendAudioPlayerEvent( uint16 sound, uint8 unk1, uint8 unk2 )
{
   	WorldPacket packet(MSG_AUDIO_PLAYER_EVENT, 0x00, _client, 3);
	packet << (uint8)unk2;
    packet << (uint16) ( (unk1 << 0x9) | (sound & 0x3FF) ) ;
	SendPacket(&packet);
}