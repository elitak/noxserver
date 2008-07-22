#include "global.h"
#include "world_session.h"
#include "world_packet.h"
#include "world_socket.h"
#include "object_mgr.h"
#include "player.h"
#include "NoxMap.h"

world_session::world_session(uint8 id, boost::asio::ip::udp::endpoint endpoint) : 
m_endpoint(endpoint), m_player_id(id), m_send_packet(0, id, endpoint), m_player_loading(false),
m_status(STATUS_AUTHED), m_player_downloading(0), m_player_observing(true)
{
}

void world_session::queue_packet(world_packet& packet)
{
	m_receive_queue.push(new world_packet(packet)); // push a copy of the packet onto the queue
}

void world_session::update(uint32 diff)
{
	while(!m_receive_queue.empty())
	{
		world_packet* packet = m_receive_queue.front();

		// do some shit

		m_receive_queue.pop();

		// we allocate this in queue_packet
		delete packet;
	}
}

void world_session::set_alias(uint8 id, uint16 extent, uint16 type, uint32 other)
{
	if(id > 0xFE) // 0xFF is reserved
		return;

	return;
}

uint8 world_session::get_alias(uint16 extent, uint16 type)
{
	return 0;
}

void world_session::send_packet(world_packet& packet, bool ifReady, bool changeUnk)
{
	if (!ifReady || !is_player_loading())
	{	
		if(changeUnk)
			packet.set_unk(m_unk);

		if(m_send_packet.get_opcode() && (packet.get_unk() != m_send_packet.get_unk() || packet.get_account_id() != m_send_packet.get_account_id())) // || packet.GetAccountId() != sendPacket.GetAccountId()
			_send_packet(m_send_packet);
		if(!m_send_packet.get_opcode())
		{
			m_send_packet.set_opcode(packet.get_opcode());
			m_send_packet.set_account_id(packet.get_account_id());
			m_send_packet.set_unk(packet.get_unk());
		}
		else
			m_send_packet << (uint8)packet.get_opcode();
		
		if(packet.size() > 0)
			m_send_packet.append(packet.contents(), packet.size());
	}
}

void world_session::_send_packet(world_packet& packet)
{
	world_socket& socket = socket_mgr::instance->get_world_socket();

	packet.set_endpoint(m_endpoint);
	packet.set_xor(m_xor);
	socket.queue_packet(packet);

	packet.initialize(0, 0, m_endpoint);
}

void world_session::_SendFullTimestampOpcode()
{
	world_packet packet(MSG_FULL_TIMESTAMP, 0x80, m_endpoint, 4);
	packet << m_timestamp;
	send_packet(packet, false);
}
void world_session::_SendJoinDataOpcode()
{
	world_packet packet(MSG_JOIN_DATA, 0x80, m_endpoint, 6);
	packet << m_player->get_extent();
	packet << (uint32)0;
	send_packet(packet, false);
}
void world_session::_SendGameSettings2Opcode()
{
	world_packet packet(MSG_GAME_SETTINGS_2, 0x80, m_endpoint, 0x30);
	packet.append("ZoaEDK's Game\0\0\0", 0x10); // 0x10
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0xFFFFFFFF;
	packet << (uint32)0x0;
	send_packet(packet, false);
}
void world_session::_SendGameSettingsOpcode()
{
	world_packet packet(MSG_GAME_SETTINGS, 0x80, m_endpoint, 0x13);
	packet << m_timestamp;
	packet << (uint32)0x0001039A;
	packet << (uint32)0x00032080;
	packet << (uint32)0x00002002;
	packet << (uint8)0x20;
	packet << (uint8)0x0F;
	packet << (uint8)0x00;
	send_packet(packet, false);
}
void world_session::_SendNewPlayerOpcode()
{
	world_packet packet(0, 0, m_endpoint);
	//m_player->_BuildNewPlayerPacket(packet);
	//objectAccessor::Instance().send_packetToAll(packet);

	//send_packet(packet, false); // to all won't send to the current player due to playerLoading variable, so we to do this ourselves
}
void world_session::_SendUseMapOpcode()
{
	m_player_loading = true;

	world_packet packet(MSG_USE_MAP, 0x80, m_endpoint, 0x28);
	packet.append("manamine.map\0", 0xD); //0xD // 0x20?
	packet << (uint16)0x0;
	packet << (uint32)0x0;
	packet << (uint32)0x0;
	packet << (uint32)0x0;
	packet << (uint16)0x0;
	packet << (uint16)0x0;
	packet << (uint8)0;
	packet << (uint32)world::instance->get_map()->GetChecksum(); //checksum
	packet << (uint32)m_timestamp;
	send_packet(packet, false);
}
void world_session::_SendPartialTimestampOpcode()
{
	world_packet packet(MSG_PARTIAL_TIMESTAMP, 0x0, m_endpoint, 2);
	packet << (uint16)m_timestamp;
	send_packet(packet, false);
}
void world_session::_SendFadeBeginOpcode()
{
	world_packet packet(MSG_FADE_BEGIN, 0x0, m_endpoint, 2);
	packet << (uint8)1;
	packet << (uint8)1;
	send_packet(packet, false);
}
void world_session::_SendImportantAckOpcode(uint32 timestamp)
{
	world_packet packet(MSG_IMPORTANT_ACK, 0x0, m_endpoint, 4);
	packet << (uint32)timestamp;
	send_packet(packet, false);
}
void world_session::_SendPlayerRespawnOpcode()
{
	world_packet packet(MSG_PLAYER_RESPAWN, 0x0, m_endpoint, 10);
	packet << (uint16)m_player->get_extent();
	packet << (uint32)m_timestamp;
	packet << (uint8)0xFF;
	packet << (uint8)0x01;
	//objacc.send_packetToAll(packet);
	packet.initialize(MSG_SIMPLE_OBJ, 0, m_endpoint);
	packet << (uint16)m_player->get_extent();
	packet << (uint16)m_player->get_type();
	packet << (uint16)m_player->get_position_x();
	packet << (uint16)m_player->get_position_y();
	//objacc.send_packetToAll(packet);
}
/*void world_session::_SendUpdateStreamOpcode()
{
	world_packet packet(MSG_UPDATE_STREAM, 0x0, m_endpoint, 11);
	packet << (uint8)0xFF;
	packet << (uint16)m_player->get_extent();
	packet << (uint16)m_player->GetType();
	packet << (uint16)m_player->GetPosition().x_coord;
	packet << (uint16)m_player->GetPosition().y_coord;
	packet << (uint8)( rot_data[(uint8)floor((double)m_player->GetAngle() / 0x20 + 0.5) & 0x7] );
	packet << (uint8)m_player->m_action;
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	packet << (uint8)0x00;
	send_packet(packet);
}*/

void world_session::_SendClientStatusOpcode()
{
	world_packet packet(MSG_REPORT_CLIENT_STATUS, 0x0, m_endpoint, 2);
	packet << (uint16)m_player->get_extent();
	packet << (uint32)m_player_observing;
	//objacc.send_packetToAll(packet);
	
	if(m_player_observing) //should a different function send this?
	{
		packet.initialize(MSG_INFORM, 0, m_endpoint);
		packet << (uint8)0x0C;
		packet << (uint32)m_player_observing;
		send_packet(packet);
	}
}
void world_session::_SendMapSendStart()
{
	if(!m_player_downloading)
	{
		world_packet packet(MSG_MAP_SEND_START, 0x80, m_endpoint, 87);
		packet << (uint8)0xF8; //unknown
		packet << (uint16)0x0013; //unknown
		packet << (uint32)world::instance->get_map()->GetNxzSize();
		packet.append(world::instance->get_map()->GetNxzName(), 0x50);

		send_packet(packet, false);
		m_player_downloading = 1;
		m_unk2 = m_unk;
	}
}
void world_session::_SendMapSendPacket()
{
	m_unk2++;
	int PACKET_SIZE = 0x200;
	uint8 data[0x200];

	//put data in data and update size
	PACKET_SIZE = world::instance->get_map()->ReadNxzBytes( (m_player_downloading - 1) * PACKET_SIZE, data, PACKET_SIZE);

	world_packet packet(MSG_MAP_SEND_PACKET, 0x80, m_endpoint, PACKET_SIZE + 0x5);
	packet << (uint8)0xF8; //unknown/not used
	packet << (uint16)m_player_downloading; //packet #
	packet << (uint16)PACKET_SIZE; //data size
	packet.append(data, PACKET_SIZE);
	packet.set_unk(m_unk2);
	send_packet(packet, false, false);

	m_player_downloading++;
	if((m_player_downloading - 1) * PACKET_SIZE > world::instance->get_map()->GetNxzSize())
		m_player_downloading = 0;
}
//ONLY PLAYER CAN HEAR IT. Used in things such as flag capture, flag return
//lesson limit, etc.
void world_session::_SendAudioPlayerEvent( uint16 sound, uint8 volume, uint8 unk2 )
{
   	world_packet packet(MSG_AUDIO_PLAYER_EVENT, 0x00, m_endpoint, 3);
	packet << (uint8)unk2;
    packet << (uint16) ( (volume << 0x9) | (sound & 0x3FF) ) ;
	send_packet(packet);
}
void world_session::_SendReportSpellStart( uint8 spell )
{
    world_packet packet(MSG_REPORT_SPELL_START, 0x00, m_endpoint, 1);
	packet << (uint8)spell;
	send_packet(packet);
}
void world_session::_SendFxDurationSpell( uint16 type, uint16 x, uint16 y )
{
    world_packet packet(MSG_FX_DURATION_SPELL, 0x00, m_endpoint, 6);
	packet << type;
    packet << x;
    packet << y;
	send_packet(packet);
}

void world_session::HandleJoinConfirmation()
{
	/// Generate XOR key
	//if(!xorKey)
	//	xorKey = (uint8)urand(0, 0xFF);
	world_packet packet(0x1F, 0x80, m_endpoint);
	packet << (uint8)0;
	packet << (uint8)1;
	packet << (uint8)m_player_id;
	packet << (uint8)0;
	packet << (uint8)0;
	packet << (uint8)0;
	packet << (uint8)m_xor;
	socket_mgr::instance->get_world_socket().queue_packet(packet);
	m_status = STATUS_CONFIRMED;
}

void world_session::HandlePlayerJoinOpcode(world_packet &recvPacket)
{
	uint8 data[0x20];	
	if(!m_player)
		m_player = new player(*this);

	try
    {
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read((uint8*)(&m_player->m_player_info), 0x61);
		m_player->m_player_info.name[0x18] = 0;
		recvPacket.read(data, 0x1F);
		recvPacket.read((uint8*)m_player->m_player_info.username, 9);
		m_player->m_player_info.username[8] = 0;
		recvPacket.read(m_player->m_player_info.unk1, 0x0C);
		recvPacket.read(data, 0x03);
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete join packet");
        return;
    }
	m_status = STATUS_LOGGEDIN;

	//objectAccessor::Instance().InsertPlayer(m_player);
	m_unk++;

	//m_player->SetName(name);

	world_packet packet(0xA9, 0x80, m_endpoint, 0x5);
	packet << (uint8)0x0C;
	packet << (uint32)1;
	send_packet(packet, false);

	_SendFullTimestampOpcode();
	_SendJoinDataOpcode();
	_SendGameSettingsOpcode();
	_SendGameSettings2Opcode();
	_SendNewPlayerOpcode();
	m_unk++;
	_SendUseMapOpcode();

	packet.set_opcode(0x04);
	packet.set_account_id(0x00);
	packet.set_unk(0x00);
	packet.clear();
	send_packet(packet, false, false);
	_send_packet(m_send_packet); //clear the send buffer

	m_timestamp++;
	_SendPartialTimestampOpcode();
}


void world_session::HandlePlayerInputOpcode(world_packet &recvPacket)
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
				//sLog.outDebug("Player attacked.");
				//if observer
				m_player->attack();
				break;
			case 0x07:
				//jumped
				//sLog.outDebug("Player jumped.");
                m_player->jump();
				break;
            case 0x30:
                 //sLog.outDebug("%s laughed.");
                 m_player->laugh();
                 break;
            case 0x31:
                 //sLog.outDebug("Player pointed.");
                 m_player->point();
                 break;
            case 0x2F:
                 //sLog.outDebug("Player taunted.");
                 m_player->taunt();
                 break;
			default:
				recvPacket.rpos(recvPacket.rpos()-1);
				//sLog.outDebug("Player input: 0x0E 0x%2X", recvPacket.read<uint8>());
				break;
			}
			recvPacket.read<uint8>();
			recvPacket.read<uint8>();
			recvPacket.read<uint8>();
			break;
		case 0x0F: //move: 0F 02 00 00 00 01
			m_player->move_towards(m_mouse_x, m_mouse_y);
			recvPacket.read<uint32>();
			recvPacket.read<uint8>();
			break;
		case 0x13: //running jump: 13 07 00 00 00 02 01 00 00 01
				   //13 06 00 00 00 02 01 00 00 01
               switch(recvPacket.read<uint32>())
               {
                    case 0x07:
                         //sLog.outDebug("Player did a running jump.");
                         m_player->running_jump();
                         break;
               }
			recvPacket.read<uint8>();
			recvPacket.read<uint8>();
			recvPacket.read<uint16>();
			recvPacket.read<uint8>();
			break;
		default:
			recvPacket.rpos(recvPacket.rpos()-1);
				//sLog.outDebug("Player input: 0x%2X", recvPacket.read<uint8>());
			break;
		}
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		m_player->set_angle(recvPacket.read<uint8>());
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
		recvPacket.read<uint8>();
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete player input packet");
        return;
    }
}
void world_session::HandleMouseOpcode(world_packet &recvPacket)
{
	try
    {
		m_mouse_x = recvPacket.read<uint16>(); //x
		m_mouse_y = recvPacket.read<uint16>(); //y
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete packet");
        return;
    }
}
void world_session::HandleUnknown06Opcode(world_packet &recvPacket)
{
	try
    {
		recvPacket.read<uint32>();
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete packet");
        return;
    }
}
void world_session::HandleImportantOpcode(world_packet &recvPacket)
{
	try
    {
		_SendImportantAckOpcode(recvPacket.read<uint32>());
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete packet");
        return;
    }
}
void world_session::HandleNeedTimestampOpcode(world_packet &recvPacket)
{
	try
    {
		_SendFullTimestampOpcode();
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete packet");
        return;
    }
}
void world_session::HandleServerCmdOpcode(world_packet &recvPacket)
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
        //sLog.outDetail("Incomplete packet");
        return;
    }
}


void world_session::HandleClientReadyOpcode(world_packet &recvPacket)
{
	m_unk = recvPacket.get_unk() + 1;
	if(m_player_loading)
	{
		m_player_loading = false;
		m_timestamp++;
		_SendPartialTimestampOpcode();
	//	m_player->SendUpdatePacket();
		_SendFadeBeginOpcode();
		m_player->respawn();
	//	objectAccessor::Instance().SendPlayerInfo(this);
	}
}

void world_session::HandleNewAliasOpcode(world_packet &recvPacket)
{
	try
    {
		set_alias(recvPacket.read<uint8>(), recvPacket.read<uint16>(), recvPacket.read<uint16>(), recvPacket.read<uint32>());
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete packet");
        return;
    }
}
void world_session::HandleTextMessageOpcode(world_packet &recvPacket)
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

		//sLog.outDebug("Player say: %s", buffer);

		delete[] buffer;

		// Send out to all players
		world_packet packet(MSG_TEXT_MESSAGE, 0, m_endpoint, pend-pstart);
		packet.append(recvPacket.contents() + pstart, pend-pstart);
		//objectAccessor::Instance().send_packetToAll(packet);
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete packet");
        return;
    }
}
void world_session::HandleOutgoingClientOpcode(world_packet &recvPacket)
{
	try
    {
		logout();
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete packet");
        return;
    }
}
void world_session::HandleIncomingClientOpcode(world_packet &recvPacket)
{
	try
    {
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete packet");
        return;
    }
}
void world_session::HandleTryAbilityOpcode(world_packet& recv_data)
{
	try
    {
		uint8 ability = recv_data.read<uint8>();
		//sLog.outDebug("Player TryAbility: 0x%2X", ability);

		//if(get_player()->is_ability_ready(ability))
		//	execute_ability(ability);
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete packet");
        return;
    }
}


void world_session::HandlePlayerSetWaypointOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	
	recv_data.read<uint16>();
	recv_data.read<uint16>();
	recv_data.read<uint16>();
}
void world_session::HandleTryDropOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
    object* obj = object_mgr::instance->get_object(recv_data.read<uint16>());
    uint16 x = recv_data.read<uint16>();
    uint16 y = recv_data.read<uint16>();
    if(!(m_player->drop(obj, 75, x, y)))
		return;
             //sLog.outDebug("Couldn't drop object.");

}
void world_session::HandleTryGetOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
	object* obj = object_mgr::instance->get_object(recv_data.read<uint16>());
	m_player->pickup(obj, 75);
}
void world_session::HandleTryUseOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
	object* obj = object_mgr::instance->get_object(recv_data.read<uint16>());
	if(obj)
		obj->use(get_player());
}
void world_session::HandleTryEquipOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
	object* obj = object_mgr::instance->get_object(recv_data.read<uint16>());
	m_player->equip(obj);
}
void world_session::HandleTryDequipOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
	object* obj = object_mgr::instance->get_object(recv_data.read<uint16>());
	m_player->dequip(obj);
}
void world_session::HandleTryCreatureCommandOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint16>();
	recv_data.read<uint8>();
}
void world_session::HandleTrySpellOpcode(world_packet& recv_data)
{
	uint8 spellId = recv_data.read<uint8>();
	recv_data.rpos(recv_data.rpos() + 19);
	uint8 dontinvert = recv_data.read<uint8>();

     //inversion was backwards.
	//if( ExecuteSpell(spellId, (bool)dontinvert) ) //if spell cast succeeded
    //    _SendReportSpellStart(spellId);
    _SendFxDurationSpell(7, get_player()->get_position_x(), get_player()->get_position_y());

	//				(sp. id)													(invert)
	//summon ghost: 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	//summon bear:  4D 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01
	//meteor:		34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
}
void world_session::HandleTryCollideOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint16>();
}
void world_session::HandleImportantAckOpcode(world_packet& recv_data)
{
	recv_data.read<uint32>(); // should check this if we sent out an important packet
}
void world_session::HandleRequestMapOpcode(world_packet& recv_data)
{
	m_unk = recv_data.get_unk();
	_SendMapSendStart();
}
void world_session::HandleCancelMapOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
}
void world_session::HandleSysopPwOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    //recv_data.hexlike();
	recv_data.rpos(recv_data.rpos()+20);
}
void world_session::HandleKeepAliveOpcode(world_packet& recv_data)
{
	// this opcode just keeps the connection open
}
void world_session::HandleReceivedMapOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();

	m_player_downloading = 0;
}
void world_session::HandleRequestSavePlayerOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint16>();
}
void world_session::HandleXferMsgOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
	recv_data.read<uint8>();
	recv_data.read<uint8>();
	recv_data.read<uint32>();
	recv_data.rpos(recv_data.rpos()+0x84);
}
void world_session::HandleTeamMsgOpcode(world_packet& recv_data)
{
    //sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.rpos(recv_data.size());
	recv_data.read<uint8>();
	recv_data.read<uint32>();
	recv_data.read<uint16>();
	recv_data.read<uint16>();
}
void world_session::HandleServerQuitAckOpcode(world_packet& recv_data)
{
//    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
}
void world_session::HandleTradeOpcode(world_packet& recv_data)
{
//    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
	recv_data.read<uint16>();
}
void world_session::HandleDialogOpcode(world_packet& recv_data)
{
//    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
	recv_data.read<uint8>();
}
void world_session::HandleRequestTimerStatusOpcode(world_packet& recv_data)
{
//    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
}
void world_session::HandleReportSecondaryWeaponOpcode(world_packet& recv_data)
{
	object* obj = object_mgr::instance->get_object(recv_data.read<uint16>());
	m_player->equip_secondary(obj);
}
void world_session::HandleInfoBookDataOpcode(world_packet& recv_data)
{
//    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint16>();
	recv_data.read<uint8>();
}
void world_session::HandleSocialOpcode(world_packet& recv_data)
{
     //30 = laugh 31 = point
//    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
}
void world_session::HandleVoteOpcode(world_packet& recv_data)
{
//    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
	recv_data.rpos(recv_data.rpos()+0x32);
}
void world_session::HandleGauntletOpcode(world_packet& recv_data)
{
//    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint8>();
}
void world_session::HandleInventoryFailOpcode(world_packet& recv_data)
{
//    sLog.outDebug("New Unknown Opcode %u", recv_data.GetOpcode());
    recv_data.hexlike();
	recv_data.read<uint16>();
}