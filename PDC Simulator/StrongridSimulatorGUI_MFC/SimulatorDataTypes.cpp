#include "stdafx.h"
#include "SimulatorDataTypes.h"

using namespace strongridgui;

IMPLEMENT_SERIAL(CPdcConfig,CObject,1)
IMPLEMENT_SERIAL(CPmuConfig,CObject,1)
IMPLEMENT_SERIAL(CPhasorUnit,CObject,1)
IMPLEMENT_SERIAL(CAnalogUnit,CObject,1)
IMPLEMENT_SERIAL(CDigitalUnitBit,CObject,1)
IMPLEMENT_SERIAL(CPmuFormat,CObject,1)


void CPdcConfig::Serialize( CArchive& ar )
{
	CObject::Serialize( ar );    // Always call base class Serialize.

	// Serialize dynamic members and other raw data
	if ( ar.IsStoring() )
	{
		ar << SG_PdcId;
		ar << SG_ListenPort;
		ar << SG_TimeBase;
		ar << SG_DataRate_FramesPerSecond;
		SG_PmuConfigArray.Serialize(ar);
	}
	else
	{
		ar >> SG_PdcId;
		ar >> SG_ListenPort;
		ar >> SG_TimeBase; 
		ar >> SG_DataRate_FramesPerSecond;
		SG_PmuConfigArray.Serialize(ar);
	}
}

void CPmuConfig::Serialize( CArchive& ar )
{
	CObject::Serialize( ar );    // Always call base class Serialize.

	// Serialize dynamic members and other raw data
	if ( ar.IsStoring() )
	{
		ar << SG_PmuStationName;
		ar << SG_PmuStationName;
		ar << SG_PmuIdCode;
		SG_PmuFormat.Serialize(ar);
		ar << SG_ChangeCnt;

		SG_PhasorNames.Serialize(ar);
		SG_AnalogNames.Serialize(ar);
		SG_DigitalNames.Serialize(ar);

		SG_PhasorUnitArray.Serialize(ar);
		SG_AnalogUnitArray.Serialize(ar);
		SG_DigitalUnitArray.Serialize(ar);

		ar << SG_FreqCode;
	}
	else
	{
		ar >> SG_PmuStationName;
		ar >> SG_PmuStationName;
		ar >> SG_PmuIdCode;
		SG_PmuFormat.Serialize(ar);
		ar >> SG_ChangeCnt;

		SG_PhasorNames.Serialize(ar);
		SG_AnalogNames.Serialize(ar);
		SG_DigitalNames.Serialize(ar);

		SG_PhasorUnitArray.Serialize(ar);
		SG_AnalogUnitArray.Serialize(ar);
		SG_DigitalUnitArray.Serialize(ar);

		ar >> SG_FreqCode;
	}
}

void CPhasorUnit::Serialize( CArchive& ar )
{
	CObject::Serialize( ar );    // Always call base class Serialize.

	// Serialize dynamic members and other raw data
	if ( ar.IsStoring() )
	{
		ar << SG_Type;
		ar << SG_Scalar;
	}
	else
	{
		ar >> SG_Type;
		ar >> SG_Scalar;
	}
}



void CAnalogUnit::Serialize( CArchive& ar )
{
	CObject::Serialize( ar );    // Always call base class Serialize.

	// Serialize dynamic members and other raw data
	if ( ar.IsStoring() )
	{
		ar << SG_Type; 
		ar << SG_Scalar;
	}
	else
	{
		ar >> SG_Type;
		ar >> SG_Scalar;
	}
}



void CDigitalUnitBit::Serialize( CArchive& ar )
{
	CObject::Serialize( ar );    // Always call base class Serialize.

	// Serialize dynamic members and other raw data
	if ( ar.IsStoring() )
	{
		ar << SG_ValidInput;
		ar << SG_NormalState;
	}
	else
	{
		ar >> SG_ValidInput;
		ar >> SG_NormalState;
	}
}


void CPmuFormat::Serialize( CArchive& ar )
{
	CObject::Serialize( ar );    // Always call base class Serialize.

	// Serialize dynamic members and other raw data
	if ( ar.IsStoring() )
	{
		ar << Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle;
		ar << Bit1_0xPhasorsIsInt_1xPhasorFloat;
		ar << Bit2_0xAnalogIsInt_1xAnalogIsFloat;
		ar << Bit3_0xFreqIsInt_1xFreqIsFloat;
	}
	else
	{		
		ar >> Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle;
		ar >> Bit1_0xPhasorsIsInt_1xPhasorFloat;
		ar >> Bit2_0xAnalogIsInt_1xAnalogIsFloat;
		ar >> Bit3_0xFreqIsInt_1xFreqIsFloat;
	}
}
