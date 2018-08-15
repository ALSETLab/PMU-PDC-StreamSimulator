#define TIXML_USE_STL
#define TINYXML2_INCLUDED 
#include <windows.h>
#include <process.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <conio.h>
#include <ctype.h>
#include <vector>
#include "../StrongridDLL/Strongrid.h"
#include <fstream>
#include "ConfigInterpreter.h"
#include <map>
#include <time.h>

using namespace std;
using namespace stresstest;

static const int TIMEOUT_MS = 30000;


static bool WRITE_OUTPUT_TO_FILE = true;
volatile bool SHUTDOWN_FLAG = false;

void AssertIsZero( int val )
{
	if( val != 0 ) throw Exception("Function did not return 0");
}

class PmuState
{
public:	
	PmuState( pmuConfig_Ver3* pmuCfg, int pmuIndex )
	{
		m_pmuConfiguration = 0;
		m_pmuConfigurationVer3 = pmuCfg;
		m_pmuIndex = pmuIndex;
		m_data = 0;

		// Create and initialize realData object
		m_data = new noArraysPmuDataFrame();
		phasorValueReal = new float[m_pmuConfigurationVer3->numberOfPhasors];
		phasorValueImaginary = new float[m_pmuConfigurationVer3->numberOfPhasors];
		analogValueArr = new float[m_pmuConfigurationVer3->numberOfAnalog];
		digitalValueArr = new uint8_t[m_pmuConfigurationVer3->numberOfDigital];
		PhasorArrayLength = m_pmuConfiguration->numberOfPhasors;
		AnalogArrayLength = m_pmuConfiguration->numberOfAnalog;
		DigitalArrayLength = m_pmuConfiguration->numberOfDigital;
	}

	PmuState( pmuConfig* pmuCfg, int pmuIndex )
	{
		m_pmuConfiguration = pmuCfg;
		m_pmuConfigurationVer3 = 0;
		m_pmuIndex = pmuIndex;
		m_data = 0;

		// Create and initialize realData object
		m_data = new noArraysPmuDataFrame();

		// Initialize arrays
		phasorValueReal = new float[m_pmuConfiguration->numberOfPhasors];
		phasorValueImaginary = new float[m_pmuConfiguration->numberOfPhasors];
		analogValueArr = new float[m_pmuConfiguration->numberOfAnalog];
		digitalValueArr = new uint8_t[m_pmuConfiguration->numberOfDigital];
		PhasorArrayLength = m_pmuConfiguration->numberOfPhasors;
		AnalogArrayLength = m_pmuConfiguration->numberOfAnalog;
		DigitalArrayLength = m_pmuConfiguration->numberOfDigital;
	}

	~PmuState()
	{
		if( m_pmuConfiguration != 0 ) {
			delete [] m_pmuConfiguration->stationname;
			delete m_pmuConfiguration;
		}
		if( m_pmuConfigurationVer3 != 0 ) {
			delete [] m_pmuConfigurationVer3->stationname;
			delete m_pmuConfigurationVer3;
		}

		delete [] phasorValueReal;
		delete [] phasorValueImaginary;
		delete [] analogValueArr;
		delete [] digitalValueArr;
		delete m_data;
	}
/*
	const pmuConfig_Ver3* PmuCfgVer3() { return m_pmuConfigurationVer3; }
	const pmuConfig* PmuCfgVer2() { return m_pmuConfiguration; }*/
	const noArraysPmuDataFrame* PmuData() { return m_data; }
	noArraysPmuDataFrame* PmuDataRef() { return m_data; }

	void ObfuscateRealDataFrame() 
	{
		//// obfuscate phasor values
		//for( int i = 0; i < m_pmuConfiguration->numberOfPhasors; ++i ) {
		//	m_data->phasorValueReal[i] = 77;
		//	m_data->phasorValueImaginary[i] = 77;
		//}

		//// Obfuscate analog data
		//for( int i = 0; i < m_pmuConfiguration->numberOfAnalog; ++i )
		//	m_data->analogValueArr[i] = 99;

		//// Obfuscate digital data
		//for( int i = 0; i < m_pmuConfiguration->numberOfDigital; ++i )
		//	m_data->digitalValueArr[i] = 88;
	}

	int PmuIndex() const { return m_pmuIndex; }

	
	float* phasorValueReal;
	float* phasorValueImaginary;
	float* analogValueArr;
	int PhasorArrayLength;
	int AnalogArrayLength;
	int DigitalArrayLength;
	uint8_t* digitalValueArr;

private:
	pmuConfig_Ver3* m_pmuConfigurationVer3;
	pmuConfig* m_pmuConfiguration;
	noArraysPmuDataFrame* m_data;
	int m_pmuIndex;
};


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


void ReadFrameLoopProc(int pseudoPdcId, ostream& strout, const std::vector<PmuState*>& pmuConfigurationMap  )
{
	int pseudoPdcIdArray[1024];
	
	// Read frame loop
	//for( int numIterations = 0; numIterations < 50 * 10 && SHUTDOWN_FLAG == false; ++numIterations )
	while( SHUTDOWN_FLAG == false )
	{	
		strout << "=============================================================\nREADING DATAFRAME\n=============================================================\n";

		// TMP DEBUG - test threadpooling base function
		int pdcWithData = 0;
		memset(pseudoPdcIdArray,0,sizeof(int)*1024);
		AssertIsZero( pollPdcWithDataWaiting(1024, pseudoPdcIdArray, &pdcWithData, 30000) );

		// Read next frame
		AssertIsZero(::readNextFrame(1500, pseudoPdcId));
			
		// Get PDC data
		pdcDataFrame pdcData;
		AssertIsZero(::getPdcRealData(&pdcData, pseudoPdcId));
			
		// Print timestamp
		ParsedTimestamp ts = pdcData.Timestamp;
		strout << getLogTs() << "Dataframe Ts: " 
			<< setfill('0') << setw(4) << ts.Year << "." 
			<< setfill('0') << setw(2) << ts.Month << "." 
			<< setfill('0') << setw(2) << ts.Day << " "
			<< setfill('0') << setw(2) << ts.Hour << ":"
			<< setfill('0') << setw(2) << ts.Minute << ":"
			<< setfill('0') << fixed << setw(2) << setprecision(3) << (ts.Second + (float)ts.Ms / 1000.0f);

		// Print timestamp quality data
		strout << ", ClockIsReliable=" << pdcData.TimeQuality.ClockIsReliable 
			<< ", ClockErrSec="  <<  pdcData.TimeQuality.ClockErrorSec 
			<< ", LeapSecPending=" << pdcData.TimeQuality.LeapSecPending
			<< ", LeapSecCorr=" << pdcData.TimeQuality.LeapSecCorrection
			<< std::endl;

		for( std::vector<PmuState*>::const_iterator iter = pmuConfigurationMap.begin(); iter != pmuConfigurationMap.end(); ++iter )
		{
			// Get a quick ref to the pmustate object for easy use
			PmuState* pmu = *iter;
								
			// FOR TEST ONLY - RANDOMIZE CONTENT OF PMU DATAFRAME
			pmu->ObfuscateRealDataFrame();

			// Print data - values
			PmuStatus pmuSts;
			AssertIsZero(::getPmuRealDataLabview(pmu->PmuDataRef(), &pmuSts, 
				pmu->PhasorArrayLength, pmu->phasorValueReal, pmu->phasorValueImaginary,
				pmu->AnalogArrayLength, pmu->analogValueArr, 
				pmu->DigitalArrayLength, pmu->digitalValueArr,
				pseudoPdcId, pmu->PmuIndex()));
				
			// Print STAT 
			strout << getLogTs() << "Statusbits - \n\t"
				<< "DataErrorCode=" << (int)pmuSts.dataErrorCode << ", "
				<< "PmuSyncFlag=" << pmuSts.pmuSyncFlag << ", "
				<< "PmuDataSortFlag=" << pmuSts.pmuDataSortingFlag << ", "
				<< "TriggerFlag=" << pmuSts.pmuTriggerFlag << ", "
				<< "ConfCngFlag=" << pmuSts.configChangeFlag << ",\n\t"
				<< "DataModified=" << pmuSts.dataModifiedFlag << ","
				<< "TimeQualityCode=" << (int)pmuSts.timeQualityCode << ", "
				<< "UnlockTimeCode=" << (int)pmuSts.unlockTimeCode << ", "
				<< "TriggerReasonCode=" << (int)pmuSts.triggerReasonCode 
				<< std::endl;
				
			// Print all phasors
			strout << getLogTs() << "Phasor values: (#" << pmu->PhasorArrayLength << "): ";
			for( int i= 0; i < pmu->PhasorArrayLength; ++i )
				strout << pmu->phasorValueReal[i] << "|" << pmu->phasorValueImaginary[i] << ",";
			strout << " :END\n";

			// Print all analog values
			strout << getLogTs() << "Analog values (#" << pmu->AnalogArrayLength << "): ";
			for( int i = 0; i < pmu->AnalogArrayLength; ++i )
				strout << pmu->analogValueArr[i] << ",";
			strout << " :END\n";

			// Print all digital bits
			strout << getLogTs() << "Digital values: (#" << pmu->DigitalArrayLength << "): ";
			for( int i = 0; i < pmu->DigitalArrayLength; ++i )
				strout << (int)pmu->digitalValueArr[i] << ",";
			strout << " :END\n\n";

			strout.flush();
		}
	}
}

void pdcThreadProc_Ver3(void* pdccfgraw)
{
	PdcConfig pdc = *((PdcConfig*)pdccfgraw);	

	while( SHUTDOWN_FLAG == false )
	{
		Sleep(2000);
		int pseudoPdcId = -1;

		ostringstream ostr;
		ostream strout(cout.rdbuf());
		filebuf   objFileBuf;

		try {
			// Connect to the pdc
			bool isConnected = false;
			while ( isConnected == false )
			{
				strout << getLogTs() << "Connecting to PDC, ID=" << pdc.PdcId << ", IP=" << pdc.IP << ", Port=" << pdc.Port << endl;
				int retval = ::connectPdc((char*)pdc.IP.c_str(), pdc.Port, atoi(pdc.PdcId.c_str()), &pseudoPdcId );
				isConnected = retval == 0;
				if( isConnected == false ) {
		 			strout << getLogTs() << "StrongridIEEEC37118Dll::connectPdc returned " << retval << ". Waiting 10s.";
					Sleep(10000);
				}
			}
			strout << getLogTs() << "Connection established" << endl;
				
			// change output buffer to 'file' if 
			if( WRITE_OUTPUT_TO_FILE )
			{	
				// Get timestamp
				_SYSTEMTIME sysTime;			
				GetLocalTime(&sysTime);

				ostr.clear(); ostr << "PDC_" << pdc.PdcId << "!" << pseudoPdcId << "_"
					<< sysTime.wYear << "." << sysTime.wMonth << "." << sysTime.wDay << "_"
					<< setfill('0') << setw(2) << sysTime.wHour  << "_"
					<< setfill('0') << setw(2) << sysTime.wMinute << "_"
					<< setfill('0') << setw(2) << sysTime.wSecond <<  ".log";
				ostr.width(2);
				strout << getLogTs() << "Switching to file based output: '" << ostr.str() << "' for PDCID=" << pdc.PdcId << " @ " << pdc.IP << ":" << pdc.Port << std::endl;
				objFileBuf.open (ostr.str(), ios :: out);
				strout.set_rdbuf(&objFileBuf);
			}	

			// Assert on the pseudopdcid - must be >= 0
			if( pseudoPdcId < 0 ) throw Exception("PseudoPdcId < 0");

		
			// Read header			
			char headerBuffer[1024];
			AssertIsZero(::readHeaderData(30000, pseudoPdcId));
			AssertIsZero(::getHeaderMsg(headerBuffer,1024, pseudoPdcId ));
			strout << getLogTs() << "Header message:\n----------------------------------------\n" << headerBuffer << "\n----------------------------------------\n";

			// Read configuration
			strout << getLogTs() << "Reading configuration...";
			int retval = ::readConfiguration_Ver3(TIMEOUT_MS, pseudoPdcId);
			if( retval != 0 ) {
				ostr.clear(); ostr << "StrongridIEEEC37118Dll::connectPdc returned " << retval << ".";
				throw Exception(ostr.str());
			}
			strout << "Configuration read successfully" << endl;

			// Print all configuration parameters: Common pdc		
			pdcConfiguration pdcCommonCfg;
			AssertIsZero(::getPdcConfig_Ver3(&pdcCommonCfg, pseudoPdcId ));
			strout << getLogTs() << "getMainConfig - " 
				<< "\n\tFrame/Sec=" << pdcCommonCfg.FramesPerSecond
				<< endl << endl;

			// Map pseudoPdcId->pmuConfig
			std::vector<PmuState*> pmuList;		

			// Get PMU configuration for each PMU in the PDC
			for( int i = 0; i < pdcCommonCfg.numberOfPMUs; ++i )
			{	

				// Extract pmu configuration for the PMU at index 'i'						
				pmuConfig_Ver3* pmuConf = new pmuConfig_Ver3(); pmuConf->stationname = new char[256];

				AssertIsZero(::getPmuConfiguration_Ver3(pmuConf, pseudoPdcId, i ));

				// Print common configuration params
				strout << getLogTs() << "getPmuConfiguration_Ver3/pmuConfig - "
					<< "\n\tpmuId=" << pmuConf->pmuid
					<< "\n\tstationname=" << pmuConf->stationname
					<< "\n\tnomFreq=" << pmuConf->nominalFrequency
					<< "\n\tnumberOfPhasors=" << pmuConf->numberOfPhasors
					<< "\n\tnumberOfAnalog=" << pmuConf->numberOfAnalog
					<< "\n\tnumberOfDigital=" << pmuConf->numberOfDigital
					<< "\n\tPosition LAT/LON/ELEV=" << pmuConf->POS_LAT << "|" << pmuConf->POS_LON << "|" << pmuConf->POS_ELEV
					<< "\n\tPhasorMsmGroupDelay=" << pmuConf->PhasorMeasurementGroupDelayMs 
					<< "\n\tPhasorMsmWindow=" << pmuConf->PhasorMeasurementWindow
					<< endl << endl;

				char tmpNameBuffer[256];

				// Print PHASOR scalar information
				for( int iPhasor = 0; iPhasor < pmuConf->numberOfPhasors; ++iPhasor )
				{
					phasorConfig_Ver3 phasorCfg; phasorCfg.name = tmpNameBuffer;
					AssertIsZero(::getPhasorConfig_Ver3(&phasorCfg, pseudoPdcId, i, iPhasor ));
					strout << getLogTs() << "getPhasorConfig_Ver3/phasorConfig_Ver3 - "
						<< "\n\tName=" << phasorCfg.name
						<< "\n\tType=" << (int)phasorCfg.type
						<< "\n\tFormat=" << (int)phasorCfg.format
						<< "\n\tDataIsScaled=" << phasorCfg.dataIsScaled
						<< "\n\tScalar_Magnitude=" << phasorCfg.scaling_magnitude
						<< "\n\tScalar_Offset=" << phasorCfg.scaling_angleOffset
						<< std::endl << std::endl;
				}

				// Print ANALOG scalar information
				for( int iAnalog = 0; iAnalog < pmuConf->numberOfAnalog; ++iAnalog )
				{
					analogConfig_Ver3 analogCfg; analogCfg.name = tmpNameBuffer;
					AssertIsZero(::getAnalogConfig_Ver3(&analogCfg, pseudoPdcId, i, iAnalog ));
					strout << getLogTs() << "getPhasorConfig_Ver3/analogConfig_Ver3 - "
						<< "\n\tName=" << analogCfg.name
						<< "\n\tDataIsScaled=" << analogCfg.dataIsScaled
						<< "\n\tScalar_Magnitude=" << analogCfg.scaling_magnitude
						<< "\n\tScalar_Offset=" << analogCfg.scaling_offset
						<< std::endl << std::endl;
				}

				// Print DIGITAL unit information
				for( int iDig = 0; iDig < pmuConf->numberOfDigital; ++iDig )
				{
					digitalConfig digCfg; digCfg.name = tmpNameBuffer;
					AssertIsZero(::getDigitalConfig_Ver3(&digCfg, pseudoPdcId, i, iDig ));
					if( strlen(digCfg.name) == 0 ) continue;
					strout << getLogTs() << "getDigitalConfig/digitalConfig - "
						<< "\n\tName=" << digCfg.name
						<< "\n\tIdx=" << iDig
						<< "\n\tNormalBit=" << digCfg.normalBit
						<< "\n\tValidBit=" << digCfg.isValidBit
						<< std::endl << std::endl;
				}

				pmuList.push_back(new PmuState(pmuConf, i));
			}

			strout << getLogTs() << "\nStarting datastream..." << endl;
			AssertIsZero(::startDataStream(pseudoPdcId));
		
			// Ender shared-readframe loop
			ReadFrameLoopProc(pseudoPdcId, strout, pmuList );		
						
			// Clean up
			for( std::vector<PmuState*>::iterator iter = pmuList.begin(); iter != pmuList.end(); ++iter )
				delete (*iter);
			pmuList.clear();

			// Disconnect from pdc
			AssertIsZero(::disconnectPdc(pseudoPdcId));
			strout << getLogTs() << "Disconnected from PDC" << endl;
			strout.flush();
		}
		catch( Exception e )
		{
			printf("An error has ocurred: %s\n", e.ErrorMessage().c_str() );
		}
		catch( ... )
		{
			printf("An unknown error has ocurred");
		}
	}
	// cleanup
	_endthread();
}



void pdcThreadProc(void* pdccfgraw)
{
	PdcConfig pdc = *((PdcConfig*)pdccfgraw);	
		
	while( SHUTDOWN_FLAG == false )
	{
		Sleep(2000);
		int pseudoPdcId = -1;

		ostringstream ostr;
		ostream strout(cout.rdbuf());
		filebuf   objFileBuf;

		try {
			// Connect to the pdc
			bool isConnected = false;
			while ( isConnected == false )
			{
				strout << getLogTs() << "Connecting to PDC, ID=" << pdc.PdcId << ", IP=" << pdc.IP << ", Port=" << pdc.Port << endl;
				int retval = ::connectPdc((char*)pdc.IP.c_str(), pdc.Port, atoi(pdc.PdcId.c_str()), &pseudoPdcId );
				isConnected = retval == 0;
				if( isConnected == false ) {
		 			strout << getLogTs() << "StrongridIEEEC37118Dll::connectPdc returned " << retval << ". Waiting 10s.";
					Sleep(10000);
				}
			}
			strout << getLogTs() << "Connection established" << endl;
				
			// change output buffer to 'file' if 
			if( WRITE_OUTPUT_TO_FILE )
			{	
				// Get timestamp
				_SYSTEMTIME sysTime;			
				GetLocalTime(&sysTime);

				ostr.clear(); ostr << "PDC_" << pdc.PdcId << "!" << pseudoPdcId << "_"
					<< sysTime.wYear << "." << sysTime.wMonth << "." << sysTime.wDay << "_"
					<< setfill('0') << setw(2) << sysTime.wHour  << "_"
					<< setfill('0') << setw(2) << sysTime.wMinute << "_"
					<< setfill('0') << setw(2) << sysTime.wSecond <<  ".log";
				ostr.width(2);
				strout << getLogTs() << "Switching to file based output: '" << ostr.str() << "' for PDCID=" << pdc.PdcId << " @ " << pdc.IP << ":" << pdc.Port << std::endl;
				objFileBuf.open (ostr.str(), ios :: out);
				strout.set_rdbuf(&objFileBuf);
			}	

			// Assert on the pseudopdcid - must be >= 0
			if( pseudoPdcId < 0 ) throw Exception("PseudoPdcId < 0");
		
			// Read header			
			char headerBuffer[1024];
			::stopDataStream(pseudoPdcId);
			if( readHeaderData(10000, pseudoPdcId) == 0 && getHeaderMsg(headerBuffer,1024, pseudoPdcId ) == 0 )
				strout << getLogTs() << "Header message:\n----------------------------------------\n" << headerBuffer << "\n----------------------------------------\n";
			else
				strout << getLogTs() << "UNABLE TO READ HEADER MESSAGE!" << endl;

			// Read configuration
			strout << getLogTs() << "Reading configuration...";
			int retval = ::readConfiguration(TIMEOUT_MS, pseudoPdcId);
			if( retval != 0 ) {
				ostr.clear(); ostr << "StrongridIEEEC37118Dll::connectPdc returned " << retval << ".";
				throw Exception(ostr.str());
			}
			strout << "Configuration read successfully" << endl;

			// Print all configuration parameters: Common pdc		
			pdcConfiguration pdcCommonCfg;
			AssertIsZero(::getPdcConfig(&pdcCommonCfg, pseudoPdcId ));
			strout << getLogTs() << "getMainConfig - " 
				<< "\n\tFrame/Sec=" << pdcCommonCfg.FramesPerSecond
				<< endl << endl;

			// Map pseudoPdcId->pmuConfig
			std::vector<PmuState*> pmuList;		

			// Get PMU configuration for each PMU in the PDC
			for( int i = 0; i < pdcCommonCfg.numberOfPMUs; ++i )
			{	

				// Extract pmu configuration for the PMU at index 'i'						
				pmuConfig* pmuConf = new pmuConfig(); pmuConf->stationname = new char[256];

				AssertIsZero(::getPmuConfiguration(pmuConf, pseudoPdcId, i ));

				// Print common configuration params
				strout << getLogTs() << "getPmuConfiguration/pmuConfig - "
					<< "\n\tpmuId=" << pmuConf->pmuid
					<< "\n\tstationname=" << pmuConf->stationname
					<< "\n\tnomFreq=" << pmuConf->nominalFrequency
					<< "\n\tnumberOfPhasors=" << pmuConf->numberOfPhasors
					<< "\n\tnumberOfAnalog=" << pmuConf->numberOfAnalog
					<< "\n\tnumberOfDigital=" << pmuConf->numberOfDigital
					<< endl << endl;

				char tmpNameBuffer[256];

				// Print PHASOR scalar information
				for( int iPhasor = 0; iPhasor < pmuConf->numberOfPhasors; ++iPhasor )
				{
					phasorConfig phasorCfg; phasorCfg.name = tmpNameBuffer;
					AssertIsZero(::getPhasorConfig(&phasorCfg, pseudoPdcId, i, iPhasor ));
					strout << getLogTs() << "getPhasorConfig/phasorConfig - "
						<< "\n\tName=" << phasorCfg.name
						<< "\n\tType=" << (int)phasorCfg.type
						<< "\n\tFormat=" << (int)phasorCfg.format
						<< "\n\tDataIsScaled=" << phasorCfg.dataIsScaled
						<< "\n\tScalar=" << phasorCfg.scalar
						<< std::endl << std::endl;
				}

				// Print ANALOG scalar information
				for( int iAnalog = 0; iAnalog < pmuConf->numberOfAnalog; ++iAnalog )
				{
					analogConfig analogCfg; analogCfg.name = tmpNameBuffer;
					AssertIsZero(::getAnalogConfig(&analogCfg, pseudoPdcId, i, iAnalog ));
					strout << getLogTs() << "getPhasorConfig/analogConfig - "
						<< "\n\tName=" << analogCfg.name
						<< "\n\tDataIsScaled=" << analogCfg.dataIsScaled
						<< "\n\tUserdefined_Scalar=" << analogCfg.userdefined_scalar
						<< std::endl << std::endl;
				}

				// Print DIGITAL unit information
				for( int iDig = 0; iDig < pmuConf->numberOfDigital; ++iDig )
				{
					digitalConfig digCfg; digCfg.name = tmpNameBuffer;
					AssertIsZero(::getDigitalConfig(&digCfg, pseudoPdcId, i, iDig ));
					if( strlen(digCfg.name) == 0 ) continue;
					strout << getLogTs() << "getDigitalConfig/digitalConfig - "
						<< "\n\tName=" << digCfg.name
						<< "\n\tIdx=" << iDig
						<< "\n\tNormalBit=" << digCfg.normalBit
						<< "\n\tValidBit=" << digCfg.isValidBit
						<< std::endl << std::endl;
				}

				pmuList.push_back( new PmuState(pmuConf, i) );
			}

			strout << getLogTs() << "\nStarting datastream..." << endl;
			AssertIsZero(::startDataStream(pseudoPdcId));
		
			// Ender shared-readframe loop
			ReadFrameLoopProc(pseudoPdcId, strout, pmuList );		

			// Clean up
			for( std::vector<PmuState*>::iterator iter = pmuList.begin(); iter != pmuList.end(); ++iter )
				delete (*iter);
			pmuList.clear();

			// Disconnect from pdc
			AssertIsZero(::disconnectPdc(pseudoPdcId));
			strout << getLogTs() << "Disconnected from PDC" << endl;
			strout.flush();
		}
		catch( Exception e )
		{
			strout << getLogTs() << "An error has ocurred: " << e.ErrorMessage() << endl;
		}
		catch( ... )
		{
			printf("An unknown error has ocurred");
		}
	}

	// cleanup
	_endthread();
}

int main(int argc, char *argv[])
{
	try {
		ProgramConfig config = ReadProgramConfig("config.xml");

		// Print all PDC's in test
		cout << "PDC's in test:" << endl;
		for( std::vector<PdcConfig>::const_iterator iter = config.PdcPool.begin(); iter != config.PdcPool.end(); ++iter )
			cout << "   - PDC mounted for test: IP=" << iter->IP << ", PORT=" << iter->Port << ", PDCID=" << iter->PdcId << endl;

		cout << endl << endl << "Press 'enter' to exit." << endl << endl;

		// Connect to each PDC..
		cout << endl << endl << "Spawning PDC worker threads:\n";
		for( std::vector<PdcConfig>::iterator iter = config.PdcPool.begin(); iter != config.PdcPool.end(); ++iter )
		{
			PdcConfig* tmp = &(*iter);
			if( tmp->Version == 1 )
				_beginthread(pdcThreadProc, 0, (void*)tmp);
			else if( tmp->Version == 2 )
				_beginthread(pdcThreadProc_Ver3, 0, (void*)tmp);
			else
				throw Exception("Invalid config - Version must be either 1 or 2");
		}


		// Close program
		while( cin.get() != 'q');
		cout << "\nShutting down..";
		SHUTDOWN_FLAG = true; // signal shutdown to PDC worker threads

		Sleep(2000);
	}
	catch( Exception e )
	{
		cout << "An error has ocurred: " << e.ErrorMessage() << endl;
		cin.get();
	}
	catch( ... )
	{
		cout << "an unknown error has ocurred. Shutting down.";
		cin.get();
	}
}
