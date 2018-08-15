#include "stdafx.h"
#include "C37118AdaptationLayer.h"

using namespace strongridgui;

C37118PdcConfiguration C37118AdaptorUtils::Translate(const CPdcConfig& in_pdcConfig)
{
	C37118PdcConfiguration out_pdcConfig;

	// Set header
	out_pdcConfig.HeaderCommon.Sync.LeadIn = 0xAA;
	out_pdcConfig.HeaderCommon.Sync.FrameType = C37118HdrFrameType::CONFIGURATION_FRAME_2;
	out_pdcConfig.HeaderCommon.Sync.Version = 1;
	out_pdcConfig.HeaderCommon.FrameSize = 0;
	out_pdcConfig.HeaderCommon.IdCode = in_pdcConfig.SG_PdcId;
	out_pdcConfig.HeaderCommon.SOC = 0;
	out_pdcConfig.HeaderCommon.FracSec.FractionOfSecond = 0;

	// Set additional pdc fields
	out_pdcConfig.TimeBase.TimeBase = in_pdcConfig.SG_TimeBase;
	out_pdcConfig.DataRate = C37118DataRate::CreateByFramesPerSecond( in_pdcConfig.SG_DataRate_FramesPerSecond);
	
	for( int i = 0; i < in_pdcConfig.SG_PmuConfigArray.items.size(); ++i )
	{
		C37118PmuConfiguration pmuCfg = Translate( *in_pdcConfig.SG_PmuConfigArray.items[i] );		
		out_pdcConfig.PMUs.push_back(pmuCfg);
	}

	return out_pdcConfig;
}

std::string ToStdString( const CString& input )
{
	CStringA tmp( input );
	return (LPCSTR)tmp ;
}

C37118PmuConfiguration C37118AdaptorUtils::Translate(const CPmuConfig& in_pmuCfg)
{
	C37118PmuConfiguration out_pmuConfig;
	out_pmuConfig.StationName = ToStdString(in_pmuCfg.SG_PmuStationName);
	out_pmuConfig.IdCode = in_pmuCfg.SG_PmuIdCode;
	out_pmuConfig.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle = in_pmuCfg.SG_PmuFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle;
	out_pmuConfig.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat = in_pmuCfg.SG_PmuFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat;
	out_pmuConfig.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat = in_pmuCfg.SG_PmuFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat;
	out_pmuConfig.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat = in_pmuCfg.SG_PmuFormat.Bit3_0xFreqIsInt_1xFreqIsFloat;

	// copy channel names (phasor/analog/digital)
	for( int i = 0; i < in_pmuCfg.SG_PhasorNames.items.size(); ++i ) out_pmuConfig.phasorChnNames.push_back( ToStdString(in_pmuCfg.SG_PhasorNames.items[i]) );
	for( int i = 0; i < in_pmuCfg.SG_AnalogNames.items.size(); ++i ) out_pmuConfig.analogChnNames.push_back( ToStdString(in_pmuCfg.SG_AnalogNames.items[i]) );
	for( int i = 0; i < in_pmuCfg.SG_DigitalNames.items.size(); ++i ) out_pmuConfig.digitalChnNames.push_back( ToStdString(in_pmuCfg.SG_DigitalNames.items[i]) );

	// copy units - PHASOR
	for( int i = 0; i < in_pmuCfg.SG_PhasorUnitArray.items.size(); ++i )
		out_pmuConfig.PhasorUnit.push_back( Translate( *in_pmuCfg.SG_PhasorUnitArray.items[i] ) );

	// Copy unit - ANALOG
	for( int i = 0; i < in_pmuCfg.SG_AnalogUnitArray.items.size(); ++i )
		out_pmuConfig.AnalogUnit.push_back( Translate( *in_pmuCfg.SG_AnalogUnitArray.items[i]) );

	// Copy unit - DIGITAL
	out_pmuConfig.DigitalUnit = Translate(in_pmuCfg.SG_DigitalUnitArray.items );

	out_pmuConfig.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60 = in_pmuCfg.SG_FreqCode;
	out_pmuConfig.ConfChangeCnt = in_pmuCfg.SG_ChangeCnt;

	return out_pmuConfig;
}

C37118PhasorUnit C37118AdaptorUtils::Translate(const CPhasorUnit& in_phasorUnit )
{
	C37118PhasorUnit phUnit;
	phUnit.Type = in_phasorUnit.SG_Type;
	phUnit.PhasorScalar = in_phasorUnit.SG_Scalar;
	return phUnit;
}

C37118AnalogUnit C37118AdaptorUtils::Translate(const CAnalogUnit& in_analogUnit )
{
	C37118AnalogUnit angUnit;
	angUnit.Type_X = in_analogUnit.SG_Type;
	angUnit.AnalogScalar = in_analogUnit.SG_Scalar;
	return angUnit;
}

std::vector<C37118DigitalUnit> C37118AdaptorUtils::Translate(const std::vector<CDigitalUnitBit*>& in_digUnitBits )
{
	std::vector<C37118DigitalUnit> digUnitArr( (in_digUnitBits.size() + 15) / 16, C37118DigitalUnit( 0,0) );

	for( int i = 0; i < in_digUnitBits.size(); ++i )
	{
		const int unitIdx = i / 16;
		const int idxInUnit = i % 16;
		digUnitArr[unitIdx].SetValidBit(idxInUnit, true ); // in_digUnitBits[i]->SG_ValidInput);
		digUnitArr[unitIdx].SetNormalStsBit(idxInUnit, in_digUnitBits[i]->SG_NormalState);
	}

	return digUnitArr;
}