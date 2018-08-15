// SimGUI_PhasorView.cpp : implementation file
//

#include "stdafx.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "SimGUI_PhasorView.h"
#include "SimGUI_PhasorDoc.h"
#include "ConstantChannelDataFeed.h"
#include "FilestreamChannelDataFeed.h"
#include "Common.h"
#include "MainFrm.h"

std::map<NewDocState, SimGUI_PhasorView*> SimGUI_PhasorView::ViewList;

// SimGUI_PhasorView

IMPLEMENT_DYNCREATE(SimGUI_PhasorView, CFormView)

SimGUI_PhasorView::SimGUI_PhasorView()
	: CFormView(SimGUI_PhasorView::IDD)
	, m_inputGroup(0)
{

}

SimGUI_PhasorView::~SimGUI_PhasorView()
{
	ViewList.erase(((SimGUI_PhasorDoc*)GetDocument())->GetDocHandle() );
}


void SimGUI_PhasorView::SetDisplayHeader( )
{
	CDocument* blah = GetDocument();
	SimGUI_PhasorDoc* doc = dynamic_cast<SimGUI_PhasorDoc*>(GetDocument());
	CPmuConfig* refPmuConfig = doc->GetSimulator()->getConfigurationRef()->SG_PmuConfigArray.items[doc->GetDocHandle().PmuIndex];
	
	CString tmp;
	tmp.Format(_T("Phasor - %s"), refPmuConfig->SG_PhasorNames.items[doc->GetDocHandle().PhasorIndex]);
	GetParent()->SetWindowText(tmp);
}
 

void SimGUI_PhasorView::HandleStateChange()
{
	SimGUI_PhasorDoc* doc = dynamic_cast<SimGUI_PhasorDoc*>(GetDocument());
	const bool SIM_ISACTIVE = doc->GetSimulator()->SimulatorIsActive();

	int checkedId = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2);
	
	m_txtInputFile.EnableWindow(!SIM_ISACTIVE && checkedId == IDC_RADIO2);
	m_btnFileLoad.EnableWindow(!SIM_ISACTIVE && checkedId == IDC_RADIO2);
	GetDlgItem( IDC_RADIO1 )->EnableWindow(!SIM_ISACTIVE);
	GetDlgItem( IDC_RADIO2 )->EnableWindow(!SIM_ISACTIVE);
	m_txtConstInt.EnableWindow(checkedId == IDC_RADIO1);
	m_txtConstImag.EnableWindow(checkedId == IDC_RADIO1);

	// Set commit RTD button state based on current input data type and simulator state
	m_btnRtdCommit.EnableWindow( !(SIM_ISACTIVE && checkedId == IDC_RADIO2 ) );
}

void SimGUI_PhasorView::OnInitialUpdate()
{
	SimGUI_PhasorDoc* doc = dynamic_cast<SimGUI_PhasorDoc*>(GetDocument());
	if( doc == NULL ) return;

	// Set the correct "input type" based on the current phasor input datafeed type
	BridgedPmuDataFeed* bridgedDataFeed = doc->GetPmuDataFeed();
	IPhasorDataFeed* phasorFeed = bridgedDataFeed->GetPhasorOverride(doc->GetDocHandle().PhasorIndex);
	FilestreamPhasorDataFeed* phasorFileStream = dynamic_cast<FilestreamPhasorDataFeed*>(phasorFeed);
	ConstantPhasorDataFeed* phasorConstInput = dynamic_cast<ConstantPhasorDataFeed*>(phasorFeed);

	// Set input type based on the type of datastream (default to constant if missing)
	if( phasorFileStream != NULL )
		m_inputGroup = 1;
	else 
		m_inputGroup = 0;	

	// ..
	CFormView::OnInitialUpdate();

	if( phasorFileStream != NULL ) {
		m_txtInputFile.SetWindowTextW(phasorFileStream->GetInputFile());
	}
	else 
	{
		// Rebuild phasor input if NULL or not deferrable to phasorConstInput
		if( phasorConstInput == NULL ) {
			delete phasorFeed;
			phasorFeed = phasorConstInput = new ConstantPhasorDataFeed();
			bridgedDataFeed->SetPhasorOverride(doc->GetDocHandle().PhasorIndex, phasorFeed);
		}

		float real, imag;
		phasorConstInput->GetRealImag(&real, &imag);
		m_txtConstInt.SetWindowTextW( NumberToCStringASCII_f(real) );
		m_txtConstImag.SetWindowTextW( NumberToCStringASCII_f(imag) );
	}

	// Initialize phasor type droplist
	m_dbPhasorType.AddString(_T("Volt"));
	m_dbPhasorType.AddString(_T("Current"));

	// Set phasor configuration		
	m_phasorName.SetWindowTextW(doc->GetPhasorName() );
	CPhasorUnit* phasorUnit = doc->GetPhasorUnitRef();
	m_dbPhasorType.SetCurSel(phasorUnit->SG_Type);
	m_txtScalar.SetWindowTextW( NumberToCStringASCII_i( phasorUnit->SG_Scalar ) );

	// Update 'enabled' state based on simulator state
	HandleStateChange();
	SetDisplayHeader( );
	
	m_btnCfgCommit.EnableWindow(false);
	m_btnRtdCommit.EnableWindow(false);

	// Bind to viewlist
	ViewList[doc->GetDocHandle()] = this;	
}

void SimGUI_PhasorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_dbPhasorType);
	DDX_Control(pDX, IDC_EDIT1, m_txtScalar);
	DDX_Radio(pDX, IDC_RADIO1, m_inputGroup);
	DDX_Control(pDX, IDC_EDIT2, m_txtInputFile);
	DDX_Control(pDX, IDC_BUTTON2, m_btnFileLoad);
	DDX_Control(pDX, IDC_EDIT3, m_txtConstInt);
	DDX_Control(pDX, IDC_EDIT4, m_txtConstImag);
	DDX_Control(pDX, IDC_PHRDTCOMMIT, m_btnRtdCommit);
	DDX_Control(pDX, IDC_EDIT7, m_phasorName);
	DDX_Control(pDX, IDC_PHCFGCOMMIT, m_btnCfgCommit);
}

BEGIN_MESSAGE_MAP(SimGUI_PhasorView, CFormView)
	ON_BN_CLICKED(IDC_RADIO1, &SimGUI_PhasorView::OnPhasorInputChanged)
	ON_BN_CLICKED(IDC_RADIO2, &SimGUI_PhasorView::OnPhasorInputChanged)
	ON_BN_CLICKED(IDC_BUTTON2, &SimGUI_PhasorView::OnBnClickedLoadInputFile)
	ON_BN_CLICKED(IDC_PHCFGCOMMIT, &SimGUI_PhasorView::OnBnClickedPhcfgcommit)
	ON_BN_CLICKED(IDC_PHRDTCOMMIT, &SimGUI_PhasorView::OnBnClickedPhrdtcommit)
	ON_EN_CHANGE(IDC_EDIT7, &SimGUI_PhasorView::OnCfgChange)
	ON_EN_CHANGE(IDC_EDIT1, &SimGUI_PhasorView::OnCfgChange)
	ON_CBN_SELCHANGE(IDC_COMBO1, &SimGUI_PhasorView::OnCfgChange)
	ON_EN_CHANGE(IDC_EDIT3, &SimGUI_PhasorView::OnRtdChange)
	ON_EN_CHANGE(IDC_EDIT4, &SimGUI_PhasorView::OnRtdChange)
END_MESSAGE_MAP()


// SimGUI_PhasorView diagnostics

#ifdef _DEBUG
void SimGUI_PhasorView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void SimGUI_PhasorView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// SimGUI_PhasorView message handlers


void SimGUI_PhasorView::OnPhasorInputChanged()
{
	HandleStateChange();
}


void SimGUI_PhasorView::OnBnClickedLoadInputFile()
{	
	// Open open-file dialog
	CFileDialog fOpenDlg(true, _T("txt"), NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, 
		_T("Phasor Data Files (*.phcsv)|*.phcsv||"), this);  
	fOpenDlg.m_pOFN->lpstrTitle= _T("Phasor input file");  
	fOpenDlg.m_pOFN->lpstrInitialDir= _T("c:");
	bool retval = fOpenDlg.DoModal()==IDOK;
  
	// handle result
	if(retval)
	{
		CString filename = fOpenDlg.GetPathName();
		m_txtInputFile.SetWindowTextW(filename);

		// Update GUI
		m_btnRtdCommit.EnableWindow(true);
	}

}

// Commit configuration
void SimGUI_PhasorView::OnBnClickedPhcfgcommit()
{
	SimGUI_PhasorDoc* doc = dynamic_cast<SimGUI_PhasorDoc*>(GetDocument());

	// Get a reference to phasor unit configuration
	CPhasorUnit* phasorUnit = dynamic_cast<SimGUI_PhasorDoc*>(GetDocument())->GetPhasorUnitRef();

	// Read data from UI and apply to phasor configuration
	phasorUnit->SG_Type = m_dbPhasorType.GetCurSel();
	phasorUnit->SG_Scalar = GetEditTextAsInteger(m_txtScalar, 0);
	doc->SetPhasorName( SimpleGetWindowText(m_phasorName) );

	// Update GUI
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->UpdatePhasorName(
		doc->GetDocHandle().PdcSimulatorID, doc->GetDocHandle().PmuIndex, doc->GetDocHandle().PhasorIndex);	
	SetDisplayHeader( );
	m_btnCfgCommit.EnableWindow(false);
}


// Commit realtime data
void SimGUI_PhasorView::OnBnClickedPhrdtcommit()
{
	//Get a reference to the current phasor feed
	SimGUI_PhasorDoc* doc = dynamic_cast<SimGUI_PhasorDoc*>(GetDocument());
	BridgedPmuDataFeed* bridgedDataFeed = doc->GetPmuDataFeed();
	IPhasorDataFeed* phasorFeed = bridgedDataFeed->GetPhasorOverride(doc->GetDocHandle().PhasorIndex);	
	NewDocState docState = doc->GetDocHandle();

	// Handle "rdt commit"  based on the current type selected
	int checkedId = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2); // radio1 = CONST / radio2 = FILE
	if( checkedId == IDC_RADIO1 )
	{
		ConstantPhasorDataFeed* constDataFeed = dynamic_cast<ConstantPhasorDataFeed*>(phasorFeed);
		if( constDataFeed == NULL ){
			constDataFeed = new ConstantPhasorDataFeed() ;
			bridgedDataFeed->SetPhasorOverride(docState.PhasorIndex, constDataFeed);
		}

		// Set constant values
		constDataFeed->SetRealImag( GetEditTextAsFloat(m_txtConstInt, 0), GetEditTextAsFloat( m_txtConstImag, 0 ) );
	}
	else if( doc->GetSimulator()->SimulatorIsActive() == false )
	{
		FilestreamPhasorDataFeed* phasorFileStream = dynamic_cast<FilestreamPhasorDataFeed*>(phasorFeed);
		if( phasorFileStream == NULL ) {
			phasorFileStream = new FilestreamPhasorDataFeed();
			bridgedDataFeed->SetPhasorOverride(docState.PhasorIndex, phasorFileStream);
		}

		// Set new input file
		phasorFileStream->SetInputFile(SimpleGetWindowText(m_txtInputFile));
	}

	// Update gui
	m_btnRtdCommit.EnableWindow(false);
}


void SimGUI_PhasorView::OnCfgChange()
{
	m_btnCfgCommit.EnableWindow(true);
}

void SimGUI_PhasorView::OnRtdChange()
{
	m_btnRtdCommit.EnableWindow(true);
}
