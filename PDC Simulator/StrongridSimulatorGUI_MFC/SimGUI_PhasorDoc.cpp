// SimGUI_PhasorDoc.cpp : implementation file
//

#include "stdafx.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "SimGUI_PhasorDoc.h"


// SimGUI_PhasorDoc

IMPLEMENT_DYNCREATE(SimGUI_PhasorDoc, CDocument)

SimGUI_PhasorDoc::SimGUI_PhasorDoc()
{
}

NewDocState SimGUI_PhasorDoc::GetDocHandle() const
{
	return NewDocState::CreateAsPhasorId(m_pdcId, m_pmuIdx, m_phasorIdx);
}


ManagedPdcSimulator* SimGUI_PhasorDoc::GetSimulator()
{
	return theApp.GetPdcSimManager()->GetPdcSimulator(m_pdcId);
}


BridgedPmuDataFeed* SimGUI_PhasorDoc::GetPmuDataFeed()
{
	return GetSimulator()->GetPmuDataFeed(m_pmuIdx);
}

BOOL SimGUI_PhasorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	NewDocState a = theApp.GetNewDocState();

	// Create the new pdc		
	m_pdcId = a.PdcSimulatorID;
	m_pmuIdx = a.PmuIndex;
	m_phasorIdx = a.PhasorIndex;

	return TRUE;
}

SimGUI_PhasorDoc::~SimGUI_PhasorDoc()
{
}


BEGIN_MESSAGE_MAP(SimGUI_PhasorDoc, CDocument)
END_MESSAGE_MAP()


// SimGUI_PhasorDoc diagnostics

#ifdef _DEBUG
void SimGUI_PhasorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void SimGUI_PhasorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// SimGUI_PhasorDoc serialization

void SimGUI_PhasorDoc::Serialize(CArchive& ar)
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

CPhasorUnit* SimGUI_PhasorDoc::GetPhasorUnitRef()
{
	CPdcConfig* pdcConfig = GetSimulator()->getConfigurationRef();
	CPmuConfig* pmuconfig = pdcConfig->SG_PmuConfigArray.items[m_pmuIdx];
	CPhasorUnit* phasorUnit = pmuconfig->SG_PhasorUnitArray.items[m_phasorIdx];
	return phasorUnit;
}


void SimGUI_PhasorDoc::SetPhasorName(CString phName)
{
	CPdcConfig* pdcConfig = GetSimulator()->getConfigurationRef();
	CPmuConfig* pmuconfig = pdcConfig->SG_PmuConfigArray.items[m_pmuIdx];	
	pmuconfig->SG_PhasorNames.items[m_phasorIdx] = phName;
}

CString SimGUI_PhasorDoc::GetPhasorName()
{
	CPdcConfig* pdcConfig = GetSimulator()->getConfigurationRef();
	CPmuConfig* pmuconfig = pdcConfig->SG_PmuConfigArray.items[m_pmuIdx];
	return pmuconfig->SG_PhasorNames.items[m_phasorIdx];
}