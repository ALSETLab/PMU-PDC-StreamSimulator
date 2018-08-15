#include "stdafx.h"
#include "SimpleConstantDataFeed.h"

using namespace strongridgui;


double SimpleConstantPdcDataFeed::GetSOC()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	return (double)time(0) + ((float)st.wMilliseconds / 1000.0f);
}

C37118FracSec SimpleConstantPdcDataFeed::GetFracSec()
{
	return C37118FracSec::Create(0, 0, false, 0);
}


// ----------------------------------------------------------------------
// -------------------------- PMU datafeed ------------------------------
// ----------------------------------------------------------------------

void SimpleConstantPmuDataFeed::GetPhasorValue( int phIdx, float* part1, float* part2 )
{
	*part1 = 0;
	*part2 = 0;
}

float SimpleConstantPmuDataFeed::GetAnalogValue( int angIdx )
{
	return 0;
}

bool SimpleConstantPmuDataFeed::GetDigitalBit( int digIdx )
{
	return false;
}

void SimpleConstantPmuDataFeed::GetFrequency( float* frequency, float* deltafrequency )
{

}

C37118PmuDataFrameStat SimpleConstantPmuDataFeed::GetFrameStat()
{
	return m_frameStat;
}
		
void SimpleConstantPmuDataFeed::SignalNextFrame()
{

}
