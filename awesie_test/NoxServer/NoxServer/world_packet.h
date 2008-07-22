#pragma once

#include "global.h"
#include "ByteBuffer.h"
#include "boost/asio.hpp"

class world_packet : public ByteBuffer
{
public:
	world_packet(uint8 opcode, uint8 id, boost::asio::ip::udp::endpoint endpoint, size_t res = 200) : ByteBuffer(res), m_opcode(opcode), m_accountid(id), m_endpoint(endpoint) { }
	world_packet(const world_packet& p) : ByteBuffer(p), m_opcode(p.m_opcode), m_endpoint(p.m_endpoint), m_accountid(p.m_accountid), m_xor(p.m_xor), m_unk(p.m_unk) { }
	
	void initialize(uint8 opcode, uint8 id, boost::asio::ip::udp::endpoint& endpoint, size_t res = 200)
	{
		clear();
		_storage.reserve(res);
		m_opcode = opcode;
		m_accountid = id;
		m_endpoint = endpoint;
	}

    uint8 get_opcode() const { return m_opcode; }
    void set_opcode(uint8 opcode) { m_opcode = opcode; }
	uint8 get_account_id() const { return m_accountid; }
    void set_account_id(uint8 id) { m_accountid = id; }
	uint8 get_xor() const { return m_xor; }
	void set_xor(uint8 xor) { m_xor = xor; }
	uint8 get_unk() const { return m_unk; }
	void set_unk(uint8 unk) {m_unk = unk; }
	boost::asio::ip::udp::endpoint get_endpoint() const { return m_endpoint; }
	void set_endpoint(boost::asio::ip::udp::endpoint endpoint) { m_endpoint = endpoint; }

protected:
    uint8 m_opcode;
	uint8 m_accountid;
	uint8 m_xor;
	uint8 m_unk;
	boost::asio::ip::udp::endpoint m_endpoint;
};