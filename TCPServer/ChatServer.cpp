#include <iostream>
#include "Server/Server.h"

int main()
{
	CServer server( "127.0.0.1", 800 );
	server.Run( );

	return 0;
}