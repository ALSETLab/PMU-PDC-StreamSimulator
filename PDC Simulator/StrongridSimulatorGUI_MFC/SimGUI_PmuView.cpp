// SimGUI_PmuView.cpp : implementation file
//

#include "stdafx.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "SimGUI_PmuView.h"
#include "SimGUI_PmuDoc.h"
#include "Common.h"
#include "MainFrm.h"

std::map<NewDocState, SimGUI_PmuView*> SimGUI_PmuView::ViewList;


// ----------------------------------------------------------------------------------------------------------
// --------------------------------------------- SimGUI_PhasorList ------------------------------------------
// ----------------------------------------------------------------------------------------------------------



void SimGUI_PhasorList::Initialize(PdcSimulatorID pdcId, int pmuIdx, CPmuConfig* pmuCfg)
{
	m_pmuCfg = pmuCfg;
	m_pdcId = pdcId;
	m_pmuIdx = pmuIdx;

	// initialize..
	for( int i = 0; i < m_pmuCfg->SG_PhasorNames.items.size(); ++i )
		this->AddItem(pmuCfg->SG_PhasorNames.items[i], 0, i );
}

// SimGUI_PdcPmuList
int SimGUI_PhasorList::AddItem(const CString& strIext, DWORD_PTR dwData, int iIndex)
{
	if( iIndex == - 1 ) // -1 means its NEW! 
	{
		// Create the new phasor
		m_pmuCfg->SG_PhasorNames.items.push_back(_T("PH_UNNAMED"));
		m_pmuCfg->SG_PhasorUnitArray.items.push_back( new CPhasorUnit(0,0) );
	}
	

	return CVSListBox::AddItem(strIext, dwData, iIndex);
}

BOOL SimGUI_PhasorList::RemoveItem(int iIndex)
{
	// "find" the correct index
	std::vector<CString>::iterator iter_a = m_pmuCfg->SG_PhasorNames.items.begin();
	std::vector<CPhasorUnit*>::iterator iter_b = m_pmuCfg->SG_PhasorUnitArray.items.begin();
	for( int i = 0; i < iIndex; ++i ) { iter_a++; iter_b++; } // move to index
	
	// Delete and remove the item
	delete *iter_b;
	m_pmuCfg->SG_PhasorNames.items.erase(iter_a);
	m_pmuCfg->SG_PhasorUnitArray.items.erase(iter_b);

	// Rebuild the required items in project view
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->RebuildPhasors(m_pdcId, m_pmuIdx);

	return CVSListBox::RemoveItem(iIndex);
}

void SimGUI_PhasorList::SetItemText(int iIndex, const CString& strText)
{
	CVSListBox::SetItemText(iIndex, strText);
}

BOOL SimGUI_PhasorList::EditItem(int iIndex)
{	
	return CVSListBox::EditItem(iIndex);
}

void SimGUI_PhasorList::OnEndEditLabel(LPCTSTR lpszLabel)
{	
	int iIndex = GetSelItem();

	if( lpszLabel != NULL )
		m_pmuCfg->SG_PhasorNames.items[iIndex] = lpszLabel;

	// Rebuild the project view
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->RebuildPhasors(m_pdcId, m_pmuIdx);

	CVSListBox::OnEndEditLabel(lpszLabel);
}




// ----------------------------------------------------------------------------------------------------------
// --------------------------------------------- SimGUI_AnalogList ------------------------------------------
// ----------------------------------------------------------------------------------------------------------



void SimGUI_AnalogList::Initialize(PdcSimulatorID pdcId, int pmuIdx, CPmuConfig* pmuCfg)
{
	m_pmuCfg = pmuCfg;
	m_pdcId = pdcId;
	m_pmuIdx = pmuIdx;

	// initialize..
	for( int i = 0; i < m_pmuCfg->SG_AnalogNames.items.size(); ++i )
		this->AddItem(pmuCfg->SG_AnalogNames.items[i], 0, i );
}

// SimGUI_PdcPmuList
int SimGUI_AnalogList::AddItem(const CString& strIext, DWORD_PTR dwData, int iIndex)
{
	if( iIndex == - 1 ) // -1 means its NEW! 
	{
		// Create the new phasor
		m_pmuCfg->SG_AnalogNames.items.push_back(_T("AN_UNNAMED"));
		m_pmuCfg->SG_AnalogUnitArray.items.push_back( new CAnalogUnit(0,0) );
	}

	return CVSListBox::AddItem(strIext, dwData, iIndex);
}

BOOL SimGUI_AnalogList::RemoveItem(int iIndex)
{
	// "find" the correct index
	std::vector<CString>::iterator iter_a = m_pmuCfg->SG_AnalogNames.items.begin();
	std::vector<CAnalogUnit*>::iterator iter_b = m_pmuCfg->SG_AnalogUnitArray.items.begin();
	for( int i = 0; i < iIndex; ++i ) { iter_a++; iter_b++; } // move to index
	
	// Delete and remove the item
	delete *iter_b;
	m_pmuCfg->SG_AnalogNames.items.erase(iter_a);
	m_pmuCfg->SG_AnalogUnitArray.items.erase(iter_b);

	// Rebuild the required items in project view
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->RebuildAnalog(m_pdcId, m_pmuIdx);

	return CVSListBox::RemoveItem(iIndex);
}

void SimGUI_AnalogList::SetItemText(int iIndex, const CString& strText)
{
	CVSListBox::SetItemText(iIndex, strText);
}

BOOL SimGUI_AnalogList::EditItem(int iIndex)
{	
	return CVSListBox::EditItem(iIndex);
}

void SimGUI_AnalogList::OnEndEditLabel(LPCTSTR lpszLabel)
{	
	int iIndex = GetSelItem();

	if( lpszLabel != NULL )
		m_pmuCfg->SG_AnalogNames.items[iIndex] = lpszLabel;
	

	// Rebuild the required items in project view
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->RebuildAnalog(m_pdcId, m_pmuIdx);

	CVSListBox::OnEndEditLabel(lpszLabel);
}




// ----------------------------------------------------------------------------------------------------------
// --------------------------------------------- SimGUI_DigitalList ------------------------------------------
// ----------------------------------------------------------------------------------------------------------



void SimGUI_DigitalList::Initialize(PdcSimulatorID pdcId, int pmuIdx, CPmuConfig* pmuCfg)
{
	m_pmuCfg = pmuCfg;
	m_pdcId = pdcId;
	m_pmuIdx = pmuIdx;

	// initialize..
	for( int i = 0; i < m_pmuCfg->SG_DigitalNames.items.size(); ++i )
		this->AddItem(pmuCfg->SG_DigitalNames.items[i], 0, i );
}


// SimGUI_PdcPmuList
int SimGUI_DigitalList::AddItem(const CString& strIext, DWORD_PTR dwData, int iIndex)
{
	if( iIndex == - 1 ) // -1 means its NEW! 
	{
		// Create the new phasor
		m_pmuCfg->SG_DigitalNames.items.push_back(_T("DIG_UNNAMED"));
		m_pmuCfg->SG_DigitalUnitArray.items.push_back( new CDigitalUnitBit(0,0) );
	}

	return CVSListBox::AddItem(strIext, dwData, iIndex);
}

BOOL SimGUI_DigitalList::RemoveItem(int iIndex)
{
	// "find" the correct index
	std::vector<CString>::iterator iter_a = m_pmuCfg->SG_DigitalNames.items.begin();
	std::vector<CDigitalUnitBit*>::iterator iter_b = m_pmuCfg->SG_DigitalUnitArray.items.begin();
	for( int i = 0; i < iIndex; ++i ) { iter_a++; iter_b++; } // move to index
	
	// Delete and remove the item
	delete *iter_b;
	m_pmuCfg->SG_DigitalNames.items.erase(iter_a);
	m_pmuCfg->SG_DigitalUnitArray.items.erase(iter_b);

	// Rebuild the required items in project view
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->RebuildDigital(m_pdcId, m_pmuIdx);

	return CVSListBox::RemoveItem(iIndex);
}


void SimGUI_DigitalList::SetItemText(int iIndex, const CString& strText)
{
	CVSListBox::SetItemText(iIndex, strText);
}

BOOL SimGUI_DigitalList::EditItem(int iIndex)
{	
	return CVSListBox::EditItem(iIndex);
}

void SimGUI_DigitalList::OnEndEditLabel(LPCTSTR lpszLabel)
{	
	int iIndex = GetSelItem();

	if( lpszLabel != NULL )
		m_pmuCfg->SG_DigitalNames.items[iIndex] = lpszLabel;
	
	// Rebuild the required items in project view
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->RebuildDigital(m_pdcId, m_pmuIdx);

	CVSListBox::OnEndEditLabel(lpszLabel);
}


// ----------------------------------------------------------------------------------------------------------


// SimGUI_PmuView

IMPLEMENT_DYNCREATE(SimGUI_PmuView, CFormView)

SimGUI_PmuView::SimGUI_PmuView()
	: CFormView(SimGUI_PmuView::IDD)
	, m_phasorFormatRect(0)
	, m_phasorTypeInt(0)
	, m_analogTypeInt(0)
	, m_freqDataTypeInt(0)
{

}

SimGUI_PmuView::~SimGUI_PmuView()
{
	ViewList.erase(((SimGUI_PmuDoc*)GetDocument())->m_pmuDocHandle);
}


void SimGUI_PmuView::SetDisplayHeader( )
{
	CDocument* blah = GetDocument();
	SimGUI_PmuDoc* doc = dynamic_cast<SimGUI_PmuDoc*>(GetDocument());
	CPmuConfig* refPmuConfig = doc->GetSimRef()->getConfigurationRef()->SG_PmuConfigArray.items[doc->m_pmuIdx];

	CString tmp;
	tmp.Format(_T("PMU - %s"), refPmuConfig->SG_PmuStationName);
	GetParent()->SetWindowText(tmp);
}

 
void SimGUI_PmuView::OnSimulatorEnabled()
{
	SetGuiState();
}

void SimGUI_PmuView::OnSimulatorDisabled()
{
	SetGuiState();
}

void SimGUI_PmuView::SetGuiState()
{	
	bool simisActive = dynamic_cast<SimGUI_PmuDoc*>(GetDocument())->GetSimRef()->SimulatorIsActive();
	m_phasorList.EnableWindow(!simisActive);
	m_analogList.EnableWindow(!simisActive);
	m_digitalList.EnableWindow(!simisActive);
}



void SimGUI_PmuView::OnInitialUpdate()
{
	CDocument* blah = GetDocument();
	SimGUI_PmuDoc* doc = dynamic_cast<SimGUI_PmuDoc*>(GetDocument());
	if( doc == NULL ) return;

	ManagedPdcSimulator* sim = doc->GetSimRef();
	CPmuConfig* cfgref = sim->getConfigurationRef()->SG_PmuConfigArray.items[doc->m_pmuDocHandle.PmuIndex];

	// set format (must be initialized before "onInitialUpdate"
	m_phasorFormatRect = 1;
	m_phasorFormatRect = cfgref->SG_PmuFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle ? 1 : 0;
	m_phasorTypeInt = cfgref->SG_PmuFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat ? 1 : 0;
	m_analogTypeInt = cfgref->SG_PmuFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat ? 1 : 0;
	m_freqDataTypeInt = cfgref->SG_PmuFormat.Bit3_0xFreqIsInt_1xFreqIsFloat ? 1 : 0;

	// Initialize the form
	CFormView::OnInitialUpdate();
	ViewList[((SimGUI_PmuDoc*)GetDocument())->m_pmuDocHandle] = this;
	
	// Initialize nominal frequency checkbox
	m_nomFreq.AddString(_T("60hz"));
	m_nomFreq.AddString(_T("50hz"));

	// Set text fields
	m_stationName.SetWindowTextW( cfgref->SG_PmuStationName );
	m_IdCode.SetWindowTextW( NumberToCStringASCII_i(cfgref->SG_PmuIdCode) );
	m_nomFreq.SetCurSel( cfgref->SG_FreqCode );
	m_cfgCngCnt.SetWindowTextW( NumberToCStringASCII_i(cfgref->SG_ChangeCnt) );

	// Initialize phasor list	
	m_phasorList.Initialize(doc->m_simId, doc->m_pmuIdx, cfgref);
	m_analogList.Initialize(doc->m_simId, doc->m_pmuIdx, cfgref);
	m_digitalList.Initialize(doc->m_simId, doc->m_pmuIdx, cfgref);
	
	// Initialize realtime data combo boxes: Data error
	m_rtdCbDataErr.AddString(_T("Good measurement data, no errors"));
	m_rtdCbDataErr.AddString(_T("PMU error. No information about the data."));
	m_rtdCbDataErr.AddString(_T("PMU in test mode (do not use values)"));
	m_rtdCbDataErr.AddString(_T("PMU error (do not use values)"));

	// Initialize realtime data combo boxes: PMU time quality ("Estimated maximum time error");
	m_rtdCbTimeQuality.AddString(_T("NOT USED"));
	m_rtdCbTimeQuality.AddString(_T("<100 ns"));
	m_rtdCbTimeQuality.AddString(_T("<1 µs"));
	m_rtdCbTimeQuality.AddString(_T("<10 µs"));
	m_rtdCbTimeQuality.AddString(_T("<100 µs"));
	m_rtdCbTimeQuality.AddString(_T("<1 ms"));
	m_rtdCbTimeQuality.AddString(_T("<10 ms"));
	m_rtdCbTimeQuality.AddString(_T(">10 ms or unknown"));

	// Initialize realtime data combo boxes: clock "unlocked" time
	m_rtdCbTimeUnlocked.AddString(_T("In sync, or unlocked < 10s") );
	m_rtdCbTimeUnlocked.AddString(_T("10s <= unlocked time < 100s") );
	m_rtdCbTimeUnlocked.AddString(_T("100s < unlock time <= 1000s") );
	m_rtdCbTimeUnlocked.AddString(_T("Unlocked time > 1000s") );

	// Initialize realtime data combo boxes: trigger reason
	m_rtdCbTriggerReason.AddString(_T("Manual"));
	m_rtdCbTriggerReason.AddString(_T("Magnitude low"));
	m_rtdCbTriggerReason.AddString(_T("Magnitude high"));
	m_rtdCbTriggerReason.AddString(_T("Phase angle diff"));
	m_rtdCbTriggerReason.AddString(_T("Frequency high or low"));
	m_rtdCbTriggerReason.AddString(_T("df/dt high"));
	m_rtdCbTriggerReason.AddString(_T("Reserved"));
	m_rtdCbTriggerReason.AddString(_T("Digital"));

	// Get a quickref to the dataframe
	BridgedPmuDataFeed* dataFeed = sim->GetPmuDataFeed(doc->m_pmuIdx);

	// Set realtime data combo boxes selectvalue: clock errata
	m_rtdCbDataErr.SetCurSel(dataFeed->GetFrameStat().getDataError());
	m_rtdCbTimeQuality.SetCurSel(dataFeed->GetFrameStat().getTimeQualityCode());
	m_rtdCbTimeUnlocked.SetCurSel(dataFeed->GetFrameStat().getUnlockTimeCode());
	m_rtdCbTriggerReason.SetCurSel(dataFeed->GetFrameStat().getTriggerReasonCode());

	// Set realtime data checkboxes
	m_rtdChkClockSynced.SetCheck(dataFeed->GetFrameStat().getPmuSyncFlag());
	m_rtdChkDataSort.SetCheck(dataFeed->GetFrameStat().getDataSortingFlag());
	m_rtdChkTriggerDetected.SetCheck(dataFeed->GetFrameStat().getPmuTriggerFlag());
	m_rtdChkCfgCngPending.SetCheck(dataFeed->GetFrameStat().getConfigChangeFlag());
	m_rtdChkDataModified.SetCheck(dataFeed->GetFrameStat().getDataModifiedFlag());

	// Set realtime data: freq
	float freq, dfreq;
	dataFeed->GetFrequency(&freq, &dfreq);
	m_frequency.SetWindowTextW( NumberToCStringASCII_f(freq) );
	m_deltaFrequency.SetWindowTextW( NumberToCStringASCII_f(dfreq) );

	SetDisplayHeader();
	SetGuiState();
	m_commit.EnableWindow(false);
}


void SimGUI_PmuView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{	
	ManagedPdcSimulator* sim = GetSimulatorRef();
	SimGUI_PmuDoc* doc = dynamic_cast<SimGUI_PmuDoc*>(GetDocument());
	CPmuConfig* cfgref = sim->getConfigurationRef()->SG_PmuConfigArray.items[doc->m_pmuDocHandle.PmuIndex];
	
	CFormView::OnUpdate(pSender, lHint, pHint);
}

void SimGUI_PmuView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_stationName);
	DDX_Control(pDX, IDC_EDIT3, m_IdCode);
	DDX_Control(pDX, IDC_EDIT1, m_cfgCngCnt);
	DDX_Control(pDX, IDC_MFCVSLISTBOX1, m_phasorList);
	DDX_Control(pDX, IDC_MFCVSLISTBOX4, m_analogList);
	DDX_Control(pDX, IDC_MFCVSLISTBOX3, m_digitalList);
	DDX_Radio(pDX, IDC_RADIO2, m_phasorFormatRect);
	DDX_Radio(pDX, IDC_RADIO4, m_phasorTypeInt);
	DDX_Radio(pDX, IDC_RADIO6, m_analogTypeInt);
	DDX_Radio(pDX, IDC_RADIO8, m_freqDataTypeInt);
	DDX_Control(pDX, IDC_MFCBUTTON1, m_commit);
	DDX_Control(pDX, IDC_CHECK4, m_rtdChkClockSynced);
	DDX_Control(pDX, IDC_CHECK5, m_rtdChkDataSort);
	DDX_Control(pDX, IDC_CHECK6, m_rtdChkTriggerDetected);
	DDX_Control(pDX, IDC_CHECK7, m_rtdChkCfgCngPending);
	DDX_Control(pDX, IDC_CHECK8, m_rtdChkDataModified);
	DDX_Control(pDX, IDC_COMBO2, m_rtdCbDataErr);
	DDX_Control(pDX, IDC_COMBO3, m_rtdCbTriggerReason);
	DDX_Control(pDX, IDC_COMBO4, m_rtdCbTimeQuality);
	DDX_Control(pDX, IDC_COMBO5, m_rtdCbTimeUnlocked);
	DDX_Control(pDX, IDC_COMBO6, m_nomFreq);
	DDX_Control(pDX, IDC_EDIT5, m_frequency);
	DDX_Control(pDX, IDC_EDIT6, m_deltaFrequency);
}

ManagedPdcSimulator* SimGUI_PmuView::GetSimulatorRef()
{
	SimGUI_PmuDoc* doc = dynamic_cast<SimGUI_PmuDoc*>(GetDocument());
	if( doc == NULL ) return 0;

	ManagedPdcSimulator* sim = doc->GetSimRef();
	return sim;
}

CPmuConfig* SimGUI_PmuView::GetPmuConfigRef()
{
	ManagedPdcSimulator* sim = GetSimulatorRef();
	SimGUI_PmuDoc* doc = dynamic_cast<SimGUI_PmuDoc*>(GetDocument());
	CPmuConfig* cfgref = sim->getConfigurationRef()->SG_PmuConfigArray.items[doc->m_pmuDocHandle.PmuIndex];
	return cfgref;
}

BEGIN_MESSAGE_MAP(SimGUI_PmuView, CFormView)
	ON_BN_CLICKED(IDC_MFCBUTTON1, &SimGUI_PmuView::OnBnClickedCommit)
	ON_BN_CLICKED(IDC_MFCBUTTON3, &SimGUI_PmuView::OnBnClickedCommitRealtimeDataParams)
	ON_EN_CHANGE(IDC_EDIT1, &SimGUI_PmuView::OnCfgChange)
	ON_EN_CHANGE(IDC_EDIT2, &SimGUI_PmuView::OnCfgChange)
	ON_EN_CHANGE(IDC_EDIT3, &SimGUI_PmuView::OnCfgChange)
	ON_CBN_SELCHANGE(IDC_COMBO6, &SimGUI_PmuView::OnCfgChange)
	ON_BN_CLICKED(IDC_RADIO1, &SimGUI_PmuView::OnCfgChange)
	ON_BN_CLICKED(IDC_RADIO2, &SimGUI_PmuView::OnCfgChange)
	ON_BN_CLICKED(IDC_RADIO3, &SimGUI_PmuView::OnCfgChange)
	ON_BN_CLICKED(IDC_RADIO4, &SimGUI_PmuView::OnCfgChange)
	ON_BN_CLICKED(IDC_RADIO5, &SimGUI_PmuView::OnCfgChange)
	ON_BN_CLICKED(IDC_RADIO6, &SimGUI_PmuView::OnCfgChange)
	ON_BN_CLICKED(IDC_RADIO7, &SimGUI_PmuView::OnCfgChange)
	ON_BN_CLICKED(IDC_RADIO8, &SimGUI_PmuView::OnCfgChange)
END_MESSAGE_MAP()


// SimGUI_PmuView diagnostics

#ifdef _DEBUG
void SimGUI_PmuView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void SimGUI_PmuView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// SimGUI_PmuView message handlers


void SimGUI_PmuView::OnBnClickedCommit()
{
	CPmuConfig* cfgref = GetPmuConfigRef();

	// Set regular fields
	cfgref->SG_PmuIdCode = GetEditTextAsInteger(m_IdCode, cfgref->SG_PmuIdCode);
	cfgref->SG_ChangeCnt = GetEditTextAsInteger(m_cfgCngCnt, cfgref->SG_ChangeCnt);
	cfgref->SG_FreqCode = m_nomFreq.GetCurSel();
	cfgref->SG_PmuStationName = SimpleGetWindowText( m_stationName );

	// Set dataformat
	bool bit0 = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2) == IDC_RADIO1;
	bool bit1 = GetCheckedRadioButton(IDC_RADIO3, IDC_RADIO4) == IDC_RADIO3;
	bool bit2 = GetCheckedRadioButton(IDC_RADIO5, IDC_RADIO6) == IDC_RADIO5;
	bool bit3 = GetCheckedRadioButton(IDC_RADIO7, IDC_RADIO8) == IDC_RADIO7;
	cfgref->SG_PmuFormat.Bit0_0xPhasorFormatRect_1xMagnitudeAndAngle = bit0;
	cfgref->SG_PmuFormat.Bit1_0xPhasorsIsInt_1xPhasorFloat = bit1;
	cfgref->SG_PmuFormat.Bit2_0xAnalogIsInt_1xAnalogIsFloat = bit2;
	cfgref->SG_PmuFormat.Bit3_0xFreqIsInt_1xFreqIsFloat = bit3;

	// Update the GUI
	SimGUI_PmuDoc* doc = dynamic_cast<SimGUI_PmuDoc*>(GetDocument());
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->UpdatePmuName(doc->m_simId, doc->m_pmuIdx);
	SetDisplayHeader();	
	m_commit.EnableWindow(false);
}


// Handle "commit realtime data parameters"
void SimGUI_PmuView::OnBnClickedCommitRealtimeDataParams()
{	
	SimGUI_PmuDoc* doc = dynamic_cast<SimGUI_PmuDoc*>(GetDocument());
	if( doc == NULL ) return;
	ManagedPdcSimulator* simRef = GetSimulatorRef();
	CPmuConfig* cfgref = GetPmuConfigRef();
		
	// Set all STAT codes
	BridgedPmuDataFeed* dataFeed = simRef->GetPmuDataFeed(doc->m_pmuIdx);
	dataFeed->GetFrameStatRef()->setDataErrorCode( m_rtdCbDataErr.GetCurSel() );
	dataFeed->GetFrameStatRef()->setTimeQualityCode(m_rtdCbTimeQuality.GetCurSel() );
	dataFeed->GetFrameStatRef()->setUnlockTimeCode( m_rtdCbTimeUnlocked.GetCurSel() );
	dataFeed->GetFrameStatRef()->setTriggerReasonCode( m_rtdCbTriggerReason.GetCurSel() );

	// Set all STAT BIT's
	dataFeed->GetFrameStatRef()->setPmuSyncFlag(m_rtdChkClockSynced.GetCheck());
	dataFeed->GetFrameStatRef()->setDataSortingFlag(m_rtdChkDataSort.GetCheck());
	dataFeed->GetFrameStatRef()->setPmuTriggerFlag(m_rtdChkTriggerDetected.GetCheck());
	dataFeed->GetFrameStatRef()->setConfigChangeFlag(m_rtdChkCfgCngPending.GetCheck());
	dataFeed->GetFrameStatRef()->setDataModifiedFlag(m_rtdChkDataModified.GetCheck());

	// Set frequency
	dataFeed->SetFrequency( GetEditTextAsFloat(m_frequency, 0), GetEditTextAsFloat(m_deltaFrequency, 0 ) );
}



void SimGUI_PmuView::OnCfgChange()
{
	m_commit.EnableWindow(true);
}

