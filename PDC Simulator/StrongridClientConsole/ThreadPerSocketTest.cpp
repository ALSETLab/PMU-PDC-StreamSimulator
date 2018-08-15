#include <iostream>
#include <iomanip>
#include <process.h>
#include <Windows.h>
#include <sstream>
#include <string>
#include <iomanip>
#include "main.h"
#include "../StrongridClientBase/PdcClient.h"
#include "../StrongridBase/common.h"

using namespace strongridclientbase;
using namespace std;


namespace ThreadPerSocketTest
{
	volatile bool isActive = true;

	std::string getLogTs()
	{
		_SYSTEMTIME sysTime;
		ostringstream ostr;
		GetLocalTime(&sysTime);
		ostr << sysTime.wYear << "." << sysTime.wMonth << "." << sysTime.wDay << " "
				<< setfill('0') << setw(2) << sysTime.wHour  << ":"
				<< setfill('0') << setw(2) << sysTime.wMinute << ":"
				<< setfill('0') << setw(2) << sysTime.wSecond << "\t";
	
		return ostr.str();
	}

	void PdcClientProc(void* pdcObj)
	{
		// Deref to pdc client object
		PdcClient* pdcClient = (PdcClient*)pdcObj;

		ostringstream ostr;
		ostream strout(cout.rdbuf());
	
		while( isActive )
		{
			try {
				// Connect to PDC
				pdcClient->Connect();
				strout << getLogTs() << "Connection established" << endl;
		
				// Read header file
				strout << getLogTs() << "Reading header...";
				pdcClient->ReadHeaderMessage(30000);
				C37118PdcHeaderFrame header = pdcClient->GetPdcHeaderFrame();
				strout << " '" << header.HeaderMessage << std::endl;

				// Read configuration frame (CFG2)
				strout << getLogTs() << "Reading configuration...";
				pdcClient->ReadConfigurationVer3(30000);
				C37118PdcConfiguration_Ver3 pdcConfig = pdcClient->GetPdcConfigurationVer3();
				strout << "Configuration read successfully" << endl;

				strout << getLogTs() << "getMainConfig - "
					<< "\n\tIdCode=" << pdcConfig.HeaderCommon.IdCode
					<< "\n\tSOC=" << pdcConfig.HeaderCommon.SOC
					<< "\n\tFracSec=" << pdcConfig.HeaderCommon.FracSec.FractionOfSecond << " (" << (unsigned int)pdcConfig.HeaderCommon.FracSec.TimeQuality << ")"
					<< "\n\tTimebase=" << pdcConfig.TimeBase.TimeBase << " (" << (unsigned int)pdcConfig.TimeBase.Flags << ")"
					<< "\n\tdataRate_raw(fps)=" << pdcConfig.DataRate.RawDataRate() << " (" << pdcConfig.DataRate.FramesPerSecond() << ")"
					<< "\n\tnumberOfPMUs=" << pdcConfig.PMUs.size()
					<< endl << endl;

				// Print per-pmu configuration parameters
				for( int iPmu = 0; iPmu < pdcConfig.PMUs.size(); ++iPmu )
				{
					C37118PmuConfiguration_Ver3 pmuConfig = pdcConfig.PMUs[iPmu];
				
					// Print common configuration params
					strout << getLogTs() << "getPmuConfiguration/pmuConfig - "
						<< "\n\tpmuId=" << pmuConfig.IdCode
						<< "\n\tstationname=" << pmuConfig.StationName
						<< "\n\tnomFreq=" << (pmuConfig.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60 ? 50 : 60)
						<< "\n\tformatCode= {" 
							<< (pmuConfig.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat ? 1 : 0) << "|"
							<< (pmuConfig.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat ? 1 : 0) << "|"
							<< (pmuConfig.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat ? 1 : 0) << "|"
							<< (pmuConfig.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle ? 1 : 0) << "}" 
						<< "\n\tnumberOfPhasors=" << pmuConfig.phasorChnNames.size()
						<< "\n\tnumberOfAnalog=" << pmuConfig.analogChnNames.size()
						<< "\n\tnumberOfDigital=" << pmuConfig.digitalChnNames.size()
						<< "\n\tPOS_LAT/LON/ELEV=" << pmuConfig.POS_LAT << " / " << pmuConfig.POS_LON << " / " << pmuConfig.POS_ELEV
						<< "\n\tSrvcClass=" << pmuConfig.ServiceClass
						<< "\n\tPhasorMeasurementWindow=" << pmuConfig.PhasorMeasurementWindow
						<< "\n\tPhasorMeasurementGroupDelayMs=" << pmuConfig.PhasorMeasurementGroupDelayMs
						<< "\n\tConfCnt=" << pmuConfig.ConfChangeCnt
						<< endl << endl;

					// Print all Phasor names
					strout << getLogTs() << "Phasor channels" << std::endl;
					for( std::vector<std::string>::iterator iter = pmuConfig.phasorChnNames.begin(); iter != pmuConfig.phasorChnNames.end(); ++iter )
						strout << "\tPHASOR X: " << *iter << std::endl;


					// Print all analog names
					strout << getLogTs() << "Analog channels" << std::endl;
					for( std::vector<std::string>::iterator iter = pmuConfig.analogChnNames.begin(); iter != pmuConfig.analogChnNames.end(); ++iter )
						strout << "\tANALOG X: " << *iter << std::endl;

					// Print all digital names
					strout << getLogTs() << "Digital channels" << std::endl;
					for( std::vector<std::string>::iterator iter = pmuConfig.digitalChnNames.begin(); iter != pmuConfig.digitalChnNames.end(); ++iter ) {
						if( iter->length() == 0 ) continue;
						strout << "\tDIGITAL X: " << *iter << std::endl;
					}

					strout << std::endl << std::endl;
				}

				// Start datastream
				strout << getLogTs() << "Starting datastream...\n\n";
				pdcClient->StartDataStream();

				// Read RTD continously until 'not active'
				while( isActive )
				{
					pdcClient->ReadDataFrame(30000);
					C37118PdcDataFrame  dataFrame = pdcClient->GetPdcDataFrame();
				
					tm timestamp = TimeConversionHelper::SecondsSinceEpochToDateTime(dataFrame.HeaderCommon.SOC);

					strout << getLogTs() << "SOC: " << dataFrame.HeaderCommon.SOC
						<< ", Frac=" << dataFrame.HeaderCommon.FracSec.FractionOfSecond << " (" << (int)dataFrame.HeaderCommon.FracSec.TimeQuality << ") - Ts="
						<< setfill('0') << setw(2) << timestamp.tm_mday << "." 
						<< setfill('0') << setw(2) << timestamp.tm_mon+1 << "." 
						<< setfill('0') << setw(2) << (timestamp.tm_year+1900) << " "
						<< setfill('0') << setw(2) << timestamp.tm_hour << ":" 
						<< setfill('0') << setw(2) << timestamp.tm_min << ":" 
						<< setfill('0') << setw(2) << (double)timestamp.tm_sec+((float)dataFrame.HeaderCommon.FracSec.FractionOfSecond / (float)pdcConfig.TimeBase.TimeBase)
						<< std::endl;

					for( int iPmu = 0; iPmu < pdcConfig.PMUs.size(); ++iPmu )
					{
						C37118PmuConfiguration_Ver3 pmuConfig = pdcConfig.PMUs[iPmu];
						C37118PmuDataFrame pmuDataFrame = dataFrame.pmuDataFrame[iPmu];
					
						// Print all analog values
						strout << getLogTs() << "Analog values (#" << pmuConfig.analogChnNames.size() << "): ";
						for( int i = 0; i < pmuConfig.analogChnNames.size(); ++i )
							strout << pmuDataFrame.AnalogValues[i].getValueAsFloat() << ",";
						strout << " :END\n";

						strout << getLogTs() << "Phasor values: (#" << pmuConfig.phasorChnNames.size() << "): ";
						for( int i= 0; i < pmuConfig.phasorChnNames.size(); ++i )
							strout << pmuDataFrame.PhasorValues[i].Real << "|" << pmuDataFrame.PhasorValues[i].Imag << ",";
						strout << " :END\n";

						strout << getLogTs() << "Digital values: (#" << pmuConfig.digitalChnNames.size() << "): ";
						for( int i = 0; i < pmuConfig.digitalChnNames.size(); ++i )
							strout << pmuDataFrame.DigitalValues[i] << ", ";
						strout << " :END\n\n";

						strout.flush();		
					}
				}
			}
			catch( Exception e )
			{
				std::cerr << "An error has ocurred: " << e.ExceptionMessage() << ". Waiting 10 seconds.";
				pdcClient->CloseConnection();
				Sleep(10000);
			}
		}
		pdcClient->CloseConnection();

		_endthread();
	}

	const int BASE_PORT = 12400;
	const int NUM_PDC_INSTANCES = 1;

	void ThreadPerSocketMain()
	{
		Sleep(5000);
		std::cout << "Connecting to #" << NUM_PDC_INSTANCES << " pdc's.." << std::endl;

		std::vector<PdcClient*> clients;

		for( int i = 0; i < NUM_PDC_INSTANCES; ++i )
		{
			PdcClient* pdcClient = new PdcClient("::1", BASE_PORT, i);
			_beginthread(PdcClientProc,0, pdcClient);
		}
	
		// wait here until user hits enter..
		std::cin.get();
		isActive = false;
		Sleep(1000);

		// Clean up
		for( std::vector<PdcClient*>::iterator iter = clients.begin(); iter != clients.end(); ++iter )
			delete *iter;
	}
}