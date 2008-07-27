#include "global.h"
#include "world.h"
#include "world_socket.h"
#include "world_packet.h"
#include "world_session.h"
#include "socket_mgr.h"

using namespace boost::asio;
using namespace boost::asio::ip;

// Only GCC 4.1.0 and later support #pragma pack(push,1) syntax
#if __GNUC__ && (GCC_MAJOR < 4 || GCC_MAJOR == 4 && GCC_MINOR < 1)
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

/// Client Packet Header
struct client_pkt_header
{
    uint8 plr;
	uint8 unk;
	uint8 cmd;
};

/// Server Packet Header
struct server_pkt_header
{
    uint8 plr;
	uint8 unk;
    uint8 cmd;
};

// Only GCC 4.1.0 and later support #pragma pack(pop) syntax
#if __GNUC__ && (GCC_MAJOR < 4 || GCC_MAJOR == 4 && GCC_MINOR < 1)
#pragma pack()
#else
#pragma pack(pop)
#endif



// we should move this to configuration later
#define GAME_PORT 18590

world_socket::world_socket(io_service& s) : m_io_service(s), m_socket(m_io_service)
{
}

void world_socket::listen()
{
	m_socket.open(udp::v4());
	m_socket.bind(udp::endpoint(udp::v4(), GAME_PORT));
}

void world_socket::update()
{
	// receive stuff
	//m_socket.async_receive_from(boost::asio::buffer(m_data, MAX_LEN), m_sender, 0, boost::bind(&world_socket::handle_receive_from, this, placeholders::error, placeholders::bytes_transferred));
	while(m_socket.available())
	{
		boost::system::error_code ec;
		size_t size = m_socket.receive_from(boost::asio::buffer(m_data, MAX_LEN), m_sender, 0, ec);
		handle_receive_from(ec, size);
	}

	std::set<world_sessions::key_type> to_destroy;

	// update sessions first (maybe we should move this out of world_socket)
	BOOST_FOREACH(world_sessions::value_type p, m_sessions)
	{
		if(!p.second->update())
			to_destroy.insert(p.first);
	}

	BOOST_FOREACH(world_sessions::key_type p, to_destroy)
	{
		delete m_sessions[p];
		m_sessions.erase(p);
	}

	// clean out send queue
	// we do this by asyncronously sending the first packet, if there is one
	// then repeating in the async. handler
	while(!m_send_queue.empty())
		send_next_packet();
}

void world_socket::send_next_packet()
{
	// we assume that m_send_queue is not empty, since we were called
	world_packet* pkt = m_send_queue.front();	
	m_send_queue.pop();

	server_pkt_header* hdr = (server_pkt_header*)m_send_data;
	hdr->plr = pkt->get_account_id();
	hdr->cmd = pkt->get_opcode();
	hdr->unk = pkt->get_unk();

	pkt->read(m_send_data+sizeof(server_pkt_header), pkt->size());

	size_t size = pkt->size() + sizeof(server_pkt_header);
	crypt_data(pkt->get_xor(), m_send_data, size);

	m_socket.send_to(boost::asio::buffer(m_send_data, size), pkt->get_endpoint());

	// we can now destroy the packet
	// we allocate this in queue_packet
	delete pkt;
}

void world_socket::queue_packet(world_packet& packet)
{
	m_send_queue.push(new world_packet(packet));
}

void world_socket::handle_receive_from(const boost::system::error_code& err, size_t bytes_recvd)
{
	if(err)
		return;

	uint8* szBuffer = m_data;
	world_session* session = NULL;
	world_sessions::iterator itr;
	size_t remaining = 0;

	//szBuffer = new uint8[bytes_recvd];
	//memcpy(szBuffer, data, datalen);
	if( (itr = m_sessions.find(m_sender)) != m_sessions.end())
		session = itr->second;
	if( session != NULL)
		crypt_data(session->get_xor_key(), szBuffer, bytes_recvd);

    ///- Read the packet header and decipher it (if needed)
    if (bytes_recvd < 3)
        return;

    client_pkt_header *hdr = (client_pkt_header*)szBuffer;
	szBuffer += sizeof(client_pkt_header);

    remaining = bytes_recvd - 3;
	
    ///- Read the remaining of the packet

	world_packet packet((uint8)hdr->cmd, (uint8)hdr->plr, m_sender, remaining);
	packet.set_unk(hdr->unk);

	if(remaining) packet.append(szBuffer, remaining);

    ///- If log of world packets is enable, log the incoming packet
    /*if( sWorldLog.LogWorld() )
    {
        sWorldLog.Log("CLIENT:\nSOCKET: %u\nLENGTH: %u\nOPCODE: %s (0x%.4X)\nDATA:\n",
            (uint32)GetSocket(),
            packet.size(),
            LookupName(packet.GetOpcode(), g_worldOpcodeNames),
            packet.GetOpcode());

        uint32 p = 0;
        while (p < packet.size())
        {
            for (uint32 j = 0; j < 16 && p < packet.size(); j++)
                sWorldLog.Log("%.2X ", packet[p++]);
            sWorldLog.Log("\n");
        }
        sWorldLog.Log("\n\n");
    }*/

    ///- If thepacket is PING or AUTH_SESSION, handle immediately
	switch (hdr->cmd)
    {
		case 0xC:
			handle_lan_ping(packet);
			break;
		case 0xE:
			handle_join(packet);
			break;
        default:
        {
            ///- Else, put it in the world session queue for this user (need to be already authenticated)
            if (session)
                session->queue_packet(packet);
            //else
            //    sLog.outDetail("Received out of place packet with cmdid 0x%.4X", _cmd);
            break;
        }
    }
}

void world_socket::crypt_data(uint8 xor, uint8* data, uint32 datalen)
{
	if(xor == 0)
		return;
	for(uint32 itr = 0; itr < datalen; itr++)
		*(data+itr) = *(data+itr) ^ xor;
	return;
}

void world_socket::handle_lan_ping(world_packet& recvPacket)
{
	uint8 unknown[5];
	uint32 stamp;

    ///- Get the ping packet content
    try
    {
        recvPacket >> unknown[0];
		recvPacket >> unknown[1];
		recvPacket >> unknown[2];
		recvPacket >> unknown[3];
		recvPacket >> unknown[4];
		recvPacket >> stamp;
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete ping packet");
        return;
    }

    ///- And put the pong answer in the to-be-sent queue
	world_packet packet( 0x0D, 0, recvPacket.get_endpoint(), 70+4 ); //73 bytes for packet plus 4 due to name of the game
    packet << (uint8)0; //Current players?
	packet << (uint8)1; //Max players
	packet << (uint8)0xF;
	packet << (uint8)0xF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet.append((char*)"Estate\0\0\0", 9); //Needs to be nine characters
	packet << (uint8)0x1;
	packet << (uint8)0x0;
	packet << (uint8)0x0;
	packet << (uint8)0x55;
	packet << (uint8)0x0;
	packet << (uint8)0x9A;
	packet << (uint8)0x3;
	packet << (uint8)0x1;
	packet << (uint8)0x0;
	packet << (uint8)0x7;
	packet << (uint8)0x21;
	packet << (uint8)0x3;
	packet << (uint8)0x10;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xC0;
	packet << (uint8)0x0;
	packet << (uint8)0xD4;
	packet << (uint8)0x0;
	packet << (uint32)stamp;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xEF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0xFF;
	packet << (uint8)0x84;
	packet << (uint8)0x82;
	packet << (uint8)0xD3;
	packet << (uint8)0x01;
	packet.append((char*)"Test", 4);
	packet << (uint8)0x0;
    queue_packet(packet);

    return;
}

void world_socket::handle_join(world_packet& recvPacket)
{
	uint8 zero1;
	uint8 data[28];
	wchar_t name[13];
	char* serial[23];
	char* username[9];
	uint16 zero2;
	uint32 zero3;
	uint32 unknown1;
	uint32 unknown2;
	
    try
    {
		recvPacket >> zero1;
        recvPacket.read((uint8*)name, 24); // 12 wchar = 24 bytes
		name[12] = 0;
		recvPacket.read(data, 28);
		recvPacket.read((uint8*)serial, 22);
		serial[22] = 0;
		recvPacket >> zero2;
		recvPacket >> unknown1;
		recvPacket >> zero3;
		recvPacket.read((uint8*)username, 8);
		username[8] = 0;
		recvPacket >> unknown2;
    }
    catch(ByteBuffer::error &)
    {
        //sLog.outDetail("Incomplete join packet");
        return;
    }

	world_session *session = new world_session(world::instance->get_free_id(), recvPacket.get_endpoint());
//	sWorld.AddSession(session);
	m_sessions[recvPacket.get_endpoint()] = session;

	world_packet packet( 0x14, 0, recvPacket.get_endpoint(), 0 );
	queue_packet(packet);
}
void world_socket::send_to_all(world_packet& packet)
{
	BOOST_FOREACH(world_sessions::value_type p, m_sessions)
	{
		p.second->send_packet(packet);
	}
}