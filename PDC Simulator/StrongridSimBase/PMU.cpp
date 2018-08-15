#include "PMU.h"
#include "../StrongridBase/common.h"

using namespace strongridsim;

PMU::PMU( C37118PmuConfiguration pmuCfg, IPmuDataFeed* feed )
{
	if( feed == 0 ) throw Exception("Datafeed cannot be NULL!");
	m_cfg = pmuCfg;
	m_feed = feed;
}

PMU::PMU( C37118PmuConfiguration_Ver3 pmuCfg, IPmuDataFeed* feed )
{
	if( feed == 0 ) throw Exception("Datafeed cannot be NULL!");
	m_cfg = C37118Protocol::DowngradePmuConfig(&pmuCfg);
	m_feed = feed;
}

PMU::~PMU()
{
}

C37118PmuDataFrame PMU::CreatePmuDataFrame()
{
	C37118PmuDataFrame frame;

	frame.Stat = m_feed->GetFrameStat();

	// Phasors
	for( int i = 0; i < m_cfg.phasorChnNames.size(); ++i )
	{
		float a, b;
		m_feed->GetPhasorValue(i, &a, &b );
		frame.PhasorValues.push_back(C37118PmuDataFramePhasorRealImag::CreateByRealImag(a,b));
	}

	// Analogs
	for( int i = 0; i < m_cfg.analogChnNames.size(); ++i )
	{
		frame.AnalogValues.push_back(C37118PmuDataFrameAnalog::CreateByFloat(m_feed->GetAnalogValue(i)));
	}

	// Digitals
	for( int i = 0; i < m_cfg.digitalChnNames.size(); ++i )
	{
		frame.DigitalValues.push_back(m_feed->GetDigitalBit(i));
	}

	// Set frequency/deltafrequency
	float freq, dfreq;
	m_feed->GetFrequency(&freq, &dfreq);
	frame.Frequency = freq;
	frame.DeltaFrequency = dfreq;

	// Signal next frame
	m_feed->SignalNextFrame();

	return frame;
}
