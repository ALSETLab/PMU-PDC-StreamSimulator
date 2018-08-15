// SimGUI_PdcView.cpp : implementation file
//

#include "stdafx.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "SimGUI_PdcView.h"
#include "SimGUI_PdcDoc.h"
#include "SimGUI_PmuView.h"
#include "SimGUI_PhasorView.h"
#include "SimGUI_AnalogView.h"
#include "SimGUI_DigitalView.h"
#include "MainFrm.h"
#include "Common.h"
#include <tchar.h>

std::map<PdcSimulatorID, SimGUI_PdcView*> SimGUI_PdcView::ViewList;


void SimGUI_PdcPmuList::Initialize(SimGUI_PdcDoc* pdcDoc)
{
	m_pdcDoc = pdcDoc;

	// initialize..
	for( int i = 0; i < m_pdcDoc->GetSimRef()->getConfigurationRef()->SG_PmuConfigArray.items.size(); ++i )
	{
		const CPmuConfig* pmuCfg = m_pdcDoc->GetSimRef()->getConfigurationRef()->SG_PmuConfigArray.items[i];

		CString tmp;
		tmp.Format(_T("[%d] %s"), pmuCfg->SG_PmuIdCode, pmuCfg->SG_PmuStationName );
		this->AddItem(tmp, 0, i );
	}

}

// SimGUI_PdcPmuList
int SimGUI_PdcPmuList::AddItem(const CString& strIext, DWORD_PTR dwData, int iIndex)
{
	if( iIndex == - 1 ) // -1 means its NEW! 
	{
		// Create the new PMU
		CPmuConfig* newPmuCfg = m_pdcDoc->GetSimRef()->AppendDefaultPMU();
		newPmuCfg->SG_PmuStationName = strIext;
	}

	return CVSListBox::AddItem(strIext, dwData, iIndex);
}

BOOL SimGUI_PdcPmuList::RemoveItem(int iIndex)
{
	m_pdcDoc->GetSimRef()->RemovePmuAtIndex(iIndex);
	return CVSListBox::RemoveItem(iIndex);
}

void SimGUI_PdcPmuList::SetItemText(int iIndex, const CString& strText)
{
	CVSListBox::SetItemText(iIndex, strText);
}

BOOL SimGUI_PdcPmuList::EditItem(int iIndex)
{
	// Change the item text first to remove the pmu ID code
	CPmuConfig* refPmu = m_pdcDoc->GetSimRef()->getConfigurationRef()->SG_PmuConfigArray.items[iIndex];
	SetItemText(iIndex, refPmu->SG_PmuStationName);
	return CVSListBox::EditItem(iIndex);
}

void SimGUI_PdcPmuList::OnEndEditLabel(LPCTSTR lpszLabel)
{

	int iIndex = GetSelItem();
	CPmuConfig* refPmu = m_pdcDoc->GetSimRef()->getConfigurationRef()->SG_PmuConfigArray.items[iIndex];
	
	if( lpszLabel != NULL )
		refPmu->SG_PmuStationName = lpszLabel;

	// Change the format of the label to include the PMU ide code
	CString tmp;
	tmp.Format(_T("[%d] %s"), refPmu->SG_PmuIdCode, refPmu->SG_PmuStationName );
	CVSListBox::OnEndEditLabel(tmp);
	
	// Rebuild the project view
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->ForceRebuild();
}


// SimGUI_PdcView

IMPLEMENT_DYNCREATE(SimGUI_PdcView, CFormView)

SimGUI_PdcView::SimGUI_PdcView()
	: CFormView(SimGUI_PdcView::IDD)
{
}

SimGUI_PdcView::~SimGUI_PdcView()
{	
	ViewList.erase( ((SimGUI_PdcDoc*)GetDocument())->m_pdcSimId);
}

void SimGUI_PdcView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TIMEBASE, m_TimeBase);
	DDX_Control(pDX, IDC_EDIT_DATARATE, m_dataRate);
	DDX_Control(pDX, IDC_EDIT_TIMEBASE2, m_pdcId);
	DDX_Control(pDX, IDC_EDIT_TIMEBASE3, m_listenPortTcp);
	DDX_Control(pDX, IDC_PMULIST, m_pmuList);
	DDX_Control(pDX, IDC_EDIT_TIMEBASE4, m_txtPdcProjectName);
	DDX_Control(pDX, IDC_MFCBUTTON1, m_btnCfgCommit);
}

void SimGUI_PdcView::OnInitialUpdate()
{	
	// Setup event handlers for activation of the "commit" button
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CFormView::OnInitialUpdate();
	m_pmuList.Initialize((SimGUI_PdcDoc*)GetDocument());
	ViewList[((SimGUI_PdcDoc*)GetDocument())->m_pdcSimId] = this;
	
	m_btnCfgCommit.EnableWindow(false);
}

void SimGUI_PdcView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CDocument* blah = GetDocument();
	SimGUI_PdcDoc* doc = dynamic_cast<SimGUI_PdcDoc*>(GetDocument());
	if( doc == NULL ) return;
	CString tmp; tmp.Format(_T("%d"), doc->m_pdcSimId );
	SetDlgItemText(IDC_EDIT1, tmp );
	
	// Update 'start/stop' simulator button
	if( doc->GetSimulatorIsActive() == false ) 
		SetDlgItemText(IDC_STARTSIM, _T("Start simulator"));
	else 
		SetDlgItemText(IDC_STARTSIM, _T("Stop simulator"));

	// Update status of control buttons	
	GetDlgItem(IDC_BTN_STARTRTDALL)->EnableWindow(doc->GetSimulatorIsActive());
	GetDlgItem(IDC_BTN_STOPRTD_ALL)->EnableWindow(doc->GetSimulatorIsActive());
	GetDlgItem(IDC_BTN_SENDCFG2_ALL)->EnableWindow(doc->GetSimulatorIsActive());
	m_txtPdcProjectName.SetReadOnly(doc->GetSimulatorIsActive());
	m_pdcId.SetReadOnly(doc->GetSimulatorIsActive());
	m_listenPortTcp.SetReadOnly(doc->GetSimulatorIsActive());
	m_dataRate.SetReadOnly(doc->GetSimulatorIsActive());
    m_TimeBase.SetReadOnly(doc->GetSimulatorIsActive());
	m_pmuList.EnableWindow(!doc->GetSimulatorIsActive());

	// Set PDC values..
	const CPdcConfig* refPdcConfig = doc->GetSimRef()->getConfigurationRef();
	m_txtPdcProjectName.SetWindowTextW(doc->GetSimRef()->SimulatorProjectName());
	m_listenPortTcp.SetWindowTextW(NumberToCStringASCII_i(refPdcConfig->SG_ListenPort));
	m_pdcId.SetWindowTextW(NumberToCStringASCII_i(refPdcConfig->SG_PdcId));
	m_TimeBase.SetWindowTextW(NumberToCStringASCII_i(refPdcConfig->SG_TimeBase));
	m_dataRate.SetWindowTextW(NumberToCStringASCII_f(refPdcConfig->SG_DataRate_FramesPerSecond));
		
	SetDisplayHeader(((SimGUI_PdcDoc*)GetDocument())->GetSimRef()->SimulatorProjectName());
	
}


void SimGUI_PdcView::CommitDataFromGUI()
{
	CDocument* blah = GetDocument();
	SimGUI_PdcDoc* doc = dynamic_cast<SimGUI_PdcDoc*>(GetDocument());
	if( doc == NULL ) return;
	
	// Get reference to the pdc itself, and assign each value in turn
	doc->GetSimRef()->SetSimulatorProjectName(SimpleGetWindowText(m_txtPdcProjectName));
	CPdcConfig* refPdcConfig = doc->GetSimRef()->getConfigurationRef();
	refPdcConfig->SG_ListenPort = GetEditTextAsInteger(m_listenPortTcp, 0);
	refPdcConfig->SG_DataRate_FramesPerSecond = GetEditTextAsFloat(m_dataRate, 0);
	refPdcConfig->SG_PdcId = GetEditTextAsInteger(m_pdcId, 0);
	refPdcConfig->SG_TimeBase = GetEditTextAsInteger(m_TimeBase, 0);
	doc->GetSimRef()->SetPdcDataRate(refPdcConfig->SG_DataRate_FramesPerSecond);

	// Update GUI
	SetDisplayHeader(SimpleGetWindowText(m_txtPdcProjectName));
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->UpdatePdcName(doc->m_pdcSimId);
	m_btnCfgCommit.EnableWindow(false);
}

void SimGUI_PdcView::SetDisplayHeader( CString str )
{
	CDocument* blah = GetDocument();
	SimGUI_PdcDoc* doc = dynamic_cast<SimGUI_PdcDoc*>(GetDocument());
	CPdcConfig* refPdcConfig = doc->GetSimRef()->getConfigurationRef();

	CString tmp;
	tmp.Format(_T("PDC [%d] - %s"), refPdcConfig->SG_PdcId, str );
	GetParent()->SetWindowText(tmp);
}

BEGIN_MESSAGE_MAP(SimGUI_PdcView, CFormView)
	 ON_BN_CLICKED(IDC_STARTSIM, &SimGUI_PdcView::OnBnClickedToggleSim)
	 ON_BN_CLICKED(IDC_BTN_STARTRTDALL, &SimGUI_PdcView::OnBnClickedBtnStartrtdall)
	 ON_BN_CLICKED(IDC_BTN_STOPRTD_ALL, &SimGUI_PdcView::OnBnClickedBtnStoprtdAll)
	 ON_BN_CLICKED(IDC_BTN_SENDCFG2_ALL, &SimGUI_PdcView::OnBnClickedBtnSendcfg2All)	 
	 ON_NOTIFY(LVN_ITEMCHANGED, IDC_PMULIST, &SimGUI_PdcView::OnLvnItemChanged)
	 ON_BN_CLICKED(IDC_MFCBUTTON1, &SimGUI_PdcView::OnBnClickedMfcbutton1)
	 ON_EN_CHANGE(IDC_EDIT_TIMEBASE4, &SimGUI_PdcView::OnChangeConfig)
	 ON_EN_CHANGE(IDC_EDIT_TIMEBASE3, &SimGUI_PdcView::OnChangeConfig)
	 ON_EN_CHANGE(IDC_EDIT_TIMEBASE2, &SimGUI_PdcView::OnChangeConfig)
	 ON_EN_CHANGE(IDC_EDIT_TIMEBASE, &SimGUI_PdcView::OnChangeConfig)
	 ON_EN_CHANGE(IDC_EDIT_DATARATE, &SimGUI_PdcView::OnChangeConfig)
END_MESSAGE_MAP()


// SimGUI_PdcView diagnostics

#ifdef _DEBUG
void SimGUI_PdcView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void SimGUI_PdcView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG



void SimGUI_PdcView::OnBnClickedToggleSim()
{	
	SimGUI_PdcDoc* doc = dynamic_cast<SimGUI_PdcDoc*>(GetDocument());
	
	// Update 'start/stop' simulator button
	if( doc->GetSimulatorIsActive() == false )
		SetDlgItemText(IDC_STARTSIM, _T("Starting simulator..."));
	else 
		SetDlgItemText(IDC_STARTSIM, _T("Stopping simulator..."));

	// Commit any changes first
	CommitDataFromGUI();

	if( doc->GetSimulatorIsActive() == false ) 
		doc->StartSimulator();
	else
		doc->StopSimulator();
	
	
	// Notify all PMU views about the change in state 
	for( std::map<NewDocState, SimGUI_PmuView*>::iterator iterPmuView = SimGUI_PmuView::ViewList.begin();
		iterPmuView != SimGUI_PmuView::ViewList.end(); ++iterPmuView )
	{
		if( iterPmuView->first.PdcSimulatorID != doc->m_pdcSimId ) continue;

		if( doc->GetSimulatorIsActive() )
			iterPmuView->second->OnSimulatorEnabled();
		else
			iterPmuView->second->OnSimulatorDisabled();
	}

	
	// Notify all PHASOR views about the change in state 
	for( std::map<NewDocState, SimGUI_PhasorView*>::iterator iterPhasorView = SimGUI_PhasorView::ViewList.begin();
		iterPhasorView != SimGUI_PhasorView::ViewList.end(); ++iterPhasorView )
	{
		if( iterPhasorView->first.PdcSimulatorID != doc->m_pdcSimId ) continue;
		iterPhasorView->second->HandleStateChange();
	}

	// Notify all ANALOG views about the change in state 
	for( std::map<NewDocState, SimGUI_AnalogView*>::iterator iterAnalogView = SimGUI_AnalogView::ViewList.begin();
		iterAnalogView != SimGUI_AnalogView::ViewList.end(); ++iterAnalogView )
	{
		if( iterAnalogView->first.PdcSimulatorID != doc->m_pdcSimId ) continue;
		iterAnalogView->second->HandleStateChange();
	}

	// Notify all DIGITAL view about the change in state
	for( std::map<NewDocState, SimGUI_DigitalView*>::iterator iterDigitalView = SimGUI_DigitalView::ViewList.begin();
		iterDigitalView != SimGUI_DigitalView::ViewList.end(); ++iterDigitalView )
	{
		if( iterDigitalView->first.PdcSimulatorID != doc->m_pdcSimId ) continue;
		iterDigitalView->second->HandleStateChange();
	}

	OnUpdate(NULL,0,NULL);
}


void SimGUI_PdcView::OnBnClickedBtnStartrtdall()
{
	SimGUI_PdcDoc* doc = dynamic_cast<SimGUI_PdcDoc*>(GetDocument());
	doc->StartRTDAllClients();
}


void SimGUI_PdcView::OnBnClickedBtnStoprtdAll()
{
	SimGUI_PdcDoc* doc = dynamic_cast<SimGUI_PdcDoc*>(GetDocument());
	doc->StopRTDAllClients();
}


void SimGUI_PdcView::OnBnClickedBtnSendcfg2All()
{
	SimGUI_PdcDoc* doc = dynamic_cast<SimGUI_PdcDoc*>(GetDocument());
	doc->SendConfigurationCfg2AllClients();
}


void SimGUI_PdcView::OnLvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{

}

void SimGUI_PdcView::OnEnChangeEditTimebase()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void SimGUI_PdcView::OnBnClickedMfcbutton1()
{
	CommitDataFromGUI();
}


void SimGUI_PdcView::OnChangeConfig()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	m_btnCfgCommit.EnableWindow( dynamic_cast<SimGUI_PdcDoc*>(GetDocument())->GetSimulatorIsActive() == false);
	// TODO:  Add your control notification handler code here
}
