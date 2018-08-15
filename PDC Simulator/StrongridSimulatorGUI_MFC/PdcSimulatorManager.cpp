#include "stdafx.h"
#include "Common.h"
#include "PdcSimulatorManager.h"

using namespace strongridgui;

PdcSimulatorManager::PdcSimulatorManager(COutputWnd* wnd)
{
	m_idCounter = 0;
	m_outputWnd = wnd;
}


PdcSimulatorManager::~PdcSimulatorManager(void)
{
	for( std::map<PdcSimulatorID,ManagedPdcSimulator*>::iterator iter = m_pdcSimulatorMap.begin(); iter != m_pdcSimulatorMap.end(); ++iter )
		delete (*iter).second;
	m_pdcSimulatorMap.clear();
}

PdcSimulatorID PdcSimulatorManager::CreateNewPdcSimulator(CString simName)
{
	ManagedPdcSimulator* sim = new ManagedPdcSimulator(simName);
	sim->AppendDefaultPMU();
	EnlistSimulator(sim);
	sim->getConfigurationRef()->SG_PdcId = m_idCounter;
	return m_idCounter;
}

void PdcSimulatorManager::EnlistSimulator(ManagedPdcSimulator* pdcSim)
{
	m_pdcSimulatorMap[++m_idCounter] = pdcSim;
	PdcEventToLogoutput* eventLoggerLink = new PdcEventToLogoutput(m_outputWnd, pdcSim);
	pdcSim->SetEventHandlerLink(eventLoggerLink);
}

void PdcSimulatorManager::DestroyPdcSimulator(PdcSimulatorID pdcSim)
{
	std::map<PdcSimulatorID,ManagedPdcSimulator*>::iterator iter = m_pdcSimulatorMap.find(pdcSim);
	if( iter == m_pdcSimulatorMap.end() ) throw new SimException(_T("No PDC found with [id]"));
	delete iter->second;
	m_pdcSimulatorMap.erase(iter);
}

ManagedPdcSimulator* PdcSimulatorManager::GetPdcSimulator(PdcSimulatorID pdcSim)
{
	std::map<PdcSimulatorID,ManagedPdcSimulator*>::iterator iter = m_pdcSimulatorMap.find(pdcSim);
	if( iter == m_pdcSimulatorMap.end() ) throw new SimException(_T("No PDC found with [id]"));
	return iter->second;
}


std::vector<PdcSimulatorID> PdcSimulatorManager::EnumeratePdcSimulators() const
{
	std::vector<PdcSimulatorID> output;
	for( std::map<PdcSimulatorID,ManagedPdcSimulator*>::const_iterator iter = m_pdcSimulatorMap.begin(); iter != m_pdcSimulatorMap.end(); ++iter )
	{
		output.push_back(iter->first);
	}

	return output;
}
