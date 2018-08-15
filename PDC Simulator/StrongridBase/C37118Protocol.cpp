#include "common.h"
#include "C37118Protocol.h"
#include "EncDec.h"

using namespace strongridbase;


void C37118Protocol::WriteFrameHeader(char* data, const C37118FrameHeader* frameHeader, int* offset)
{
	// Write SYNC field	
	WriteSyncField(data, &frameHeader->Sync, offset );

	// Skip FRAMESIZE until the end
	*offset += 2;

	// Write IDCODE
	EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)frameHeader->IdCode), offset );
	
	// Write SOC
	EncDec::put_U32(data, EncDec::ToNetByteOrder((uint32_t)frameHeader->SOC), offset );

	// Write FRACSEC
	WriteFracSecField(data, &frameHeader->FracSec, offset );
}

void C37118Protocol::WriteConfigurationFrame(char* data, const C37118PdcConfiguration* pdcconfig, int* offset)
{
	int offsetAtStart = *offset;

	// Write HEADER
	WriteFrameHeader(data, &pdcconfig->HeaderCommon, offset);
	
	// Write TIME_BASE
	WriteTimeBaseField(data, &pdcconfig->TimeBase, offset );

	// Write NUM_PMU
	EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)pdcconfig->PMUs.size()), offset );

	for( std::vector<C37118PmuConfiguration>::const_iterator iter = pdcconfig->PMUs.begin(); iter != pdcconfig->PMUs.end(); ++iter )
	{
		const int tmpMinDigNames = ((iter->digitalChnNames.size() + 15) / 16) * 16;
		
		// Assert on number of PHASORS/analog/digital's VS number of format/units
		if( iter->phasorChnNames.size() != iter->PhasorUnit.size() ) throw Exception("Phasor names/units mismatch");
		if( iter->analogChnNames.size() != iter->AnalogUnit.size() ) throw Exception("Analog names/units mismatch");
		if( tmpMinDigNames != iter->DigitalUnit.size() * 16 ) throw Exception("Digital names/units mismatch");


		// Write STN - station name
		EncDec::put_String(data, iter->StationName, 16, offset );

		// Write IDCODE
		EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)iter->IdCode), offset );

		// Write FORMAT
		WriteC37118PmuFormat(data, &iter->DataFormat, offset );

		uint32_t tmp = (iter->digitalChnNames.size() + 15) / 16;

		// Write PHNMR/ANNMR/CHNAM - Number of phasors/analog/digwords
		EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)iter->phasorChnNames.size()), offset );
		EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)iter->analogChnNames.size()), offset );
		EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)tmp), offset );

		// Write phasors names
		for( std::vector<std::string>::const_iterator phNamIter = iter->phasorChnNames.begin(); phNamIter != iter->phasorChnNames.end(); ++phNamIter )
			EncDec::put_String(data, *phNamIter, 16, offset );

		// Write analog names
		for( std::vector<std::string>::const_iterator angNamIter = iter->analogChnNames.begin(); angNamIter != iter->analogChnNames.end(); ++angNamIter )
			EncDec::put_String(data, *angNamIter, 16, offset );

		// Write digital names (in blocks of 16)
		int digNamesWritten = 0;
		for( std::vector<std::string>::const_iterator digNamIter = iter->digitalChnNames.begin(); digNamIter != iter->digitalChnNames.end(); ++digNamIter ) {
			EncDec::put_String(data, *digNamIter, 16, offset );
			digNamesWritten++;
		}
		for( ; digNamesWritten < tmpMinDigNames; ++digNamesWritten )
			EncDec::put_String(data, "", 16, offset );

		// Write PHUNIT/ANUNIT/DIGUNIT
		for( std::vector<C37118PhasorUnit>::const_iterator iterUnit = iter->PhasorUnit.begin(); iterUnit != iter->PhasorUnit.end(); ++iterUnit )
			WriteC37118PhasorUnit( data, &(*iterUnit), offset );
		
		for( std::vector<C37118AnalogUnit>::const_iterator iterUnit = iter->AnalogUnit.begin(); iterUnit != iter->AnalogUnit.end(); ++iterUnit )
			WriteC37118AnalogUnit(data, &(*iterUnit), offset );

		for( std::vector<C37118DigitalUnit>::const_iterator iterUnit = iter->DigitalUnit.begin(); iterUnit != iter->DigitalUnit.end(); ++iterUnit )
			WriteC37118DigitalUnit(data, &(*iterUnit), offset );

		// Write FNOM
		WriteNomFreqField(data, &iter->NomFreqCode, offset );

		// Write CFGCNT
		EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)iter->ConfChangeCnt), offset );
	}

	// Write DATA_RATE
	EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)pdcconfig->DataRate.RawDataRate()), offset );


	WriteFooter(data, offsetAtStart, offset);
}


void C37118Protocol::WriteConfigurationFrame_Ver3( char* data, const C37118PdcConfiguration_Ver3* pdcconfig, int* offset)
{
	int offsetAtStart = *offset;

	// Write HEADER
	WriteFrameHeader(data, &pdcconfig->HeaderCommon, offset);
	
	// Write CONT_IDX
	EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)pdcconfig->ContinuationIndex.GetRawC37118Value()), offset);

	// Write TIME_BASE
	WriteTimeBaseField(data, &pdcconfig->TimeBase, offset );

	// Write NUM_PMU
	EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)pdcconfig->PMUs.size()), offset );

	for( std::vector<C37118PmuConfiguration_Ver3>::const_iterator iter = pdcconfig->PMUs.begin(); iter != pdcconfig->PMUs.end(); ++iter )
	{
		const int tmpMinDigNames = ((iter->digitalChnNames.size() + 15) / 16) * 16;

		// Assert on number of PHASORS/analog/digital's VS number of format/units
		if( iter->phasorChnNames.size() != iter->PhasorScales.size() ) throw Exception("Phasor names/units mismatch");
		if( iter->analogChnNames.size() != iter->AnalogScales.size() ) throw Exception("Analog names/units mismatch");
		if( tmpMinDigNames != iter->DigitalUnits.size() * 16 ) throw Exception("Digital names/units mismatch");

		// Write STN - station name
		if( iter->StationName.length() > 255 ) throw Exception("StationName too long. Must be <= 255");
		EncDec::put_U8(data, (uint8_t)iter->StationName.length(), offset );
		EncDec::put_String(data, iter->StationName, iter->StationName.length(), offset );

		// Write IDCODE
		EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)iter->IdCode), offset );

		// Write G_PMU_ID (16 bytes of raw data)
		for( int i=0; i < 16; ++i)
			EncDec::put_U8( data, iter->GlobalPmuId[i], offset );

		// Write FORMAT
		WriteC37118PmuFormat(data, &iter->DataFormat, offset );

		uint32_t tmp = (iter->digitalChnNames.size() + 15) / 16;

		// Write PHNMR/ANNMR/CHNAM - Number of phasors/analog/digwords
		EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)iter->phasorChnNames.size()), offset );
		EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)iter->analogChnNames.size()), offset );
		EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)tmp), offset );

		// Write phasors names
		for( std::vector<std::string>::const_iterator phNamIter = iter->phasorChnNames.begin(); phNamIter != iter->phasorChnNames.end(); ++phNamIter ) {
			EncDec::put_U8(data, (*phNamIter).length(), offset );
			EncDec::put_String(data, *phNamIter, phNamIter->size(), offset );
		}

		// Write analog names
		for( std::vector<std::string>::const_iterator angNamIter = iter->analogChnNames.begin(); angNamIter != iter->analogChnNames.end(); ++angNamIter ) {
			EncDec::put_U8(data, (*angNamIter).length(), offset );
			EncDec::put_String(data, *angNamIter, angNamIter->length(), offset );
		}

		// Write digital names (in blocks of 16)		
		int digNamesWritten = 0;
		for( std::vector<std::string>::const_iterator digNamIter = iter->digitalChnNames.begin(); digNamIter != iter->digitalChnNames.end(); ++digNamIter ) {
			EncDec::put_U8(data, (*digNamIter).length(), offset );
			EncDec::put_String(data, *digNamIter, digNamIter->length(), offset );
			digNamesWritten++;
		}
		for( ; digNamesWritten < tmpMinDigNames; ++digNamesWritten )
			EncDec::put_U8(data, (unsigned char)0, offset );

		// Write PHUNIT/ANUNIT/DIGUNIT
		for( std::vector<C37118PhasorScale_Ver3>::const_iterator phScaleIter = iter->PhasorScales.begin(); phScaleIter != iter->PhasorScales.end(); ++phScaleIter)
			WriteC37118PhasorScale_Ver3( data, &(*phScaleIter), offset );
		
		for( std::vector<C37118AnalogScale_Ver3>::const_iterator angScaleIter = iter->AnalogScales.begin(); angScaleIter != iter->AnalogScales.end(); ++angScaleIter )
			WriteC37118AnalogScale_Ver3(data, &(*angScaleIter), offset );

		for( std::vector<C37118DigitalUnit>::const_iterator digUnitIter = iter->DigitalUnits.begin(); digUnitIter != iter->DigitalUnits.end(); ++digUnitIter )
			WriteC37118DigitalUnit(data, &(*digUnitIter), offset );

		// Write PMU_LAT / PMU_LON / PMU_ELEV
		EncDec::put_Single(data, EncDec::ToNetByteOrder(iter->POS_LAT), offset);
		EncDec::put_Single(data, EncDec::ToNetByteOrder(iter->POS_LON), offset);
		EncDec::put_Single(data, EncDec::ToNetByteOrder(iter->POS_ELEV), offset);

		// Write SVC_CLASS
		EncDec::put_U8(data, iter->ServiceClass, offset);

		// Write WINDOW (signed integer)
		EncDec::put_S32(data, EncDec::ToNetByteOrder(iter->PhasorMeasurementWindow), offset );

		// Write GRP_DLY
		EncDec::put_S32(data, EncDec::ToNetByteOrder(iter->PhasorMeasurementGroupDelayMs), offset );

		// Write FNOM
		WriteNomFreqField(data, &iter->NomFreqCode, offset );

		// Write CFGCNT
		EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)iter->ConfChangeCnt), offset );
	}

	// Write DATA_RATE
	EncDec::put_U16(data, EncDec::ToNetByteOrder((uint16_t)pdcconfig->DataRate.RawDataRate()), offset );


	WriteFooter(data, offsetAtStart, offset);
}

void C37118Protocol::WriteDataFrame(char* data, const C37118PdcDataDecodeInfo* config, const C37118PdcDataFrame* dataFrame, int* offset)
{
	int offsetAtStart = 0;

	// Write header
	WriteFrameHeader(data, &dataFrame->HeaderCommon, offset );

	// Per  PMU
	if( dataFrame->pmuDataFrame.size() != config->PMUs.size() ) throw Exception("Invalid dataframe - does not match config: Invalid pmu count");
	for( int iPmu = 0; iPmu < dataFrame->pmuDataFrame.size(); ++iPmu )
	{
		// Get quickref to PMU dataframe, and corresponding PMU configuration frame
		const C37118PmuDataDecodeInfo* pmuConfig = &config->PMUs[iPmu];
		const C37118PmuDataFrame* pmuData = &dataFrame->pmuDataFrame[iPmu];

		// Write STAT
		EncDec::put_U16( data, EncDec::ToNetByteOrder((uint16_t)pmuData->Stat.ToRaw()), offset );

		// Write PHASORS	
		if( pmuData->PhasorValues.size() != pmuConfig->numPhasors ) throw Exception("Invalid dataframe - does not match config: Invalid phasor count");
		for( std::vector<C37118PmuDataFramePhasorRealImag>::const_iterator iterPhasor = pmuData->PhasorValues.begin(); iterPhasor != pmuData->PhasorValues.end(); ++iterPhasor )
		{
			if( pmuConfig->DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat == true )  // FLOAT
			{
				if( pmuConfig->DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle == false ) // RECT
				{
					float real, imag;
					iterPhasor->getRealImagAsFloat(&real, &imag);
					EncDec::put_Single(data, EncDec::ToNetByteOrder(real), offset);
					EncDec::put_Single(data, EncDec::ToNetByteOrder(imag), offset);
				}				
				else // mag + angle
				{
					float mag, angle;
					iterPhasor->getMagAngleAsFloat(&mag, &angle);
					EncDec::put_Single(data, EncDec::ToNetByteOrder(mag), offset);
					EncDec::put_Single(data, EncDec::ToNetByteOrder(angle), offset);
				}
			}		
			else // int16
			{
				if( pmuConfig->DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle == false ) // RECT
				{
					int16_t real, imag;
					iterPhasor->getRealImagAsInt16(&real, &imag);
					EncDec::put_S16(data, EncDec::ToNetByteOrder(real), offset);
					EncDec::put_S16(data, EncDec::ToNetByteOrder(imag), offset);
				}				
				else // mag + angle
				{
					uint16_t mag;
					int16_t angle;
					iterPhasor->getMagAngleAsInt16(&mag, &angle);
					EncDec::put_U16(data, EncDec::ToNetByteOrder(mag), offset);
					EncDec::put_S16(data, EncDec::ToNetByteOrder(angle), offset);
				}
			}
		}
		
		// Write FREQ/DFREQ
		if( pmuConfig->DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat == false ) // Freq is int
		{
			EncDec::put_S16(data, EncDec::ToNetByteOrder((int16_t)pmuData->Frequency), offset);
			EncDec::put_S16(data, EncDec::ToNetByteOrder((int16_t)(pmuData->DeltaFrequency * 100 )), offset);
		}
		else															// Freq is float
		{
			EncDec::put_Single(data, EncDec::ToNetByteOrder(pmuData->Frequency), offset);
			EncDec::put_Single(data, EncDec::ToNetByteOrder(pmuData->DeltaFrequency), offset);
		}

		// Write ANALOG
		if( pmuData->AnalogValues.size() != pmuConfig->numAnalogs ) throw Exception("Invalid dataframe - does not match config: Invalid analog count");
		for( std::vector<C37118PmuDataFrameAnalog>::const_iterator iterAnalog = pmuData->AnalogValues.begin(); iterAnalog != pmuData->AnalogValues.end(); ++iterAnalog )
		{
			if( pmuConfig->DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat == true ) // FLOAT
			{			
				float val = iterAnalog->getValueAsFloat();
				EncDec::put_Single(data, EncDec::ToNetByteOrder(val), offset);
			}
			else																  // INT16
			{
				int16_t val = iterAnalog->getValueAsInt16();
				EncDec::put_S16(data, EncDec::ToNetByteOrder(val), offset);
			}
		}

		// Write DIGITAL
		if( pmuData->DigitalValues.size() != pmuConfig->numDigitals ) throw Exception("Invalid dataframe - does not match config: Invalid digital count");
		C37118PmuDataFrameDigitalHelper digHelper = C37118PmuDataFrameDigitalHelper::CreateByBoolArray(pmuData->DigitalValues);
		std::vector<uint16_t> digWordArray = digHelper.ToDigWord();
		for( std::vector<uint16_t>::const_iterator iter = digWordArray.begin(); iter != digWordArray.end(); ++iter )
		{
			EncDec::put_U16(data, EncDec::ToNetByteOrder( (uint16_t)*iter ), offset );
		}
	}

	// Write update framesize and write crc16
	WriteFooter(data, offsetAtStart, offset );
}

void C37118Protocol::WriteFooter(char* data, int offsetAtStart, int* offset )
{
	// Update "framesize"
	EncDec::put_U16(data+2, EncDec::ToNetByteOrder((uint16_t)(*offset+2)), &offsetAtStart);

	// Write CRC16
	EncDec::put_U16(data, EncDec::ToNetByteOrder(CalcCrc16(data,*offset)), offset );
}

void C37118Protocol::WriteHeaderFrame(char* data, const C37118PdcHeaderFrame* headerFrame, int* offset)
{
	int offsetAtStart = *offset;

	// Write frame header
	WriteFrameHeader(data, &headerFrame->Header, offset );

	// Write header message data
	EncDec::put_String(data, headerFrame->HeaderMessage, headerFrame->HeaderMessage.length(), offset);
	
	// Update framesize and write CRC field
	WriteFooter(data, offsetAtStart, offset );
}

void C37118Protocol::WriteCommandFrame(char* data, const C37118CommandFrame* cmdFrame, int* offset)
{
	int offsetAtStart = *offset;

	// Write frame header
	WriteFrameHeader(data, &cmdFrame->Header, offset);

	// Write CMD field
	EncDec::put_U16( data, EncDec::ToNetByteOrder( (uint16_t)cmdFrame->CmdType ), offset );

	// Write EXTFRAME data - null
	// ...
		
	// Update framesize and write CRC field
	WriteFooter(data, offsetAtStart, offset );
}

void C37118Protocol::WriteSyncField(char* data,const C37118SyncField* syncField, int* offset)
{
	EncDec::put_U8(data, syncField->LeadIn, offset ); // Should always be 0xAA

	uint8_t rawTypeVer = 
		((syncField->FrameType << 4) & 0x70) |
		(syncField->Version << 0);
	EncDec::put_U8(data, rawTypeVer, offset );
}

void C37118Protocol::WriteFracSecField(char* data, const C37118FracSec* fracSecField, int* offset)
{
	uint32_t composedFracSec = (fracSecField->TimeQuality << 24) | (fracSecField->FractionOfSecond & 0x00FFFFFF);
	EncDec::put_U32(data, EncDec::ToNetByteOrder(composedFracSec), offset );	
}

void C37118Protocol::WriteNomFreqField(char* data, const C37118NomFreq* nomFreqField, int* offset)
{
	uint16_t composedNomFreqField = nomFreqField->Bit0_1xFreqIs50_0xFreqIs60 ? 0x1 : 0x0;
	EncDec::put_U16(data, EncDec::ToNetByteOrder(composedNomFreqField), offset );
}

void C37118Protocol::WriteTimeBaseField(char* data,  const C37118TimeBase* timeBaseField , int* offset)
{
	uint32_t composedTimeBaseField = 
		(timeBaseField->Flags << 24) | 
		(timeBaseField->TimeBase & 0x00FFFFFF);
	EncDec::put_U32(data, EncDec::ToNetByteOrder(composedTimeBaseField), offset);
}

void C37118Protocol::WriteC37118PmuFormat(char* data, const C37118PmuFormat* pmuFormat, int* offset )
{
	uint16_t composedFormat = 
		((pmuFormat->Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle ? 1 : 0) << 0) |
		((pmuFormat->Bit1_0xPhasorsIsInt_1xPhasorFloat ? 1 : 0) << 1) |
		((pmuFormat->Bit2_0xAnalogIsInt_1xAnalogIsFloat ? 1 : 0) << 2) |
		((pmuFormat->Bit3_0xFreqIsInt_1xFreqIsFloat ? 1 : 0) << 3);
	EncDec::put_U16(data, EncDec::ToNetByteOrder(composedFormat), offset ); 
}

void C37118Protocol::WriteC37118PhasorUnit(char* data, const C37118PhasorUnit* phasorUnit , int* offset)
{
	uint32_t composedUnit = (phasorUnit->Type << 24) | (phasorUnit->PhasorScalar & 0x00FFFFFF);
	EncDec::put_U32(data, EncDec::ToNetByteOrder(composedUnit), offset );
}

void C37118Protocol::WriteC37118AnalogUnit(char* data, const C37118AnalogUnit* analogUnit, int* offset )
{
	uint32_t composedUnit = (analogUnit->Type_X << 24) | (analogUnit->AnalogScalar & 0x00FFFFFF);
	EncDec::put_U32(data, EncDec::ToNetByteOrder(composedUnit), offset );
}

void C37118Protocol::WriteC37118DigitalUnit(char* data, const C37118DigitalUnit* digUnit, int* offset )
{
	EncDec::put_U16(data, EncDec::ToNetByteOrder(digUnit->DigNormalStatus), offset );
	EncDec::put_U16(data, EncDec::ToNetByteOrder(digUnit->DigValidInputs), offset );
}


void C37118Protocol::WriteC37118PhasorScale_Ver3(char* data, const C37118PhasorScale_Ver3* phScale, int* offset)
{
	// Assemble and write FIRST "LONG WORD"
	uint32_t firstLongWord = 0;
	firstLongWord |= (phScale->PhasorBits & 0x000000FF); // Write the phasors bits
	firstLongWord |= (phScale->VoltOrCurrent & 0x1) << (16+3);
	firstLongWord |= ((uint8_t)phScale->PhasorComponentCode & 0x7) << (16);
	EncDec::put_U32(data, EncDec::ToNetByteOrder((uint32_t)firstLongWord), offset);

	// Write SECOND & THIRD "Long word"
	EncDec::put_Single(data, EncDec::ToNetByteOrder(phScale->ScaleFactorOne_Y), offset);
	EncDec::put_Single(data, EncDec::ToNetByteOrder(phScale->ScaleFactorTwo_Angle), offset);
}

void C37118Protocol::WriteC37118AnalogScale_Ver3(char* data, const C37118AnalogScale_Ver3* angScale, int* offset)
{
	EncDec::put_Single(data, EncDec::ToNetByteOrder(angScale->Scale), offset );
	EncDec::put_Single(data, EncDec::ToNetByteOrder(angScale->Offset), offset );
}

C37118FrameHeader C37118Protocol::ReadFrameHeader(char* data, int length, int* offset)
{
	C37118FrameHeader output;

	// Read SYNC field
	output.Sync = ReadSyncField(data, offset );

	// Read FRAMESIZE - validate against "length"
	output.FrameSize = EncDec::ToHostByteOrder(EncDec::get_U16(data,offset));
	if( length + *offset < output.FrameSize ) throw Exception("Input databuffer is too short to describe the entire header");
	
	// Read IDCODE
	output.IdCode = EncDec::ToHostByteOrder(EncDec::get_U16(data,offset));

	// Read SOC
	output.SOC = EncDec::ToHostByteOrder(EncDec::get_U32(data,offset) );

	// Read FRACSEC
	output.FracSec = ReadFracSecField(data,offset);

	return output;
}

C37118PdcConfiguration C37118Protocol::ReadConfigurationFrame(char* data, int length)
{
	C37118PdcConfiguration output;
	int offset = 0;

	// Read frame header
	output.HeaderCommon = ReadFrameHeader(data, length, &offset );

	// Read TIME_BASE
	output.TimeBase = ReadTimeBaseField(data, &offset );

	// Read NUM_PMU
	int numPmu = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));

	// Read STN
	for( int ipmu = 0; ipmu < numPmu; ++ipmu )
	{
		C37118PmuConfiguration pmuCfg;

		// Read STN
		pmuCfg.StationName = EncDec::get_String(data, 16, &offset);

		// Read IDCODE
		pmuCfg.IdCode = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));

		// Read FORMAT
		pmuCfg.DataFormat = ReadC37118PmuFormat(data, &offset);

		// Read PHNMR / ANNMR / DGNMR
		uint32_t numPhasors = EncDec::ToHostByteOrder( EncDec::get_U16(data, &offset));
		uint32_t numAnalog = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));
		uint32_t numDigWords = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));

		// Read CHNAM - phasors
		for( int i = 0; i < numPhasors; ++i ) 
			pmuCfg.phasorChnNames.push_back( EncDec::get_String(data,16,&offset) );

		// Read CHNAM - analog
		for( int i = 0; i < numAnalog; ++i )
			pmuCfg.analogChnNames.push_back( EncDec::get_String(data,16,&offset) );

		// Read CHNAM - dig chns
		for( int i = 0; i < numDigWords * 16; ++i )
			pmuCfg.digitalChnNames.push_back( EncDec::get_String(data, 16, &offset ) );

		// Read PHUNIT
		for( int i = 0; i < numPhasors; ++i )
			pmuCfg.PhasorUnit.push_back( ReadC37118PhasorUnit(data, &offset) );

		// Read ANUNIT
		for( int i = 0; i < numAnalog; ++i )
			pmuCfg.AnalogUnit.push_back( ReadC37118AnalogUnit(data, &offset) );

		// Read DIGUINT
		for( int i = 0; i < numDigWords; ++i )
			pmuCfg.DigitalUnit.push_back( ReadC37118DigitalUnit(data, &offset) );

		// Read FNOM
		pmuCfg.NomFreqCode = ReadNomFreqField(data, &offset);

		// Read CFGCNT
		pmuCfg.ConfChangeCnt = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));

		// Add PMU to the list
		output.PMUs.push_back(pmuCfg);
	}

	// Read DATA_RATE
	output.DataRate = C37118DataRate ::CreateByRawC37118Format(EncDec::ToHostByteOrder(EncDec::get_S16(data, &offset)));


	// Read CRC16
	output.FooterCrc16 = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));

	return output;
}


C37118PdcConfiguration_Ver3 C37118Protocol::ReadConfigurationFrame_Ver3(char* data, int length)
{
	C37118PdcConfiguration_Ver3 output;
	int offset = 0;

	// Read frame header
	output.HeaderCommon = ReadFrameHeader(data, length, &offset );

	// Read CONT_IDX
	output.ContinuationIndex = C37118ContIdx::CreateByC37118Raw(EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset)));

	// Read TIME_BASE
	output.TimeBase = ReadTimeBaseField(data, &offset );

	// Read NUM_PMU
	int numPmu = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));

	// Read STN
	for( int ipmu = 0; ipmu < numPmu; ++ipmu )
	{
		C37118PmuConfiguration_Ver3 pmuCfg;

		// Read STN
		int tmp = EncDec::get_U8(data,&offset);
		pmuCfg.StationName = EncDec::get_String(data, tmp, &offset);

		// Read IDCODE
		pmuCfg.IdCode = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));
		
		// Read G_PMU_ID
		for( int i = 0; i < 16; ++i )
			pmuCfg.GlobalPmuId[i] = EncDec::get_U8(data,&offset);

		// Read FORMAT
		pmuCfg.DataFormat = ReadC37118PmuFormat(data, &offset);

		// Read PHNMR / ANNMR / DGNMR
		uint32_t numPhasors = EncDec::ToHostByteOrder( EncDec::get_U16(data, &offset));
		uint32_t numAnalog = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));
		uint32_t numDigWords = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));

		// Read CHNAM - phasors
		for( int i = 0; i < numPhasors; ++i ) {
			int nameLength = EncDec::get_U8(data,&offset);
			pmuCfg.phasorChnNames.push_back( EncDec::get_String(data,nameLength,&offset) );
		}

		// Read CHNAM - analog
		for( int i = 0; i < numAnalog; ++i ) {
			int nameLength = EncDec::get_U8(data,&offset);
			pmuCfg.analogChnNames.push_back( EncDec::get_String(data,nameLength,&offset) );
		}

		// Read CHNAM - dig chns
		for( int i = 0; i < numDigWords * 16; ++i ) {
			int nameLength = EncDec::get_U8(data,&offset);
			pmuCfg.digitalChnNames.push_back( EncDec::get_String(data,nameLength, &offset ) );
		}

		// Read PHSCALE
		for( int i = 0; i < numPhasors; ++i )
			pmuCfg.PhasorScales.push_back(ReadC37118PhasorScale_Ver3(data, &offset));

		// Read ANGSCALE
		for( int i = 0; i < numAnalog; ++i )
			pmuCfg.AnalogScales.push_back( ReadC37118AnalogScale_Ver3(data, &offset));

		// Read DIGUINT
		for( int i = 0; i < numDigWords; ++i )
			pmuCfg.DigitalUnits.push_back(ReadC37118DigitalUnit(data, &offset));

		// Read PMU_LAT/PMU_LON/PMU_ELEV
		pmuCfg.POS_LAT = EncDec::ToHostByteOrder(EncDec::get_Single(data,&offset));
		pmuCfg.POS_LON = EncDec::ToHostByteOrder(EncDec::get_Single(data,&offset));
		pmuCfg.POS_ELEV = EncDec::ToHostByteOrder(EncDec::get_Single(data,&offset));

		// Read SVC_CLASS
		pmuCfg.ServiceClass = EncDec::get_U8(data,&offset);

		// Read WINDOW
		pmuCfg.PhasorMeasurementWindow = EncDec::ToHostByteOrder(EncDec::get_S32(data,&offset));

		// Read GRP_DLY
		pmuCfg.PhasorMeasurementGroupDelayMs = EncDec::ToHostByteOrder(EncDec::get_S32(data,&offset));

		// Read FNOM
		pmuCfg.NomFreqCode = ReadNomFreqField(data, &offset);

		// Read CFGCNT
		pmuCfg.ConfChangeCnt = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));

		// Add PMU to the list
		output.PMUs.push_back(pmuCfg);
	}

	// Read DATA_RATE
	output.DataRate = C37118DataRate ::CreateByRawC37118Format(EncDec::ToHostByteOrder(EncDec::get_S16(data, &offset)));


	// Read CRC16
	output.FooterCrc16 = EncDec::ToHostByteOrder(EncDec::get_U16(data, &offset));

	return output;
}

C37118PdcDataFrame C37118Protocol::ReadDataFrame(char* data, int length, const C37118PdcDataDecodeInfo* config, int* offset)
{
	C37118PdcDataFrame dataFrame;

	// Read header
	dataFrame.HeaderCommon = ReadFrameHeader(data, length, offset);

	// Per pmu
	for( std::vector<C37118PmuDataDecodeInfo>::const_iterator iterPmuCfg = config->PMUs.begin(); iterPmuCfg != config->PMUs.end(); ++iterPmuCfg )
	{
		C37118PmuDataFrame pmuDataFrame;

		// Read STAT
		pmuDataFrame.Stat = C37118PmuDataFrameStat(EncDec::ToHostByteOrder((uint16_t)EncDec::get_U16(data,offset)));

		// Read PHASORS
		for( int i = 0; i < iterPmuCfg->numPhasors; ++i )		
		{
			if( iterPmuCfg->DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat == true) // FLOAT
			{
				if( iterPmuCfg->DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle == false ) // RECT
				{
					float real = EncDec::ToHostByteOrder(EncDec::get_Single(data,offset));
					float imag = EncDec::ToHostByteOrder(EncDec::get_Single(data,offset));
					pmuDataFrame.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByRealImag(real,imag) );
				}
				else																			 // mag+angle
				{
					float mag = EncDec::ToHostByteOrder(EncDec::get_Single(data,offset));
					float angle = EncDec::ToHostByteOrder(EncDec::get_Single(data,offset));
					pmuDataFrame.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByPolarMag(mag, angle));
				}
			}
			else																 // INT
			{
				if( iterPmuCfg->DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle == false ) // RECT
				{
					int16_t real = EncDec::ToHostByteOrder(EncDec::get_S16(data,offset));
					int16_t imag = EncDec::ToHostByteOrder(EncDec::get_S16(data,offset));
					pmuDataFrame.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByRealImag(real,imag) );
				}
				else																			 // mag+angle
				{
					uint16_t mag = EncDec::ToHostByteOrder(EncDec::get_U16(data,offset));
					int16_t angle = EncDec::ToHostByteOrder(EncDec::get_S16(data,offset));
					pmuDataFrame.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByPolarMag(mag, angle));
				}
			}
		}

		// Read FREQ / DFREQ
		if( iterPmuCfg->DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat == 0 )  {
			pmuDataFrame.Frequency = EncDec::ToHostByteOrder( EncDec::get_S16(data,offset) ); // freq is int
			pmuDataFrame.DeltaFrequency = (float)EncDec::ToHostByteOrder(EncDec::get_S16(data,offset)) / 100.0f; // dfreq is int
		}
		else {
			pmuDataFrame.Frequency = EncDec::ToHostByteOrder( EncDec::get_Single(data,offset) ); // freq is float
			pmuDataFrame.DeltaFrequency = EncDec::ToHostByteOrder( EncDec::get_Single(data,offset) ); // dfreq is float
		}

		// Read ANALOG
		for( int i = 0; i < iterPmuCfg->numAnalogs; ++i )
		{
			if( iterPmuCfg->DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat == false )  // Analog is int
			{
				pmuDataFrame.AnalogValues.push_back( C37118PmuDataFrameAnalog::CreateByInt16( EncDec::ToHostByteOrder(EncDec::get_S16(data,offset))));
			}
			else // Analog is float
			{
				pmuDataFrame.AnalogValues.push_back( C37118PmuDataFrameAnalog::CreateByFloat( EncDec::ToHostByteOrder(EncDec::get_Single(data,offset))));
			}
		}

		// Read DIGITAL
		const int numDigWords = (iterPmuCfg->numDigitals + 15) / 16;
		std::vector<uint16_t> digWords;
		for( int iDigWord = 0; iDigWord < numDigWords; ++iDigWord )
			digWords.push_back( EncDec::ToHostByteOrder( EncDec::get_U16(data, offset) ));

		// Push each individual value unto dataframe
		std::vector<bool> digVals = C37118PmuDataFrameDigitalHelper::CreateByUint16Arr(digWords).ToBoolArray();
		for( std::vector<bool>::const_iterator iterDigVal = digVals.begin(); iterDigVal != digVals.end(); ++iterDigVal )
			pmuDataFrame.DigitalValues.push_back(*iterDigVal);

		// Add PMU dataframe to PDC dataframe
		dataFrame.pmuDataFrame.push_back(pmuDataFrame);
	}

	// Read crc16 information
	dataFrame.CRC16 = EncDec::ToHostByteOrder(EncDec::get_U16(data,offset));

	return dataFrame;
}

C37118PdcHeaderFrame C37118Protocol::ReadHeaderFrame(char* data, int length, int* offset)
{
	C37118PdcHeaderFrame headerFrame;
	
	// Read frame header
	headerFrame.Header = ReadFrameHeader(data, length, offset);

	// Read message payload
	const int messagePayloadLength = headerFrame.Header.FrameSize - 16;
	headerFrame.HeaderMessage = EncDec::get_String(data, messagePayloadLength, offset);

	// Read CRC16
	headerFrame.FooterCrc16 = EncDec::ToHostByteOrder(EncDec::get_U16(data, offset));

	return headerFrame;
}

C37118CommandFrame C37118Protocol::ReadCommandFrame(char* data, int bufferSize, int* offset)
{
	C37118CommandFrame cmdFrame;

	// Read header
	cmdFrame.Header = ReadFrameHeader(data, bufferSize, offset);

	// Read CMD
	cmdFrame.CmdType = (C37118CmdType)EncDec::ToHostByteOrder(EncDec::get_U16(data,offset));

	// Read (extframe)
	const int extFrameBytes = cmdFrame.Header.FrameSize - 18;
	*offset += extFrameBytes; // skip it

	// Read CRC
	cmdFrame.CRC16 = EncDec::ToHostByteOrder(EncDec::get_U16(data,offset));

	return cmdFrame;
}

C37118SyncField C37118Protocol::ReadSyncField(char* data, int* offset)
{
	C37118SyncField sync;	
	sync.LeadIn = EncDec::get_U8(data, offset);
	uint8_t rawVerType = EncDec::get_U8(data, offset);
	sync.Version = rawVerType & 0xF;
	sync.FrameType = (C37118HdrFrameType)((rawVerType & 0x70) >> 4);
	return sync;
}

C37118FracSec C37118Protocol::ReadFracSecField(char* data,int* offset)
{
	C37118FracSec fracSec;
	uint32_t raw = EncDec::ToHostByteOrder(EncDec::get_U32(data, offset));
	fracSec.TimeQuality = (raw & 0xFF000000) >> 24;
	fracSec.FractionOfSecond = (raw & 0x00FFFFFF);
	return fracSec;
}

C37118NomFreq C37118Protocol::ReadNomFreqField(char* data, int* offset)
{
	C37118NomFreq nomFreq;
	uint16_t raw = EncDec::ToHostByteOrder(EncDec::get_U16(data, offset));
	nomFreq.Bit0_1xFreqIs50_0xFreqIs60 = raw & 0x1;
	return nomFreq;
}

C37118TimeBase C37118Protocol::ReadTimeBaseField(char* data,int* offset)
{
	C37118TimeBase timebase;
	uint32_t raw = EncDec::ToHostByteOrder(EncDec::get_U32(data, offset));
	timebase.Flags = (raw & 0xFF000000) >> 24;
	timebase.TimeBase = (raw & 0x00FFFFFF);
	return timebase;
}

C37118PmuFormat C37118Protocol::ReadC37118PmuFormat(char* data, int* offset )
{
	C37118PmuFormat pmuf;
	uint16_t rawformat = EncDec::ToHostByteOrder(EncDec::get_U16(data, offset));
	pmuf.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle	= (rawformat & (1 << 0)) != 0;
	pmuf.Bit1_0xPhasorsIsInt_1xPhasorFloat				= (rawformat & (1 << 1)) != 0;
	pmuf.Bit2_0xAnalogIsInt_1xAnalogIsFloat				= (rawformat & (1 << 2)) != 0;
	pmuf.Bit3_0xFreqIsInt_1xFreqIsFloat					= (rawformat & (1 << 3)) != 0;
	return pmuf;
}

C37118PhasorUnit C37118Protocol::ReadC37118PhasorUnit(char* data,int* offset)
{
	C37118PhasorUnit phunit;
	uint32_t raw = EncDec::ToHostByteOrder(EncDec::get_U32(data, offset));
	phunit.Type = (raw & 0xFF000000) >> 24;
	phunit.PhasorScalar = (raw & 0x00FFFFFF);//Set max value for scalar
	return phunit;
}

C37118AnalogUnit C37118Protocol::ReadC37118AnalogUnit(char* data, int* offset )
{
	C37118AnalogUnit anunit;
	uint32_t raw = EncDec::ToHostByteOrder(EncDec::get_U32(data, offset));
	anunit.Type_X = (raw & 0xFF000000) >> 24;
	anunit.AnalogScalar = (raw & 0x00FFFFFF); // TODO: REVIEW - UNSIGNED / SIGNED
	return anunit;
}

C37118DigitalUnit C37118Protocol::ReadC37118DigitalUnit(char* data,int* offset )
{
	C37118DigitalUnit digUnit;
	digUnit.DigNormalStatus = EncDec::ToHostByteOrder(EncDec::get_U16(data, offset));
	digUnit.DigValidInputs = EncDec::ToHostByteOrder(EncDec::get_U16(data, offset));
	return digUnit;
}


C37118PhasorScale_Ver3 C37118Protocol::ReadC37118PhasorScale_Ver3(char* data, int* offset)
{
	C37118PhasorScale_Ver3 tmp;

	uint32_t longWordOne = EncDec::ToHostByteOrder(EncDec::get_U32(data, offset));
	uint32_t longWordTwo = EncDec::ToHostByteOrder(EncDec::get_U32(data, offset));
	uint32_t longWordThree = EncDec::ToHostByteOrder(EncDec::get_U32(data, offset));

	// First long word: bitmapped flags
	tmp.PhasorBits = longWordOne & 0x0000FFFF;
	tmp.VoltOrCurrent = ((longWordOne &  0x00FF0000) >> 16) & 0x8;
	tmp.PhasorComponentCode = (PhasorComponentCodeEnum)(((longWordOne & 0x00FF0000) >> 16) & 0x7);

	// Second long word: scaling information
	tmp.ScaleFactorOne_Y = *(float*)(&longWordTwo);
	tmp.ScaleFactorTwo_Angle =  *(float*)(&longWordThree);

	// Return the phasor scaling structure
	return tmp;
}

C37118AnalogScale_Ver3 C37118Protocol::ReadC37118AnalogScale_Ver3(char* data, int* offset)
{
	C37118AnalogScale_Ver3 tmp;
	tmp.Scale = EncDec::ToHostByteOrder(EncDec::get_Single(data,offset));
	tmp.Offset = EncDec::ToHostByteOrder(EncDec::get_Single(data,offset));
	return tmp;
}


C37118PdcDataDecodeInfo C37118Protocol::CreateDecodeInfoByPdcConfig(const C37118PdcConfiguration& pdccfg) 
{
	C37118PdcDataDecodeInfo output;
	output.timebase = pdccfg.TimeBase;

	for( std::vector<C37118PmuConfiguration>::const_iterator iter = pdccfg.PMUs.begin(); iter != pdccfg.PMUs.end(); ++iter )
	{
		C37118PmuDataDecodeInfo pmu;
		pmu.DataFormat = iter->DataFormat;
		pmu.numPhasors = iter->phasorChnNames.size();//Records number of channels
		pmu.numAnalogs = iter->analogChnNames.size();
		pmu.numDigitals = iter->digitalChnNames.size();
		output.PMUs.push_back(pmu);
	}

	return output;
}

C37118PdcDataDecodeInfo C37118Protocol::CreateDecodeInfoByPdcConfig(const C37118PdcConfiguration_Ver3& pdccfg) 
{
	C37118PdcDataDecodeInfo output;
	output.timebase = pdccfg.TimeBase;

	for( std::vector<C37118PmuConfiguration_Ver3>::const_iterator iter = pdccfg.PMUs.begin(); iter != pdccfg.PMUs.end(); ++iter )
	{
		C37118PmuDataDecodeInfo pmu;
		pmu.DataFormat = iter->DataFormat;
		pmu.numPhasors = iter->phasorChnNames.size();
		pmu.numAnalogs = iter->analogChnNames.size();
		pmu.numDigitals = iter->digitalChnNames.size();
		output.PMUs.push_back(pmu);
	}

	return output;
}


C37118PdcConfiguration C37118Protocol::DowngradePdcConfig(const C37118PdcConfiguration_Ver3* pdccfg)
{
	C37118PdcConfiguration oldPdcCfg;
	oldPdcCfg.HeaderCommon = pdccfg->HeaderCommon;
	oldPdcCfg.HeaderCommon.Sync.FrameType = C37118HdrFrameType::CONFIGURATION_FRAME_2;
	oldPdcCfg.TimeBase = pdccfg->TimeBase;
	oldPdcCfg.DataRate = pdccfg->DataRate;

	for( std::vector<C37118PmuConfiguration_Ver3>::const_iterator iter = pdccfg->PMUs.begin(); iter != pdccfg->PMUs.end(); ++iter )
	{
		// Add to list
		C37118PmuConfiguration oldPmuCfg = DowngradePmuConfig(&(*iter));
		oldPdcCfg.PMUs.push_back(oldPmuCfg);
	}

	return oldPdcCfg;
}

C37118PmuConfiguration C37118Protocol::DowngradePmuConfig(const C37118PmuConfiguration_Ver3* pdccfg)
{
	C37118PmuConfiguration oldPmuCfg;
	oldPmuCfg.StationName = pdccfg->StationName;
	oldPmuCfg.IdCode = pdccfg->IdCode;
	oldPmuCfg.DataFormat = pdccfg->DataFormat;
	oldPmuCfg.phasorChnNames = pdccfg->phasorChnNames;
	oldPmuCfg.analogChnNames = pdccfg->analogChnNames;
	oldPmuCfg.digitalChnNames = pdccfg->digitalChnNames;

	
	// Phasor scalar / unit conversion
	for( std::vector<C37118PhasorScale_Ver3>::const_iterator unitIter = pdccfg->PhasorScales.begin(); unitIter != pdccfg->PhasorScales.end(); ++unitIter )
		oldPmuCfg.PhasorUnit.push_back( C37118PhasorUnit(unitIter->VoltOrCurrent, unitIter->ScaleFactorOne_Y) );

	// Analog scalar
	for( std::vector<C37118AnalogScale_Ver3>::const_iterator unitIter = pdccfg->AnalogScales.begin(); unitIter != pdccfg->AnalogScales.end(); ++unitIter )
		oldPmuCfg.AnalogUnit.push_back( C37118AnalogUnit( 0, unitIter->Scale )  );

	// Digital unit
	for( std::vector<C37118DigitalUnit>::const_iterator unitIter = pdccfg->DigitalUnits.begin(); unitIter != pdccfg->DigitalUnits.end(); ++unitIter )
		oldPmuCfg.DigitalUnit.push_back(*unitIter);


	// etc
	oldPmuCfg.NomFreqCode = pdccfg->NomFreqCode;
	oldPmuCfg.ConfChangeCnt = pdccfg->ConfChangeCnt;

	return oldPmuCfg;
}

uint16_t C37118Protocol::CalcCrc16(char* data, int length)
{
  unsigned short crc = 0xFFFF; /*0xFFFF -> 0x0 -> 0xFFFF;*/
  unsigned short temp;
  unsigned short quick;
  unsigned int crcIdx;
  unsigned char *bufPtr = (unsigned char *)data;

  for(crcIdx = 0; crcIdx < length ;crcIdx++){
    temp = (crc >> 8) ^ bufPtr[crcIdx];
    crc <<= 8;
    quick = temp ^ (temp >> 4);
    crc ^= quick;
    quick <<= 5;
    crc ^= quick;
    quick <<= 7;
    crc ^= quick; 
  }
  return crc;
}
