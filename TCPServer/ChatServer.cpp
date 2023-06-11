#include <iostream>
#include "Server/Server.h"
// This project will be tcp server for a chat application, where people can login with a username and chat with each other.
// The server will be able to handle multiple clients at the same time.

int main()
{

	CServer server( "127.0.0.1", 800 );
	server.Run( );

	return 0;
}