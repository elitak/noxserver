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

/// Player session in the World
class MANGOS_DLL_SPEC WorldSession
{
    public:
        WorldSession(uint32 id, WorldSocket *sock, uint32 sec);
        ~WorldSession();

        bool PlayerLoading() { return m_playerLoading; }

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

        /// Is the user engaged in a log out process?
        bool isLogingOut() const
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

        //void SendTestCreatureQueryOpcode( uint32 entry, uint64 guid, uint32 testvalue );
//        void SendNameQueryOpcode(Player* p);
        void SendNameQueryOpcodeFromDB(uint64 guid);

        void SendCreatureQuery( uint32 entry, uint64 guid );
        void SendTrainerList( uint64 guid );
        void SendTrainerList( uint64 guid,std::string strTitle );
        void SendListInventory( uint64 guid );
        void SendShowBank( uint64 guid );
        void SendTabardVendorActivate( uint64 guid );
        void SendSpiritResurrect();
        void SendBindPoint(Creature* npc);
        void SendGMTicketGetTicket(uint32 status, char const* text);

        void SendTradeStatus(uint32 status);
        void SendCancelTrade();

        void SendStablePet(uint64 guid );
        void SendPetitionQueryOpcode( uint64 petitionguid);
        void SendUpdateTrade();

        //pet
        void SendPetNameQuery(uint64 guid, uint32 petnumber);

        //mail
                                                            //used with item_page table
        bool SendItemInfo( uint32 itemid, WorldPacket data );
        //auction
        void SendAuctionHello( uint64 guid, Creature * unit );
        void SendAuctionCommandResult( uint32 auctionId, uint32 Action, uint32 ErrorCode, uint32 bidError = 0);
        void SendAuctionBidderNotification( uint32 location, uint32 auctionId, uint64 bidder, uint32 bidSum, uint32 diff, uint32 item_template);
        void SendAuctionOwnerNotification( AuctionEntry * auction );
        bool SendAuctionInfo(WorldPacket & data, AuctionEntry* auction);
        void SendAuctionOutbiddedMail( AuctionEntry * auction, uint32 newPrice );
        void SendAuctionCancelledToBidderMail( AuctionEntry* auction );

        //Item Enchantement
        void SendEnchantmentLog(uint64 Target, uint64 Caster,uint32 ItemID,uint32 SpellID);
        void SendItemEnchantTimeUpdate(uint64 Playerguid, uint64 Itemguid,uint32 slot,uint32 Duration);

        //Taxi
        void SendTaxiStatus( uint64 guid );
        void SendTaxiMenu( uint64 guid );
        void SendDoFlight( uint16 MountId, uint32 path );
        bool SendLearnNewTaxiNode( uint64 guid );
    protected:

		// Spells
		bool ExecuteSpell(const char* name, bool invert)
		{
			uint8 id = LookupId(name, g_spellNames);
			if(id == INVALID_ID)
				return false;
			else
				return ExecuteSpell(id, invert);
		}
		bool ExecuteSpell(uint8 spellId, bool invert);
		void HandleSpellUnknown(bool invert) {};
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
		void HandleAbilityUnknown() {};
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
		void _SendImportantAckOpcode();
		void _SendClientStatusOpcode();

		void _SendPacket(WorldPacket* packet);

    private:
        Player *_player;
        WorldSocket *_socket;
		ClientAddr _client;
		WorldPacket sendPacket; //Combined packet

        uint32 _security;
        uint32 _accountId;
		POSITION _mouse;

        time_t _logoutTime;
        bool m_playerLoading;
        bool m_playerRecentlyLogout;

		uint8 xorKey;
		SessionStatus m_status;
		uint32 m_timestamp;
		uint8 m_unk;

        void FillOpcodeHandlerHashTable();
		void FillSpellHandlerHashTable();
		void FillAbilityHandlerHashTable();

        ZThread::LockedQueue<WorldPacket*,ZThread::FastMutex> _recvQueue;
		ByteBuffer loginBuffer;
};
#endif
/// @}
