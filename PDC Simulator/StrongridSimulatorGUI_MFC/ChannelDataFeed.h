#pragma once
#include "stdafx.h"

namespace strongridgui 
{
	class IPhasorDataFeed 
	{
	public:
		virtual void GetPhasorValue( float* part1, float* part2 ) = 0;
		virtual void SignalNextFrame() = 0;
	};

	class IAnalogDataFeed
	{
	public:
		virtual void GetAnalogValue( float* val ) = 0;
		virtual void SignalNextFrame() = 0;
	};

	class IDigitalDataFeed
	{
	public:
		virtual bool GetDigitalStatus() = 0;
		virtual void SignalNextFrame() = 0;
	};
}