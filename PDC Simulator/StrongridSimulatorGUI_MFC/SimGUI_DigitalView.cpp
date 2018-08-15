// SimGUI_DigitalView.cpp : implementation file
//

#include "stdafx.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "SimGUI_DigitalView.h"
#include "SimGUI_DigitalDoc.h"
#include "FilestreamChannelDataFeed.h"
#include "ConstantChannelDataFeed.h"
#include "Common.h"
#include "MainFrm.h"

std::map<NewDocState, SimGUI_DigitalView*> SimGUI_DigitalView::ViewList;

// SimGUI_DigitalView

IMPLEMENT_DYNCREATE(SimGUI_DigitalView, CFormView)

SimGUI_DigitalView::SimGUI_DigitalView()
	: CFormView(SimGUI_DigitalView::IDD)
	, m_rbRtdInputType(0)
{

}

SimGUI_DigitalView::~SimGUI_DigitalView()
{
	ViewList.erase(((SimGUI_DigitalDoc*)GetDocument())->GetDocHandle() );
}


void SimGUI_DigitalView::SetDisplayHeader( )
{
	CDocument* blah = GetDocument();
	SimGUI_DigitalDoc* doc = dynamic_cast<SimGUI_DigitalDoc*>(GetDocument());
	CPmuConfig* refPmuConfig = doc->GetSimulator()->getConfigurationRef()->SG_PmuConfigArray.items[doc->GetDocHandle().PmuIndex];
	
	CString tmp;
	tmp.Format(_T("Analog - %s"), refPmuConfig->SG_DigitalNames.items[doc->GetDocHandle().DigitalIndex]);
	GetParent()->SetWindowText(tmp);
}
 



void SimGUI_DigitalView::HandleStateChange()
{
	SimGUI_DigitalDoc* doc = dynamic_cast<SimGUI_DigitalDoc*>(GetDocument());
	const bool SIM_ISACTIVE = doc->GetSimulator()->SimulatorIsActive();

	int checkedId = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2);
	
	m_txtRtdFileInput.EnableWindow(!SIM_ISACTIVE && checkedId == IDC_RADIO2);
	m_btnRtdFind.EnableWindow(!SIM_ISACTIVE && checkedId == IDC_RADIO2);
	GetDlgItem( IDC_RADIO1 )->EnableWindow(!SIM_ISACTIVE);
	GetDlgItem( IDC_RADIO2 )->EnableWindow(!SIM_ISACTIVE);
	m_dbRtdValue.EnableWindow(checkedId == IDC_RADIO1);

	// Set commit RTD button state based on current input data type and simulator state
	m_btnRtdCommit.EnableWindow( !(SIM_ISACTIVE && checkedId == IDC_RADIO2 ) );
}


void SimGUI_DigitalView::OnInitialUpdate()
{
	SimGUI_DigitalDoc* doc = dynamic_cast<SimGUI_DigitalDoc*>(GetDocument());
	if( doc == NULL ) return;

	// Set the correct "input type" based on the current Digital input datafeed type
	BridgedPmuDataFeed* bridgedDataFeed = doc->GetPmuDataFeed();
	IDigitalDataFeed* digitalFeed = bridgedDataFeed->GetDigitalOverride(doc->GetDocHandle().DigitalIndex);
	FilestreamDigitalDataFeed* digitalFileStream = dynamic_cast<FilestreamDigitalDataFeed*>(digitalFeed);
	ConstantDigitalDataFeed* digitalConstInput = dynamic_cast<ConstantDigitalDataFeed*>(digitalFeed);

	// Set input type based on the type of datastream (default to constant if missing)
	if( digitalFileStream != NULL )
		m_rbRtdInputType = 1;
	else 
		m_rbRtdInputType = 0;	

	// ..
	CFormView::OnInitialUpdate();

	// Initialize rtd value droplist
	m_dbRtdValue.AddString(_T("Off"));
	m_dbRtdValue.AddString(_T("On"));

	if( digitalFileStream != NULL ) {
		m_txtRtdFileInput.SetWindowTextW(digitalFileStream->GetInputFile());
	}
	else 
	{		
		// Rebuild digital input if NULL or not deferrable to digitalConstInput
		if( digitalConstInput == NULL )
		{
			delete digitalFeed;
			digitalFeed = digitalConstInput = new ConstantDigitalDataFeed();
			bridgedDataFeed->SetDigitalOverride(doc->GetDocHandle().DigitalIndex, digitalFeed);
		}
				
		bool bitIsSet = digitalConstInput->GetDigitalStatus();
		m_dbRtdValue.SetCurSel( bitIsSet ? 1 : 0 );
	}

	// Initialize Digital type 
	m_dbCfgNormalState.AddString(_T("Off"));
	m_dbCfgNormalState.AddString(_T("On"));

	// Set Digital configuration	
	m_txtCfgName.SetWindowTextW(doc->GetDigitalName() );
	CDigitalUnitBit* digUnit = doc->GetDigitalUnitBitRef();
	m_dbCfgNormalState.SetCurSel( digUnit->SG_NormalState );

	// Update 'enabled' state based on simulator state
	HandleStateChange();
	SetDisplayHeader( );
	m_btnCfgCommit.EnableWindow(false);
	m_btnRtdCommit.EnableWindow(false);

	// Bind to viewlist
	ViewList[doc->GetDocHandle()] = this;	
}

void SimGUI_DigitalView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_dbCfgNormalState);
	DDX_Radio(pDX, IDC_RADIO1, m_rbRtdInputType);
	DDX_Control(pDX, IDC_EDIT2, m_txtRtdFileInput);
	DDX_Control(pDX, IDC_BUTTON2, m_btnRtdFind);
	DDX_Control(pDX, IDC_COMBO4, m_dbRtdValue);
	DDX_Control(pDX, IDC_DIGRDTCOMMIT, m_btnRtdCommit);
	DDX_Control(pDX, IDC_EDIT3, m_txtCfgName);
	DDX_Control(pDX, IDC_DIGCFGCOMMIT, m_btnCfgCommit);
}
	afx_msg void OnBnClickedCfgCommit();
	afx_msg void OnBnClickedRdtCommit();
	afx_msg void OnBnClickedRtdLoad();
	afx_msg void OnBnClickedRadioInputType();

BEGIN_MESSAGE_MAP(SimGUI_DigitalView, CFormView)
	ON_BN_CLICKED(IDC_DIGCFGCOMMIT, &SimGUI_DigitalView::OnBnClickedCfgCommit)
	ON_BN_CLICKED(IDC_DIGRDTCOMMIT, &SimGUI_DigitalView::OnBnClickedRdtCommit)
	ON_BN_CLICKED(IDC_BUTTON2, &SimGUI_DigitalView::OnBnClickedRtdLoad)
	ON_BN_CLICKED(IDC_RADIO1, &SimGUI_DigitalView::OnBnClickedRadioInputType)
	ON_BN_CLICKED(IDC_RADIO2, &SimGUI_DigitalView::OnBnClickedRadioInputType)
	ON_EN_CHANGE(IDC_EDIT3, &SimGUI_DigitalView::OnCfgChange)
	ON_CBN_SELCHANGE(IDC_COMBO1, &SimGUI_DigitalView::OnCfgChange)
	ON_CBN_SELCHANGE(IDC_COMBO4, &SimGUI_DigitalView::OnRtdChange)
END_MESSAGE_MAP()


// SimGUI_DigitalView diagnostics

#ifdef _DEBUG
void SimGUI_DigitalView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void SimGUI_DigitalView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG



void SimGUI_DigitalView::OnBnClickedCfgCommit()
{
	// Get a reference to Digital unit configuration
	SimGUI_DigitalDoc* doc = dynamic_cast<SimGUI_DigitalDoc*>(GetDocument());
	CDigitalUnitBit* digUnit = doc->GetDigitalUnitBitRef();

	// Read data from UI and apply to Digital unit	
	digUnit->SG_NormalState = m_dbCfgNormalState.GetCurSel();
	doc->SetDigitalName( SimpleGetWindowText(m_txtCfgName) );
	
	// Rebuild analog channels in projectview and update window header
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->UpdateDigitalName(
		doc->GetDocHandle().PdcSimulatorID, doc->GetDocHandle().PmuIndex, doc->GetDocHandle().DigitalIndex);	
	SetDisplayHeader( );
	m_btnCfgCommit.EnableWindow(false);
}


void SimGUI_DigitalView::OnBnClickedRdtCommit()
{
	//Get a reference to the current Digital feed
	SimGUI_DigitalDoc* doc = dynamic_cast<SimGUI_DigitalDoc*>(GetDocument());
	BridgedPmuDataFeed* bridgedDataFeed = doc->GetPmuDataFeed();
	IDigitalDataFeed* DigitalFeed = bridgedDataFeed->GetDigitalOverride(doc->GetDocHandle().DigitalIndex);	
	NewDocState docState = doc->GetDocHandle();

	// Handle "rdt commit"  based on the current type selected
	int checkedId = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2); // radio1 = CONST / radio2 = FILE
	if( checkedId == IDC_RADIO1 )
	{
		ConstantDigitalDataFeed* constDataFeed = dynamic_cast<ConstantDigitalDataFeed*>(DigitalFeed);
		if( constDataFeed == NULL ){
			constDataFeed = new ConstantDigitalDataFeed() ;
			bridgedDataFeed->SetDigitalOverride(docState.DigitalIndex, constDataFeed);
		}

		// Set constant values
		constDataFeed->SetDigitalVal( m_dbRtdValue.GetCurSel() != 0 );
	}
	else if( doc->GetSimulator()->SimulatorIsActive() == false )
	{
		FilestreamDigitalDataFeed* digFileStream = dynamic_cast<FilestreamDigitalDataFeed*>(DigitalFeed);
		if( digFileStream == NULL ) {
			digFileStream = new FilestreamDigitalDataFeed();
			bridgedDataFeed->SetDigitalOverride(docState.DigitalIndex, digFileStream);
		}

		// Set new input file
		digFileStream->SetInputFile(SimpleGetWindowText(m_txtRtdFileInput));
	}

	// Update gui
	m_btnRtdCommit.EnableWindow(false);
}


void SimGUI_DigitalView::OnBnClickedRtdLoad()
{
	// Open open-file dialog
	CFileDialog fOpenDlg(true, _T("txt"), NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, 
		_T("Digital Data Files (*.digcsv)|*.digcsv||"), this);  
	fOpenDlg.m_pOFN->lpstrTitle= _T("Digital input file");  
	fOpenDlg.m_pOFN->lpstrInitialDir= _T("c:");
	bool retval = fOpenDlg.DoModal()==IDOK;
  
	// handle result
	if(retval)
	{
		CString filename = fOpenDlg.GetPathName();
		m_txtRtdFileInput.SetWindowTextW(filename);
	}
}


void SimGUI_DigitalView::OnBnClickedRadioInputType()
{
	HandleStateChange();
}


void SimGUI_DigitalView::OnCfgChange()
{
	m_btnCfgCommit.EnableWindow(true);
}

void SimGUI_DigitalView::OnRtdChange()
{
	m_btnRtdCommit.EnableWindow(true);
}
