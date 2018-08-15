#include "stdafx.h"
#include "BridgedPmuDataFeed.h"

using namespace strongridgui;

BridgedPmuDataFeed::BridgedPmuDataFeed(IPmuDataFeed* own_pmuDataFeed)
{
	m_pmuDataFeed = own_pmuDataFeed;
	
	m_masterFrequency = 0; 
	m_masterDeltaFrequency = 0;
}

BridgedPmuDataFeed::~BridgedPmuDataFeed()
{
	delete m_pmuDataFeed;
}

// Set new PMU datafeed - the old one is deleted 
void BridgedPmuDataFeed::SetPmuDataFeed( IPmuDataFeed* ownDatafeed )
{
	delete m_pmuDataFeed;
	m_pmuDataFeed = ownDatafeed;
}

// Set "override" values "per signal"
void BridgedPmuDataFeed::SetPhasorOverride( int phasorIndex, IPhasorDataFeed* phDataFeed )
{
	delete m_phasorDataFeedMap[phasorIndex];
	m_phasorDataFeedMap[phasorIndex] = phDataFeed;
}
void BridgedPmuDataFeed::SetAnalogOverride( int analogIndex, IAnalogDataFeed* angDataFeed )
{
	delete m_analogDataFeedMap[analogIndex];
	m_analogDataFeedMap[analogIndex] = angDataFeed;
}
void BridgedPmuDataFeed::SetDigitalOverride( int digitalIdx, IDigitalDataFeed* digDataFeed )
{
	delete m_digitalDataFeedMap[digitalIdx];
	m_digitalDataFeedMap[digitalIdx] = digDataFeed;
}


void BridgedPmuDataFeed::SetFrequency( float freq, float dfreq )
{
	m_masterFrequency = freq;
	m_masterDeltaFrequency = dfreq;
}


IPhasorDataFeed* BridgedPmuDataFeed::GetPhasorOverride( int phasorIndex )
{
	return m_phasorDataFeedMap[phasorIndex];
}

IAnalogDataFeed* BridgedPmuDataFeed::GetAnalogOverride( int analogIndex )
{
	return m_analogDataFeedMap[analogIndex];
}

IDigitalDataFeed* BridgedPmuDataFeed::GetDigitalOverride( int digitalIndex )
{
	return m_digitalDataFeedMap[digitalIndex];
}


void BridgedPmuDataFeed::GetPhasorValue( int phIdx, float* part1, float* part2 )
{
	if( m_phasorDataFeedMap[phIdx] != 0 ) m_phasorDataFeedMap[phIdx]->GetPhasorValue(part1, part2);
	else m_pmuDataFeed->GetPhasorValue(phIdx, part1, part2);
}

float BridgedPmuDataFeed::GetAnalogValue( int angIdx )
{	
	if( m_analogDataFeedMap[angIdx] != 0 ) {
		float tmp;
		m_analogDataFeedMap[angIdx]->GetAnalogValue(&tmp);
		return tmp;
	}
	else return m_pmuDataFeed->GetAnalogValue(angIdx);
}

bool BridgedPmuDataFeed::GetDigitalBit( int digIdx )
{
	if( m_digitalDataFeedMap[digIdx] != 0 )
		return m_digitalDataFeedMap[digIdx]->GetDigitalStatus();
	else
		return m_pmuDataFeed->GetDigitalBit(digIdx);
}

void BridgedPmuDataFeed::GetFrequency( float* frequency, float* deltafrequency )
{
	*frequency = m_masterFrequency;
	*deltafrequency = m_masterDeltaFrequency;
}


C37118PmuDataFrameStat BridgedPmuDataFeed::GetFrameStat()
{
	return m_masterFrameStat;
}

C37118PmuDataFrameStat* BridgedPmuDataFeed::GetFrameStatRef()
{
	return &m_masterFrameStat;
}
		
void BridgedPmuDataFeed::SignalNextFrame()
{
	// Signal all phasor/analog/digital overrides
	for( std::map<int,IPhasorDataFeed*>::iterator iter = m_phasorDataFeedMap.begin(); iter != m_phasorDataFeedMap.end(); ++iter )
		if( iter->second != 0 ) iter->second->SignalNextFrame();
	for( std::map<int,IAnalogDataFeed*>::iterator iter = m_analogDataFeedMap.begin(); iter != m_analogDataFeedMap.end(); ++iter )
		if( iter->second != 0 ) iter->second->SignalNextFrame();
	for( std::map<int,IDigitalDataFeed*>::iterator iter = m_digitalDataFeedMap.begin(); iter != m_digitalDataFeedMap.end(); ++iter )
		if( iter->second != 0 ) iter->second->SignalNextFrame();

	// Signal next frame
	m_pmuDataFeed->SignalNextFrame();
}