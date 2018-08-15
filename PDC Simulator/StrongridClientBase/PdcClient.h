#pragma once
#include <string>
#include "TcpClient.h"
#include "../StrongridBase/C37118Protocol.h"

using namespace strongridbase;

namespace strongridclientbase
{
	class PdcClient
	{
	public:
		PdcClient( std::string ipAddress, int port, int pdcIdCode );
		~PdcClient();
		void CloseConnection();

		C37118PdcDataDecodeInfo GetDecodeInfo() const { return m_datadecodeInfo; }

		int GetSocketDescriptor() const;

	public:
		void Connect();
		void ReadConfiguration(int timeoutMs);
		void ReadConfigurationVer3(int timeoutMs);
		void ReadHeaderMessage(int timeoutMs);
		void StartDataStream();
		void StopDataStream();
		void ReadDataFrame(int timeoutMs);

		C37118PdcConfiguration GetPdcConfiguration();
		C37118PdcConfiguration_Ver3 GetPdcConfigurationVer3();
		C37118PdcHeaderFrame GetPdcHeaderFrame();
		C37118PdcDataFrame GetPdcDataFrame();

	private:
		void ProcessInputStreamUntilTargetFrameType(C37118HdrFrameType targetType, int timeoutMs);
		void HandleHeaderMessage();
		void HandleConfigurationFrame();
		void HandleConfigurationFrame_Ver3();
		void HandleDataFrame();

	private:
		C37118FrameHeader CreateGenericHeaderFrame(C37118HdrFrameType cmdType);
		C37118CommandFrame CreateCommandFrame(C37118CmdType cmdType);

		static void PdcClientProc(void* pdcObj);

	private:
		TcpClient* m_tcpClient;
		char* m_buffer;
		int m_pdcIdCode;

		bool m_pdcCfgVer2_isAvailable;
		bool m_pdcCfgVer3_isAvailable;
		bool m_headerFrame_isAvailable;
		bool m_pdcDataFrame_isAvailable;

		// Data read from PDC
		C37118PdcConfiguration m_pdcConfig;
		C37118PdcConfiguration_Ver3 m_pdcConfigVer3;
		C37118PdcDataDecodeInfo m_datadecodeInfo;
		C37118PdcHeaderFrame m_headerFrame;		
		C37118PdcDataFrame m_currDataFrame;
	};
}