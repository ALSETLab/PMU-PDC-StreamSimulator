#include "stdafx.h"
#include "BridgedPdcDataFeed.h"

using namespace strongridgui;

BridgedPdcDataFeed::BridgedPdcDataFeed(IPdcDataFeed* own_pdcDataFeed)
{
	m_pdcDataFeed = own_pdcDataFeed;
}

BridgedPdcDataFeed::~BridgedPdcDataFeed()
{
	delete m_pdcDataFeed;
}

// Set new PDC datafeed - the old one is deleted 
void BridgedPdcDataFeed::SetPdcDataFeed( IPdcDataFeed* ownDatafeed )
{
	delete m_pdcDataFeed;
	m_pdcDataFeed = ownDatafeed;
}

double BridgedPdcDataFeed::GetSOC()
{
	return m_pdcDataFeed->GetSOC();
}

C37118FracSec BridgedPdcDataFeed::GetFracSec()
{
	return m_pdcDataFeed->GetFracSec();
}