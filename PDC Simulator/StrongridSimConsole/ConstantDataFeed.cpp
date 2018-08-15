#include <windows.h>
#include "../StrongridBase/common.h"
#include "../StrongridBase/C37118Protocol.h"
#include "ConstantDataFeed.h"

using namespace strongridsim;
using namespace strongridbase;

void ConstantDataFeed::GetPhasorValue( int phIdx, float* part1, float* part2 )
{
	*part1 = 4 + (m_counter % 2);
	*part2 = 6 + (m_counter % 2);
}

float ConstantDataFeed::GetAnalogValue( int angIdx )
{
	return (angIdx+1) * 5;
}

bool ConstantDataFeed::GetDigitalBit( int digIdx )
{
	return (digIdx % 2) == 0;
}

void ConstantDataFeed::GetFrequency( float* frequency, float* deltafrequency )
{
	*frequency = 43;
	*deltafrequency = 4.3f;
}

void ConstantDataFeed::SignalNextFrame()
{

}

double ConstantDataFeed::GetSOC()
{
	m_counter++;

	SYSTEMTIME st;
	GetSystemTime(&st);

	return (double)time(0) + ((float)st.wMilliseconds / 1000.0f);
}

C37118FracSec ConstantDataFeed::GetFracSec()
{
	return m_fracSec;		
}

C37118PmuDataFrameStat ConstantDataFeed::GetFrameStat()
{
	return m_frameStat;
}