#pragma once

#include "../StrongridSimBase/IPmuDataFeed.h"
#include "../StrongridSimBase/IPdcDataFeed.h"

namespace strongridsim
{
	class ConstantDataFeed : public IPmuDataFeed, public IPdcDataFeed 
	{
	public:
		ConstantDataFeed(C37118FracSec fracSec, C37118PmuDataFrameStat stat) 
		{
			m_counter = 0; 
			m_fracSec = fracSec;
			m_frameStat = stat;
		}

		virtual void GetPhasorValue( int phIdx, float* part1, float* part2 );
		virtual float GetAnalogValue( int angIdx );
		virtual bool GetDigitalBit( int digIdx );
		virtual void GetFrequency( float* frequency, float* deltafrequency );
		virtual C37118PmuDataFrameStat GetFrameStat();
		
		virtual void SignalNextFrame(); // ONCE PER PMU
		virtual double GetSOC();		// ONCE PER PDC
		virtual C37118FracSec GetFracSec();

	private:
		int m_counter;
		C37118FracSec m_fracSec;
		C37118PmuDataFrameStat m_frameStat;
	};
}