#include <process.h>
#include <iostream>
#include <windows.h>
#include "PdcClient.h"
#include "../StrongridBase/common.h"
#include "../StrongridBase/C37118Protocol.h";

using namespace strongridclientbase;
using namespace strongridbase;

const int BUFFER_SIZE = 131072;

PdcClient::PdcClient( std::string ipAddress, int port, int pdcIdCode )
{
	m_tcpClient = new TcpClient(ipAddress,port);
	m_buffer = new char[BUFFER_SIZE];
	m_pdcIdCode = pdcIdCode;
	
	m_pdcCfgVer2_isAvailable = false;
	m_pdcCfgVer3_isAvailable = false;
	m_headerFrame_isAvailable = false;
	m_pdcDataFrame_isAvailable = false;
}

PdcClient::~PdcClient()
{
	delete [] m_buffer ; m_buffer = 0;
	if( m_tcpClient != 0 ) delete m_tcpClient;
}


void PdcClient::CloseConnection()
{
	m_tcpClient->Close();
}

int PdcClient::GetSocketDescriptor() const
{
	return m_tcpClient->GetSocketDescriptor();
}

void PdcClient::Connect()
{
	m_tcpClient->Connect();
}

int ReadC37118FrameIntoBuffer( char* buffer, TcpClient* tcpClient, C37118FrameHeader* header, int timeoutMs )
{
	// Read frame header: 14 bytes
	int numbytes = tcpClient->Recv(buffer, 14, timeoutMs);
	if( numbytes != 14 ) throw Exception("Datastream too short: Unable to read frame header");

	// Interpret as header
	int tmp = 0;
	*header = C37118Protocol::ReadFrameHeader(buffer, BUFFER_SIZE, &tmp );

	// Read the remainder
	numbytes += tcpClient->Recv(buffer+numbytes, header->FrameSize - numbytes, timeoutMs);

	// Assert - the total number of bytes read should equal "framesize"
	if( numbytes != header->FrameSize ) throw Exception("Invalid datalength of frame");

	return numbytes;
}

C37118FrameHeader PdcClient::CreateGenericHeaderFrame(C37118HdrFrameType cmdType)
{
	C37118FrameHeader header;
	header.Sync.FrameType = cmdType;
	header.Sync.LeadIn = (char)0xAA;
	header.Sync.Version = 1;
	header.IdCode = m_pdcIdCode;
	header.SOC = (uint32_t)time(0);
	header.FracSec.FractionOfSecond = 0;
	header.FracSec.TimeQuality = 0;
	return header;
}

C37118CommandFrame PdcClient::CreateCommandFrame(C37118CmdType cmdType)
{
	C37118CommandFrame cmdframe;
	cmdframe.Header = CreateGenericHeaderFrame(C37118HdrFrameType::COMMAND_FRAME);
	cmdframe.CmdType = cmdType;
	return cmdframe;
}

void PdcClient::ReadConfiguration(int timeoutMs)
{
	// Create command frame
	int offset = 0;
	C37118CommandFrame requestCfg = CreateCommandFrame(C37118CmdType::SEND_CFG2_FRAME);
	C37118Protocol::WriteCommandFrame(m_buffer,&requestCfg, &offset );

	// Send request to server..
	m_tcpClient->Send(m_buffer, offset );


	// Read configuration frame	
	ProcessInputStreamUntilTargetFrameType(C37118HdrFrameType::CONFIGURATION_FRAME_2, timeoutMs);
		
	
}
void PdcClient::HandleConfigurationFrame()
{
	// Interpret config frame
	C37118PdcConfiguration pdcConfig;
	m_pdcConfig = C37118Protocol::ReadConfigurationFrame(m_buffer,BUFFER_SIZE);
	m_datadecodeInfo = C37118Protocol::CreateDecodeInfoByPdcConfig(m_pdcConfig);			
	m_pdcCfgVer2_isAvailable = true;
}

void PdcClient::ReadConfigurationVer3(int timeoutMs)
{
	// Create command frame
	int offset = 0;
	C37118CommandFrame requestCfg = CreateCommandFrame(C37118CmdType::SEND_CFG3_FRAME);
	C37118Protocol::WriteCommandFrame(m_buffer,&requestCfg, &offset );

	// Send request to server..
	m_tcpClient->Send(m_buffer, offset );

	// Read config frame 3
	ProcessInputStreamUntilTargetFrameType( C37118HdrFrameType::CONFIGURATION_FRAME_3, timeoutMs );	
}
void PdcClient::HandleConfigurationFrame_Ver3()
{
	// Interpret config frame
	C37118PdcConfiguration pdcConfig;
	m_pdcConfigVer3 = C37118Protocol::ReadConfigurationFrame_Ver3(m_buffer,BUFFER_SIZE);
	m_datadecodeInfo = C37118Protocol::CreateDecodeInfoByPdcConfig(m_pdcConfigVer3);
	m_pdcCfgVer3_isAvailable = true;
}

void PdcClient::ProcessInputStreamUntilTargetFrameType(C37118HdrFrameType targetType,int timeoutMs)
{
	// Keep reading until the correct frame was received
	while( true )
	{
		int offset = 0;
		C37118FrameHeader frameHeader;
		ReadC37118FrameIntoBuffer(m_buffer, m_tcpClient, &frameHeader, timeoutMs);

		if( frameHeader.Sync.FrameType == C37118HdrFrameType::HEADER_FRAME )
			HandleHeaderMessage();
		else if( frameHeader.Sync.FrameType == C37118HdrFrameType::CONFIGURATION_FRAME_1 ||
			frameHeader.Sync.FrameType == C37118HdrFrameType::CONFIGURATION_FRAME_2 )
			HandleConfigurationFrame();
		else if( frameHeader.Sync.FrameType == C37118HdrFrameType::CONFIGURATION_FRAME_3 )
			HandleConfigurationFrame_Ver3();
		else if( frameHeader.Sync.FrameType == C37118HdrFrameType::DATA_FRAME )
			HandleDataFrame();
				
		// Stop processing the inputstream once the target messagetype has been handled.
		if( frameHeader.Sync.FrameType == targetType) break;
	}
}

void PdcClient::ReadHeaderMessage(int timeoutMs)
{
	// Create command frame
	int offset = 0;
	C37118CommandFrame requestCfg = CreateCommandFrame(C37118CmdType::SEND_HDR_FRAME);
	C37118Protocol::WriteCommandFrame(m_buffer,&requestCfg, &offset );

	// Send request to server..
	m_tcpClient->Send(m_buffer, offset );

	// Keep reading until the headermessage is handled
	ProcessInputStreamUntilTargetFrameType(C37118HdrFrameType::HEADER_FRAME, timeoutMs);
}
void PdcClient::HandleHeaderMessage()
{
	// Interpret config frame
	int offset = 0;
	m_headerFrame = C37118Protocol::ReadHeaderFrame(m_buffer,BUFFER_SIZE,&offset);	
	m_headerFrame_isAvailable = true;
}

void PdcClient::StartDataStream()
{
	// Create command frame
	int offset = 0;
	C37118CommandFrame requestCfg = CreateCommandFrame(C37118CmdType::START_RTD);
	C37118Protocol::WriteCommandFrame(m_buffer,&requestCfg, &offset );

	// Send request to server..
	m_tcpClient->Send(m_buffer, offset );
}

void PdcClient::StopDataStream()
{
	// Create command frame
	int offset = 0;
	C37118CommandFrame requestCfg = CreateCommandFrame(C37118CmdType::KILL_RTD);
	C37118Protocol::WriteCommandFrame(m_buffer,&requestCfg, &offset );

	// Send request to server..
	m_tcpClient->Send(m_buffer, offset );
}


void PdcClient::ReadDataFrame(int timeoutMs)
{
	// config must be available
	if( m_pdcCfgVer3_isAvailable == false && m_pdcCfgVer2_isAvailable == false ) throw Exception("Configuration must be read first.");

	// Read from input stream until the dataframe is received
	ProcessInputStreamUntilTargetFrameType(C37118HdrFrameType::DATA_FRAME, timeoutMs);

	// Interpret dataframe
	int offset = 0;
	m_currDataFrame = C37118Protocol::ReadDataFrame(m_buffer,BUFFER_SIZE, &m_datadecodeInfo, &offset);	
	m_pdcDataFrame_isAvailable = true;
}

void PdcClient::HandleDataFrame()
{
	// Do nothing - handled by ReadDataFrame (the dataframe will be lost)
}


C37118PdcConfiguration PdcClient::GetPdcConfiguration()
{
	if( m_pdcCfgVer2_isAvailable == false ) throw Exception("Configurationframe Ver2 has not been read");
	return m_pdcConfig;
}

C37118PdcConfiguration_Ver3 PdcClient::GetPdcConfigurationVer3()
{
	if( m_pdcCfgVer3_isAvailable == false ) throw Exception("Configurationframe Ver3 has not been read");
	return m_pdcConfigVer3;
}

C37118PdcHeaderFrame PdcClient::GetPdcHeaderFrame()
{
	if( m_headerFrame_isAvailable == false ) throw Exception("Header has not been read");
	return m_headerFrame;
}

C37118PdcDataFrame PdcClient::GetPdcDataFrame()
{
	if( m_pdcDataFrame_isAvailable == false ) throw Exception("Dataframe has not been read");
	return m_currDataFrame;
}

