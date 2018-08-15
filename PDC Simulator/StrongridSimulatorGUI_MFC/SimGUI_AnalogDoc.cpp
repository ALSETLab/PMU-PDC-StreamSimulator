// SimGUI_AnalogDoc.cpp : implementation file
//

#include "stdafx.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "SimGUI_AnalogDoc.h"


// SimGUI_AnalogDoc

IMPLEMENT_DYNCREATE(SimGUI_AnalogDoc, CDocument)

SimGUI_AnalogDoc::SimGUI_AnalogDoc()
{
}

NewDocState SimGUI_AnalogDoc::GetDocHandle() const
{
	return NewDocState::CreateAsAnalogId(m_pdcId, m_pmuIdx, m_analogIdx);
}


ManagedPdcSimulator* SimGUI_AnalogDoc::GetSimulator()
{
	return theApp.GetPdcSimManager()->GetPdcSimulator(m_pdcId);
}


BridgedPmuDataFeed* SimGUI_AnalogDoc::GetPmuDataFeed()
{
	return GetSimulator()->GetPmuDataFeed(m_pmuIdx);
}


BOOL SimGUI_AnalogDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;	

	NewDocState a = theApp.GetNewDocState();

	// Create the new pdc		
	m_pdcId = a.PdcSimulatorID;
	m_pmuIdx = a.PmuIndex;
	m_analogIdx = a.AnalogIndex;

	return TRUE;
}

SimGUI_AnalogDoc::~SimGUI_AnalogDoc()
{
}


BEGIN_MESSAGE_MAP(SimGUI_AnalogDoc, CDocument)
END_MESSAGE_MAP()


// SimGUI_AnalogDoc diagnostics

#ifdef _DEBUG
void SimGUI_AnalogDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void SimGUI_AnalogDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// SimGUI_AnalogDoc serialization

void SimGUI_AnalogDoc::Serialize(CArchive& ar)
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

CAnalogUnit* SimGUI_AnalogDoc::GetAnalogUnitRef()
{
	CPdcConfig* pdcConfig = GetSimulator()->getConfigurationRef();
	CPmuConfig* pmuconfig = pdcConfig->SG_PmuConfigArray.items[m_pmuIdx];
	CAnalogUnit* analogUnit = pmuconfig->SG_AnalogUnitArray.items[m_analogIdx];
	return analogUnit;
}

void SimGUI_AnalogDoc::SetAnalogName(CString phName)
{
	CPdcConfig* pdcConfig = GetSimulator()->getConfigurationRef();
	CPmuConfig* pmuconfig = pdcConfig->SG_PmuConfigArray.items[m_pmuIdx];
	pmuconfig->SG_AnalogNames.items[m_analogIdx] = phName;
}

CString SimGUI_AnalogDoc::GetAnalogName()
{
	CPdcConfig* pdcConfig = GetSimulator()->getConfigurationRef();
	CPmuConfig* pmuconfig = pdcConfig->SG_PmuConfigArray.items[m_pmuIdx];
	return pmuconfig->SG_AnalogNames.items[m_analogIdx];
}
// SimGUI_AnalogDoc commands
