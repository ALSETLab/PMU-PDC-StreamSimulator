#include <iostream>
#include <Windows.h>
#include <process.h>
#include <time.h>
#include "../StrongridBase/common.h"
#include "IPdcDataFeed.h"
#include "PDC.h"
#include "../StrongridBase/C37118Protocol.h"
#include "../StrongridBase/EncDec.h"

using namespace strongridsim;
using namespace strongridbase;

C37118PdcDataFrame CreatePdcDataFrame()
{
	C37118PdcDataFrame pdcDataFrame;
	pdcDataFrame.HeaderCommon.Sync.FrameType = C37118HdrFrameType::DATA_FRAME;
	pdcDataFrame.HeaderCommon.Sync.LeadIn = (char)0xAA;
	pdcDataFrame.HeaderCommon.Sync.Version = 1;
	pdcDataFrame.HeaderCommon.IdCode = 1;
	pdcDataFrame.HeaderCommon.SOC = 0;
	pdcDataFrame.HeaderCommon.FracSec.FractionOfSecond = 0;
	pdcDataFrame.HeaderCommon.FracSec.TimeQuality = 0;
	return pdcDataFrame;
}

PDC::PDC(C37118PdcConfiguration pdcConfig, C37118PdcHeaderFrame header, int listenPort, std::vector<PMU*> pmuList, IPdcDataFeed* clockSrv) 
{
	m_version = ProtocolVersion::VERSION_1;
	m_isActive = false;
	m_tcpServer = new TcpServer(listenPort, &OnNewConnection, this);
	m_pmuList = pmuList;
	m_pdcConfig = pdcConfig;
	m_timeBase = m_pdcConfig.TimeBase.TimeBase;
	m_decodeInfo = C37118Protocol::CreateDecodeInfoByPdcConfig(m_pdcConfig);
	m_header = header;
	m_pdcDataFeed = clockSrv;
	m_dataRate = 50;
}


PDC::PDC(C37118PdcConfiguration_Ver3 pdcConfig, C37118PdcHeaderFrame header, int listenPort, std::vector<PMU*> pmuList, IPdcDataFeed* clockSrv)
{
	m_version = ProtocolVersion::VERSION_2;
	m_isActive = false;
	m_tcpServer = new TcpServer(listenPort, &OnNewConnection, this);
	m_pmuList = pmuList;
	m_pdcConfigVer3 = pdcConfig;
	m_timeBase = m_pdcConfigVer3.TimeBase.TimeBase;
	m_pdcConfig = C37118Protocol::DowngradePdcConfig(&pdcConfig);
	m_decodeInfo = C37118Protocol::CreateDecodeInfoByPdcConfig(m_pdcConfig);
	m_header = header;
	m_pdcDataFeed = clockSrv;
	m_dataRate = 50;
}

PDC::~PDC()
{
	Shutdown();
}

void PDC::Start()
{
	if( m_isActive ) throw Exception("PDC already started.");
	m_isActive = true;	

	m_actionSendConfig = false;
	m_actionStartRTD = false;
	m_actionStopRTD = false;

	// Start listening
	m_tcpServer->Start();

	// Start PDC procedure
	_beginthread(&PdcProc, 0, this);
	m_threadIsActive = true;
}

void PDC::Shutdown()
{
	// Signal shutdown and wait for the thread to close
	m_isActive = false;
	while( m_threadIsActive == true )
		Sleep(20);

	{
		MutexFragment lock(&m_lock);	 // enter lock

		// Shutdown the TCP server first
		if( m_tcpServer != 0 )
		{
			m_tcpServer->Shutdown();
			delete m_tcpServer;
			m_tcpServer = 0;
		}

		// Release all clients
		for( std::vector<PdcClient*>::iterator iter = m_clients.begin(); iter != m_clients.end(); ++iter )
			delete (*iter);
		m_clients.clear();

		// Release all PMU's
		for( std::vector<PMU*>::iterator iter = m_pmuList.begin(); iter != m_pmuList.end(); ++iter )
			delete (*iter);
		m_pmuList.clear();

		lock.Finalize();				// release lock
	}
}


void PDC::SetConfiguration(const C37118PdcConfiguration& pdcConfig)
{	
	MutexFragment lock = MutexFragment(&m_lock);
	m_pdcConfig = pdcConfig;
	lock.Finalize();
}

void PDC::SetDataRate(int dr)
{
	m_dataRate = dr;
}

void PDC::SendConfigurationAll()
{
	m_actionSendConfig = true;
}

void PDC::StartRealtimeDataAll()
{
	m_actionStartRTD = true;
}

void PDC::StopRealtimeDataAll()
{
	m_actionStopRTD = true;
}


void PDC::OnNewConnection( Socket* sock, void* objdata )
{
	PDC* pdcThis = (PDC*)objdata;
	pdcThis->SafeEnlistClient(sock);
}


void PDC::SafeEnlistClient(Socket* cli)
{
	MutexFragment lock(&m_lock);
	PdcClient* tmpClient = new PdcClient(cli);
	 
	// Publish event: client connected
	onClientConnected.Publish(tmpClient);

	tmpClient->ActivateDataStream();
	m_clients.push_back(tmpClient);	
	lock.Finalize();
}

void PDC::SafeDropClient(Socket* sock)
{
	MutexFragment lock(&m_lock);

	// Find the correct client handle
	std::vector<PdcClient*>::iterator iter = m_clients.begin();
	for( ; iter != m_clients.end(); ++iter ) if ( (*iter)->BaseSocket() == sock ) break;
	if( iter == m_clients.end() ) return;

	// Send event: client dropped
	onClientDropped.Publish(*iter);

	// Drop client
	(*iter)->BaseSocket()->Close();
	delete (*iter);
	m_clients.erase(iter);

	lock.Finalize();
}

int ReadC37118FrameIntoBuffer( char* buffer, Socket* sock, C37118FrameHeader* header )
{
	// Read frame header: 14 bytes
	int numbytes = sock->Recv(buffer, 14);
	if( numbytes != 14 ) throw Exception("Datastream too short: Unable to read frame header");

	// Interpret as header
	int tmp = 0;
	*header = C37118Protocol::ReadFrameHeader(buffer, numbytes, &tmp );

	// Read the remainder
	numbytes += sock->Recv(buffer+numbytes, header->FrameSize - numbytes);

	// Assert - the total number of bytes read should equal "framesize"
	if( numbytes != header->FrameSize ) throw Exception("Invalid datalength of frame");

	return numbytes;
}

void PDC::PdcProc(void* pdcObj)
{
	const int BUFFER_SIZE = 4096;
	PDC* pdc = (PDC*)pdcObj;
	char buffer[BUFFER_SIZE];

	// publish event: PDC started
	pdc->onPdcStarted.Publish(pdc);

	while( pdc->m_isActive )
	{
		Sleep((int)(1000.0 / (double)pdc->m_dataRate));

		MutexFragment lock = MutexFragment(&pdc->m_lock);
		if( pdc->m_clients.size() == 0 ) continue;

		std::vector<Socket*> faultySockets;

		// Create a list of all socket to check for data
		std::vector<Socket*> checkSockList;		
		std::vector<PdcClient*> activeRdtList;

		// Look for "one-time-actions", and cancel them out if identified 
		bool otaSendConfig = pdc->m_actionSendConfig;
		bool otaStartRTD = pdc->m_actionStartRTD;
		bool otaStopRTD = pdc->m_actionStopRTD;
		if( otaSendConfig ) pdc->m_actionSendConfig = false;
		if( otaStartRTD ) pdc->m_actionStartRTD = false;
		if( otaStopRTD ) pdc->m_actionStopRTD = false;

		for( std::vector<PdcClient*>::iterator iter =  pdc->m_clients.begin(); iter != pdc->m_clients.end(); ++iter ) {			
			// Perform one-time-actions
			if( otaSendConfig ) // Write configuration frame
			{
				// Serialize and send configuration frame
				int cfgFrameOffset = 0;
				C37118Protocol::WriteConfigurationFrame(buffer, &pdc->m_pdcConfig, &cfgFrameOffset);
				(*iter)->BaseSocket()->Send(buffer, cfgFrameOffset );
			}
			if( otaStartRTD ) (*iter)->ActivateDataStream(); // Activate datastream
			if( otaStopRTD ) (*iter)->DeactivateDataStream(); // Deactivate datastream

			// Package the clients into lists for quick access
			checkSockList.push_back((*iter)->BaseSocket());
			if( (*iter)->DataStreamIsActive() ) activeRdtList.push_back(*iter);
		}
				
		// Poll all sockets for "data waiting"
		std::vector<Socket*> readSockets = pdc->m_tcpServer->SelectReadyToReadSockets(checkSockList, 0);
		
		// Read from all sockets with data...
		for( std::vector<Socket*>::iterator iter = readSockets.begin(); iter != readSockets.end(); ++iter )
		{
			Socket* sock = *iter;

			try {
				// Read the entire frame into buffer
				C37118FrameHeader header;
				int bytesRead = ReadC37118FrameIntoBuffer(buffer,sock, &header);// Read and interpret the entire frame
				if( bytesRead > BUFFER_SIZE ) throw Exception("CRITICAL SERVER ERROR! POSSIBLE BUFFER OVERFLOW DETECTED!");

				if( header.Sync.FrameType != C37118HdrFrameType::COMMAND_FRAME )
					throw Exception("Unsupported frame requested. Terminating datastream.");
							
				int tmp = 0;
				C37118CommandFrame cmdFrame = C37118Protocol::ReadCommandFrame(buffer, BUFFER_SIZE, &tmp);
				if( cmdFrame.CmdType == C37118CmdType::SEND_CFG1_FRAME || cmdFrame.CmdType == C37118CmdType::SEND_CFG2_FRAME )
				{					
					std::cout << header.IdCode <<"; CMD RECEIVED: SEND_CFG1_FRAME/SEND_CFG2_FRAME" << std::endl;

					// Serialize and send configuration frame
					int cfgFrameOffset = 0;
					C37118Protocol::WriteConfigurationFrame(buffer, &pdc->m_pdcConfig, &cfgFrameOffset);
					sock->Send(buffer, cfgFrameOffset );
				}
				else if( cmdFrame.CmdType == C37118CmdType::SEND_CFG3_FRAME )
				{
					std::cout << header.IdCode <<"; CMD RECEIVED: SEND_CFG3_FRAME" << std::endl;

					if( pdc->m_version != ProtocolVersion::VERSION_2 ) 
						throw Exception("SEND_CFG3_FRAME received but this PDC does not suppport version 2. Terminating datastream.");

					// Serialize and send configuration frame
					int cfgFrameOffset = 0;
					C37118Protocol::WriteConfigurationFrame_Ver3(buffer, &pdc->m_pdcConfigVer3, &cfgFrameOffset);
					sock->Send(buffer, cfgFrameOffset );
				}
				else if( cmdFrame.CmdType == C37118CmdType::SEND_HDR_FRAME )
				{
					std::cout << header.IdCode <<"; CMD RECEIVED: SEND_HDR_FRAME" << std::endl;

					// Serialize and send header frame
					int hdrFrameOffset = 0;
					C37118Protocol::WriteHeaderFrame(buffer, &pdc->m_header, &hdrFrameOffset);
					sock->Send(buffer, hdrFrameOffset );
				}
				else if( cmdFrame.CmdType == C37118CmdType::START_RTD )
				{
					std::cout << header.IdCode <<"; CMD RECEIVED: START_RTD" << std::endl;

					PdcClient* cli = pdc->GetClientBySocket(sock);
					if( cli == 0 ) throw Exception("Client not found. Terminating connection.");

					cli->ActivateDataStream();
				}
				else if( cmdFrame.CmdType == C37118CmdType::KILL_RTD )
				{
					std::cout << header.IdCode <<"; CMD RECEIVED: KILL_RTD" << std::endl;

					PdcClient* cli = pdc->GetClientBySocket(sock);
					if( cli == 0 ) throw Exception("Client not found. Terminating connection.");

					cli->DeactivateDataStream();
				}
				else
					throw Exception("Unsupported command frame received. Terminating datastream.");
			}
			catch( Exception e )
			{
				std::cout << "An error has ocurred: " << e.ExceptionMessage() << ". Dropping client." << std::endl;
				faultySockets.push_back(*iter);
			}
			catch( ... )
			{
				std::cout << "Socket fault!";
				faultySockets.push_back(*iter);
			}
		}

		// Create dataframe
		C37118PdcDataFrame pdcDataFrame = CreatePdcDataFrame(); pdcDataFrame.pmuDataFrame.clear();
		for( std::vector<PMU*>::iterator iterPmu = pdc->m_pmuList.begin(); iterPmu != pdc->m_pmuList.end(); ++iterPmu )
		{
			// Generate data from client, and add to PDC dataframe
			C37118PmuDataFrame dataframe = (*iterPmu)->CreatePmuDataFrame();
			pdcDataFrame.pmuDataFrame.push_back(dataframe);
		}

		double soc = pdc->m_pdcDataFeed->GetSOC();
		pdcDataFrame.HeaderCommon.SOC = (uint32_t)soc;
		pdcDataFrame.HeaderCommon.FracSec = pdc->m_pdcDataFeed->GetFracSec();
		pdcDataFrame.HeaderCommon.FracSec.FractionOfSecond = (uint16_t)((soc - (uint32_t)soc) * pdc->m_pdcConfig.TimeBase.TimeBase);

		// Send data to all clients with active datastream
		for( std::vector<PdcClient*>::iterator iter = activeRdtList.begin(); iter != activeRdtList.end(); ++iter )
		{
			try {
				int offset = 0;
				C37118Protocol::WriteDataFrame(buffer, &pdc->m_decodeInfo, &pdcDataFrame, &offset );
				int bytesSent = (*iter)->BaseSocket()->Send(buffer,offset);			
			}
			catch( Exception e )
			{
				std::cout << "An error has ocurred: " << e.ExceptionMessage() << ". Dropping client." << std::endl;
				faultySockets.push_back((*iter)->BaseSocket());
			}
		}

		// Clean up all faulty sockets
		for( std::vector<Socket*>::iterator faultyIter = faultySockets.begin(); faultyIter !=faultySockets.end(); ++faultyIter )
			pdc->SafeDropClient(*faultyIter);
				
		lock.Finalize();
	}
		
	// publish event: PDC stopped
	pdc->onPdcStopped.Publish(pdc);

	pdc->m_threadIsActive = false;
	_endthread();
}

PdcClient* PDC::GetClientBySocket(Socket* sock)
{
	MutexFragment lock(&m_lock);
	// Find the correct client handle
	std::vector<PdcClient*>::iterator iter = m_clients.begin();
	for( ; iter != m_clients.end(); ++iter ) if ( (*iter)->BaseSocket() == sock ) break;
	if( iter == m_clients.end() ) 0;
	lock.Finalize();

	return *iter;
}


// ----------------------------------------------------------------

PdcClient::PdcClient(Socket* sock)
{
	m_rtdActive = false;
	m_socket = sock;
}

PdcClient::~PdcClient()
{
	delete m_socket;
}

void PdcClient::ActivateDataStream()
{
	m_rtdActive = true;
}
void PdcClient::DeactivateDataStream()
{
	m_rtdActive = false;
}
bool PdcClient::DataStreamIsActive() const
{
	return m_rtdActive;
}

Socket* PdcClient::BaseSocket()
{
	return m_socket;
}