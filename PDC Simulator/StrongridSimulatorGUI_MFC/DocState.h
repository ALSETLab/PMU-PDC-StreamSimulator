#pragma once

#include "stdafx.h"
#include "PdcSimulatorManager.h"

using namespace strongridgui;

struct NewDocState
{
	NewDocState()
	{
		DocType = 0;
		PdcSimulatorID = 0;
		PmuIndex = 0;
		PhasorIndex = AnalogIndex = DigitalIndex = 0;
	}

	static NewDocState CreateAsPmuId( PdcSimulatorID pdcSimid, int pmuidx )
	{
		NewDocState tmp;
		tmp.DocType = 2;
		tmp.PdcSimulatorID = pdcSimid;
		tmp.PmuIndex = pmuidx;
		return tmp;
	}

	static NewDocState CreateAsPhasorId( PdcSimulatorID pdcSimId, int pmuIdx, int phasorIdx )
	{
		NewDocState tmp;
		tmp.DocType = 3;
		tmp.PdcSimulatorID = pdcSimId;
		tmp.PmuIndex = pmuIdx;
		tmp.PhasorIndex = phasorIdx;
		return tmp;
	}
	
	static NewDocState CreateAsAnalogId( PdcSimulatorID pdcSimId, int pmuIdx, int analogid )
	{
		NewDocState tmp;
		tmp.DocType = 4;
		tmp.PdcSimulatorID = pdcSimId;
		tmp.PmuIndex = pmuIdx;
		tmp.AnalogIndex = analogid;
		return tmp;
	}

	static NewDocState CreateAsDigitalId( PdcSimulatorID pdcSimId, int pmuIdx, int digitalid )
	{
		NewDocState tmp;
		tmp.DocType = 5;
		tmp.PdcSimulatorID = pdcSimId;
		tmp.PmuIndex = pmuIdx;
		tmp.DigitalIndex = digitalid;
		return tmp;
	}

	// Document types:
	//  0) Invalid
	//  1) PDC 
	//  2) PMU
	//  3) Phasor
	//  4) Analog
	//  5) Digital
	int DocType; 
	PdcSimulatorID PdcSimulatorID; 

	// Individual indices used to uniquely identify the object / -1 if not present
	int PmuIndex;
	int PhasorIndex;
	int AnalogIndex;
	int DigitalIndex; 

	bool operator<(const NewDocState& n) const
	{
		if( PdcSimulatorID < n.PdcSimulatorID ) return true;
		if( PdcSimulatorID == n.PdcSimulatorID && PmuIndex < n.PmuIndex ) return true;
		if( DocType == 3 && PdcSimulatorID == n.PdcSimulatorID && PmuIndex == n.PmuIndex ) return PhasorIndex < n.PhasorIndex;
		if( DocType == 4 && PdcSimulatorID == n.PdcSimulatorID && PmuIndex == n.PmuIndex ) return AnalogIndex < n.AnalogIndex;
		if( DocType == 5 && PdcSimulatorID == n.PdcSimulatorID && PmuIndex == n.PmuIndex ) return DigitalIndex < n.DigitalIndex;
		
		return false;
	}
	bool operator>(const NewDocState& n) const
	{
		if( PdcSimulatorID > n.PdcSimulatorID ) return true;
		if( PdcSimulatorID == n.PdcSimulatorID && PmuIndex > n.PmuIndex ) return true;
		if( DocType == 3 && PdcSimulatorID == n.PdcSimulatorID && PmuIndex == n.PmuIndex ) return PhasorIndex > n.PhasorIndex;
		if( DocType == 4 && PdcSimulatorID == n.PdcSimulatorID && PmuIndex == n.PmuIndex ) return AnalogIndex > n.AnalogIndex;
		if( DocType == 5 && PdcSimulatorID == n.PdcSimulatorID && PmuIndex == n.PmuIndex ) return DigitalIndex > n.DigitalIndex;
		
		return false;
	}
	bool operator=(const NewDocState& n) const
	{
		return PdcSimulatorID == n.PdcSimulatorID &&
			PmuIndex == n.PmuIndex &&
			PhasorIndex == n.PhasorIndex &&
			AnalogIndex == n.AnalogIndex &&
			DigitalIndex == n.DigitalIndex;
	}
};