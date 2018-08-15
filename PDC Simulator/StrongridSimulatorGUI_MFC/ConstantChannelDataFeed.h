#pragma once
#include "stdafx.h"
#include "ChannelDataFeed.h"

namespace strongridgui 
{	
	class ConstantPhasorDataFeed : public IPhasorDataFeed
	{
	public:
		ConstantPhasorDataFeed();
		void SetRealImag( float real, float imag );
		void GetRealImag( float* real, float* imag );

		virtual void GetPhasorValue( float* part1, float* part2 );
		virtual void SignalNextFrame();

	private:
		volatile float m_real, m_imag;
	};

	class ConstantAnalogDataFeed : public IAnalogDataFeed
	{
	public:
		ConstantAnalogDataFeed();
		void SetAnalogValue( float val );

		virtual void GetAnalogValue( float* val );
		virtual void SignalNextFrame();

	private:
		volatile float m_value;
	};

	class ConstantDigitalDataFeed : public IDigitalDataFeed
	{
	public:
		ConstantDigitalDataFeed();
		void SetDigitalVal( bool b );

		virtual bool GetDigitalStatus();
		virtual void SignalNextFrame();

	private:
		volatile bool m_bitIsSet;
	};
}