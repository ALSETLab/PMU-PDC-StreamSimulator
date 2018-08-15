#pragma once
#include "../StrongridBase/C37118Protocol.h"

using namespace strongridbase;

namespace strongridsim
{
	class IPmuDataFeed
	{
	public:
		virtual void GetPhasorValue( int phIdx, float* part1, float* part2 ) = 0;
		virtual float GetAnalogValue( int angIdx ) = 0;
		virtual bool GetDigitalBit( int digIdx ) = 0;
		virtual void GetFrequency( float* frequency, float* deltafrequency ) = 0;
		virtual C37118PmuDataFrameStat GetFrameStat() = 0;
		
		virtual void SignalNextFrame() = 0;
	};
}