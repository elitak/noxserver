/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
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

#ifndef MANGOSSERVER_WORLDPACKET_H
#define MANGOSSERVER_WORLDPACKET_H

#include "Common.h"
#include "ByteBuffer.h"
#include "WorldSocket.h"

class WorldPacket : public ByteBuffer
{
    public:
                                                            // just container for later use
        WorldPacket()                                       : ByteBuffer(0), m_opcode(0), m_client(NULL), m_accountid(0), m_xor(0), m_unk(0)
        {
        }
        explicit WorldPacket(uint8 opcode, uint8 id=0, ClientAddr client=NULL, size_t res=200) : ByteBuffer(res), m_client(client) , m_opcode(opcode), m_accountid(id), m_xor(0), m_unk(0) { }
                                                            // copy constructor
        WorldPacket(const WorldPacket &packet)              : ByteBuffer(packet), m_client(packet.m_client), m_opcode(packet.m_opcode), m_accountid(packet.m_accountid), m_xor(packet.m_xor), m_unk(packet.m_unk)
        {
        }

        void Initialize(uint8 opcode, uint8 id=0, ClientAddr client=NULL, size_t newres=200)
        {
            clear();
            _storage.reserve(newres);
            m_opcode = opcode;
			m_client = client;
			m_accountid = id;
        }

        uint8 GetOpcode() const { return m_opcode; }
        void SetOpcode(uint8 opcode) { m_opcode = opcode; }
		ClientAddr GetClient() const { return m_client; }
		void SetClient(ClientAddr client) { m_client = client; }
		uint8 GetAccountId() const { return m_accountid; }
        void SetAccountId(uint8 id) { m_accountid = id; }
		uint8 GetXor() const { return m_xor; }
		void SetXor(uint8 xor) { m_xor = xor; }
		uint8 GetUnk() const { return m_unk; }
		void SetUnk(uint8 unk) {m_unk = unk; }

    protected:
        uint8 m_opcode;
		uint8 m_accountid;
		uint8 m_xor;
		uint8 m_unk;
		ClientAddr m_client;
};
#endif
