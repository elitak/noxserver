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
#include "WolSocket.h"
#include "Policies/SingletonImp.h"
#include "WorldLog.h"
#include "NoxCrypt.h"
#include <fstream>
using namespace std;
// Only GCC 4.1.0 and later support #pragma pack(push,1) syntax
#if __GNUC__ && (GCC_MAJOR < 4 || GCC_MAJOR == 4 && GCC_MINOR < 1)
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

// Only GCC 4.1.0 and later support #pragma pack(pop) syntax
#if __GNUC__ && (GCC_MAJOR < 4 || GCC_MAJOR == 4 && GCC_MINOR < 1)
#pragma pack()
#else
#pragma pack(pop)
#endif

/// WolSocket construction and initialisation.
WolSocket::WolSocket(ISocketHandler &sh) : TcpSocket(sh)
{
	this->SetLineProtocol(true);
	strcpy(m_username, "Server");
	strcpy(m_password, "Secret");
}

/// WolSocket destructor
WolSocket::~WolSocket()
{
    WorldPacket *packet;

    ///- Go through the to-be-sent queue and delete remaining packets
    while(!_sendQueue.empty())
    {
        packet = _sendQueue.next();
        delete packet;
    }
}

void WolSocket::OnLine(const std::string & line)
{
	sLog.outDebug(line.c_str());
	if(line.find("PING") != std::string::npos)
		Send("PONG :ircdev.westwood.com\n");
}

void WolSocket::OnConnect()
{
	unsigned char gameData[] = { 0x9A,0x03,0x01,0x80,0xFE,0x87,0x88,0x80,0xE4,0xFF,0xFF,0xFF,0xFF,0xFB,0xC4,0xB4,0xEA,0xDB,0xAF,0xCF,0xE0,0x95,0xF3,0x86,0x80,0x80,0xAD,0xEF,0xC2,0x95,0xA2,0xB4,0x89,0x88,0xAA,0xE5,0xE6,0xD1,0x83,0x80,0x80,0x80,0x80,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x87,0xC1,0xBC,0x80,0x80,0x80,0x0D,0x0A,0x00 };
	//unsigned char gameData2[] = { 0x9A,0x03,0x01,0x80,0xFE,0x87,0x88,0x80,0xE4,0xFF,0xFF,0xFF,0xFF,0xFB,0xC4,0xB4,0xEA,0xDB,0xAF,0xCF,0xE0,0x95,0xF3,0x86,0x80,0x80,0xAD,0xEF,0xC2,0x95,0xA2,0xB4,0x89,0x88,0xAA,0xE5,0xE6,0xD1,0x83,0x80,0x80,0x80,0x80,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x87,0xC1,0xBC,0x80,0x80,0x80,0x0D,0x0A,0x00 };
	//fstream file;
	//NoxCrypt crypt;
	//file.open("outpacket.bin",ios::out | ios::binary);
	//crypt.decrypt_bitwise((unsigned char*)&gameData2[3], 0x4E);
	//file.write((const char*)&gameData2[0],0x54);
	//file.close();
	char buffer[0x100];
	int length;

	Send("CVERS 11015 4610\n");
	Send("PASS supersecret\n");
	Sendf("NICK %s\n", m_username);
	Sendf("apgar %s\n", m_password);
	Send("USER UserName HostName irc.westwood.com :RealName\n");
	Send("verchk 32512 720911\n");
	Send("SETLOCALE 2\n");
	Send("SETCODEPAGE 1252\n");
	Send("SETOPT 17,33\n");
	Sendf("JOINGAME #%s's_game 1 32 37 3 1 1 41681472\n", m_username);
	sprintf(buffer, "TOPIC #%s's_game :g1P3", m_username);
	length = strlen(buffer);
	memcpy(buffer+length, gameData, 0x54);
	SendBuf(buffer, 0x54+length-1); 
	Send("LIST 37 37\n");
	SendBuf(buffer, 0x54+length-1); 

	Sendf("LISTSEARCH 9472 -1 0 0 0 :%s:\n", m_username);
	Send("LIST 37 37\n");
	
}
/// Copy the packet to the to-be-sent queue
void WolSocket::SendPacket(WorldPacket* packet)
{
    WorldPacket *pck = new WorldPacket(*packet);
    ASSERT(pck);
    _sendQueue.add(pck);
}

void WolSocket::encode(unsigned char *password, int len) {
    unsigned char    pwd1[9],
            pwd2[9],
            edx,
            esi,
            i,
            *p1,
            *p2;

	const char HASH[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";


    memset(pwd1, 0x00, sizeof(pwd1));
    memset(pwd2, 0x00, sizeof(pwd2));
    memcpy(pwd1, password, len);

    esi = len;
    p1 = pwd1;
    p2 = pwd2;
    for(i = 0; i < len; i++) {
        if(*p1 & 1) {
            edx = *p1 << 1;
            edx &= *(pwd1 + esi);
        } else {
            edx = *p1 ^ *(pwd1 + esi);
        }
        *p2++ = edx;
        esi--;
        p1++;
    }


    p1 = pwd1;
    p2 = pwd2;
    for(i = 0; i < 8; i++) {
        edx = *p2++ & 0x3f;
        *p1++ = HASH[edx];
    }

    memcpy(password, pwd1, len);
}

