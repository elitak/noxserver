#include "global.h"

#include "socket_mgr.h"

#include "wol_socket.h"
#include "world_socket.h"

socket_mgr::socket_mgr(boost::restricted) : _wol(_io_service), _world(_io_service)
{
}

void socket_mgr::update()
{
	get_wol_socket().update();
	get_world_socket().update();
}

void socket_mgr::run()
{
	_io_service.run();
}