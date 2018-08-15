#pragma once

#include <map>
#include <list>
#include "ManagedPdcSimulator.h"
#include "OutputWnd.h"

namespace strongridgui 
{	
	typedef int PdcSimulatorID;

	class PdcSimulatorManager
	{
	public:
		PdcSimulatorManager(COutputWnd* wnd);
		~PdcSimulatorManager();

		PdcSimulatorID CreateNewPdcSimulator(CString name);
		void EnlistSimulator(ManagedPdcSimulator* pdcsim);
		void DestroyPdcSimulator(PdcSimulatorID pdcSim);
		ManagedPdcSimulator* GetPdcSimulator(PdcSimulatorID pdcSim);
		std::vector<PdcSimulatorID> EnumeratePdcSimulators() const;

	private:
		int m_idCounter;
		std::map<PdcSimulatorID, ManagedPdcSimulator*> m_pdcSimulatorMap;
		COutputWnd* m_outputWnd;
	};
}