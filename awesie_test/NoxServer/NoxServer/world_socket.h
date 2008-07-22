#pragma once

#include "global.h"
#include "boost/asio.hpp"
#include <map>
#include <queue>

#define MAX_LEN 1024

using namespace boost::asio;
using namespace boost::asio::ip;

class world_session;
class world_packet;

typedef std::map<udp::endpoint,world_session*> world_sessions;

class world_socket
{
public:
	world_socket(boost::asio::io_service& s);

	void listen();
	void update();
	void queue_packet(world_packet& packet);
private:
	io_service& m_io_service;
	udp::socket m_socket;
	std::queue<world_packet*> m_send_queue;

	udp::endpoint m_sender;
	uint8 m_data[MAX_LEN];
	uint8 m_send_data[MAX_LEN];

	world_sessions m_sessions;

	void crypt_data(uint8 xor, uint8* data, uint32 datalen);
	void send_next_packet();
	void handle_receive_from(const boost::system::error_code& err, size_t bytes_recvd);
	void handle_send_to(const boost::system::error_code& err, size_t bytes_sent);
	void handle_lan_ping(world_packet& recvPacket);
	void handle_join(world_packet& recvPacket);
};