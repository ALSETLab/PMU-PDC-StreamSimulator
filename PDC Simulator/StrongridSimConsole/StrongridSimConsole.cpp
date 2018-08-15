#include <iostream>
#include "../StrongridSimBase/PDC.h"
#include "../StrongridSimBase/PMU.h"
#include "../StrongridBase/EncDec.h"
#include "../StrongridBase/C37118Protocol.h"
#include "ConstantDataFeed.h"

using namespace strongridsim;

C37118PmuConfiguration_Ver3 BuildPmu1Config()
{
	C37118PmuConfiguration_Ver3 pmu1;
	pmu1.IdCode = 12;
	pmu1.StationName = "PMU_NUMERO_UNO";

	// dataformat
	pmu1.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle = 0;
	pmu1.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat = 1;
	pmu1.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat = 1;
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
	pmu1.PhasorScales.push_back( C37118PhasorScale_Ver3( 1, PhasorComponentCodeEnum::PHC2_NEGATIVE_SEQUENCE, 123.0f, 93.9f) );
	pmu1.PhasorScales.push_back( C37118PhasorScale_Ver3( 1, PhasorComponentCodeEnum::PHC2_NEGATIVE_SEQUENCE, 123.0f, 93.9f) );
	pmu1.PhasorScales.push_back( C37118PhasorScale_Ver3( 1, PhasorComponentCodeEnum::PHC2_NEGATIVE_SEQUENCE, 123.0f, 93.9f) );

	// Set analog unit
	pmu1.AnalogScales.push_back( C37118AnalogScale_Ver3( 32.7f, 445.3f ) );
	pmu1.AnalogScales.push_back( C37118AnalogScale_Ver3( 32.7f, 445.3f ) );

	// Set digital unit
	pmu1.DigitalUnits.push_back( C37118DigitalUnit(0x0000, 0x3) ); // first 2 bits

	// POS
	pmu1.POS_LAT = 100001.1;
	pmu1.POS_LON = 100002.2;
	pmu1.POS_ELEV = 100003.3;

	pmu1.PhasorMeasurementWindow = 66;
	pmu1.PhasorMeasurementGroupDelayMs = 77;

	pmu1.ServiceClass = 'g';
	
	// Nominal frequency
	pmu1.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60 = 0x1;

	// config count
	pmu1.ConfChangeCnt = 2;

	return pmu1;
}

enum ProtVer
{
	VERSION_1 = 1,
	VERSION_2 = 2
};

C37118FrameHeader CreateDefaultHeader(int pmuId, C37118HdrFrameType type, ProtVer ver)
{
	C37118FrameHeader tmp;
	tmp.Sync.FrameType = type;
	tmp.Sync.LeadIn = 0xAA;
	tmp.Sync.Version = (int)ver;
	tmp.SOC = 0;
	tmp.IdCode = pmuId;
	tmp.FracSec.FractionOfSecond = 0;
	tmp.FracSec.TimeQuality = 0;
	return tmp;
}

void StartPDC_Ver3( int listenPort )
{
	C37118PmuConfiguration_Ver3 pmu1cfg = BuildPmu1Config();

	// Setup PDC/PMU configuration
	C37118PdcConfiguration_Ver3 pdcConfig;
	pdcConfig.HeaderCommon = CreateDefaultHeader(1, C37118HdrFrameType::CONFIGURATION_FRAME_3, ProtVer::VERSION_1 );
	pdcConfig.DataRate = C37118DataRate::CreateByFramesPerSecond(50);
	pdcConfig.TimeBase.Flags = 0;
	pdcConfig.TimeBase.TimeBase = 100;
	pdcConfig.PMUs.push_back(pmu1cfg);

	// Setup PDC header message
	C37118PdcHeaderFrame pdcHeader;
	pdcHeader.Header = CreateDefaultHeader(1, C37118HdrFrameType::HEADER_FRAME, ProtVer::VERSION_1 );
	pdcHeader.HeaderMessage = "This is a header message";

	// C37118FracSec
	C37118FracSec fracSec = C37118FracSec::Create(0, 0, false, 0);

	// PMU stat
	C37118PmuDataFrameStat stat;

	// Set up PMU object and aggregate to list
	ConstantDataFeed* pmuAndPdcFeed = new ConstantDataFeed(fracSec, stat);
	std::vector<PMU*> pmuList;
	PMU* pmu1 = new PMU(pmu1cfg, pmuAndPdcFeed);	
	pmuList.push_back(pmu1);

	// Create and initialize PDC object
	PDC* pdcObject = new PDC(pdcConfig, pdcHeader, listenPort, pmuList, pmuAndPdcFeed);
	pdcObject->Start();
}



C37118PmuConfiguration BuildPmuConfig_Ver2Standard()
{
	C37118PmuConfiguration pmu1;
	pmu1.IdCode = 12;
	pmu1.StationName = "PMU_NUMERO_UNO";

	// dataformat
	pmu1.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle = 0;
	pmu1.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat = 1;
	pmu1.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat = 1;
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
	pmu1.PhasorUnit.push_back( C37118PhasorUnit( 1, 1000 ) );
	pmu1.PhasorUnit.push_back( C37118PhasorUnit( 1, 1500 ) );
	pmu1.PhasorUnit.push_back( C37118PhasorUnit( 1, 2000 ) );

	// Set analog unit
	pmu1.AnalogUnit.push_back( C37118AnalogUnit( 0, 415.3f ) );
	pmu1.AnalogUnit.push_back( C37118AnalogUnit( 1, 425.3f ) );

	// Set digital unit
	pmu1.DigitalUnit.push_back( C37118DigitalUnit(0x0000, 0x3) ); // first 2 bits (normal == 0 on both)
		
	// Nominal frequency
	pmu1.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60 = 0x1;

	// config count
	pmu1.ConfChangeCnt = 4;

	return pmu1;
}

C37118PmuConfiguration BuildPmuConfig_Ver2Opposite()
{
	C37118PmuConfiguration pmu1;
	pmu1.IdCode = 21;
	pmu1.StationName = "PMU_NUMERO_ZWEI";

	// dataformat
	pmu1.DataFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle = 1;
	pmu1.DataFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat = 0;
	pmu1.DataFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat = 0;
	pmu1.DataFormat.Bit3_0xFreqIsInt_1xFreqIsFloat = 0;

	// Phasors 
	pmu1.phasorChnNames.push_back("Ph1");
	pmu1.phasorChnNames.push_back("Ph2");

	// Analog
	pmu1.analogChnNames.push_back("Ang1");
	pmu1.analogChnNames.push_back("Ang2");
	pmu1.analogChnNames.push_back("Ang3");

	// Digital chns
	pmu1.digitalChnNames.push_back("Dig1");
	pmu1.digitalChnNames.push_back("Dig2");
	pmu1.digitalChnNames.push_back("Dig4");
	pmu1.digitalChnNames.push_back("Dig5");
	pmu1.digitalChnNames.push_back("Dig6");

	// Set phasor unit 
	pmu1.PhasorUnit.push_back( C37118PhasorUnit(0, 5 ) );
	pmu1.PhasorUnit.push_back( C37118PhasorUnit(0, 10 ) );

	// Set analog unit
	pmu1.AnalogUnit.push_back( C37118AnalogUnit( 2, 15.3f ) );
	pmu1.AnalogUnit.push_back( C37118AnalogUnit( 1, 25.3f ) );
	pmu1.AnalogUnit.push_back( C37118AnalogUnit( 3, 35.3f ) );

	// Set digital unit
	pmu1.DigitalUnit.push_back( C37118DigitalUnit(0xFF, 0x3F) ); // first 6 bits

	// Nominal frequency
	pmu1.NomFreqCode.Bit0_1xFreqIs50_0xFreqIs60 = 0x0; // 60 hz

	// config count
	pmu1.ConfChangeCnt = 1;

	return pmu1;
}

void StartPDC_Ver2( int listenPort )
{
	C37118PmuConfiguration pmu1cfg = BuildPmuConfig_Ver2Standard();
	C37118PmuConfiguration pmu2cfg = BuildPmuConfig_Ver2Opposite();

	// Setup PDC/PMU configuration
	C37118PdcConfiguration pdcConfig;
	pdcConfig.HeaderCommon = CreateDefaultHeader(1, C37118HdrFrameType::CONFIGURATION_FRAME_2, ProtVer::VERSION_1 );
	pdcConfig.DataRate = C37118DataRate::CreateByFramesPerSecond(50);
	pdcConfig.TimeBase.Flags = 0;
	pdcConfig.TimeBase.TimeBase = 100;
	pdcConfig.PMUs.push_back(pmu1cfg);
	pdcConfig.PMUs.push_back(pmu2cfg);

	// Setup PDC header message
	C37118PdcHeaderFrame pdcHeader;
	pdcHeader.Header = CreateDefaultHeader(1, C37118HdrFrameType::HEADER_FRAME, ProtVer::VERSION_1 );
	pdcHeader.HeaderMessage = "This is a header message";
		
	// C37118FracSec
	C37118FracSec fracSec = C37118FracSec::Create(0, 0, false, 0);

	// PMU stat
	C37118PmuDataFrameStat stat;
	stat.setDataErrorCode(0x2);
	stat.setPmuSyncFlag(false);
	stat.setDataSortingFlag(true);
	stat.setPmuTriggerFlag(true);
	stat.setConfigChangeFlag(false);
	stat.setDataModifiedFlag(true);
	stat.setTimeQualityCode(false);
	stat.setUnlockTimeCode(false);
	stat.setTriggerReasonCode(false);

	// Set up PMU object and aggregate to list
	ConstantDataFeed* pmuAndPdcFeed = new ConstantDataFeed(fracSec, stat);
	std::vector<PMU*> pmuList;
	PMU* pmu1 = new PMU(pmu1cfg, pmuAndPdcFeed);	
	pmuList.push_back(pmu1);
	PMU* pmu2 = new PMU(pmu2cfg, pmuAndPdcFeed);	
	pmuList.push_back(pmu2);

	// Create and initialize PDC object
	PDC* pdcObject = new PDC(pdcConfig, pdcHeader, listenPort, pmuList, pmuAndPdcFeed);
	pdcObject->Start();
}

const int NUM_PDC_INSTANCES = 1;

int main()
{
	for( int i = 0; i < NUM_PDC_INSTANCES; i += 2 ) {
		StartPDC_Ver2(12400 + i + 0);
		StartPDC_Ver3(12400 + i + 1);
	}

	std::cout << "Listening for connections on port " << 12400 << " to " << 12400+NUM_PDC_INSTANCES << std::endl;

	while( true ) std::cin.get();
	return 0;
}
