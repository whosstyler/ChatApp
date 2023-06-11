#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <vector>
#pragma comment (lib, "ws2_32.lib")

#define MAX_BUFFER_SIZE (49152)

class CServer
{
public:
    // Constructor / Destructor
    CServer( const std::string& ipAddress, unsigned int port );
    ~CServer( );

    // Public methods
    void Run( );

    // Server state
    bool m_bRunServer = true;

private:
    // Initialization
    bool Init( );
    void Cleanup( );

    // Message processing
    void ProcessMessage( unsigned int clientSocket, const std::string& message );
    void SendToAllClients( unsigned int sendingClient, const std::string& message );
    void SendToClient( unsigned int clientSocket, const std::string& message );

    // Connection management
    void WaitForClientToConnect( );
    void ListenForClientMessage( SOCKET clientSocket );
    void RemoveClient( SOCKET clientSocket );

    // Server configuration
    std::string m_IPAddress = "";
    unsigned int m_iPort = 0;

    // Client management
    std::vector<unsigned int> m_vClientConnections;
    std::vector<std::thread> m_vThreads;

    // Socket management
    SOCKET m_ListenSocket = INVALID_SOCKET;

    // Connection statistics
    unsigned int m_iTotalConnections = 0;
};
