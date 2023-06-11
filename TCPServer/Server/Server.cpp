#include "Server.h"

CServer::CServer( const std::string& sIpAddress, unsigned int iPort ) :
    m_IPAddress( sIpAddress ), m_iPort( iPort ), m_iTotalConnections( 0 ), m_bRunServer( true ) {}

CServer::~CServer( ) {
    Cleanup( );
}

bool CServer::Init( ) {
    WSADATA wsaData;
    int iWsaStartupResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    if ( iWsaStartupResult != 0 ) {
        std::cerr << "WSAStartup failed: " << iWsaStartupResult << '\n';
        return false;
    }

    m_ListenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( m_ListenSocket == INVALID_SOCKET ) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError( ) << '\n';
        WSACleanup( );
        return false;
    }

    sockaddr_in sinHint;
    sinHint.sin_family = AF_INET;
    sinHint.sin_port = htons( m_iPort );
    inet_pton( AF_INET, m_IPAddress.c_str( ), &sinHint.sin_addr );

    int iBindResult = bind( m_ListenSocket, reinterpret_cast< sockaddr* >( &sinHint ), sizeof( sinHint ) );
    if ( iBindResult == SOCKET_ERROR ) {
        std::cerr << "Bind failed with error: " << WSAGetLastError( ) << '\n';
        closesocket( m_ListenSocket );
        WSACleanup( );
        return false;
    }

    int iListenResult = listen( m_ListenSocket, SOMAXCONN );
    if ( iListenResult == SOCKET_ERROR ) {
        std::cerr << "Listen failed with error: " << WSAGetLastError( ) << '\n';
        closesocket( m_ListenSocket );
        WSACleanup( );
        return false;
    }

    return true;
}

void CServer::Run( ) {
    bool didServerInit = Init( );
    if ( !didServerInit ) {
        std::cerr << "Server initialization failed!\n";
        return;
    }

    while ( m_bRunServer ) {
        WaitForClientToConnect( );
    }

    Cleanup( );
    std::cout << "Server shutdown!\n";
}

void CServer::WaitForClientToConnect( ) {
    sockaddr_in sinClient;
    int iClientSize = sizeof( sinClient );

    SOCKET clientSocket = accept( m_ListenSocket, reinterpret_cast< sockaddr* >( &sinClient ), &iClientSize );

    if ( clientSocket == INVALID_SOCKET ) {
        std::cerr << "Accept failed with error: " << WSAGetLastError( ) << '\n';
        return;
    }

    m_vClientConnections.push_back( clientSocket );
    std::cout << "Client connected! Socket: " << clientSocket << '\n';
    std::cout << "Total connections: " << ++m_iTotalConnections << '\n';

    SendToClient( clientSocket, "Welcome to the chat server!" );
    m_vThreads.push_back( std::thread( &CServer::ListenForClientMessage, this, clientSocket ) );
}

void CServer::ListenForClientMessage( SOCKET clientSocket ) {
    char buffer[ MAX_BUFFER_SIZE ];

    while ( m_bRunServer ) {
        ZeroMemory( buffer, MAX_BUFFER_SIZE );
        int iBytesReceived = recv( clientSocket, buffer, MAX_BUFFER_SIZE, 0 );

        if ( iBytesReceived <= 0 ) {
            std::cout << "Client disconnected!\n";
            RemoveClient( clientSocket );
            return;
        }

        std::string sReceivedMessage( buffer, 0, iBytesReceived );
        ProcessMessage( clientSocket, sReceivedMessage );
    }
}

void CServer::ProcessMessage( unsigned int clientSocket, const std::string& sMessage ) {
    std::cout << "Client " << clientSocket << " sent the following message: " << sMessage << '\n';
    SendToAllClients( clientSocket, sMessage );
}

void CServer::SendToAllClients( unsigned int sendingClient, const std::string& sMessage ) {
    for ( auto clientSocket : m_vClientConnections ) {
        if ( clientSocket != sendingClient ) {
            SendToClient( clientSocket, sMessage );
        }
    }
}

void CServer::SendToClient( unsigned int clientSocket, const std::string& sMessage ) {
    int iSendResult = send( clientSocket, sMessage.c_str( ), sMessage.size( ) + 1, 0 );
    if ( iSendResult == SOCKET_ERROR ) {
        std::cerr << "Send failed with error: " << WSAGetLastError( ) << '\n';
        RemoveClient( clientSocket );
    }
}

void CServer::RemoveClient( SOCKET clientSocket ) {
    closesocket( clientSocket );

    m_vClientConnections.erase( std::remove( m_vClientConnections.begin( ), m_vClientConnections.end( ), clientSocket ),
        m_vClientConnections.end( ) );

    std::cout << "Client disconnected! Total connections: " << --m_iTotalConnections << '\n';
}

void CServer::Cleanup( ) {
    closesocket( m_ListenSocket );

    for ( auto clientSocket : m_vClientConnections ) {
        closesocket( clientSocket );
    }

    WSACleanup( );

    for ( auto& thread : m_vThreads ) {
        if ( thread.joinable( ) ) {
            thread.join( );
        }
    }
}
