// SimGUI_DigitalDoc.cpp : implementation file
//

#include "stdafx.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "SimGUI_DigitalDoc.h"


// SimGUI_DigitalDoc

IMPLEMENT_DYNCREATE(SimGUI_DigitalDoc, CDocument)

SimGUI_DigitalDoc::SimGUI_DigitalDoc()
{
}

NewDocState SimGUI_DigitalDoc::GetDocHandle() const
{
	return NewDocState::CreateAsDigitalId(m_pdcId, m_pmuIdx, m_digitalIdx);
}


ManagedPdcSimulator* SimGUI_DigitalDoc::GetSimulator()
{
	return theApp.GetPdcSimManager()->GetPdcSimulator(m_pdcId);
}


BridgedPmuDataFeed* SimGUI_DigitalDoc::GetPmuDataFeed()
{
	return GetSimulator()->GetPmuDataFeed(m_pmuIdx);
}

CDigitalUnitBit* SimGUI_DigitalDoc::GetDigitalUnitBitRef()
{
	CPdcConfig* pdcConfig = GetSimulator()->getConfigurationRef();
	CPmuConfig* pmuconfig = pdcConfig->SG_PmuConfigArray.items[m_pmuIdx];
	CDigitalUnitBit* digitalUnit = pmuconfig->SG_DigitalUnitArray.items[m_digitalIdx];
	return digitalUnit;
}


void SimGUI_DigitalDoc::SetDigitalName(CString phName)
{
	CPdcConfig* pdcConfig = GetSimulator()->getConfigurationRef();
	CPmuConfig* pmuconfig = pdcConfig->SG_PmuConfigArray.items[m_pmuIdx];	
	pmuconfig->SG_DigitalNames.items[m_digitalIdx] = phName;
}

CString SimGUI_DigitalDoc::GetDigitalName()
{
	CPdcConfig* pdcConfig = GetSimulator()->getConfigurationRef();
	CPmuConfig* pmuconfig = pdcConfig->SG_PmuConfigArray.items[m_pmuIdx];
	return pmuconfig->SG_DigitalNames.items[m_digitalIdx];
}


BOOL SimGUI_DigitalDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;	

	NewDocState a = theApp.GetNewDocState();

	// Create the new pdc		
	m_pdcId = a.PdcSimulatorID;
	m_pmuIdx = a.PmuIndex;
	m_digitalIdx = a.DigitalIndex;

	return TRUE;
}

SimGUI_DigitalDoc::~SimGUI_DigitalDoc()
{
}


BEGIN_MESSAGE_MAP(SimGUI_DigitalDoc, CDocument)
END_MESSAGE_MAP()


// SimGUI_DigitalDoc diagnostics

#ifdef _DEBUG
void SimGUI_DigitalDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void SimGUI_DigitalDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// SimGUI_DigitalDoc serialization

void SimGUI_DigitalDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif


// SimGUI_DigitalDoc commands
