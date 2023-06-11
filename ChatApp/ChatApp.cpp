#include "Client/Communication/Client.h"

int main()
{
	CClient client( "127.0.0.1", 800 );
	client.Run( );

	return 0;
}