#pragma once

#include "IPmuDataFeed.h"
#include "../StrongridBase/C37118Protocol.h"

using namespace strongridbase;

namespace strongridsim
{
	class PMU
	{
	public:
		PMU( C37118PmuConfiguration pmuCfg, IPmuDataFeed* feed );
		PMU( C37118PmuConfiguration_Ver3 pmuCfgV3, IPmuDataFeed* feed );
		~PMU();

		// Use the feed to create a pmu dataframe
		C37118PmuDataFrame CreatePmuDataFrame();

	private:
		C37118PmuConfiguration m_cfg;
		C37118PmuConfiguration_Ver3 m_cfgVer3;
		IPmuDataFeed* m_feed;
	};
}