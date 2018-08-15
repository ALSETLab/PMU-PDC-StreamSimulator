#include "stdafx.h"
#include "ConstantChannelDataFeed.h"

using namespace strongridgui;

// --------------------------------------------------------
// -------- ConstantPhasorDataFeed -----------------------
// --------------------------------------------------------

ConstantPhasorDataFeed::ConstantPhasorDataFeed()
{
	m_real = 0;
	m_imag = 0;
}

void ConstantPhasorDataFeed::SetRealImag( float real, float imag )
{
	m_real = real;
	m_imag = imag;
}

void ConstantPhasorDataFeed::GetRealImag( float* real, float* imag )
{
	*real = m_real;
	*imag = m_imag;
}
	
void ConstantPhasorDataFeed::GetPhasorValue( float* part1, float* part2 )
{
	*part1 = m_real;
	*part2 = m_imag;
}

void ConstantPhasorDataFeed::SignalNextFrame()
{
}


	
// --------------------------------------------------------
// -------- ConstantAnalogDataFeed -----------------------
// --------------------------------------------------------


ConstantAnalogDataFeed::ConstantAnalogDataFeed()
{
	m_value = 0;
}

void ConstantAnalogDataFeed::SetAnalogValue( float val )
{
	m_value = val;
}

void ConstantAnalogDataFeed::GetAnalogValue( float* val )
{
	*val = m_value;
}

void ConstantAnalogDataFeed::SignalNextFrame()
{
}


	
// --------------------------------------------------------
// -------- ConstantDigitalDataFeed -----------------------
// --------------------------------------------------------


ConstantDigitalDataFeed::ConstantDigitalDataFeed()
{
	m_bitIsSet = false;
}

void ConstantDigitalDataFeed::SetDigitalVal( bool a )
{
	m_bitIsSet = a;//changed from b
}


bool ConstantDigitalDataFeed::GetDigitalStatus()
{
	return m_bitIsSet;
}

void ConstantDigitalDataFeed::SignalNextFrame()
{
}
