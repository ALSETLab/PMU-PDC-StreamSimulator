#include <WinSock2.h>
#include "C37118ProtocolTests.h"
#include "Assert.h"
#include "../StrongridBase/C37118Protocol.h"

using namespace tests;
using namespace strongridbase;


C37118PmuConfiguration BuildPmu1Config()
{
	C37118PmuConfiguration pmu1;
	pmu1.IdCode = 12;
	pmu1.StationName = "PMU_NUMERO_UNO";

	// dataformat
	pmu1.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle = 0;
	pmu1.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat = 1;
	pmu1.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat = 0;
	pmu1.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat = 1;

	// Phasors 
	pmu1.phasorChnNames.push_back("Ph1");
	pmu1.phasorChnNames.push_back("Ph2");
	pmu1.phasorChnNames.push_back("Ph3");

	// Analog
	pmu1.analogChnNames.push_back("Ang1");
	pmu1.analogChnNames.push_back("Ang2");

	// Digital chns
	pmu1.digitalChnNames.push_back("Dig1");
	pmu1.digitalChnNames.push_back("Dig2");

	// Set phasor unit 
	pmu1.PhasorUnit.push_back( C37118PhasorUnit(0, 45) );
	pmu1.PhasorUnit.push_back( C37118PhasorUnit(0, 45) );
	pmu1.PhasorUnit.push_back( C37118PhasorUnit(0, 45) );

	// Set analog unit
	pmu1.AnalogUnit.push_back( C37118AnalogUnit(0, 55) );
	pmu1.AnalogUnit.push_back( C37118AnalogUnit(0, 55) );

	// Set digital unit
	pmu1.DigitalUnit.push_back( C37118DigitalUnit(0x0000, 0x3) ); // first 2 bits

	// Nominal frequency
	pmu1.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60 = 0x1;

	pmu1.ConfChangeCnt = 2;

	return pmu1;
}

void AssertOnPmuConfig_PMU1(C37118PmuConfiguration pmu)
{
	Assert::AreEqual("PMU_NUMERO_UNO", pmu.StationName );

	// dataformat
	Assert::IsFalse(pmu.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle);
	Assert::IsTrue(pmu.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat);
	Assert::IsFalse(pmu.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat);
	Assert::IsTrue(pmu.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat);

	// Assert on phasors names
	Assert::AreEqual(3, pmu.phasorChnNames.size());
	Assert::AreEqual("Ph1", pmu.phasorChnNames[0]);
	Assert::AreEqual("Ph2", pmu.phasorChnNames[1]);
	Assert::AreEqual("Ph3", pmu.phasorChnNames[2]);

	// Assert on Analog channel names
	Assert::AreEqual(2, pmu.analogChnNames.size());
	Assert::AreEqual("Ang1", pmu.analogChnNames[0]);
	Assert::AreEqual("Ang2", pmu.analogChnNames[1]);

	// Assert on digital channel names
	Assert::AreEqual(2, pmu.digitalChnNames.size() );
	Assert::AreEqual("Dig1", pmu.digitalChnNames[0]);
	Assert::AreEqual("Dig2", pmu.digitalChnNames[1]);

	// Assert on phasor unit
	Assert::AreEqual(0, pmu.PhasorUnit[0].Type);
	Assert::AreEqual(45, pmu.PhasorUnit[0].PhasorScalar );

	// Assert on analog unit
	Assert::AreEqual(0, pmu.AnalogUnit[0].Type_X);
	Assert::AreEqual(55, pmu.AnalogUnit[0].AnalogScalar);

	// Assert on digital unit
	Assert::AreEqual(0, pmu.DigitalUnit[0].DigNormalStatus);
	Assert::AreEqual(0x3, pmu.DigitalUnit[0].DigValidInputs);

	// Assert on nomfreq
	Assert::AreEqual(1, pmu.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60);
}

C37118PmuConfiguration BuildPmu2Config()
{
	C37118PmuConfiguration pmu;
	pmu.IdCode = 12;
	pmu.StationName = "PMU_NUMERO_TWO";

	// dataformat
	pmu.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle = 1;
	pmu.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat = 0;
	pmu.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat = 1;
	pmu.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat = 0;

	// Phasors 
	pmu.phasorChnNames.push_back("P2Ph1");
	pmu.phasorChnNames.push_back("P2Ph2");
	pmu.phasorChnNames.push_back("P2Ph3");
	pmu.phasorChnNames.push_back("P2Ph4");

	// Analog
	pmu.analogChnNames.push_back("P2Ang1");

	// Digital chns
	pmu.digitalChnNames.push_back("P2Dig1");

	// Set phasor unit 
	pmu.PhasorUnit.push_back( C37118PhasorUnit(1, 77) );
	pmu.PhasorUnit.push_back( C37118PhasorUnit(1, 77) );
	pmu.PhasorUnit.push_back( C37118PhasorUnit(1, 77) );
	pmu.PhasorUnit.push_back( C37118PhasorUnit(1, 77) );

	// Set analog unit
	pmu.AnalogUnit.push_back( C37118AnalogUnit(1, 22) );

	// Set digital unit
	pmu.DigitalUnit.push_back( C37118DigitalUnit(0xFACD, 0x1) ); // first 1 bit

	// Nominal frequency
	pmu.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60 = 0x0;
	
	pmu.ConfChangeCnt = 2;

	return pmu;
}

void AssertOnPmuConfig_PMU2(C37118PmuConfiguration pmu)
{
	Assert::AreEqual("PMU_NUMERO_TWO", pmu.StationName );

	// dataformat
	Assert::IsTrue(pmu.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle);
	Assert::IsFalse(pmu.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat);
	Assert::IsTrue(pmu.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat);
	Assert::IsFalse(pmu.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat);

	// Assert on phasors names
	Assert::AreEqual(4, pmu.phasorChnNames.size());
	Assert::AreEqual("P2Ph1", pmu.phasorChnNames[0]);
	Assert::AreEqual("P2Ph2", pmu.phasorChnNames[1]);
	Assert::AreEqual("P2Ph3", pmu.phasorChnNames[2]);
	Assert::AreEqual("P2Ph4", pmu.phasorChnNames[3]);

	// Assert on Analog channel names
	Assert::AreEqual(1, pmu.analogChnNames.size());
	Assert::AreEqual("P2Ang1", pmu.analogChnNames[0]);

	// Assert on digital channel names
	Assert::AreEqual(1, pmu.digitalChnNames.size() );
	Assert::AreEqual("P2Dig1", pmu.digitalChnNames[0]);

	// Assert on phasor unit
	Assert::AreEqual(1, pmu.PhasorUnit[0].Type);
	Assert::AreEqual(77, pmu.PhasorUnit[0].PhasorScalar );

	// Assert on analog unit
	Assert::AreEqual(1, pmu.AnalogUnit[0].Type_X);
	Assert::AreEqual(22, pmu.AnalogUnit[0].AnalogScalar);

	// Assert on digital unit
	Assert::AreEqual(0xFACD, pmu.DigitalUnit[0].DigNormalStatus);
	Assert::AreEqual(0x1, pmu.DigitalUnit[0].DigValidInputs);

	// Assert on nomfreq
	Assert::AreEqual(0, pmu.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60);
}

C37118PdcConfiguration CreateDefaultPdcConfig()
{
	C37118PdcConfiguration pdcCfg;
	pdcCfg.PMUs.push_back(BuildPmu1Config());
	pdcCfg.PMUs.push_back(BuildPmu2Config());
	pdcCfg.DataRate = C37118DataRate::CreateByFramesPerSecond(34);
	return pdcCfg;
}

void EncodeDecodeConfigurationFrame_SerializationLoopbackTest()
{
	char data[1024];
	int offset = 0;

	// encode dataframe into buffer
	C37118PdcConfiguration pdcCfg = CreateDefaultPdcConfig();
	C37118Protocol::WriteConfigurationFrame(data, &pdcCfg, &offset);

	// deserialize from buffer
	C37118PdcConfiguration readBack = C37118Protocol::ReadConfigurationFrame(data,1024);

	// Assert - PDC
	Assert::AreEqual(34, readBack.DataRate.RawDataRate());
	Assert::AreEqual(2, pdcCfg.PMUs.size()); // Number of PMU'S

	// Assert - PMU1
	AssertOnPmuConfig_PMU1(pdcCfg.PMUs[0]);
	AssertOnPmuConfig_PMU2(pdcCfg.PMUs[1]);
}

void EncodeDecodeHeaderFrame_SerializationLoopbackTest()
{
	char buffer[1024];
	C37118PdcHeaderFrame headerFrame;
	headerFrame.HeaderMessage = "This is a testmessage!";
	int writeOffset = 0, readOffset = 0;
	
	// Method under test - serialize+deserialize
	C37118Protocol::WriteHeaderFrame(buffer, &headerFrame, &writeOffset);
	C37118PdcHeaderFrame result = C37118Protocol::ReadHeaderFrame(buffer, writeOffset, &readOffset);
	
	// Assert on header frame content
	Assert::AreEqual(writeOffset,readOffset);
	Assert::AreEqual("This is a testmessage!", result.HeaderMessage );
}

void EncodeDecodeCommandFrame_SerializationLoopbackTest()
{
	char buffer[1024];
	C37118CommandFrame cmdFrame;
	cmdFrame.CmdType = C37118CmdType::SEND_CFG2_FRAME;
	int writeOffset = 0, readOffset = 0;
	
	// Method under test - serialize+deserialize
	C37118Protocol::WriteCommandFrame(buffer, &cmdFrame, &writeOffset);
	C37118CommandFrame result = C37118Protocol::ReadCommandFrame(buffer, writeOffset, &readOffset);

	// Assert 
	Assert::AreEqual(writeOffset,readOffset);
	Assert::AreEqual(C37118CmdType::SEND_CFG2_FRAME,result.CmdType);
}

C37118PmuDataFrame CreateDefaultDataFramePmu1()
{
	// Setup pmu dataframe: PMU1
	C37118PmuDataFrame pmu1;

	// Set up STAT
	pmu1.Stat.setDataErrorCode(0x1);
	pmu1.Stat.setPmuSyncFlag(true);
	pmu1.Stat.setDataSortingFlag(false);
	pmu1.Stat.setPmuTriggerFlag(false);
	pmu1.Stat.setConfigChangeFlag(true);
	pmu1.Stat.setDataModifiedFlag(true);
	pmu1.Stat.setTimeQualityCode(0x0);
	pmu1.Stat.setUnlockTimeCode(0x1);
	pmu1.Stat.setTriggerReasonCode(0x2);

	// Set up phasors
	pmu1.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByRealImag((int16_t)5,(int16_t)6) );
	pmu1.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByRealImag((int16_t)12,(int16_t)3) );
	pmu1.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByRealImag(5.6f,6.23f) );

	// Set FREQ/DFREQ
	pmu1.Frequency = 76.5f;
	pmu1.DeltaFrequency = 11.7f; 

	// Set up analogs
	pmu1.AnalogValues.push_back( C37118PmuDataFrameAnalog::CreateByFloat(35.66f) );
	pmu1.AnalogValues.push_back( C37118PmuDataFrameAnalog::CreateByInt16((int16_t)-65));

	// Set up digitals
	pmu1.DigitalValues.push_back( true );
	pmu1.DigitalValues.push_back( false );

	return pmu1;
}

void AssertOnPmu1DataFrame(C37118PmuDataFrame pmu1)
{
	// Test STAT
	Assert::AreEqual( 0x1, pmu1.Stat.getDataError() );
	Assert::IsTrue(pmu1.Stat.getPmuSyncFlag() );
	Assert::IsFalse(pmu1.Stat.getDataSortingFlag() );
	Assert::IsFalse(pmu1.Stat.getPmuTriggerFlag() );
	Assert::IsTrue(pmu1.Stat.getConfigChangeFlag() );
	Assert::IsTrue(pmu1.Stat.getDataModifiedFlag() );
	Assert::AreEqual(0x0, pmu1.Stat.getTimeQualityCode() );
	Assert::AreEqual(0x1, pmu1.Stat.getUnlockTimeCode() );
	Assert::AreEqual(0x2, pmu1.Stat.getTriggerReasonCode() );

	// Test PHASOR values
	float a, b;
	Assert::AreEqual(3, pmu1.PhasorValues.size());
	pmu1.PhasorValues[0].getRealImagAsFloat(&a, &b); Assert::AreEqual(5,a); Assert::AreEqual(6,b);		// Ph1
	pmu1.PhasorValues[1].getRealImagAsFloat(&a, &b); Assert::AreEqual(12,a); Assert::AreEqual(3,b);		// Ph2
	pmu1.PhasorValues[2].getRealImagAsFloat(&a, &b); Assert::AreEqual(5.6,a); Assert::AreEqual(6.23,b); // Ph3

	// FREQ/DFREQ
	Assert::AreEqual(76.5f, pmu1.Frequency, 0.1f);
	Assert::AreEqual(11.7f, pmu1.DeltaFrequency, 0.1f);

	// Test ANALOG values
	Assert::AreEqual(2, pmu1.AnalogValues.size());
	Assert::AreEqual(35.66f, pmu1.AnalogValues[0].getValueAsFloat() );
	Assert::AreEqual(-65.0f, pmu1.AnalogValues[1].getValueAsFloat() );

	// Test digital values
	Assert::AreEqual(16, pmu1.DigitalValues.size() );
	Assert::IsTrue(pmu1.DigitalValues[0]);
	Assert::IsFalse(pmu1.DigitalValues[1]);
}


C37118PmuDataFrame CreateDefaultDataFramePmu2()
{
	// Setup pmu dataframe: PMU1
	C37118PmuDataFrame pmu2;

	// Set up STAT
	pmu2.Stat.setDataErrorCode(0x2);
	pmu2.Stat.setPmuSyncFlag(true);
	pmu2.Stat.setDataSortingFlag(true);
	pmu2.Stat.setPmuTriggerFlag(true);
	pmu2.Stat.setConfigChangeFlag(false);
	pmu2.Stat.setDataModifiedFlag(false);
	pmu2.Stat.setTimeQualityCode(0x1);
	pmu2.Stat.setUnlockTimeCode(0x3);
	pmu2.Stat.setTriggerReasonCode(0x0);

	// Set up phasors
	pmu2.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByRealImag((int16_t)57,(int16_t)8) );
	pmu2.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByRealImag((int16_t)135,(int16_t)1) );
	pmu2.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByRealImag(52.6f,61.23f) );
	pmu2.PhasorValues.push_back( C37118PmuDataFramePhasorRealImag::CreateByRealImag(522.6f,615.23f) );

	// Freq/dfreq
	pmu2.Frequency = 66.5;
	pmu2.DeltaFrequency = 88.9;

	// Set up analogs
	pmu2.AnalogValues.push_back( C37118PmuDataFrameAnalog::CreateByFloat(325.66f) );

	// Set up digitals
	pmu2.DigitalValues.push_back( false );

	return pmu2;
}

void AssertOnPmu2DataFrame(C37118PmuDataFrame pmu2)
{
	// Test STAT
	Assert::AreEqual( 0x2, pmu2.Stat.getDataError() );
	Assert::IsTrue(pmu2.Stat.getPmuSyncFlag() );
	Assert::IsTrue(pmu2.Stat.getDataSortingFlag() );
	Assert::IsTrue(pmu2.Stat.getPmuTriggerFlag() );
	Assert::IsFalse(pmu2.Stat.getConfigChangeFlag() );
	Assert::IsFalse(pmu2.Stat.getDataModifiedFlag() );
	Assert::AreEqual(0x1, pmu2.Stat.getTimeQualityCode() );
	Assert::AreEqual(0x3, pmu2.Stat.getUnlockTimeCode() );
	Assert::AreEqual(0x0, pmu2.Stat.getTriggerReasonCode() );

	// Test PHASOR values
	float a, b;
	Assert::AreEqual(4, pmu2.PhasorValues.size());
	pmu2.PhasorValues[0].getRealImagAsFloat(&a, &b); Assert::AreEqual(57.0f,a, 1.0f); Assert::AreEqual(8.0f,b, 1.0f);		// Ph1
	pmu2.PhasorValues[1].getRealImagAsFloat(&a, &b); Assert::AreEqual(135.0f,a, 1.0f); Assert::AreEqual(1.0f,b, 1.0f);		// Ph2
	pmu2.PhasorValues[2].getRealImagAsFloat(&a, &b); Assert::AreEqual(52.6f,a, 1.0f); Assert::AreEqual(61.23f,b, 1.0f);		// Ph3
	pmu2.PhasorValues[3].getRealImagAsFloat(&a, &b); Assert::AreEqual(522.6f,a, 1.0f); Assert::AreEqual(615.23f,b, 1.0f);	// Ph4

	// Test FREQ/DFREQ
	Assert::AreEqual(66, pmu2.Frequency);
	Assert::AreEqual(88.9, pmu2.DeltaFrequency, 0.1f);

	// Test ANALOG values
	Assert::AreEqual(1, pmu2.AnalogValues.size());
	Assert::AreEqual(325.66f, pmu2.AnalogValues[0].getValueAsFloat() );

	// Test digital values
	Assert::AreEqual(16, pmu2.DigitalValues.size() );
	Assert::IsFalse(pmu2.DigitalValues[0]);
}



void EncodeDecodeDataFrame_SerializationLoopbackTest()
{
	char buffer[1024];
	C37118PdcConfiguration pdcCfg = CreateDefaultPdcConfig();
	C37118PdcDataDecodeInfo decodeInfo = C37118Protocol::CreateDecodeInfoByPdcConfig(pdcCfg);
	int writeOffset = 0, readOffset = 0;	

	// Setup PDC dtaframe
	C37118PdcDataFrame inframe;
	inframe.pmuDataFrame.push_back(CreateDefaultDataFramePmu1());
	inframe.pmuDataFrame.push_back(CreateDefaultDataFramePmu2());

	// Method under test - serialize, deserialize dataframes
	C37118Protocol::WriteDataFrame(buffer, &decodeInfo, &inframe, &writeOffset );
	C37118PdcDataFrame outframe = C37118Protocol::ReadDataFrame(buffer, writeOffset, &decodeInfo, &readOffset );

	// Assert - input and output frames should be equal
	Assert::AreEqual(writeOffset, readOffset);
	Assert::AreEqual(2, outframe.pmuDataFrame.size() );
	AssertOnPmu1DataFrame(outframe.pmuDataFrame[0] );
	AssertOnPmu2DataFrame(outframe.pmuDataFrame[1] );
}


C37118PmuConfiguration_Ver3 BuildPmu1Config_Ver3()
{
	C37118PmuConfiguration_Ver3 pmu1;
	pmu1.IdCode = 12;
	memset(pmu1.GlobalPmuId,0xAA, 16);
	pmu1.StationName = "PMU_NUMERO_UNO_VER3";

	// dataformat
	pmu1.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle = 0;
	pmu1.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat = 1;
	pmu1.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat = 0;
	pmu1.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat = 1;

	// Phasors 
	pmu1.phasorChnNames.push_back("Ph1");
	pmu1.phasorChnNames.push_back("Ph2");
	pmu1.phasorChnNames.push_back("Ph3");

	// Analog
	pmu1.analogChnNames.push_back("Ang1");
	pmu1.analogChnNames.push_back("Ang2");

	// Digital chns
	pmu1.digitalChnNames.push_back("Dig1");
	pmu1.digitalChnNames.push_back("Dig2");
	pmu1.digitalChnNames.push_back("Dig3");
	pmu1.digitalChnNames.push_back("Dig4");

	// Set phasor scales
	pmu1.PhasorScales.push_back( C37118PhasorScale_Ver3(0, PhasorComponentCodeEnum::PHC4_PHASE_A, 55.7f,63.4f ) );
	pmu1.PhasorScales.push_back( C37118PhasorScale_Ver3(0, PhasorComponentCodeEnum::PHC4_PHASE_A, 55.7f,63.4f ) );
	pmu1.PhasorScales.push_back( C37118PhasorScale_Ver3(0, PhasorComponentCodeEnum::PHC4_PHASE_A, 55.7f,63.4f ) );

	// Set analog unit
	pmu1.AnalogScales.push_back( C37118AnalogScale_Ver3(34.2f, 74.3f) );
	pmu1.AnalogScales.push_back( C37118AnalogScale_Ver3(34.2f, 74.3f) );

	// Set digital unit
	pmu1.DigitalUnits.push_back( C37118DigitalUnit(0x0000, 0x3 ) ); // first 2 bits

	// Position lat / lon / elev
	pmu1.POS_LAT = 54.2f;
	pmu1.POS_LON = 85.1f;
	pmu1.POS_ELEV = 13.3f;

	// Srvc class
	pmu1.ServiceClass = 'g';

	// Phasor measurement window/delay 
	pmu1.PhasorMeasurementWindow = 5342;
	pmu1.PhasorMeasurementGroupDelayMs = 32423;
	
	// Nominal frequency
	pmu1.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60 = 0x1;

	pmu1.ConfChangeCnt = 2;

	return pmu1;
}


void AssertOnPmuConfig_PMU1_Ver3(C37118PmuConfiguration_Ver3 pmu)
{
	Assert::AreEqual("PMU_NUMERO_UNO_VER3", pmu.StationName );

	// dataformat
	Assert::IsFalse(pmu.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle);
	Assert::IsTrue(pmu.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat);
	Assert::IsFalse(pmu.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat);
	Assert::IsTrue(pmu.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat);

	// Assert on phasors names
	Assert::AreEqual(3, pmu.phasorChnNames.size());
	Assert::AreEqual("Ph1", pmu.phasorChnNames[0]);
	Assert::AreEqual("Ph2", pmu.phasorChnNames[1]);
	Assert::AreEqual("Ph3", pmu.phasorChnNames[2]);

	// Assert on Analog channel names
	Assert::AreEqual(2, pmu.analogChnNames.size());
	Assert::AreEqual("Ang1", pmu.analogChnNames[0]);
	Assert::AreEqual("Ang2", pmu.analogChnNames[1]);

	// Assert on digital channel names
	Assert::AreEqual(4, pmu.digitalChnNames.size() );
	Assert::AreEqual("Dig1", pmu.digitalChnNames[0]);
	Assert::AreEqual("Dig2", pmu.digitalChnNames[1]);
	Assert::AreEqual("Dig3", pmu.digitalChnNames[2]);
	Assert::AreEqual("Dig4", pmu.digitalChnNames[3]);

	// Assert on phasor scale
	Assert::AreEqual(0, pmu.PhasorScales[0].PhasorBits);
	Assert::AreEqual((int)PhasorComponentCodeEnum::PHC4_PHASE_A, (int)pmu.PhasorScales[0].PhasorComponentCode );
	Assert::AreEqual(0, pmu.PhasorScales[0].VoltOrCurrent);
	Assert::AreEqual(55.7f, pmu.PhasorScales[0].ScaleFactorOne_Y, 0.1f );
	Assert::AreEqual(63.4f, pmu.PhasorScales[0].ScaleFactorTwo_Angle, 0.1f );

	// Assert on analog unit
	Assert::AreEqual(34.2f, pmu.AnalogScales[0].Scale);
	Assert::AreEqual(74.3f, pmu.AnalogScales[0].Offset);

	// Assert on digital unit
	Assert::AreEqual(0, pmu.DigitalUnits[0].DigNormalStatus);
	Assert::AreEqual(0x3, pmu.DigitalUnits[0].DigValidInputs);

	// position
	Assert::AreEqual(54.2f, pmu.POS_LAT, 0.1f );
	Assert::AreEqual(85.1f, pmu.POS_LON, 0.1f );
	Assert::AreEqual(13.3f, pmu.POS_ELEV, 0.1f );

	// service class
	Assert::AreEqual('g', pmu.ServiceClass );

	// phasor measurement window/delay
	Assert::AreEqual(5342, pmu.PhasorMeasurementWindow );
	Assert::AreEqual(32423, pmu.PhasorMeasurementGroupDelayMs);

	// Assert on nomfreq
	Assert::AreEqual(1, pmu.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60);
}


C37118PmuConfiguration_Ver3 BuildPmu2Config_Ver3()
{
	C37118PmuConfiguration_Ver3 pmu1;
	pmu1.IdCode = 12;
	memset(pmu1.GlobalPmuId,0xAA, 16);
	pmu1.StationName = "PMU_NUMERO_ZWEI_VER3";

	// dataformat
	pmu1.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle = 1;
	pmu1.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat = 0;
	pmu1.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat = 1;
	pmu1.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat = 0;

	// Phasors 
	pmu1.phasorChnNames.push_back("Ph1");
	pmu1.phasorChnNames.push_back("Ph2");
	pmu1.phasorChnNames.push_back("Ph3");

	// Analog
	pmu1.analogChnNames.push_back("Ang1");
	pmu1.analogChnNames.push_back("Ang2");

	// Digital chns
	pmu1.digitalChnNames.push_back("Dig1");
	pmu1.digitalChnNames.push_back("Dig2");
	pmu1.digitalChnNames.push_back("Dig3");
	pmu1.digitalChnNames.push_back("Dig4");

	// Set phasor scale
	pmu1.PhasorScales.push_back( C37118PhasorScale_Ver3(1, PhasorComponentCodeEnum::PHC1_POSITIVE_SEQUENCE, 55.7f, 63.4f ) );
	pmu1.PhasorScales.push_back( C37118PhasorScale_Ver3(1, PhasorComponentCodeEnum::PHC1_POSITIVE_SEQUENCE, 55.7f, 63.4f ) );
	pmu1.PhasorScales.push_back( C37118PhasorScale_Ver3(1, PhasorComponentCodeEnum::PHC1_POSITIVE_SEQUENCE, 55.7f, 63.4f ) );

	// Set analog unit
	pmu1.AnalogScales.push_back( C37118AnalogScale_Ver3(34.2f, 74.3f) );
	pmu1.AnalogScales.push_back( C37118AnalogScale_Ver3(34.2f, 74.3f) );

	// Set digital unit
	pmu1.DigitalUnits.push_back( C37118DigitalUnit(0x0000, 0X3) );

	// Position lat / lon / elev
	pmu1.POS_LAT = 54.2f;
	pmu1.POS_LON = 85.1f;
	pmu1.POS_ELEV = 13.3f;

	// Srvc class
	pmu1.ServiceClass = 'c';

	// Phasor measurement window/delay 
	pmu1.PhasorMeasurementWindow = 543;
	pmu1.PhasorMeasurementGroupDelayMs = 777;
	
	// Nominal frequency
	pmu1.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60 = 0x0;

	pmu1.ConfChangeCnt = 2;

	return pmu1;
}


void AssertOnPmuConfig_PMU2_Ver3(C37118PmuConfiguration_Ver3 pmu)
{
	Assert::AreEqual("PMU_NUMERO_ZWEI_VER3", pmu.StationName );

	// dataformat
	Assert::IsTrue(pmu.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle);
	Assert::IsFalse(pmu.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat);
	Assert::IsTrue(pmu.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat);
	Assert::IsFalse(pmu.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat);

	// Assert on phasors names
	Assert::AreEqual(3, pmu.phasorChnNames.size());
	Assert::AreEqual("Ph1", pmu.phasorChnNames[0]);
	Assert::AreEqual("Ph2", pmu.phasorChnNames[1]);
	Assert::AreEqual("Ph3", pmu.phasorChnNames[2]);

	// Assert on Analog channel names
	Assert::AreEqual(2, pmu.analogChnNames.size());
	Assert::AreEqual("Ang1", pmu.analogChnNames[0]);
	Assert::AreEqual("Ang2", pmu.analogChnNames[1]);

	// Assert on digital channel names
	Assert::AreEqual(4, pmu.digitalChnNames.size() );
	Assert::AreEqual("Dig1", pmu.digitalChnNames[0]);
	Assert::AreEqual("Dig2", pmu.digitalChnNames[1]);
	Assert::AreEqual("Dig3", pmu.digitalChnNames[2]);
	Assert::AreEqual("Dig4", pmu.digitalChnNames[3]);

	// Assert on phasor scale
	Assert::AreEqual(0, pmu.PhasorScales[0].PhasorBits);
	Assert::AreEqual(1, pmu.PhasorScales[0].VoltOrCurrent);
	Assert::AreEqual((int)PhasorComponentCodeEnum::PHC1_POSITIVE_SEQUENCE, (int)pmu.PhasorScales[0].PhasorComponentCode );
	Assert::AreEqual(55.7f, pmu.PhasorScales[0].ScaleFactorOne_Y, 0.1f );
	Assert::AreEqual(63.4f, pmu.PhasorScales[0].ScaleFactorTwo_Angle, 0.1f );

	// Assert on analog unit
	Assert::AreEqual(34.2f, pmu.AnalogScales[0].Scale);
	Assert::AreEqual(74.3f, pmu.AnalogScales[0].Offset);

	// Assert on digital unit
	Assert::AreEqual(0, pmu.DigitalUnits[0].DigNormalStatus);
	Assert::AreEqual(0x3, pmu.DigitalUnits[0].DigValidInputs);

	// position
	Assert::AreEqual(54.2f, pmu.POS_LAT, 0.1f );
	Assert::AreEqual(85.1f, pmu.POS_LON, 0.1f );
	Assert::AreEqual(13.3f, pmu.POS_ELEV, 0.1f );

	// service class
	Assert::AreEqual('c', pmu.ServiceClass );

	// phasor measurement window/delay
	Assert::AreEqual(543, pmu.PhasorMeasurementWindow );
	Assert::AreEqual(777, pmu.PhasorMeasurementGroupDelayMs);

	// Assert on nomfreq
	Assert::AreEqual(0, pmu.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60);
}

C37118PdcConfiguration_Ver3 CreateDefaultPdcConfigVer3()
{
	C37118PdcConfiguration_Ver3 pdcCfg;
	pdcCfg.ContinuationIndex = C37118ContIdx::CreateAsFrameInSequence(1,1);	
	pdcCfg.TimeBase.Flags = 0xAA;
	pdcCfg.TimeBase.TimeBase = 100;
	pdcCfg.PMUs.push_back(BuildPmu1Config_Ver3());
	pdcCfg.PMUs.push_back(BuildPmu2Config_Ver3());
	pdcCfg.DataRate = C37118DataRate::CreateByFramesPerSecond(0.5);
	return pdcCfg;
}

void EncodeDecodeConfigurationFrame_Ver3_SerializationLoopbackTest()
{
	char data[1024];
	int offset = 0;

	// encode dataframe into buffer
	C37118PdcConfiguration_Ver3 pdcCfg = CreateDefaultPdcConfigVer3();
	C37118Protocol::WriteConfigurationFrame_Ver3(data, &pdcCfg, &offset);

	// deserialize from buffer
	C37118PdcConfiguration_Ver3 readBack = C37118Protocol::ReadConfigurationFrame_Ver3(data,1024);

	// Assert - PDC
	Assert::AreEqual(0.5f, readBack.DataRate.FramesPerSecond());
	Assert::AreEqual(2, pdcCfg.PMUs.size()); // Number of PMU'S

	// Assert - PMU1
	AssertOnPmuConfig_PMU1_Ver3(pdcCfg.PMUs[0]);
	AssertOnPmuConfig_PMU2_Ver3(pdcCfg.PMUs[1]);
}

C37118ProtocolTests::C37118ProtocolTests()
{
	PushTestMethod("EncodeDecodeConfigurationFrame_SerializationLoopbackTest", &EncodeDecodeConfigurationFrame_SerializationLoopbackTest);
	PushTestMethod("EncodeDecodeHeaderFrame_SerializationLoopbackTest", &EncodeDecodeHeaderFrame_SerializationLoopbackTest);
	PushTestMethod("EncodeDecodeCommandFrame_SerializationLoopbackTest", &EncodeDecodeCommandFrame_SerializationLoopbackTest);
	PushTestMethod("EncodeDecodeDataFrame_SerializationLoopbackTest", &EncodeDecodeDataFrame_SerializationLoopbackTest);
	PushTestMethod("EncodeDecodeConfigurationFrame_Ver3_SerializationLoopbackTest", &EncodeDecodeConfigurationFrame_Ver3_SerializationLoopbackTest);
}

std::string C37118ProtocolTests::TestClassName()
{
	return "C37118ProtocolTests";
}

