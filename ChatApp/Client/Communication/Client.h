#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <vector>

#pragma comment (lib, "ws2_32.lib")

#define MAX_BUFFER_SIZE (49152)

class CClient
{
public:
    CClient( const std::string& ipAddress, unsigned int port );
    ~CClient( );

    bool Init( );
    void Cleanup( );
    void Run( );
    void ReceiveMessages( );
    void SendMessages( );

private:
    SOCKET m_Socket;
    std::string m_IPAddress;
    unsigned int m_iPort;
    bool m_bRunClient = true;
    char m_Buffer[ MAX_BUFFER_SIZE ];
};
