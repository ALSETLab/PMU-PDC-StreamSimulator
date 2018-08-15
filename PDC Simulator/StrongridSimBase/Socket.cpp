#include <cstdlib> 
#include <WinSock2.h>
#include <Windows.h>
#include <time.h>
#include "../StrongridBase/common.h"
#include "Socket.h"

using namespace strongridsim;
using namespace strongridbase;

const bool SIMULATE_SPLIT_DATAPACKET = false;
bool RAND_IS_INITIALIZED = false;

Socket::Socket( int sockFd, std::string remoteAddr )
{
	m_sockFd = sockFd;
	m_remoteAddr = remoteAddr;
}

Socket::~Socket()
{
	Close();
}


std::string Socket::GetRemoteAddress()
{
	return m_remoteAddr;
}

int Socket::getSocketFd() const
{
	return this->m_sockFd;
}

int GetRandomInRange(int lowest, int highest)
{
	if( RAND_IS_INITIALIZED == false ) {
		srand(time(0));
		RAND_IS_INITIALIZED = true;
	}

	unsigned long blah = time(0);
    int range=(highest-lowest)+1; 
	int tmp = rand();
    return lowest+int(range*tmp/(RAND_MAX + 1.0));     
}

int Socket::Send( const char* data, int length )
{
	int bytesSent = 0;
	while( bytesSent < length )
	{
		int maxSendBytes = length - bytesSent;
		if( SIMULATE_SPLIT_DATAPACKET ) {
			const int tmp = GetRandomInRange(20, 200);
			maxSendBytes = min( (length - bytesSent), tmp );
		}

		int retVal = send(m_sockFd, data + bytesSent, maxSendBytes, 0 );
		if( retVal <= 0 ) 
		{
			Close();
			throw SocketException("Unable to send data!");
		}

		bytesSent += retVal;
	}
	return bytesSent;
}

int Socket::Recv( char* refData, int length )
{
	int bytesReceived = 0;
	while( bytesReceived < length )
	{
		int retVal = recv(m_sockFd, refData+bytesReceived, length-bytesReceived, 0 );
		if( retVal <= 0 )
		{
			Close();
			throw SocketException("Unable to read data!");
		}

		bytesReceived += retVal;
	}
	return bytesReceived;
}

void Socket::Close()
{
	if( m_sockFd == -1 ) return;
	closesocket(m_sockFd);
	m_sockFd = -1;
}