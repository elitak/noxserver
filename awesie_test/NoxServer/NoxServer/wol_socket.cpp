#include "global.h"

#include "wol_socket.h"
#include "socket_mgr.h"

using namespace boost::asio::ip;

// we should move this to configuration later
#define WOL_HOST "irc.westwood.com"
#define WOL_PORT "4000"

wol_socket::wol_socket(boost::asio::io_service& s) : _io_service(s), _socket(_io_service)
{
}

void wol_socket::connect(std::string username, std::string password)
{
	_username = username;
	_password = password;

	tcp::resolver resolver(_io_service);
	tcp::resolver_query query(tcp::v4(), WOL_HOST, WOL_PORT);
	tcp::resolver_iterator itr = resolver.resolve(query);
	
	_socket.async_connect(*itr, boost::bind(&wol_socket::handle_connect, this, placeholders::error));
}

void wol_socket::handle_connect(const boost::system::error_code &err)
{
	if(err)
		return;

	boost::asio::streambuf data_buffer;
	std::ostream data_stream(&data_buffer);

	char* hash = new char[_password.length()];
	strcpy(hash, _password.c_str());
	hash_password((unsigned char*)hash, _password.length());

	data_stream << "CVERS 11015 4610\n";
	data_stream << "PASS supersecret\n";
	data_stream << "NICK " << _username << "\n";
	data_stream << "apgar " << hash << "\n";
	data_stream << "USER UserName HostName irc.westwood.com :RealName\n";
	data_stream << "verchk 32512 720911\n";
	data_stream << "SETLOCALE 2\n";
	data_stream << "SETCODEPAGE 1252\n";
	data_stream << "SETOPT 17,33\n";

	boost::asio::write( _socket, data_buffer );
	boost::asio::async_read_until( _socket, _receive, '\n', boost::bind( &wol_socket::handle_read, this, boost::asio::placeholders::error ) );
}

void wol_socket::handle_read(const boost::system::error_code &err)
{
	if(err)
		return;

	boost::asio::streambuf data_buffer;
	std::ostream data_stream(&data_buffer);

	std::istream receive_stream(&_receive);
	std::string word;

	while(!receive_stream.eof())
	{
		receive_stream >> word;

		std::cout << word;
		
		if(word.compare("PING") == 0)
		{
			data_stream << "PONG :ircdev.westwood.com\n";
		}
	}

	std::cout << "\n";

	boost::asio::write( _socket, data_buffer );

	// continue reading
	boost::asio::async_read_until( _socket, _receive, '\n', boost::bind( &wol_socket::handle_read, this, boost::asio::placeholders::error ) );
}

#include <cstring>

void wol_socket::hash_password(unsigned char *password, int len) 
{
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