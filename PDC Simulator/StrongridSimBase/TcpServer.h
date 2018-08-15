#pragma once

#include <vector>
#include "Socket.h"

namespace strongridsim
{
	typedef void (*OnConnection)(Socket* sock, void* objdata);

	class TcpServer
	{
	public:
		TcpServer( int listenPort, OnConnection oc, void* onConnectionObjData );
		~TcpServer();

		void Start();
		void Shutdown();

		std::vector<Socket*> SelectReadyToReadSockets(std::vector<Socket*> sockVec, int timeoutMs);

	private:
		static void TcpServerProc(void* mythis);
		static void InitializeWindowsSocket();
		static void HandlenewConnectionEvent(TcpServer* tcpSrv, int listener);

	private:
		int m_listenPort;
		OnConnection m_onConnection;
		void* m_onCallbackObjData;
		volatile bool m_isActive, m_threadIsAlive;
		static bool m_win32Initialized;
	};
}