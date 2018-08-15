#include "stdafx.h"
#include "../StrongridSimBase/IPdcDataFeed.h"

using namespace strongridsim;

namespace strongridgui
{
	class BridgedPdcDataFeed : public IPdcDataFeed
	{
	public:
		BridgedPdcDataFeed(IPdcDataFeed* own_pdcDataFeed);
		~BridgedPdcDataFeed();

		// Set new PDC datafeed - the old one is deleted 
		void SetPdcDataFeed( IPdcDataFeed* ownDatafeed );		

	public:
		virtual double GetSOC();
		virtual C37118FracSec GetFracSec();

	private:
		IPdcDataFeed* m_pdcDataFeed;
	};
}