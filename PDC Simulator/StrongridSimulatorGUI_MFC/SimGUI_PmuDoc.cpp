// SimGUI_PmuDoc.cpp : implementation file
//

#include "stdafx.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "SimGUI_PmuDoc.h"


// SimGUI_PmuDoc

IMPLEMENT_DYNCREATE(SimGUI_PmuDoc, CDocument)

SimGUI_PmuDoc::SimGUI_PmuDoc()
{
}

BOOL SimGUI_PmuDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
				
	NewDocState a = theApp.GetNewDocState();

	// Create the new pdc		
	m_pmuDocHandle.DocType = a.DocType;
	m_pmuDocHandle.PdcSimulatorID = a.PdcSimulatorID;
	m_pmuDocHandle.PmuIndex = a.PmuIndex;
	m_simId = a.PdcSimulatorID;
	m_pmuIdx = a.PmuIndex;
	return TRUE;
}

SimGUI_PmuDoc::~SimGUI_PmuDoc()
{
}

ManagedPdcSimulator* SimGUI_PmuDoc::GetSimRef()
{
	return theApp.GetPdcSimManager()->GetPdcSimulator(m_simId);
}


BEGIN_MESSAGE_MAP(SimGUI_PmuDoc, CDocument)
END_MESSAGE_MAP()


// SimGUI_PmuDoc diagnostics

#ifdef _DEBUG
void SimGUI_PmuDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void SimGUI_PmuDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// SimGUI_PmuDoc serialization

void SimGUI_PmuDoc::Serialize(CArchive& ar)
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


// SimGUI_PmuDoc commands
