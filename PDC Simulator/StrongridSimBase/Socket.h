#pragma once

#include "../StrongridBase/common.h"

using namespace strongridbase;

namespace strongridsim
{
	class SocketException : public Exception
	{
	public:
		SocketException(std::string error) : Exception(error)
		{
		}
	};

	class Socket
	{
	public:
		Socket( int sockFd, std::string remoteAddr );
		~Socket();

		int Send( const char* data, int length );
		int Recv( char* refData, int length );
		void Close();

		std::string GetRemoteAddress();
		int getSocketFd() const;

	private:
		int m_sockFd;
		std::string m_remoteAddr;
	};
}