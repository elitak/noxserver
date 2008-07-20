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

/// \addtogroup u2w
/// @{
/// \file

#ifndef __WORLDSESSION_H
#define __WORLDSESSION_H

#include "Common.h"
#include "NameTables.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "worldpacket.h"
#include "WorldSocket.h"

struct ItemPrototype;
struct AuctionEntry;

class Creature;
class WorldPacket;
class WorldSocket;
class WorldSession;

#define CHECK_PACKET_SIZE(P,S) if((P).size() < (S)) return SizeError((P),(S));

struct Alias
{
public: 
	uint16 extent;
	uint16 type;
	uint32 other;
};

/// Player session in the World
class MANGOS_DLL_SPEC WorldSession
{
	friend Player;
    public:
        WorldSession(uint32 id, WorldSocket *sock, uint32 sec);
        ~WorldSession();

        bool PlayerLoading() { return m_playerLoading; }

        void _SendAudioPlayerEvent( uint16 sound, uint8 volume = 100, uint8 unk2 = 0 );

        void SizeError(WorldPacket const& packet, uint32 size) const;

        void SendPacket(WorldPacket* packet, bool ifReady = true, bool changeUnk = true);
        void SendNotification(const char *format,...);
        void SendLfgResult(uint32 type, uint32 entry);
        void SendPartyResult(uint32 unk, std::string member, uint32 state);
        void SendAreaTriggerMessage(const char* Text, ...);

        uint32 GetSecurity() const { return _security; }
        uint32 GetAccountId() const { return _accountId; }
        Player* GetPlayer() const { return _player; }
        wchar_t const* GetPlayerName() const;
        void SetSecurity(uint32 security) { _security = security; }
        void SetSocket(WorldSocket *sock);
        void SetPlayer(Player *plr) { _player = plr; }
		void SetClient(ClientAddr addr) { _client = addr; }
		ClientAddr GetClient() { return _client; }
		SessionStatus GetStatus() { return m_status; }
		GridPair GetCursor() { return _mouse; }

        /// Is the user engaged in a log out process?
        bool isLoggingOut() const
        {
            if (_logoutTime) return true;
            else return false;
        }

        /// Engage the logout process for the user
        void LogoutRequest(time_t requestTime)
        {
            _logoutTime = requestTime;
        }

        /// Is logout cooldown expired?
        bool ShouldLogOut(time_t currTime) const
        {
            return (_logoutTime > 0 && currTime >= _logoutTime);
        }

        void LogoutPlayer(bool Save);
        void KickPlayer();

        void QueuePacket(WorldPacket& packet);
        bool Update(uint32 diff);

		void CryptData(uint8* data, uint32 datalen);

		//Cursor
		void MoveMouse( uint16 deltax, uint16 deltay )
		{
			_mouse >> deltax;
			_mouse += deltay;
		}
		bool IsObserving() { return m_playerObserving; }
		void SetObserving(bool state)
		{
			m_playerObserving = state;
			_SendClientStatusOpcode();
		}
		uint8 GetAlias( uint16 extent, uint16 type );
		void SetAlias( uint8 id, uint16 extent, uint16 type, uint32 other );
        void _SendFxDeathRaySpell( uint16 x, uint16 y, uint16 x2, uint16 y2 );
    protected:

		// Spells
		bool ExecuteSpell(const char* name, bool dontinvert)
		{
			uint8 id = LookupId(name, g_spellNames);
			if(id == INVALID_ID)
				return false;
			else
				return ExecuteSpell(id, dontinvert);
		}
		bool ExecuteSpell(uint8 spellId, bool dontinvert);
		// Abilities
		bool ExecuteAbility(const char* name)
		{
			uint8 id = LookupId(name, g_abilityNames);
			if(id == INVALID_ID)
				return false;
			else
				return ExecuteAbility(id);
		}
		bool ExecuteAbility(uint8 abilityId);
		// Opcodes
		void HandleJoinConfirmation();
		void HandleNewUnknownOpcode(WorldPacket& recv_data);
		void HandlePlayerJoinOpcode(WorldPacket& recv_data);
		void HandlePlayerInputOpcode(WorldPacket& recv_data);
		void HandleMouseOpcode(WorldPacket& recv_data);
		void HandleImportantOpcode(WorldPacket& recv_data);
		void HandleServerCmdOpcode(WorldPacket& recv_data);
		void HandleUnknown06Opcode(WorldPacket& recv_data);
		void HandleNeedTimestampOpcode(WorldPacket& recv_data);
		void HandleClientReadyOpcode(WorldPacket& recv_data);
		void HandleNewAliasOpcode(WorldPacket& recv_data);
		void HandleTextMessageOpcode(WorldPacket& recv_data);
		void HandleOutgoingClientOpcode(WorldPacket& recv_data);
		void HandleIncomingClientOpcode(WorldPacket& recv_data);
		void HandleTryAbilityOpcode(WorldPacket& recv_data);
		void HandlePlayerSetWaypointOpcode(WorldPacket& recv_data);
		void HandleTryDropOpcode(WorldPacket& recv_data);
		void HandleTryGetOpcode(WorldPacket& recv_data);
		void HandleTryUseOpcode(WorldPacket& recv_data);
		void HandleTryEquipOpcode(WorldPacket& recv_data);
		void HandleTryDequipOpcode(WorldPacket& recv_data);
		void HandleTryCreatureCommandOpcode(WorldPacket& recv_data);
		void HandleTrySpellOpcode(WorldPacket& recv_data);
		void HandleTryCollideOpcode(WorldPacket& recv_data);
		void HandleImportantAckOpcode(WorldPacket& recv_data);
		void HandleRequestMapOpcode(WorldPacket& recv_data);
		void HandleCancelMapOpcode(WorldPacket& recv_data);
		void HandleSysopPwOpcode(WorldPacket& recv_data);
		void HandleKeepAliveOpcode(WorldPacket& recv_data);
		void HandleReceivedMapOpcode(WorldPacket& recv_data);
		void HandleRequestSavePlayerOpcode(WorldPacket& recv_data);
		void HandleXferMsgOpcode(WorldPacket& recv_data);
		void HandleTeamMsgOpcode(WorldPacket& recv_data);
		void HandleServerQuitAckOpcode(WorldPacket& recv_data);
		void HandleTradeOpcode(WorldPacket& recv_data);
		void HandleDialogOpcode(WorldPacket& recv_data);
		void HandleRequestTimerStatusOpcode(WorldPacket& recv_data);
		void HandleReportSecondaryWeaponOpcode(WorldPacket& recv_data);
		void HandleInfoBookDataOpcode(WorldPacket& recv_data);
		void HandleSocialOpcode(WorldPacket& recv_data);
		void HandleVoteOpcode(WorldPacket& recv_data);
		void HandleGauntletOpcode(WorldPacket& recv_data);
		void HandleInventoryFailOpcode(WorldPacket& recv_data);

		void _SendFullTimestampOpcode();
		void _SendJoinDataOpcode();
		void _SendGameSettingsOpcode();
		void _SendGameSettings2Opcode();
		void _SendNewPlayerOpcode();
		void _SendUseMapOpcode();
		void _SendPartialTimestampOpcode();
		void _SendFadeBeginOpcode();
		void _SendPlayerRespawnOpcode();
		//void _SendUpdateStreamOpcode();
		void _SendImportantAckOpcode(uint32 timestamp);
		void _SendClientStatusOpcode();
		void _SendMapSendStart();
		void _SendMapSendPacket();
        void _SendReportSpellStart( uint8 spell );
        void _SendFxDurationSpell( uint16 type, uint16 x, uint16 y );
        //void _SendFxDeathRaySpell( uint16 x, uint16 y, uint16 x2, uint16 y2 );

		void _SendPacket(WorldPacket* packet);

    private:
        Player *_player;
        WorldSocket *_socket;
		ClientAddr _client;
		WorldPacket sendPacket; //Combined packet

        uint32 _security;
        uint32 _accountId;
		GridPair _mouse;

        time_t _logoutTime;
        bool m_playerLoading;
        bool m_playerRecentlyLogout;
		bool m_playerObserving;
		int32 m_playerDownloading;
		uint32 m_connTimer;

		uint8 xorKey;
		SessionStatus m_status;
		uint32 m_timestamp;
		uint8 m_unk;
		uint8 m_unk2;
		//struct Alias m_aliases[254];

        void FillOpcodeHandlerHashTable();

        ZThread::LockedQueue<WorldPacket*,ZThread::FastMutex> _recvQueue;
		ByteBuffer loginBuffer;
};
#endif
/// @}
