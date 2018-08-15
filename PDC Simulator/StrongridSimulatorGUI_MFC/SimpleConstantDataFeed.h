#include "stdafx.h"
#include "../StrongridSimBase/IPdcDataFeed.h"
#include "../StrongridSimBase/IPmuDataFeed.h"

using namespace strongridsim;

namespace strongridgui
{
	class SimpleConstantPdcDataFeed : public IPdcDataFeed
	{
	public:
		virtual double GetSOC();
		virtual C37118FracSec GetFracSec();
	};

	class SimpleConstantPmuDataFeed : public IPmuDataFeed
	{
	public:
		virtual void GetPhasorValue( int phIdx, float* part1, float* part2 );
		virtual float GetAnalogValue( int angIdx );
		virtual bool GetDigitalBit( int digIdx );
		virtual void GetFrequency( float* frequency, float* deltafrequency );
		virtual C37118PmuDataFrameStat GetFrameStat();
		
		virtual void SignalNextFrame();

	private:
		C37118PmuDataFrameStat m_frameStat;
	};
}