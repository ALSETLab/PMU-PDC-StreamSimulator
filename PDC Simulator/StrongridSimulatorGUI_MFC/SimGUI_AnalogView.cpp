// SimGUI_AnalogView.cpp : implementation file
//

#include "stdafx.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "SimGUI_AnalogView.h"
#include "SimGUI_AnalogDoc.h"
#include "FilestreamChannelDataFeed.h"
#include "ConstantChannelDataFeed.h"
#include "Common.h"
#include "MainFrm.h"

std::map<NewDocState, SimGUI_AnalogView*> SimGUI_AnalogView::ViewList;

// SimGUI_AnalogView

IMPLEMENT_DYNCREATE(SimGUI_AnalogView, CFormView)

SimGUI_AnalogView::SimGUI_AnalogView()
	: CFormView(SimGUI_AnalogView::IDD)
	, m_rbRtdInputType(0)
{

}

SimGUI_AnalogView::~SimGUI_AnalogView()
{
	ViewList.erase(((SimGUI_AnalogDoc*)GetDocument())->GetDocHandle() );
}


void SimGUI_AnalogView::SetDisplayHeader( )
{
	CDocument* blah = GetDocument();
	SimGUI_AnalogDoc* doc = dynamic_cast<SimGUI_AnalogDoc*>(GetDocument());
	CPmuConfig* refPmuConfig = doc->GetSimulator()->getConfigurationRef()->SG_PmuConfigArray.items[doc->GetDocHandle().PmuIndex];
	
	CString tmp;
	tmp.Format(_T("Analog - %s"), refPmuConfig->SG_AnalogNames.items[doc->GetDocHandle().AnalogIndex]);
	GetParent()->SetWindowText(tmp);
}
 


void SimGUI_AnalogView::HandleStateChange()
{
	SimGUI_AnalogDoc* doc = dynamic_cast<SimGUI_AnalogDoc*>(GetDocument());
	const bool SIM_ISACTIVE = doc->GetSimulator()->SimulatorIsActive();

	int checkedId = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2);
	
	m_txtRtdInputFile.EnableWindow(!SIM_ISACTIVE && checkedId == IDC_RADIO2);
	m_btnRtdFind.EnableWindow(!SIM_ISACTIVE && checkedId == IDC_RADIO2);
	GetDlgItem( IDC_RADIO1 )->EnableWindow(!SIM_ISACTIVE);
	GetDlgItem( IDC_RADIO2 )->EnableWindow(!SIM_ISACTIVE);
	m_txtRtdValue.EnableWindow(checkedId == IDC_RADIO1);

	// Set commit RTD button state based on current input data type and simulator state
	m_btnRtdCommit.EnableWindow( !(SIM_ISACTIVE && checkedId == IDC_RADIO2 ) );
}

void SimGUI_AnalogView::OnInitialUpdate()
{
	SimGUI_AnalogDoc* doc = dynamic_cast<SimGUI_AnalogDoc*>(GetDocument());
	if( doc == NULL ) return;

	// Set the correct "input type" based on the current analog input datafeed type
	BridgedPmuDataFeed* bridgedDataFeed = doc->GetPmuDataFeed();
	IAnalogDataFeed* analogFeed = bridgedDataFeed->GetAnalogOverride(doc->GetDocHandle().AnalogIndex);
	FilestreamAnalogDataFeed* analogFileStream = dynamic_cast<FilestreamAnalogDataFeed*>(analogFeed);
	ConstantAnalogDataFeed* analogConstInput = dynamic_cast<ConstantAnalogDataFeed*>(analogFeed);

	// Set input type based on the type of datastream (default to constant if missing)
	if( analogFileStream != NULL )
		m_rbRtdInputType = 1;
	else 
		m_rbRtdInputType = 0;	

	// ..
	CFormView::OnInitialUpdate();

	if( analogFileStream != NULL ) {
		m_txtRtdInputFile.SetWindowTextW(analogFileStream->GetInputFile());
	}
	else 
	{
		// Rebuild analog input if NULL or not deferrable to analogConstInput
		if( analogConstInput == NULL )
		{
			delete analogFeed;
			analogFeed = analogConstInput = new ConstantAnalogDataFeed();
			bridgedDataFeed->SetAnalogOverride(doc->GetDocHandle().AnalogIndex, analogFeed);
		}

		float ang_value;
		analogConstInput->GetAnalogValue(&ang_value);
		m_txtRtdValue.SetWindowTextW( NumberToCStringASCII_f(ang_value) );
	}

	// Initialize analog type droplist
	m_cbCfgType.AddString(_T("Single point on wave"));
	m_cbCfgType.AddString(_T("RMS of analog input"));
	m_cbCfgType.AddString(_T("Peak of analog input"));

	// Set analog configuration	
	m_txtCfgAnalogName.SetWindowTextW(doc->GetAnalogName() );
	CAnalogUnit* analogUnit = doc->GetAnalogUnitRef();
	m_cbCfgType.SetCurSel(analogUnit->SG_Type);
	m_txtCfgScalar.SetWindowTextW( NumberToCStringASCII_i( analogUnit->SG_Scalar ) );

	// Update 'enabled' state based on simulator state
	HandleStateChange();
	SetDisplayHeader( );	
	m_btnCfgCommit.EnableWindow(false);
	m_btnRtdCommit.EnableWindow(false);

	// Bind to viewlist
	ViewList[doc->GetDocHandle()] = this;	
}

void SimGUI_AnalogView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ANGRDTCOMMIT, m_btnRtdCommit);
	DDX_Control(pDX, IDC_ANGCFGCOMMIT, m_btnCfgCommit);
	DDX_Control(pDX, IDC_COMBO1, m_cbCfgType);
	DDX_Control(pDX, IDC_EDIT1, m_txtCfgScalar);
	DDX_Control(pDX, IDC_BUTTON2, m_btnRtdFind);
	DDX_Control(pDX, IDC_EDIT2, m_txtRtdInputFile);
	DDX_Control(pDX, IDC_EDIT3, m_txtRtdValue);
	DDX_Radio(pDX, IDC_RADIO1, m_rbRtdInputType);
	DDX_Control(pDX, IDC_EDIT7, m_txtCfgAnalogName);
}

BEGIN_MESSAGE_MAP(SimGUI_AnalogView, CFormView)
	ON_BN_CLICKED(IDC_ANGCFGCOMMIT, &SimGUI_AnalogView::OnBnClickedCfgCommit)
	ON_BN_CLICKED(IDC_ANGRDTCOMMIT, &SimGUI_AnalogView::OnBnClickedRdtCommit)
	ON_BN_CLICKED(IDC_BUTTON2, &SimGUI_AnalogView::OnBnClickedRtdLoad)
	ON_BN_CLICKED(IDC_RADIO1, &SimGUI_AnalogView::OnBnClickedRadioInputType)
	ON_BN_CLICKED(IDC_RADIO2, &SimGUI_AnalogView::OnBnClickedRadioInputType)
	ON_EN_CHANGE(IDC_EDIT7, &SimGUI_AnalogView::OnCfgChange)
	ON_CBN_SELCHANGE(IDC_COMBO1, &SimGUI_AnalogView::OnCfgChange)
	ON_EN_CHANGE(IDC_EDIT1, &SimGUI_AnalogView::OnCfgChange)
	ON_EN_CHANGE(IDC_EDIT3, &SimGUI_AnalogView::OnRtdChange)
END_MESSAGE_MAP()


// SimGUI_AnalogView diagnostics

#ifdef _DEBUG
void SimGUI_AnalogView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void SimGUI_AnalogView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// SimGUI_AnalogView message handlers


void SimGUI_AnalogView::OnBnClickedCfgCommit()
{
	// Get a reference to analog unit configuration
	SimGUI_AnalogDoc* doc = dynamic_cast<SimGUI_AnalogDoc*>(GetDocument());
	CAnalogUnit* AnalogUnit = doc->GetAnalogUnitRef();

	// Read data from UI and apply to Analog unit	
	AnalogUnit->SG_Type = m_cbCfgType.GetCurSel();
	AnalogUnit->SG_Scalar = GetEditTextAsInteger(m_txtCfgScalar, 0);
	doc->SetAnalogName( SimpleGetWindowText(m_txtCfgAnalogName) );
	
	// Update GUI
	((CMainFrame*)GetParentOwner())->GetProjectViewRef()->UpdateAnalogName(
		doc->GetDocHandle().PdcSimulatorID, doc->GetDocHandle().PmuIndex, doc->GetDocHandle().AnalogIndex);	
	SetDisplayHeader( );
	m_btnCfgCommit.EnableWindow(false);
}

void SimGUI_AnalogView::OnBnClickedRtdLoad()//Moved up by one function
{
	// Open open-file dialog
	CFileDialog fOpenDlg(true, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("Analog Data Files (*.angcsv)|*.angcsv||"), this);
	fOpenDlg.m_pOFN->lpstrTitle = _T("Analog input file");
	fOpenDlg.m_pOFN->lpstrInitialDir = _T("c:");
	bool retval = fOpenDlg.DoModal() == IDOK;

	// handle result
	if (retval)
	{
		CString filename = fOpenDlg.GetPathName();
		m_txtRtdInputFile.SetWindowTextW(filename);

		m_btnRtdCommit.EnableWindow(true);
	}
}

void SimGUI_AnalogView::OnBnClickedRdtCommit()
{
	//Get a reference to the current Analog feed
	SimGUI_AnalogDoc* doc = dynamic_cast<SimGUI_AnalogDoc*>(GetDocument());
	BridgedPmuDataFeed* bridgedDataFeed = doc->GetPmuDataFeed();
	IAnalogDataFeed* AnalogFeed = bridgedDataFeed->GetAnalogOverride(doc->GetDocHandle().AnalogIndex);	
	NewDocState docState = doc->GetDocHandle();

	// Handle "rdt commit"  based on the current type selected
	int checkedId = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2); // radio1 = CONST / radio2 = FILE
	if( checkedId == IDC_RADIO1 )
	{
		ConstantAnalogDataFeed* constDataFeed = dynamic_cast<ConstantAnalogDataFeed*>(AnalogFeed);
		if( constDataFeed == NULL ){
			constDataFeed = new ConstantAnalogDataFeed() ;
			bridgedDataFeed->SetAnalogOverride(docState.AnalogIndex, constDataFeed);
		}

		// Set constant values
		constDataFeed->SetAnalogValue( GetEditTextAsFloat(m_txtRtdValue, 0) );
	}
	else if( doc->GetSimulator()->SimulatorIsActive() == false )
	{
		FilestreamAnalogDataFeed* AnalogFileStream = dynamic_cast<FilestreamAnalogDataFeed*>(AnalogFeed);
		if( AnalogFileStream == NULL ) {
			AnalogFileStream = new FilestreamAnalogDataFeed();
			bridgedDataFeed->SetAnalogOverride(docState.AnalogIndex, AnalogFileStream);
		}

		// Set new input file
		AnalogFileStream->SetInputFile(SimpleGetWindowText(m_txtRtdInputFile));
	}

	// Update GUI
	m_btnRtdCommit.EnableWindow(false);
}





void SimGUI_AnalogView::OnBnClickedRadioInputType()
{
	HandleStateChange();
}


void SimGUI_AnalogView::OnCfgChange()
{
	m_btnCfgCommit.EnableWindow(true);
}


void SimGUI_AnalogView::OnRtdChange()
{
	m_btnRtdCommit.EnableWindow(true);
}

