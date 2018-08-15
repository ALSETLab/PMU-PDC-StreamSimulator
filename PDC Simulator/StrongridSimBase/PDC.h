#pragma once

#include <vector>
#include <string>
#include "PMU.h"
#include "TcpServer.h"
#include "Mutex.h"
#include "../StrongridBase/C37118Protocol.h"
#include "IPdcDataFeed.h"
#include "Event.h"

namespace strongridsim
{
	class PdcClient
	{
	public:
		PdcClient(Socket* sock);
		~PdcClient();

		void ActivateDataStream();
		void DeactivateDataStream();
		bool DataStreamIsActive() const;
		Socket* BaseSocket();

	private:
		Socket* m_socket;
		bool m_rtdActive;
	};

	enum ProtocolVersion
	{
		VERSION_1 = 1,	// CFG1+CFG2 support
		VERSION_2 = 2	// CFG3 support
	};	
	

	class PDC
	{
	public:
		PDC( C37118PdcConfiguration pdcConfig, C37118PdcHeaderFrame header, int listenport, std::vector<PMU*> pmuList, IPdcDataFeed* clockSrv = 0 );
		PDC( C37118PdcConfiguration_Ver3 pdcConfig, C37118PdcHeaderFrame header, int listenport, std::vector<PMU*> pmuList, IPdcDataFeed* clockSrv = 0 );
		~PDC();
		void Start();
		void Shutdown();

		void SetConfiguration(const C37118PdcConfiguration& pdcConfig);
		void SetDataRate(int dr);

		void SendConfigurationAll();
		void StartRealtimeDataAll();
		void StopRealtimeDataAll();

		// Events
		Event onPdcStarted;
		Event onPdcStopped;
		Event onClientConnected;
		Event onClientDropped;
		
	private:
		static void OnNewConnection( Socket* sock, void* objdata );
		void SafeEnlistClient(Socket* cli);
		void SafeDropClient(Socket* sock);
		static void PdcProc(void* pdc);

		PdcClient* GetClientBySocket(Socket* sock);

	private:
		ProtocolVersion m_version;
		C37118PdcConfiguration m_pdcConfig;
		C37118PdcConfiguration_Ver3 m_pdcConfigVer3;
		int m_timeBase;
		volatile int m_dataRate;

		C37118PdcDataDecodeInfo m_decodeInfo;
		C37118PdcHeaderFrame m_header;
		int m_port;
		std::vector<PMU*> m_pmuList;
		TcpServer* m_tcpServer;
		volatile bool m_isActive, m_threadIsActive;
		IPdcDataFeed* m_pdcDataFeed;

		// SYNCED ACCESS DATA
		Mutex m_lock;
		std::vector<PdcClient*> m_clients;

		// 
		volatile bool m_actionSendConfig;
		volatile bool m_actionStopRTD;
		volatile bool m_actionStartRTD;

	};
}