
#pragma once
#include <stdint.h>
#include "../StrongridBase/C37118Protocol.h"

using namespace strongridbase;

namespace strongridsim
{
	class IPdcDataFeed  // ONCE PER PDC dataframe
	{
	public:
		virtual double GetSOC() = 0;
		virtual C37118FracSec GetFracSec() = 0;
	};
}

