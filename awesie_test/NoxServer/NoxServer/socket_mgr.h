#pragma once

#include "global.h"

#include "boost/asio.hpp"

#include "wol_socket.h"
#include "world_socket.h"

class socket_mgr : public boost::singleton<socket_mgr>
{
public:
	socket_mgr(boost::restricted);

	wol_socket& get_wol_socket()
	{
		return _wol;
	}

	world_socket& get_world_socket()
	{
		return _world;
	}

	void run();
private:
	boost::asio::io_service _io_service;
	wol_socket _wol;
	world_socket _world;
};
