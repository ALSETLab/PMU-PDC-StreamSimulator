#pragma once
#include <string>

namespace strongridclientbase
{
	class TcpClient
	{
	public:
		TcpClient(std::string ipAddress, int port);
		 ~TcpClient();

		 int GetSocketDescriptor() const;

		 void Connect();
		 void Close();
		 int Send(const char* src, int len);
		 int Recv(char* dest, int len, int timeoutMs);

	private:
		void InitializeWindowsSocket();

	private:
		int m_sockfd;
		std::string m_ipAddr;
		int m_port;
		bool m_win32Initialized;
	};
}