// StrongridIEEEC37118Dll.cpp : Defines the exported functions for the DLL application.

#include <winsock2.h>
#include <Windows.h>
#include <math.h>
#include <stdint.h>
#include "Strongrid.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <map>
#include "../StrongridClientBase/PdcClient.h"
#include "../StrongridBase/common.h"
#include "Mutex.h"

using namespace std;
using namespace strongridclientbase;
using namespace stronggriddll;

static const int RETERR_OK = 0;
static const int RETERR_UNKNOWN_ERR = 1;
static const int RETERR_NET_TIMEOUT = 2;
static const int RETERR_INVALID_INPUT_PHASOR_ARR = 3;
static const int RETERR_INVALID_INPUT_ANALOG_ARR = 4;
static const int RETERR_INVALID_INPUT_DIGITAL_ARR = 5;

static Mutex s_clientMapLock;
const int MAXIMUM_CONCURRENT_CLIENTS = 8192;
static int s_pdcClientCursor = 0;
static PdcClient* s_pdcClientMap[MAXIMUM_CONCURRENT_CLIENTS]; // maps: index -> PdcClient* [0 => no client]
static std::vector<std::pair<int,int>> s_socketPollVector; // Maps: pseudopdcid, socket FD | only contains active clients

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  msg, 
					  LPVOID lpReserved)
{
	switch (msg)
	{
	case DLL_PROCESS_ATTACH:
		memset(s_pdcClientMap, 0, sizeof(PdcClient*) ); // set all to 0
		break;

	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;

	case DLL_PROCESS_DETACH:

		break;
	}
	return TRUE;
}


STRONGRIDIEEEC37118DLL_API int init(void)
{
	return RETERR_OK;
}

STRONGRIDIEEEC37118DLL_API int connectPdc( char *ipAddress,  int32_t port, int32_t pdcId, int32_t* pseudoPdcId )
{
	std::string ipAddr = string(ipAddress);
	PdcClient* client = new PdcClient(ipAddr, port, pdcId);
	try {
		client->Connect();
	}
	catch(...)
	{
		delete client;
		return RETERR_UNKNOWN_ERR;
	}

	MutexFragment mux(&s_clientMapLock);
	{
		// Find the first available slot in the map
		while( true )
		{
			s_pdcClientCursor = ++s_pdcClientCursor % MAXIMUM_CONCURRENT_CLIENTS;
			if( s_pdcClientMap[s_pdcClientCursor] == 0 )
				break;
		}

		// Add to client map table
		s_pdcClientMap[s_pdcClientCursor] = client;
		*pseudoPdcId = s_pdcClientCursor;

		// Add to socket-listener vector
		s_socketPollVector.push_back(std::pair<int,int>(s_pdcClientCursor,client->GetSocketDescriptor()));
	}
	mux.Finalize();

	return RETERR_OK;
}

bool PseudoPdcIdIsValidClient( int32_t pseudoPdcId )
{	
	return !(pseudoPdcId > MAXIMUM_CONCURRENT_CLIENTS || pseudoPdcId <= 0 || s_pdcClientMap[pseudoPdcId] == 0);
}

STRONGRIDIEEEC37118DLL_API int disconnectPdc(int32_t pseudoPdcId)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;
	try {
		MutexFragment mux(&s_clientMapLock);
		{
			// Drop from 'socket listener' vector
			std::vector<std::pair<int,int>>::iterator iterErase;
			for( iterErase = s_socketPollVector.begin(); iterErase != s_socketPollVector.end(); ++iterErase )
				if( iterErase->first == pseudoPdcId ) break;
			s_socketPollVector.erase(iterErase);

			// Disconnect and drop from table
			s_pdcClientMap[pseudoPdcId]->CloseConnection();
			delete s_pdcClientMap[pseudoPdcId];
			s_pdcClientMap[pseudoPdcId] = 0;
		}
		mux.Finalize();
		return RETERR_OK;
	}
	catch( SocketTimeout )
	{
		return RETERR_NET_TIMEOUT;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}


STRONGRIDIEEEC37118DLL_API int __cdecl readHeaderData( int32_t timeout, int32_t pseudoPdcId)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		s_pdcClientMap[pseudoPdcId]->ReadHeaderMessage(timeout);
		return RETERR_OK;
	}
	catch( SocketTimeout )
	{
		return RETERR_NET_TIMEOUT;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}

STRONGRIDIEEEC37118DLL_API int __cdecl readConfiguration(  int32_t timeout, int32_t pseudoPdcId)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		s_pdcClientMap[pseudoPdcId]->ReadConfiguration(timeout);
		return RETERR_OK;
	}
	catch( SocketTimeout )
	{
		return RETERR_NET_TIMEOUT;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}



STRONGRIDIEEEC37118DLL_API int __cdecl readConfiguration_Ver3(  int32_t timeout, int32_t pseudoPdcId)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		s_pdcClientMap[pseudoPdcId]->ReadConfigurationVer3(timeout);
		return RETERR_OK;
	}
	catch( SocketTimeout )
	{
		return RETERR_NET_TIMEOUT;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}




STRONGRIDIEEEC37118DLL_API int startDataStream( int32_t pseudoPdcId)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		s_pdcClientMap[pseudoPdcId]->StartDataStream();
		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}

STRONGRIDIEEEC37118DLL_API int stopDataStream(int32_t pseudoPdcId)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		s_pdcClientMap[pseudoPdcId]->StopDataStream();
		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}


STRONGRIDIEEEC37118DLL_API int readNextFrame(int32_t timeOut, int32_t pseudoPdcId)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		s_pdcClientMap[pseudoPdcId]->ReadDataFrame(timeOut);
		return RETERR_OK;
	}
	catch( SocketTimeout )
	{
		return RETERR_NET_TIMEOUT;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}

STRONGRIDIEEEC37118DLL_API int dllshutdown(void)
{
	try {
		// Traverse the entire clientmap and shutdown/delete every non-null entry
		for( int i = 0; i < MAXIMUM_CONCURRENT_CLIENTS; ++i )
		{
			PdcClient* client = s_pdcClientMap[i];
			client->CloseConnection();
			delete client;
		}
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}

	return RETERR_OK;
}


std::vector<int> CheckPortsForDataToRead(int  timeoutMs)
{	
	// Copy the data into a temporary array to avoid blocking too long
	MutexFragment mux(&s_clientMapLock);
	const int arrayLength = s_socketPollVector.size();
	WSAPOLLFD* socketListenArray = new WSAPOLLFD[arrayLength];
	int* pseudoPdcIdArr = new int[arrayLength];

	for( int i = 0; i < arrayLength; ++i ) {
		socketListenArray[i].fd = s_socketPollVector[i].second;
		socketListenArray[i].events = POLLIN;
		pseudoPdcIdArr[i] = s_socketPollVector[i].first;
	}
	mux.Finalize();

	// Poll for data
	int ret = WSAPoll(socketListenArray, arrayLength, timeoutMs);	

	std::vector<int> output;
	if( ret > 0 )
	{
		for( int i = 0; i < arrayLength; ++i )
		{
			if( socketListenArray[i].revents & (POLLRDNORM | POLLERR | POLLHUP)  ) output.push_back(pseudoPdcIdArr[i]);
		}
	}

	delete [] socketListenArray;
	delete [] pseudoPdcIdArr;

	return output;
}

STRONGRIDIEEEC37118DLL_API int __cdecl pollPdcWithDataWaiting( int pseudoPdcIdArrayLength, int32_t* outPseudoPdcIdArr, int32_t* outNumPdcWithData, int pollTimeoutMs )
{
	try {
		// Get a list of all sockets available for reading
		std::vector<int> readsockVec = CheckPortsForDataToRead(pollTimeoutMs);
		
		// Copy data from result to output arrray
		for( int i = 0; i < readsockVec.size() && i < pseudoPdcIdArrayLength; ++i )
			outPseudoPdcIdArr[i] = readsockVec[i];
		*outNumPdcWithData = min(readsockVec.size(),pseudoPdcIdArrayLength);
		return RETERR_OK;
	}
	catch( Exception e )
	{
		return RETERR_UNKNOWN_ERR;
	}
}


// -----------------------------------------------------------------------------------------------------------------
// -------------------------------------------------- GET FUNCTIONS ------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------


ParsedTimestamp GetParsedTimestamp( uint32_t soc, int fracseq, int timebase, double* outSocWithFrac  )
{
	tm tmp = TimeConversionHelper::SecondsSinceEpochToDateTime(soc);
	ParsedTimestamp ts;
	ts.Year = tmp.tm_year + 1900;
	ts.Month = tmp.tm_mon + 1;
	ts.Day = tmp.tm_mday;
	ts.Hour = tmp.tm_hour;
	ts.Minute = tmp.tm_min;
	ts.Second = tmp.tm_sec;
	ts.Ms = ((float)fracseq / (float)timebase)*1000;
	*outSocWithFrac = (double)soc + (float)fracseq / (float)timebase;
	return ts;
}

TimeStatus GetClockStatus(const C37118FracSec& frac)
{
	TimeStatus sts;
	bool leapSec, clockReliable;
	frac.GetParsedQuality(&sts.LeapSecCorrection, &leapSec, &sts.ClockErrorSec, &clockReliable);
	sts.LeapSecPending = leapSec != 0;
	sts.ClockIsReliable = clockReliable != 0;
	return sts;
}


STRONGRIDIEEEC37118DLL_API int getPdcConfig(pdcConfiguration* outCfg, int32_t pseudoPdcId)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return 1;
	
	try {
		const C37118PdcConfiguration& pdcCfg = s_pdcClientMap[pseudoPdcId]->GetPdcConfiguration();

		outCfg->TimeQuality = GetClockStatus(pdcCfg.HeaderCommon.FracSec);
		outCfg->Timestamp = GetParsedTimestamp(pdcCfg.HeaderCommon.SOC, pdcCfg.HeaderCommon.FracSec.FractionOfSecond,
			pdcCfg.TimeBase.TimeBase, &outCfg->SecondOfCentury );
		outCfg->FramesPerSecond = pdcCfg.DataRate.FramesPerSecond();
		outCfg->numberOfPMUs = pdcCfg.PMUs.size();

		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}

STRONGRIDIEEEC37118DLL_API int getPdcConfig_Ver3(pdcConfiguration* outCfg, int32_t pseudoPdcId)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;
	
	try {
		const C37118PdcConfiguration_Ver3& pdcCfg = s_pdcClientMap[pseudoPdcId]->GetPdcConfigurationVer3();

		outCfg->TimeQuality = GetClockStatus(pdcCfg.HeaderCommon.FracSec);
		outCfg->Timestamp = GetParsedTimestamp(pdcCfg.HeaderCommon.SOC, pdcCfg.HeaderCommon.FracSec.FractionOfSecond,
			pdcCfg.TimeBase.TimeBase, &outCfg->SecondOfCentury );
		outCfg->FramesPerSecond = pdcCfg.DataRate.FramesPerSecond();
		outCfg->numberOfPMUs = pdcCfg.PMUs.size();

		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}

STRONGRIDIEEEC37118DLL_API int getPmuConfiguration(pmuConfig* pmuconf, int32_t pseudoPdcId, int32_t pmuIndex)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;
	
	try {
		const C37118PdcConfiguration& pdcCfg = s_pdcClientMap[pseudoPdcId]->GetPdcConfiguration();
		const C37118PmuConfiguration& pmuCfg = pdcCfg.PMUs[pmuIndex];

		pmuconf->pmuid = pmuCfg.IdCode;		
		memset(pmuconf->stationname, 0, 256);
		strncpy(pmuconf->stationname, pmuCfg.StationName.c_str(), min(pmuCfg.StationName.length(), 255) );
		pmuconf->nominalFrequency = pmuCfg.NomFreqCode.GetAsFrequency();
		pmuconf->numberOfPhasors = pmuCfg.phasorChnNames.size();
		pmuconf->numberOfAnalog = pmuCfg.analogChnNames.size();
		pmuconf->numberOfDigital = pmuCfg.digitalChnNames.size();

		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}


STRONGRIDIEEEC37118DLL_API int getPmuConfiguration_Ver3(pmuConfig_Ver3* pmuconf, int32_t pseudoPdcId, int32_t pmuIndex)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;
	
	try {
		const C37118PdcConfiguration_Ver3& pdcCfg = s_pdcClientMap[pseudoPdcId]->GetPdcConfigurationVer3();
		const C37118PmuConfiguration_Ver3& pmuCfg = pdcCfg.PMUs[pmuIndex];

		pmuconf->pmuid = pmuCfg.IdCode;		
		memset(pmuconf->stationname, 0, 256);
		strncpy(pmuconf->stationname, pmuCfg.StationName.c_str(), min(pmuCfg.StationName.length(), 255) );
		pmuconf->nominalFrequency = pmuCfg.NomFreqCode.GetAsFrequency();
		pmuconf->numberOfPhasors = pmuCfg.phasorChnNames.size();
		pmuconf->numberOfAnalog = pmuCfg.analogChnNames.size();
		pmuconf->numberOfDigital = pmuCfg.digitalChnNames.size();

		pmuconf->POS_LAT = pmuCfg.POS_LAT;
		pmuconf->POS_LON = pmuCfg.POS_LON;
		pmuconf->POS_ELEV = pmuCfg.POS_ELEV;

		pmuconf->SVC_CLASS = pmuCfg.ServiceClass;
		pmuconf->PhasorMeasurementWindow = pmuCfg.PhasorMeasurementWindow;
		pmuconf->PhasorMeasurementGroupDelayMs = pmuCfg.PhasorMeasurementGroupDelayMs;
		
		for( int i = 0; i < 16; ++i )
			pmuconf->globalPmuId[i] = pmuCfg.GlobalPmuId[i];

		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}

STRONGRIDIEEEC37118DLL_API int getPhasorConfig( phasorConfig* phasorCfg, int32_t pseudoPdcId, int32_t pmuIndex, int32_t phasorIndex)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		const C37118PdcConfiguration& pdcCfg = s_pdcClientMap[pseudoPdcId]->GetPdcConfiguration();
		const C37118PmuConfiguration& pmuCfg = pdcCfg.PMUs[pmuIndex];
		const C37118PhasorUnit& phUnit = pmuCfg.PhasorUnit[phasorIndex];
				
		memset( phasorCfg->name, 0, 256 );
		strncpy(phasorCfg->name, pmuCfg.phasorChnNames[phasorIndex].c_str(), min(pmuCfg.phasorChnNames[phasorIndex].length(), 255) );
		phasorCfg->type = phUnit.Type;
		phasorCfg->format = 0;
		phasorCfg->dataIsScaled = pmuCfg.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat == 1; // if "float" => data is scaled
		phasorCfg->scalar = phUnit.PhasorScalar;

		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}


STRONGRIDIEEEC37118DLL_API int getPhasorConfig_Ver3( phasorConfig_Ver3* phasorCfg, int32_t pseudoPdcId, int32_t pmuIndex, int32_t phasorIndex)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		const C37118PdcConfiguration_Ver3& pdcCfg = s_pdcClientMap[pseudoPdcId]->GetPdcConfigurationVer3();
		const C37118PmuConfiguration_Ver3& pmuCfg = pdcCfg.PMUs[pmuIndex];
		const C37118PhasorScale_Ver3& phUnit = pmuCfg.PhasorScales[phasorIndex];
				
		memset( phasorCfg->name, 0, 256 );
		strncpy(phasorCfg->name, pmuCfg.phasorChnNames[phasorIndex].c_str(), min(pmuCfg.phasorChnNames[phasorIndex].length(), 255) );
		phasorCfg->type = phUnit.VoltOrCurrent;
		phasorCfg->format = 0;
		phasorCfg->dataIsScaled = pmuCfg.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat == 1; // if "float" => data is scaled
		phasorCfg->scaling_magnitude = phUnit.ScaleFactorOne_Y;
		phasorCfg->scaling_angleOffset = phUnit.ScaleFactorTwo_Angle;

		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}




STRONGRIDIEEEC37118DLL_API int getAnalogConfig( analogConfig *analogCfg, int32_t pseudoPdcId, int32_t pmuIndex, int32_t analogIndex)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		const C37118PdcConfiguration& pdcCfg = s_pdcClientMap[pseudoPdcId]->GetPdcConfiguration();
		const C37118PmuConfiguration& pmuCfg = pdcCfg.PMUs[pmuIndex];
		const C37118AnalogUnit& angUnit = pmuCfg.AnalogUnit[analogIndex];

		memset(analogCfg->name, 0, 256);
		strncpy(analogCfg->name, pmuCfg.analogChnNames[analogIndex].c_str(), min(pmuCfg.analogChnNames[analogIndex].length(), 255) );
		analogCfg->Type = angUnit.Type_X;
		analogCfg->dataIsScaled = false;
		analogCfg->userdefined_scalar = angUnit.AnalogScalar;

		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}


STRONGRIDIEEEC37118DLL_API int getAnalogConfig_Ver3( analogConfig_Ver3 *analogCfg, int32_t pseudoPdcId, int32_t pmuIndex, int32_t analogIndex)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		const C37118PdcConfiguration_Ver3& pdcCfg = s_pdcClientMap[pseudoPdcId]->GetPdcConfigurationVer3();
		const C37118PmuConfiguration_Ver3& pmuCfg = pdcCfg.PMUs[pmuIndex];
		const C37118AnalogScale_Ver3& angScale = pmuCfg.AnalogScales[analogIndex];

		memset(analogCfg->name, 0, 256);
		strncpy(analogCfg->name, pmuCfg.analogChnNames[analogIndex].c_str(), min(pmuCfg.analogChnNames[analogIndex].length(), 255) );
		analogCfg->dataIsScaled = false;
		analogCfg->scaling_magnitude = angScale.Scale;
		analogCfg->scaling_offset = angScale.Offset;

		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}


STRONGRIDIEEEC37118DLL_API int getDigitalConfig(  digitalConfig* digitalCfg, int32_t pseudoPdcId, int32_t pmuIndex, int32_t digitalIndex )
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		const int unitWordIdx = digitalIndex / 16;

		const C37118PdcConfiguration& pdcCfg = s_pdcClientMap[pseudoPdcId]->GetPdcConfiguration();
		const C37118PmuConfiguration& pmuCfg = pdcCfg.PMUs[pmuIndex];
		const C37118DigitalUnit& digUnit = pmuCfg.DigitalUnit[unitWordIdx];
			
		memset(digitalCfg->name, 0, 256);
		strncpy(digitalCfg->name, pmuCfg.digitalChnNames[digitalIndex].c_str(), min(pmuCfg.digitalChnNames[digitalIndex].length(), 255) ); 

		bool normBit, validBit;
		digUnit.BitAtIdx(digitalIndex % 16, &normBit, &validBit);
		digitalCfg->normalBit = normBit;
		digitalCfg->isValidBit = validBit;
		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}

STRONGRIDIEEEC37118DLL_API int getDigitalConfig_Ver3(  digitalConfig* digitalCfg, int32_t pseudoPdcId, int32_t pmuIndex, int32_t digitalIndex )
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		const int unitWordIdx = digitalIndex / 16;

		const C37118PdcConfiguration_Ver3& pdcCfg = s_pdcClientMap[pseudoPdcId]->GetPdcConfigurationVer3();
		const C37118PmuConfiguration_Ver3& pmuCfg = pdcCfg.PMUs[pmuIndex];
		const C37118DigitalUnit& digUnit = pmuCfg.DigitalUnits[unitWordIdx];
			
		memset(digitalCfg->name, 0, 256);
		strncpy(digitalCfg->name, pmuCfg.digitalChnNames[digitalIndex].c_str(), min(pmuCfg.digitalChnNames[digitalIndex].length(), 255) ); 

		bool normBit, validBit;
		digUnit.BitAtIdx(digitalIndex % 16, &normBit, &validBit);
		digitalCfg->normalBit = normBit;
		digitalCfg->isValidBit = validBit;

		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}


STRONGRIDIEEEC37118DLL_API int getPdcRealData(pdcDataFrame* rd, int32_t pseudoPdcId)
{	
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		const C37118PdcDataDecodeInfo& decodeInfo = s_pdcClientMap[pseudoPdcId]->GetDecodeInfo();
		const C37118PdcDataFrame& dataframe = s_pdcClientMap[pseudoPdcId]->GetPdcDataFrame();

		// PDC portion of realdata
		rd->TimeQuality = GetClockStatus(dataframe.HeaderCommon.FracSec);
		rd->Timestamp = GetParsedTimestamp(dataframe.HeaderCommon.SOC, dataframe.HeaderCommon.FracSec.FractionOfSecond, decodeInfo.timebase.TimeBase, &rd->SecondOfCentury);	
		rd->NumPmuInDataFrame = dataframe.pmuDataFrame.size();

		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}


STRONGRIDIEEEC37118DLL_API int getPmuRealData(pmuDataFrame* rd, PmuStatus* rdsts, int32_t pseudoPdcId, int32_t pmuIndex)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		const C37118PdcDataDecodeInfo& pdcDecodeInfo = s_pdcClientMap[pseudoPdcId]->GetDecodeInfo();
		const C37118PmuDataDecodeInfo& pmuDecodeInfo = pdcDecodeInfo.PMUs[pmuIndex];
		const C37118PdcDataFrame& dataframe = s_pdcClientMap[pseudoPdcId]->GetPdcDataFrame();
		const C37118PmuDataFrame& pmuDataFrame = dataframe.pmuDataFrame[pmuIndex];

		// frequency / delta-frequency
		rd->frequency = dataframe.pmuDataFrame[pmuIndex].Frequency;
		rd->deltaFrequency = dataframe.pmuDataFrame[pmuIndex].DeltaFrequency;

		// Stat - field
		rdsts->dataErrorCode = pmuDataFrame.Stat.getDataError();
		rdsts->pmuSyncFlag = pmuDataFrame.Stat.getPmuSyncFlag();
		rdsts->pmuDataSortingFlag = pmuDataFrame.Stat.getDataSortingFlag();
		rdsts->pmuTriggerFlag = pmuDataFrame.Stat.getPmuTriggerFlag();
		rdsts->configChangeFlag = pmuDataFrame.Stat.getConfigChangeFlag();
		rdsts->dataModifiedFlag = pmuDataFrame.Stat.getDataModifiedFlag();
		rdsts->timeQualityCode = pmuDataFrame.Stat.getTimeQualityCode();
		rdsts->unlockTimeCode = pmuDataFrame.Stat.getUnlockTimeCode();
		rdsts->triggerReasonCode = pmuDataFrame.Stat.getTriggerReasonCode();

		// Phasors
		rd->PhasorArrayLength = pmuDecodeInfo.numPhasors;
		for( int i = 0; i < dataframe.pmuDataFrame[pmuIndex].PhasorValues.size(); ++i ) {
			rd->phasorValueReal[i] = dataframe.pmuDataFrame[pmuIndex].PhasorValues[i].Real;
			rd->phasorValueImaginary[i] = dataframe.pmuDataFrame[pmuIndex].PhasorValues[i].Imag;
		}
		
		// Analog
		rd->AnalogArrayLength = pmuDecodeInfo.numAnalogs;
		for( int i = 0; i < dataframe.pmuDataFrame[pmuIndex].AnalogValues.size(); ++i )
			rd->analogValueArr[i] = dataframe.pmuDataFrame[pmuIndex].AnalogValues[i].getValueAsFloat();

		// Digital
		rd->DigitalArrayLength = pmuDecodeInfo.numDigitals;
		for( int i = 0; i < dataframe.pmuDataFrame[pmuIndex].DigitalValues.size(); ++i )
			rd->digitalValueArr[i] = dataframe.pmuDataFrame[pmuIndex].DigitalValues[i] ? 1 : 0;
		
		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}


STRONGRIDIEEEC37118DLL_API int getHeaderMsg( char* msg, int maxMsgLength, int32_t pseudoPdcId)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		const C37118PdcHeaderFrame& hdr = s_pdcClientMap[pseudoPdcId]->GetPdcHeaderFrame();

		memset(msg, 0, maxMsgLength);
		strncpy(msg, hdr.HeaderMessage.c_str(), min(maxMsgLength, hdr.HeaderMessage.length()));	msg[maxMsgLength - 1] = 0;
		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}



// ------------------------------------------------------------------------------------------------------------------------------------
// ------------- LABVIEW SPECIFIC FUNCTIONS / NO ARRAYS WITHIN STRUCTURES
// ------------------------------------------------------------------------------------------------------------------------------------

STRONGRIDIEEEC37118DLL_API int __cdecl getPmuRealDataLabview(noArraysPmuDataFrame* rd, PmuStatus* rdsts,
															uint16_t PhasorArrayLength, float* phasorValueReal, float* phasorValueImaginary,
															uint16_t AnalogArrayLength, float* analogValueArr,
															uint16_t DigitalArrayLength, uint8_t* digitalValueArr,
															int32_t pseudoPdcId, int32_t pmuIndex)
{
	if( PseudoPdcIdIsValidClient(pseudoPdcId) == false) return RETERR_UNKNOWN_ERR;

	try {
		const C37118PdcDataDecodeInfo& pdcDecodeInfo = s_pdcClientMap[pseudoPdcId]->GetDecodeInfo();
		const C37118PmuDataDecodeInfo& pmuDecodeInfo = pdcDecodeInfo.PMUs[pmuIndex];
		const C37118PdcDataFrame& dataframe = s_pdcClientMap[pseudoPdcId]->GetPdcDataFrame();
		const C37118PmuDataFrame& pmuDataFrame = dataframe.pmuDataFrame[pmuIndex];

		// Validate input arrays
		if( PhasorArrayLength < pmuDecodeInfo.numPhasors ) return RETERR_INVALID_INPUT_PHASOR_ARR;
		if( AnalogArrayLength < pmuDecodeInfo.numAnalogs ) return RETERR_INVALID_INPUT_ANALOG_ARR;
		if( DigitalArrayLength < pmuDecodeInfo.numDigitals ) return RETERR_INVALID_INPUT_DIGITAL_ARR;

		// frequency / delta-frequency
		rd->frequency = dataframe.pmuDataFrame[pmuIndex].Frequency;
		rd->deltaFrequency = dataframe.pmuDataFrame[pmuIndex].DeltaFrequency;

		// Stat - field
		rdsts->dataErrorCode = pmuDataFrame.Stat.getDataError();
		rdsts->pmuSyncFlag = pmuDataFrame.Stat.getPmuSyncFlag();
		rdsts->pmuDataSortingFlag = pmuDataFrame.Stat.getDataSortingFlag();
		rdsts->pmuTriggerFlag = pmuDataFrame.Stat.getPmuTriggerFlag();
		rdsts->configChangeFlag = pmuDataFrame.Stat.getConfigChangeFlag();
		rdsts->dataModifiedFlag = pmuDataFrame.Stat.getDataModifiedFlag();
		rdsts->timeQualityCode = pmuDataFrame.Stat.getTimeQualityCode();
		rdsts->unlockTimeCode = pmuDataFrame.Stat.getUnlockTimeCode();
		rdsts->triggerReasonCode = pmuDataFrame.Stat.getTriggerReasonCode();

		// Phasors
		for( int i = 0; i < dataframe.pmuDataFrame[pmuIndex].PhasorValues.size(); ++i ) {
			phasorValueReal[i] = dataframe.pmuDataFrame[pmuIndex].PhasorValues[i].Real;
			phasorValueImaginary[i] = dataframe.pmuDataFrame[pmuIndex].PhasorValues[i].Imag;
		}
		
		// Analog
		for( int i = 0; i < dataframe.pmuDataFrame[pmuIndex].AnalogValues.size(); ++i )
			analogValueArr[i] = dataframe.pmuDataFrame[pmuIndex].AnalogValues[i].getValueAsFloat();

		// Digital
		for( int i = 0; i < dataframe.pmuDataFrame[pmuIndex].DigitalValues.size(); ++i )
			digitalValueArr[i] = dataframe.pmuDataFrame[pmuIndex].DigitalValues[i] ? 1 : 0;
		
		return RETERR_OK;
	}
	catch( ... )
	{
		return RETERR_UNKNOWN_ERR;
	}
}



STRONGRIDIEEEC37118DLL_API int __cdecl getPmuConfigurationLabview(noArraysPmuConfig* pmuconf, char* stationName, int32_t pseudoPdcId, int32_t pmuIndex)
{
	pmuConfig tmpConfig; tmpConfig.stationname = stationName;
	int retval = getPmuConfiguration(&tmpConfig, pseudoPdcId, pmuIndex);

	pmuconf->nominalFrequency = tmpConfig.nominalFrequency;
	pmuconf->numberOfAnalog = tmpConfig.numberOfAnalog;
	pmuconf->numberOfDigital = tmpConfig.numberOfDigital;
	pmuconf->numberOfPhasors = tmpConfig.numberOfPhasors;
	pmuconf->pmuid = tmpConfig.pmuid;

	return retval;
}

STRONGRIDIEEEC37118DLL_API int __cdecl getPhasorConfigLabview(noArraysPhasorConfig *phasorCfg, char* name, int32_t pseudoPdcId, int32_t pmuIndex, int32_t phasorIndex)
{
	phasorConfig tmpCfg; tmpCfg.name = name;
	int retval = getPhasorConfig(&tmpCfg, pseudoPdcId, pmuIndex, phasorIndex);

	phasorCfg->dataIsScaled = tmpCfg.dataIsScaled;
	phasorCfg->format = tmpCfg.format;
	phasorCfg->scalar = tmpCfg.scalar;
	phasorCfg->type = tmpCfg.type;

	return retval;
}
 
STRONGRIDIEEEC37118DLL_API int __cdecl getAnalogConfigLabview(noArraysAnalogConfig *analogCfg, char* name, int32_t pseudoPdcId, int32_t pmuIndex, int32_t analogIndex)
{
	analogConfig tmpCfg; tmpCfg.name = name;
	int retval = getAnalogConfig(&tmpCfg, pseudoPdcId, pmuIndex, analogIndex);

	analogCfg->dataIsScaled = tmpCfg.dataIsScaled;
	analogCfg->Type = tmpCfg.Type;
	analogCfg->userdefined_scalar = tmpCfg.userdefined_scalar;

	return retval;
}

STRONGRIDIEEEC37118DLL_API int __cdecl getDigitalConfigLabview(noArraysDigitalConfig* digitalCfg, char* name, int32_t pseudoPdcId, int32_t pmuIndex, int32_t digitalIndex )
{
	digitalConfig tmpCfg; tmpCfg.name = name;
	int retval = getDigitalConfig(&tmpCfg, pseudoPdcId, pmuIndex, digitalIndex);

	digitalCfg->isValidBit = tmpCfg.isValidBit;
	digitalCfg->normalBit = tmpCfg.normalBit;

	return retval;
}



