#pragma once

#include "stdafx.h"
#include <map>
#include "SimulatorDataTypes.h"
#include "../StrongridSimBase/PDC.h"
#include "BridgedPdcDataFeed.h"
#include "BridgedPmuDataFeed.h"
#include "OutputWnd.h"

using namespace strongridsim;

namespace strongridgui 
{	
	class ManagedPdcSimulator;

	class PdcEventToLogoutput
	{
	public:
		PdcEventToLogoutput(COutputWnd* outputWnd, ManagedPdcSimulator* pdcsim );

		static void onEvtSimulatorStarted(void* thisptr, void* pdc);
		static void onEvtSimulatorStopped(void* thisptr, void* pdc);
		static void onEvtClientConnected(void* thisptr, void* pmuClient);
		static void onEvtClientDisconnected(void* thisptr, void* pmuClient);

	private:
		void WriteToOutput(CString cstr);

	private:
		COutputWnd* m_outputWindow;
		ManagedPdcSimulator* m_pdcSim;
	};

	class ManagedPdcSimulator
	{
	public:
		ManagedPdcSimulator( CString simname ); // create a new simulator with default values
		static ManagedPdcSimulator* CreateByByArchive(CArchive& ar); // Read from file..	
		~ManagedPdcSimulator();
		void SetEventHandlerLink(PdcEventToLogoutput* eventlink);

		CString SimulatorProjectName() const;
		void SetSimulatorProjectName(CString projectName);
		void StartSimulator();
		void StopSimulator();
		bool SimulatorIsActive() const;

		// Serialize
		void Serialize(CArchive& archive); // Serialize into archive

		void StartRealtimeData();
		void StopRealtimeData();
		void SendConfiguration();
		void SetConfiguration();

		void setDataFeed( IPdcDataFeed* own_datafeed );
		CPdcConfig* getConfigurationRef();
		CPmuConfig* AppendDefaultPMU();
		void RemovePmuAtIndex(int pmuidx);
		BridgedPmuDataFeed* GetPmuDataFeed( int pmuIdx );
		void SetPdcDataRate(int dr);
	
	private:
		int m_pmuIdCounter;
		CString m_simName;
		PDC* m_simPdc;
		CPdcConfig m_pdcConfig; // Serializable pdc configuration
		BridgedPdcDataFeed* m_bridgedPdcFeed;
		std::vector<BridgedPmuDataFeed*> m_bridgedPmuFeedMap; // pmu datafeed | parallell to the PMU array in m_pdcConfig
		PdcEventToLogoutput* m_eventLink;
	};
}