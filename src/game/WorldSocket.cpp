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
#include "Log.h"
#include "Opcodes.h"
#include "Database/DatabaseEnv.h"
#include "Auth/Sha1.h"
#include "WorldPacket.h"
#include "WorldSocket.h"
#include "WorldSession.h"
#include "World.h"
#include "WorldSocketMgr.h"
#include "NameTables.h"
#include "Policies/SingletonImp.h"
#include "WorldLog.h"
#include <cwctype>                                          // needs for towupper

// Only GCC 4.1.0 and later support #pragma pack(push,1) syntax
#if __GNUC__ && (GCC_MAJOR < 4 || GCC_MAJOR == 4 && GCC_MINOR < 1)
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

/// Client Packet Header
struct ClientPktHeader
{
    uint8 plr;
	uint8 unk;
	uint8 cmd;
};

/// Server Packet Header
struct ServerPktHeader
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

/// WorldSocket construction and initialisation.
WorldSocket::WorldSocket(ISocketHandler &sh): UdpSocket(sh), _cmd(0), _remaining(0)
{
    _seed = 0xDEADBABE;
    m_LastPingMSTime = 0;                                   // first time it will counted as overspeed maybe, but this is not important
    m_OverSpeedPings = 0;
}

/// WorldSocket destructor
WorldSocket::~WorldSocket()
{
	WorldSession *session;
    for(WorldSessions::iterator itr = _sessions.begin(); itr != _sessions.end(); ++itr)
		if(session = sWorld.FindSession(itr->second))
			session->SetSocket(0);

    WorldPacket *packet;

    ///- Go through the to-be-sent queue and delete remaining packets
    while(!_sendQueue.empty())
    {
        packet = _sendQueue.next();
        delete packet;
    }
}

/// Copy the packet to the to-be-sent queue
void WorldSocket::SendPacket(WorldPacket* packet)
{
    WorldPacket *pck = new WorldPacket(*packet);
    ASSERT(pck);
    _sendQueue.add(pck);
}

/// On client connection
void WorldSocket::OnAccept()
{
    ///- Add the current socket to the list of sockets to be managed (WorldSocketMgr)
    sWorldSocketMgr.AddSocket(this);

    ///- Send a AUTH_CHALLENGE packet
//  WorldPacket packet( SMSG_AUTH_CHALLENGE, 4 );
//	packet << _seed;

//    SendPacket(&packet);
}

/// Read the client transmitted data
void WorldSocket::OnRawData(const char * data,size_t datalen,struct sockaddr * sa,socklen_t salen)
{
	uint8* szBuffer;
	ClientAddr client;
	WorldSession* session = NULL;
	WorldSessions::iterator itr;
	_remaining = 0;

	szBuffer = new uint8[datalen];
	memcpy(szBuffer, data, datalen);
	client = IPtoClientAddr( ((sockaddr_in*)(sa))->sin_addr.S_un.S_addr, ntohs(((sockaddr_in*)(sa))->sin_port) );
	if( (itr = _sessions.find(client)) != _sessions.end())
		session = sWorld.FindSession(itr->second);
	else if( (itr = _sessions.find(client & 0xFFFFFFFF)) != _sessions.end())
	{
		session = sWorld.FindSession(itr->second);
		_sessions[client] = itr->second;
		_sessions.erase(itr);
		session->SetClient(client);
	}
	if( session != NULL)
		session->CryptData(szBuffer, datalen);

    ///- Read the packet header and decipher it (if needed)
    if (datalen < 3)
        return;

    ClientPktHeader *hdr = (ClientPktHeader*)szBuffer;
	szBuffer += sizeof(ClientPktHeader);

    _remaining = datalen - 3;
    _cmd = hdr->cmd;
	
    ///- Read the remaining of the packet

	WorldPacket packet((uint8)_cmd, (uint8)hdr->plr, client, _remaining);
	packet.SetUnk(hdr->unk);

    packet.resize(_remaining);
    if(_remaining) memcpy((char*)packet.contents(), szBuffer, _remaining);

    ///- If log of world packets is enable, log the incoming packet
    if( sWorldLog.LogWorld() )
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
    }

    ///- If thepacket is PING or AUTH_SESSION, handle immediately
    switch (_cmd)
    {
		case 0xC:
			_HandleLANPing(packet);
			break;
		case 0xE:
			_HandleJoin(packet);
			break;
        default:
        {
            ///- Else, put it in the world session queue for this user (need to be already authenticated)
            if (session)
                session->QueuePacket(packet);
            //else
            //    sLog.outDetail("Received out of place packet with cmdid 0x%.4X", _cmd);
            break;
        }
    }
	delete (uint8*)hdr;
}

/// On socket closing
void WorldSocket::OnDelete()
{
	WorldSession *session;
    for(WorldSessions::iterator itr = _sessions.begin(); itr != _sessions.end(); ++itr)
		if(session = sWorld.FindSession(itr->second))
		{
			session->SetSocket(0);
			sWorld.RemoveSession(itr->second);
		}

    ///- Remove the socket from the WorldSocketMgr list
    sWorldSocketMgr.RemoveSocket(this);
}

/// Handle the Join packet
void WorldSocket::_HandleJoin(WorldPacket& recvPacket)
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
        sLog.outDetail("Incomplete join packet");
        return;
    }

	WorldSession *session = new WorldSession(sWorld.RequestFreeId(), this, 0);
	session->SetClient(recvPacket.GetClient());
	sWorld.AddSession(session);
	_sessions[recvPacket.GetClient()&0xFFFFFFFF] = session->GetAccountId();

	WorldPacket packet( 0x14, 0, recvPacket.GetClient(), 0 );
	SendPacket(&packet);
}

/// Handle the Ping packet
void WorldSocket::_HandleLANPing(WorldPacket& recvPacket)
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
        sLog.outDetail("Incomplete ping packet");
        return;
    }

    ///- And put the pong answer in the to-be-sent queue
    WorldPacket packet( 0x0D, 0, recvPacket.GetClient(), 70+4 ); //73 bytes for packet plus 4 due to name of the game
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
    SendPacket(&packet);

    return;
}

/// Handle the update order for the socket
void WorldSocket::Update(time_t diff)
{
    WorldPacket *packet;
    ServerPktHeader hdr;
    ///- While we have packets to send
    while(!_sendQueue.empty())
    {
        packet = _sendQueue.next();

		hdr.plr = packet->GetAccountId();
		hdr.unk = packet->GetUnk();
        hdr.cmd = packet->GetOpcode();

        if( sWorldLog.LogWorld() )
        {
            sWorldLog.Log("SERVER:\nSOCKET: %u\nLENGTH: %u\nOPCODE: %s (0x%.4X)\nDATA:\n",
                (uint32)GetSocket(),
                packet->size(),
                LookupName(packet->GetOpcode(), g_worldOpcodeNames),
                packet->GetOpcode());

            uint32 p = 0;
            while (p < packet->size())
            {
                for (uint32 j = 0; j < 16 && p < packet->size(); j++)
                    sWorldLog.Log("%.2X ", (*packet)[p++]);

                sWorldLog.Log("\n");
            }

            sWorldLog.Log("\n\n");
        }

        ///- Encrypt (if needed) the header
    //    _crypt.EncryptSend((uint8*)&hdr, 4);

        ///- Send the header and body to the client
		uint8* buffer = new uint8[3+packet->size()];
		memcpy(buffer, &hdr, 3);
		if(packet->size()) memcpy(buffer+3, packet->contents(), packet->size());
		if(packet->GetXor()) CryptData(packet->GetXor(), buffer, packet->size()+3);
        UdpSocket::SendToBuf(ClientAddrtoIP(packet->GetClient()), ClientAddrtoPort(packet->GetClient()), (char*)buffer, packet->size()+3);
		delete buffer;

        delete packet;
    }
}
void WorldSocket::CryptData(uint8 xor, uint8* data, uint32 datalen)
{
	if(xor == 0)
		return;
	for(uint32 itr = 0; itr < datalen; itr++)
		*(data+itr) = *(data+itr) ^ xor;
	return;
}