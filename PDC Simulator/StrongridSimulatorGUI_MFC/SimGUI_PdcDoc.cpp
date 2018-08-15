// SimGUI_PdcDoc.cpp : implementation file
//

#include "stdafx.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "SimGUI_PdcDoc.h"
#include "MainFrm.h"

// SimGUI_PdcDoc

IMPLEMENT_DYNCREATE(SimGUI_PdcDoc, CDocument)

SimGUI_PdcDoc::SimGUI_PdcDoc()
{
}

BOOL SimGUI_PdcDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
			
	// Create the new pdc		
	m_pdcSimId = theApp.GetNewDocState().PdcSimulatorID;
	
	return TRUE;
}

SimGUI_PdcDoc::~SimGUI_PdcDoc()
{
}


BEGIN_MESSAGE_MAP(SimGUI_PdcDoc, CDocument)
END_MESSAGE_MAP()


// SimGUI_PdcDoc diagnostics

#ifdef _DEBUG
void SimGUI_PdcDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void SimGUI_PdcDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// SimGUI_PdcDoc serialization

void SimGUI_PdcDoc::Serialize(CArchive& ar)
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

ManagedPdcSimulator* SimGUI_PdcDoc::GetSimRef()
{	
	ManagedPdcSimulator* sim = theApp.GetPdcSimManager()->GetPdcSimulator(m_pdcSimId);
	if( sim == NULL ) throw int(1); // SimException(_T("Simulator not found"));
	return sim;
}

void SimGUI_PdcDoc::StartSimulator()
{
	GetSimRef()->StartSimulator();
}

void SimGUI_PdcDoc::StopSimulator()
{
	GetSimRef()->StopSimulator();
}

void SimGUI_PdcDoc::SendConfigurationCfg2AllClients()
{
	GetSimRef()->SendConfiguration();
}

void SimGUI_PdcDoc::StartRTDAllClients()
{
	GetSimRef()->StartRealtimeData();
}

void SimGUI_PdcDoc::StopRTDAllClients()
{
	GetSimRef()->StopRealtimeData();
}

bool SimGUI_PdcDoc::GetSimulatorIsActive()
{
	return GetSimRef()->SimulatorIsActive();
}


// SimGUI_PdcDoc commands
