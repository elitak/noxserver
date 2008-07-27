#pragma once

#include "global.h"
#include "world_packet.h"
#include "object_mgr.h"

#include "boost/asio.hpp"
#include <queue>

class player;

struct _alias
{
public: 
	uint16 extent;
	uint16 type;
	uint32 other;
};

typedef struct _alias alias;

class world_session
{
public:
	world_session(uint8 id, boost::asio::ip::udp::endpoint endpoint);
	~world_session();

	void queue_packet(world_packet& packet);

	uint8 get_xor_key() { return m_xor; }
	void set_xor_key(uint8 xor) { m_xor = xor; }

	player* get_player() { return m_player; }
	uint8 get_player_id() { return m_player_id; }

	const boost::asio::ip::udp::endpoint& get_endpoint() { return m_endpoint; }

	bool update();
	bool is_player_loading() { return m_player_loading; }
	void set_observing(bool obs)
	{
		m_player_observing = obs;
		_SendClientStatusOpcode();
	}
	bool is_observing() { return m_player_observing; }
	void logout();

	// we should really find a different way to do this ifReady, changeUnk thing
	// this doesn't actually send packet, only adds it to the combined packet with some mangling
	// BUG: packet should probably be const, but the only thing we change is unk, so it is okay for now
	void send_packet(world_packet& packet, bool ifReady = true, bool changeUnk = true);

protected:
	boost::asio::ip::udp::endpoint m_endpoint;
	
	// this needs to be made threadsafe if network is ever run outside of world thread
	// right now it isn't, so this is not threadsafe
	std::queue<world_packet*> m_receive_queue;

	// combined packet, this should probably be done a less archaic way
	world_packet m_send_packet;

	session_status m_status;
	uint8 m_player_id;
	uint8 m_xor;
	uint32 m_timestamp;
	uint8 m_unk;
	uint8 m_unk2; // used for some shit, hell if i know

	bool m_player_loading;
	bool m_player_observing;
	uint32 m_player_downloading; // this is an int, because we send the map in multiple packets
	player* m_player;
	bool m_player_logging_out;

	float m_mouse_x;
	float m_mouse_y;

	// actually sends the packet
	void _send_packet(world_packet& packet);

	void set_alias(uint8 id, uint16 extent, uint16 type, uint32 other);
	uint8 get_alias(uint16 extent, uint16 type);

	// Opcodes
	void fill_opcode_handler_table();
	void HandleJoinConfirmation();
	void HandleNewUnknownOpcode(world_packet& recv_data) {};
	void HandlePlayerJoinOpcode(world_packet& recv_data);
	void HandlePlayerInputOpcode(world_packet& recv_data);
	void HandleMouseOpcode(world_packet& recv_data);
	void HandleImportantOpcode(world_packet& recv_data);
	void HandleServerCmdOpcode(world_packet& recv_data);
	void HandleUnknown06Opcode(world_packet& recv_data);
	void HandleNeedTimestampOpcode(world_packet& recv_data);
	void HandleClientReadyOpcode(world_packet& recv_data);
	void HandleNewAliasOpcode(world_packet& recv_data);
	void HandleTextMessageOpcode(world_packet& recv_data);
	void HandleOutgoingClientOpcode(world_packet& recv_data);
	void HandleIncomingClientOpcode(world_packet& recv_data);
	void HandleTryAbilityOpcode(world_packet& recv_data);
	void HandlePlayerSetWaypointOpcode(world_packet& recv_data);
	void HandleTryDropOpcode(world_packet& recv_data);
	void HandleTryGetOpcode(world_packet& recv_data);
	void HandleTryUseOpcode(world_packet& recv_data);
	void HandleTryEquipOpcode(world_packet& recv_data);
	void HandleTryDequipOpcode(world_packet& recv_data);
	void HandleTryCreatureCommandOpcode(world_packet& recv_data);
	void HandleTrySpellOpcode(world_packet& recv_data);
	void HandleTryCollideOpcode(world_packet& recv_data);
	void HandleImportantAckOpcode(world_packet& recv_data);
	void HandleRequestMapOpcode(world_packet& recv_data);
	void HandleCancelMapOpcode(world_packet& recv_data);
	void HandleSysopPwOpcode(world_packet& recv_data);
	void HandleKeepAliveOpcode(world_packet& recv_data);
	void HandleReceivedMapOpcode(world_packet& recv_data);
	void HandleRequestSavePlayerOpcode(world_packet& recv_data);
	void HandleXferMsgOpcode(world_packet& recv_data);
	void HandleTeamMsgOpcode(world_packet& recv_data);
	void HandleServerQuitAckOpcode(world_packet& recv_data);
	void HandleTradeOpcode(world_packet& recv_data);
	void HandleDialogOpcode(world_packet& recv_data);
	void HandleRequestTimerStatusOpcode(world_packet& recv_data);
	void HandleReportSecondaryWeaponOpcode(world_packet& recv_data);
	void HandleInfoBookDataOpcode(world_packet& recv_data);
	void HandleSocialOpcode(world_packet& recv_data);
	void HandleVoteOpcode(world_packet& recv_data);
	void HandleGauntletOpcode(world_packet& recv_data);
	void HandleInventoryFailOpcode(world_packet& recv_data);
	void HandleExitingOpcode(world_packet& recv_data);

	void _SendFullTimestampOpcode();
	void _SendJoinDataOpcode();
	void _SendGameSettingsOpcode();
	void _SendGameSettings2Opcode();
	void _SendNewPlayerOpcode();
	void _SendUseMapOpcode();
	void _SendPartialTimestampOpcode();
	void _SendFadeBeginOpcode();
	void _SendPlayerRespawnOpcode();
	void _SendImportantAckOpcode(uint32 timestamp);
	void _SendClientStatusOpcode();
	void _SendMapSendStart();
	void _SendMapSendPacket();
	void _SendReportSpellStart( uint8 spell );
	void _SendFxDurationSpell( uint16 type, uint16 x, uint16 y );
	void _SendAudioPlayerEvent( uint16 sound, uint8 volume = 100, uint8 unk2 = 0 );

	friend player;
};