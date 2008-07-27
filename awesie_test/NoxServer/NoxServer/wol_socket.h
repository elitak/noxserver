#pragma once

#include "global.h"
#include "boost/asio.hpp"

using namespace boost::asio::ip;

class wol_socket
{
public:
	wol_socket(boost::asio::io_service& s);

	void connect(std::string username, std::string password);
	void update();
private:
	boost::asio::io_service& _io_service;
	tcp::socket _socket;
	boost::asio::streambuf _receive;

	std::string _username;
	std::string _password;

	void hash_password(unsigned char *password, int len);

	void handle_connect(const boost::system::error_code& err);
	void handle_read(const boost::system::error_code& err);
};