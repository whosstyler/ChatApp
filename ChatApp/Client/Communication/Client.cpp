#include "Client.h"

CClient::CClient( const std::string& strIPAddress, unsigned int uiPort )
    : m_IPAddress( strIPAddress ), m_iPort( uiPort ), m_bRunClient( true )
{
}

CClient::~CClient( ) {
    Cleanup( );
}

bool CClient::Init( ) {
    WSADATA wsaData;
    if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) {
        std::cerr << "WSAStartup failed.\n";
        return false;
    }

    m_Socket = socket( AF_INET, SOCK_STREAM, 0 );
    if ( m_Socket == INVALID_SOCKET ) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup( );
        return false;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons( m_iPort );
    if ( inet_pton( AF_INET, m_IPAddress.c_str( ), &hint.sin_addr ) <= 0 ) {
        std::cerr << "Failed to setup server IP address.\n";
        closesocket( m_Socket );
        WSACleanup( );
        return false;
    }

    if ( connect( m_Socket, reinterpret_cast< sockaddr* >( &hint ), sizeof( hint ) ) == SOCKET_ERROR ) {
        std::cerr << "Failed to connect to server.\n";
        closesocket( m_Socket );
        WSACleanup( );
        return false;
    }

    return true;
}

void CClient::Cleanup( )
{
    closesocket( m_Socket );
    WSACleanup( );
}

void CClient::Run( ) {
    bool didClientInit = Init( );
    if ( !didClientInit ) {
        std::cerr << "Client initialization failed!\n";
        return;
    }

    std::thread receiveThread( &CClient::ReceiveMessages, this );
    SendMessages( );
    receiveThread.join( );

    Cleanup( );

    std::cout << "Client shutdown!\n";
}

void CClient::ReceiveMessages( ) {
    while ( m_bRunClient ) {
        ZeroMemory( m_Buffer, 4096 );
        int iBytesReceived = recv( m_Socket, m_Buffer, 4096, 0 );
        if ( iBytesReceived > 0 ) {
            std::cout << "SERVER> " << std::string( m_Buffer, 0, iBytesReceived ) << std::endl;
        }
    }
}

void CClient::SendMessages( )
{
    std::string strUserInput;
    do {
        std::cout << "> ";
        std::getline( std::cin, strUserInput );

        if ( !strUserInput.empty( ) ) {
            int iSendResult = send( m_Socket, strUserInput.c_str( ), strUserInput.size( ) + 1, 0 );
            if ( iSendResult == SOCKET_ERROR ) {
                std::cerr << "Send failed with error: " << WSAGetLastError( ) << '\n';
                break;
            }
        }
    } while ( !strUserInput.empty( ) );
}
