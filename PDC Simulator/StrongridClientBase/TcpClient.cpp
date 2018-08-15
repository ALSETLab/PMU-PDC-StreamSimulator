#include <WinSock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include "../StrongridBase/common.h"
#include "TcpClient.h"

using namespace strongridbase;
using namespace strongridclientbase;

TcpClient::TcpClient(std::string ipAddress, int port)
{
	m_sockfd = 0;
	m_ipAddr = ipAddress;
	m_port = port;
	m_win32Initialized  = false;
	InitializeWindowsSocket();
}

TcpClient::~TcpClient()
{
	Close();
}

int TcpClient::GetSocketDescriptor() const
{
	return m_sockfd;
}

void TcpClient::Connect()
{
	int sockfd;  
    addrinfo hints, *servinfo, *p;
    int rv;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;


	std::stringstream ss; ss << m_port;
    if ((rv = getaddrinfo(m_ipAddr.c_str(), ss.str().c_str(), &hints, &servinfo)) != 0) 
        throw Exception("Unable to get address information.");

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            closesocket(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL)        
        throw Exception("Failed to connect to host");

    freeaddrinfo(servinfo); // all done with this structure

	m_sockfd = sockfd;
}

void TcpClient::Close()
{
	if( m_sockfd != 0 ) closesocket(m_sockfd);
	m_sockfd = 0;
}

int TcpClient::Send( const char* data, int length )
{
	int bytesSent = 0;
	while( bytesSent < length )
	{
		int retVal = send(m_sockfd, data + bytesSent, length - bytesSent, 0 );
		if( retVal <= 0 ) 
		{
			Close();
			throw Exception("Unable to send data!");
		}

		bytesSent += retVal;
	}
	return bytesSent;
}

int TcpClient::Recv( char* refData, int length, int timeoutMs )
{
	int bytesReceived = 0;
	while( bytesReceived < length )
	{
		// set timeout
		DWORD dwto = timeoutMs;
		setsockopt(m_sockfd,SOL_SOCKET, SO_RCVTIMEO , (const char*)&dwto, sizeof(DWORD) );

		// Perform read
		int retVal = recv(m_sockfd, refData + bytesReceived, length - bytesReceived, 0 );
		if( retVal <= 0 )
		{
			if( WSAGetLastError() == WSAETIMEDOUT ) throw SocketTimeout("Unable to read within timeout");
			else {
				Close();
				throw SocketException("An error ocurred while attempting to read data");
			}
		}

		bytesReceived += retVal;
	}
	return bytesReceived;
}


void TcpClient::InitializeWindowsSocket()
{
	// Only initialize once
	if( m_win32Initialized == true ) return;
	m_win32Initialized = true;

	// Initialize window32 socket
	 WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
		throw Exception("Unable to initialize winsock2!");
    }
}