#include "stdafx.h"
#include "../StrongridSimBase/PDC.h"
#include "ManagedPdcSimulator.h"
#include "C37118AdaptationLayer.h"
#include "SimpleConstantDataFeed.h"
#include "../StrongridSimBase/PDC.h"


using namespace strongridgui;

PdcEventToLogoutput::PdcEventToLogoutput(COutputWnd* outputWnd, ManagedPdcSimulator* pdcsim)
{
	m_outputWindow = outputWnd;
	m_pdcSim = pdcsim;
}

void PdcEventToLogoutput::onEvtSimulatorStarted(void* thisptr, void* na)
{
	PdcEventToLogoutput* t = (PdcEventToLogoutput*)thisptr;
	CString tmp; tmp.Format(_T("Simulator with PDCID=%d has started"), t->m_pdcSim->getConfigurationRef()->SG_PdcId);
	t->WriteToOutput(tmp);
}

void PdcEventToLogoutput::onEvtSimulatorStopped(void* thisptr, void* na)
{
	PdcEventToLogoutput* t = (PdcEventToLogoutput*)thisptr;
	CString tmp; tmp.Format(_T("Simulator with PDCID=%d has stopped"), t->m_pdcSim->getConfigurationRef()->SG_PdcId);
	t->WriteToOutput(tmp);
}

void PdcEventToLogoutput::onEvtClientConnected(void* thisptr, void* client)
{
	PdcEventToLogoutput* t = (PdcEventToLogoutput*)thisptr;
	PdcClient* pdcClient = (PdcClient*)client;
	CString tmp; tmp.Format(_T("A client has connected to PDCID=%d from %s"), 
		t->m_pdcSim->getConfigurationRef()->SG_PdcId,
		CString(pdcClient->BaseSocket()->GetRemoteAddress().c_str()) );
	t->WriteToOutput(tmp);

}

void PdcEventToLogoutput::onEvtClientDisconnected(void* thisptr, void* client)
{
	PdcEventToLogoutput* t = (PdcEventToLogoutput*)thisptr;
	PdcClient* pdcClient = (PdcClient*)client;
	CString tmp; tmp.Format(_T("A client has disconnected from PDCID=%d (%s)"), 
		t->m_pdcSim->getConfigurationRef()->SG_PdcId,
		CString(pdcClient->BaseSocket()->GetRemoteAddress().c_str()) );
	t->WriteToOutput(tmp);
}

void PdcEventToLogoutput::WriteToOutput(CString cstr)
{
	m_outputWindow->PushOutputMessage(cstr);
}

// -------------------- ManagedPdcSimulator ------------------------

void SetDefaultValues_PDC( CPdcConfig* m_pdcConfig )
{
	// Initialize default PDC configuration
	m_pdcConfig->SG_PdcId = 1;
	m_pdcConfig->SG_ListenPort = 4712;
	m_pdcConfig->SG_TimeBase = 100;
	m_pdcConfig->SG_DataRate_FramesPerSecond = 50;
}

ManagedPdcSimulator::ManagedPdcSimulator(CString simName)
{
	m_eventLink  = 0;
	m_pmuIdCounter = 0;
	m_simName = simName;
	m_bridgedPdcFeed = new BridgedPdcDataFeed( new SimpleConstantPdcDataFeed() );
	SetDefaultValues_PDC(&m_pdcConfig);
}

ManagedPdcSimulator::~ManagedPdcSimulator(void)
{
	// Shutdown PDC if running
	StopSimulator();

	delete m_bridgedPdcFeed;
	delete m_eventLink;

	// Delete all PMU data-feeds
	for( std::vector<BridgedPmuDataFeed*>::iterator iter = m_bridgedPmuFeedMap.begin(); iter != m_bridgedPmuFeedMap.end(); ++iter )
		delete *iter;
	m_bridgedPmuFeedMap.clear();
}

void ManagedPdcSimulator::SetEventHandlerLink(PdcEventToLogoutput* eventlink)
{
	delete m_eventLink;
	m_eventLink = eventlink;
}

CString ManagedPdcSimulator::SimulatorProjectName() const
{
	return m_simName;
}

void ManagedPdcSimulator::SetSimulatorProjectName(CString projectName)
{
	m_simName = projectName;
}

BridgedPmuDataFeed* GetPmuDataFeedOrCreateDefault( std::vector<BridgedPmuDataFeed*>* pmuMap, int pmuIndex )
{
	while( pmuMap->size() <= pmuIndex )
		pmuMap->push_back( new BridgedPmuDataFeed(new SimpleConstantPmuDataFeed()) );

	return (*pmuMap)[pmuIndex];
}

void ManagedPdcSimulator::StartSimulator()
{
	// If an instance already exist - recycle it
	if( m_simPdc != 0 ) {
		m_simPdc->Shutdown();
		delete m_simPdc; m_simPdc = 0;
	}

	C37118PdcHeaderFrame header;
	header.HeaderMessage = "This is a header!";

	// Translate current serializable PDC config into C37118 pdc configuration
	C37118PdcConfiguration pdcConfig = C37118AdaptorUtils::Translate(m_pdcConfig);

	// Create one PMU per pmu object in pdcConfic
	std::vector<PMU*> pmuList;
	for( int i = 0; i < pdcConfig.PMUs.size(); ++i )
		pmuList.push_back( new PMU( pdcConfig.PMUs[i], GetPmuDataFeedOrCreateDefault(&m_bridgedPmuFeedMap, i)));

	// Create the simulator
	m_simPdc = new PDC(pdcConfig, header, m_pdcConfig.SG_ListenPort, pmuList, m_bridgedPdcFeed);

	// rig up event handlers
	if( m_eventLink != NULL )
	{
		m_simPdc->onPdcStarted.AddHandler(m_eventLink->onEvtSimulatorStarted, m_eventLink);
		m_simPdc->onPdcStopped.AddHandler(m_eventLink->onEvtSimulatorStopped, m_eventLink);
		m_simPdc->onClientConnected.AddHandler(m_eventLink->onEvtClientConnected, m_eventLink);
		m_simPdc->onClientDropped.AddHandler(m_eventLink->onEvtClientDisconnected, m_eventLink);
	}

	// Start it
	m_simPdc->Start();
	m_simPdc->SetDataRate( m_pdcConfig.SG_DataRate_FramesPerSecond );
}

void ManagedPdcSimulator::StopSimulator()
{
	if( m_simPdc != 0 )
	{
		m_simPdc->Shutdown();
		delete m_simPdc;
		m_simPdc = 0;
	}
}


bool ManagedPdcSimulator::SimulatorIsActive() const
{
	return m_simPdc != 0;
}

// Deserialize 
ManagedPdcSimulator* ManagedPdcSimulator::CreateByByArchive(CArchive& ar)
{	
	uint32_t tmp;
	ar >> tmp;
	if( tmp != 0xAA ) throw; // out of sync

	CString simulatorName;
	ar >> simulatorName;

	ManagedPdcSimulator* pdcSim = new ManagedPdcSimulator(simulatorName);
	try {
		pdcSim->m_pdcConfig.Serialize(ar);

		// Create datafeed objects for all PMU's
		for( int i = pdcSim->m_bridgedPmuFeedMap.size(); i < pdcSim->m_pdcConfig.SG_PmuConfigArray.items.size(); ++i )
			pdcSim->m_bridgedPmuFeedMap.push_back( new BridgedPmuDataFeed( new SimpleConstantPmuDataFeed() ) );
	}
	catch(...)
	{
		delete pdcSim;
		throw;
	}

	return pdcSim;
}

// Serialize
void ManagedPdcSimulator::Serialize(CArchive& ar)
{
	if( ar.IsLoading() ) throw;

	ar << (uint32_t)0xAA;
	ar << m_simName;
	m_pdcConfig.Serialize(ar);
}


void ManagedPdcSimulator::StartRealtimeData()
{
	m_simPdc->StartRealtimeDataAll();
}

void ManagedPdcSimulator::StopRealtimeData()
{
	m_simPdc->StopRealtimeDataAll();
}

void ManagedPdcSimulator::SendConfiguration()
{
	m_simPdc->SendConfigurationAll();
}


void ManagedPdcSimulator::SetConfiguration()
{
	// Translate current serializable PDC config into C37118 pdc configuration
	C37118PdcConfiguration pdcConfig = C37118AdaptorUtils::Translate(m_pdcConfig);

	m_simPdc->SetConfiguration(pdcConfig);
}


void ManagedPdcSimulator::setDataFeed( IPdcDataFeed* own_datafeed )
{
	m_bridgedPdcFeed->SetPdcDataFeed(own_datafeed);
}

CPdcConfig* ManagedPdcSimulator::getConfigurationRef()
{
	return &m_pdcConfig;
}

CPmuConfig* ManagedPdcSimulator::AppendDefaultPMU()
{
	CPmuConfig* pmu1 = new CPmuConfig();
	m_pdcConfig.SG_PmuConfigArray.items.push_back( pmu1 );

	// Initialize default PMU configuration (pmu1)
	pmu1->SG_PmuStationName = "station name";
	pmu1->SG_PmuIdCode = ++m_pmuIdCounter;
	pmu1->SG_PmuFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle = 0; // phasor format: rectangular
	pmu1->SG_PmuFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat = 1; // Phasor unit: float
	pmu1->SG_PmuFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat = 1; // analog unit: float
	pmu1->SG_PmuFormat.Bit3_0xFreqIsInt_1xFreqIsFloat = 1; // Freq is float

	// Phasor names
	pmu1->SG_PhasorNames.items.push_back( _T("Phasor1" ));
	pmu1->SG_PhasorNames.items.push_back( _T("Phasor2" ));
	pmu1->SG_PhasorNames.items.push_back( _T("Phasor3" ));

	// Analog names
	pmu1->SG_AnalogNames.items.push_back( _T("Analog1"));
	pmu1->SG_AnalogNames.items.push_back( _T("Analog2"));
	
	// Digital names
	pmu1->SG_DigitalNames.items.push_back( _T("Dig1"));
	pmu1->SG_DigitalNames.items.push_back( _T("Dig2"));
	pmu1->SG_DigitalNames.items.push_back( _T("Dig3"));
	pmu1->SG_DigitalNames.items.push_back( _T("Dig4"));

	// Phasor unit
	pmu1->SG_PhasorUnitArray.items.push_back( new CPhasorUnit(1,0) );
	pmu1->SG_PhasorUnitArray.items.push_back( new CPhasorUnit(1,0) );
	pmu1->SG_PhasorUnitArray.items.push_back( new CPhasorUnit(1,0) );

	// Analog unit
	pmu1->SG_AnalogUnitArray.items.push_back( new CAnalogUnit(1,0) );
	pmu1->SG_AnalogUnitArray.items.push_back( new CAnalogUnit(1,0) );	

	// Digital unit
	pmu1->SG_DigitalUnitArray.items.push_back( new CDigitalUnitBit(true,false) );
	pmu1->SG_DigitalUnitArray.items.push_back( new CDigitalUnitBit(true,false) );
	pmu1->SG_DigitalUnitArray.items.push_back( new CDigitalUnitBit(true,false) );
	pmu1->SG_DigitalUnitArray.items.push_back( new CDigitalUnitBit(false,false) );

	// Set change - count
	pmu1->SG_FreqCode = 0;
	pmu1->SG_ChangeCnt = 0;

	// Create a default bridged datafeed
	m_bridgedPmuFeedMap.push_back( new BridgedPmuDataFeed( new SimpleConstantPmuDataFeed() ) );

	return pmu1;
}

void ManagedPdcSimulator::RemovePmuAtIndex(int pmuidx)
{
	if( pmuidx > m_pdcConfig.SG_PmuConfigArray.items.size() ) throw;

	// Get an iterator to the pmu based on its index
	std::vector<CPmuConfig*>::iterator iter = m_pdcConfig.SG_PmuConfigArray.items.begin();
	for( int i = 0; i < pmuidx; ++i ) iter++; // Advance iterator to 'pmu index'

	// Delete the item
	m_pdcConfig.SG_PmuConfigArray.items.erase(iter);
}


BridgedPmuDataFeed* ManagedPdcSimulator::GetPmuDataFeed( int pmuIdx )
{
	if( pmuIdx > m_bridgedPmuFeedMap.size() ) throw;	
	return m_bridgedPmuFeedMap[pmuIdx];

}


void ManagedPdcSimulator::SetPdcDataRate(int dr)
{
	if( SimulatorIsActive() && m_simPdc != 0 )
		m_simPdc->SetDataRate(dr);
}