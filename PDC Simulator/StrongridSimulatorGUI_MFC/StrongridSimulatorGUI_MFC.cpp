
// StrongridSimulatorGUI_MFC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "StrongridSimulatorGUI_MFC.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "StrongridSimulatorGUI_MFCDoc.h"
#include "StrongridSimulatorGUI_MFCView.h"
#include "SimGUI_PdcView.h"
#include "SimGUI_PmuView.h"
#include "SimGUI_PhasorView.h"
#include "SimGUI_AnalogView.h"
#include "SimGUI_DigitalView.h"
#include "SimulatorDataTypes.h"
#include "SimGUI_PdcDoc.h"
#include "SimGUI_PmuDoc.h"
#include "SimGUI_PhasorDoc.h"
#include "SimGUI_AnalogDoc.h"
#include "SimGUI_DigitalDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(StrongridSIM_App, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &StrongridSIM_App::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_NEW_NEWSOLUTION, &StrongridSIM_App::OnCreateNewSolution)
	ON_COMMAND(ID_NEW_NEWPDC, &StrongridSIM_App::OnCreateNewPdcWindow)
	ON_COMMAND(ID_FILE_OPEN, &StrongridSIM_App::OnSimulatorSolutionOpen)
	ON_COMMAND(ID_FILE_SAVE_MY, &StrongridSIM_App::OnSimulatorSolutionSave)
	ON_COMMAND(ID_FILE_SAVE_AS_MY, &StrongridSIM_App::OnSimulatorSolutionSaveAs)
	ON_COMMAND(ID_FILE_CLOSE_MY, &StrongridSIM_App::OnCloseSolution)
	
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// StrongridSIM_App construction

StrongridSIM_App::StrongridSIM_App()
{
	m_solutionFilePath = "";
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	SetAppID(_T("StrongridSimulatorGUI_MFC.AppID.NoVersion"));
}


// The one and only StrongridSIM_App object

StrongridSIM_App theApp;




PdcSimulatorManager* StrongridSIM_App::GetPdcSimManager()
{
	return this->m_pdcSimulatorManager;
}

void StrongridSIM_App::OpenDocumentViewByPdcIndex(int pdcSimulatorId)
{
	
	if( SimGUI_PdcView::ViewList.find( pdcSimulatorId ) != SimGUI_PdcView::ViewList.end() )
	{
		CView* view = SimGUI_PdcView::ViewList[pdcSimulatorId];	
		CMDIChildWnd* tmp = (CMDIChildWnd*)view->GetParentFrame();
		tmp->MDIActivate();
		tmp->BringWindowToTop();
	}
	else
	{
		CDocTemplate* tmp =0;
		POSITION p = this->GetFirstDocTemplatePosition();
		while( p != 0 ) {
			tmp = this->GetNextDocTemplate(p);
			CString docname;
			tmp->GetDocString(docname, CDocTemplate::DocStringIndex::docName);
			if( docname.CompareNoCase(_T("pdcmainview")) == 0 ) break;
		}
		if( tmp == 0 ) throw EXCEPTION_DEBUG_EVENT;

		// Setup info for new window
		m_prevDocState.DocType = 1;
		m_prevDocState.PdcSimulatorID = pdcSimulatorId;

		// Open the window
		tmp->OpenDocumentFile(NULL);
		//((CMainFrame*)GetMainWnd())->GetProjectViewRef()->ForceRebuild();
	}

}


void StrongridSIM_App::OpenDocumentViewByPmuIndex(PdcSimulatorID pdcSimulatorId, int pmuIdx)
{
	NewDocState tmpId = NewDocState::CreateAsPmuId(pdcSimulatorId, pmuIdx);
	std::map<NewDocState, SimGUI_PmuView*>::iterator iter = SimGUI_PmuView::ViewList.find( tmpId );

	if( iter != SimGUI_PmuView::ViewList.end() )
	{
		CView* view = iter->second;
		CMDIChildWnd* tmp = (CMDIChildWnd*)view->GetParentFrame();
		tmp->MDIActivate();
		tmp->BringWindowToTop();
	}
	else
	{
		CDocTemplate* tmp =0;
		POSITION p = this->GetFirstDocTemplatePosition();
		while( p != 0 ) {
			tmp = this->GetNextDocTemplate(p);
			CString docname;
			tmp->GetDocString(docname, CDocTemplate::DocStringIndex::docName);
			if( docname.CompareNoCase(_T("pmumainview")) == 0 ) break;
		}
		if( tmp == 0 ) throw EXCEPTION_DEBUG_EVENT;

		// Setup info for new window
		m_prevDocState.DocType = 2;
		m_prevDocState.PdcSimulatorID = pdcSimulatorId;
		m_prevDocState.PmuIndex = pmuIdx;

		// Open the window
		tmp->OpenDocumentFile(NULL);
	}
}

void StrongridSIM_App::OpenDocumentViewByPhasorIndex(int pdcSimulatorId, int pmuIdx, int phIdx)
{
	NewDocState tmpId = NewDocState::CreateAsPhasorId(pdcSimulatorId, pmuIdx, phIdx);
	std::map<NewDocState, SimGUI_PhasorView*>::iterator iter = SimGUI_PhasorView::ViewList.find( tmpId );

	if( iter != SimGUI_PhasorView::ViewList.end() )
	{
		CView* view = iter->second;
		CMDIChildWnd* tmp = (CMDIChildWnd*)view->GetParentFrame();
		tmp->MDIActivate();
		tmp->BringWindowToTop();
	}
	else
	{
		CDocTemplate* tmp =0;
		POSITION p = this->GetFirstDocTemplatePosition();
		while( p != 0 ) {
			tmp = this->GetNextDocTemplate(p);
			CString docname;
			tmp->GetDocString(docname, CDocTemplate::DocStringIndex::docName);
			if( docname.CompareNoCase(_T("phasormainview")) == 0 ) break;
		}
		if( tmp == 0 ) throw EXCEPTION_DEBUG_EVENT;

		// Setup info for new window
		m_prevDocState.DocType = 3;
		m_prevDocState.PdcSimulatorID = pdcSimulatorId;
		m_prevDocState.PmuIndex = pmuIdx;
		m_prevDocState.PhasorIndex = phIdx;

		// Open the window
		tmp->OpenDocumentFile(NULL);
	}
}

void StrongridSIM_App::OpenDocumentViewByAnalogIndex(int pdcSimulatorId, int pmuIdx, int angIdx)
{
	NewDocState tmpId = NewDocState::CreateAsAnalogId(pdcSimulatorId, pmuIdx, angIdx);
	std::map<NewDocState, SimGUI_AnalogView*>::iterator iter = SimGUI_AnalogView::ViewList.find( tmpId );

	if( iter != SimGUI_AnalogView::ViewList.end() )
	{
		CView* view = iter->second;
		CMDIChildWnd* tmp = (CMDIChildWnd*)view->GetParentFrame();
		tmp->MDIActivate();
		tmp->BringWindowToTop();
	}
	else
	{
		CDocTemplate* tmp =0;
		POSITION p = this->GetFirstDocTemplatePosition();
		while( p != 0 ) {
			tmp = this->GetNextDocTemplate(p);
			CString docname;
			tmp->GetDocString(docname, CDocTemplate::DocStringIndex::docName);
			if( docname.CompareNoCase(_T("analogmainview")) == 0 ) break;
		}
		if( tmp == 0 ) throw EXCEPTION_DEBUG_EVENT;

		// Setup info for new window
		m_prevDocState.DocType = 4;
		m_prevDocState.PdcSimulatorID = pdcSimulatorId;
		m_prevDocState.PmuIndex = pmuIdx;
		m_prevDocState.AnalogIndex = angIdx;

		// Open the window
		tmp->OpenDocumentFile(NULL);
	}
}

void StrongridSIM_App::OpenDocumentViewByDigitalIndex(int pdcSimulatorId, int pmuIdx, int digIdx)
{
	NewDocState tmpId = NewDocState::CreateAsDigitalId(pdcSimulatorId, pmuIdx, digIdx);
	std::map<NewDocState, SimGUI_DigitalView*>::iterator iter = SimGUI_DigitalView::ViewList.find( tmpId );

	if( iter != SimGUI_DigitalView::ViewList.end() )
	{
		CView* view = iter->second;
		CMDIChildWnd* tmp = (CMDIChildWnd*)view->GetParentFrame();
		tmp->MDIActivate();
		tmp->BringWindowToTop();
	}
	else
	{
		CDocTemplate* tmp =0;
		POSITION p = this->GetFirstDocTemplatePosition();
		while( p != 0 ) {
			tmp = this->GetNextDocTemplate(p);
			CString docname;
			tmp->GetDocString(docname, CDocTemplate::DocStringIndex::docName);
			if( docname.CompareNoCase(_T("digitalmainview")) == 0 ) break;
		}
		if( tmp == 0 ) throw EXCEPTION_DEBUG_EVENT;

		// Setup info for new window
		m_prevDocState.DocType = 5;
		m_prevDocState.PdcSimulatorID = pdcSimulatorId;
		m_prevDocState.PmuIndex = pmuIdx;
		m_prevDocState.DigitalIndex = digIdx;

		// Open the window
		tmp->OpenDocumentFile(NULL);
	}
}

NewDocState StrongridSIM_App::GetNewDocState()
{
	return m_prevDocState;
}

// StrongridSIM_App initialization

BOOL StrongridSIM_App::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	
	pDocTemplate = new CMultiDocTemplate(IDR_StrongridPdcType,
		RUNTIME_CLASS(SimGUI_PdcDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(SimGUI_PdcView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Add template: PDC view
	pDocTemplate = new CMultiDocTemplate(IDR_StrongridSimulaTYPE,
		RUNTIME_CLASS(StrongridSIM_Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(StrongridSIM_View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	
	// Add template: PMU view
	pDocTemplate = new CMultiDocTemplate(IDR_StrongridPmuType,
		RUNTIME_CLASS(SimGUI_PmuDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(SimGUI_PmuView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Add: Phasor view
	pDocTemplate = new CMultiDocTemplate(IDR_StrongridPhasorType,
		RUNTIME_CLASS(SimGUI_PhasorDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(SimGUI_PhasorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Add: Analog view
	pDocTemplate = new CMultiDocTemplate(IDR_StrongridAnalogType,
		RUNTIME_CLASS(SimGUI_AnalogDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(SimGUI_AnalogView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Add: Digital view
	pDocTemplate = new CMultiDocTemplate(IDR_StrongridDigitalType,
		RUNTIME_CLASS(SimGUI_DigitalDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(SimGUI_DigitalView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	 // Don't display a new MDI child window during startup
    if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
      cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	
	
	m_pdcSimulatorManager = new PdcSimulatorManager(&((CMainFrame*)GetMainWnd())->m_wndOutput);
	theApp.OnCreateNewSolution();

	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int StrongridSIM_App::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// StrongridSIM_App message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void StrongridSIM_App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// StrongridSIM_App customization load/save methods

void StrongridSIM_App::OnCreateNewSolution()
{
	OnCloseSolution();
	OnCreateNewPdcWindow();
	((CMainFrame*)GetMainWnd())->GetProjectViewRef()->ForceRebuild();
}

void StrongridSIM_App::OnCreateNewPdcWindow() 
{		
	// Create the simulator	
	int pdcSimid = theApp.GetPdcSimManager()->CreateNewPdcSimulator(_T("New Pdc"));

	// Create/Show the windows
	OpenDocumentViewByPdcIndex(pdcSimid);
}


void StrongridSIM_App::OnSimulatorSolutionSave()
{
	// If the simulator is not associated with an already solution file - redirect to "save as"
	if( m_solutionFilePath.GetLength() == 0 ) {
		OnSimulatorSolutionSaveAs();
		return;
	}

	// Setup serialization structures
	CFile file( m_solutionFilePath, CFile::OpenFlags::modeWrite );
	CArchive ca(&file, CArchive::Mode::store);
	
	// Serialize to archive
	SerializeSolutionToArchive(ca);

	// Cleanup
	ca.Flush();
	ca.Close();
	file.Close();
}


void StrongridSIM_App::OnSimulatorSolutionSaveAs()
{
	TCHAR szFilters[]= _T("PDC simulator solution Files (*.simsln)|*.simsln|All Files (*.*)|*.*||");

	// Create an Open dialog
	CFileDialog fileDlg(FALSE, _T("simsln"), _T("pdcsimulator.simsln"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	// Handle result
	if(fileDlg.DoModal() == IDOK)
	{
		m_solutionFilePath = fileDlg.GetPathName();
		m_solutionFileName = fileDlg.GetFileTitle();

		// Setup serialization structures
		CFile file( m_solutionFilePath, CFile::OpenFlags::modeCreate | CFile::OpenFlags::modeWrite );
		CArchive ca(&file, CArchive::Mode::store);
	
		// Serialize to archive
		SerializeSolutionToArchive(ca);

		// Cleanup
		ca.Flush();
		ca.Close();
		file.Close();
	}
}

void StrongridSIM_App::SerializeSolutionToArchive(CArchive& ca)
{		
	std::vector<PdcSimulatorID> simList = m_pdcSimulatorManager->EnumeratePdcSimulators();

	// Write the number of simulators within the project
	ca << (int)simList.size();

	// Serialize all PDC's to file in sequence
	for( std::vector<PdcSimulatorID>::iterator iterSimId = simList.begin(); iterSimId != simList.end(); ++iterSimId )
	{
		ManagedPdcSimulator* sim = m_pdcSimulatorManager->GetPdcSimulator(*iterSimId);
		sim->Serialize(ca);
	}
}

void StrongridSIM_App::OnSimulatorSolutionOpen()
{
	// Create an Open dialog
	TCHAR szFilters[]= _T("PDC simulator solution Files (*.simsln)|*.simsln|All Files (*.*)|*.*||");
	CFileDialog fileDlg(FALSE, _T("simsln"), _T("*.simsln"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	// handle result
	if(fileDlg.DoModal() == IDOK)
	{
		OnCloseSolution();
		
		// Get filenames of the new solution
		m_solutionFilePath = fileDlg.GetPathName(); 
		m_solutionFileName = fileDlg.GetFileTitle();
		
		// Setup serialization structures
		CFile file( m_solutionFilePath, CFile::OpenFlags::modeRead );
		CArchive ca(&file, CArchive::Mode::load);

		// Read the solution file
		int numSimulators;
		ca >> numSimulators;

		for( int i = 0; i < numSimulators; ++i )
		{
			ManagedPdcSimulator* sim = ManagedPdcSimulator::CreateByByArchive(ca);
			m_pdcSimulatorManager->EnlistSimulator(sim);
		}
			 			 
		// Cleanup
		ca.Flush();
		ca.Close();
		file.Close();

		// rebuild the project tree
		((CMainFrame*)GetMainWnd())->GetProjectViewRef()->ForceRebuild();
	}

}

void StrongridSIM_App::OnCloseSolution()
{	
	// clear the project tree
	((CMainFrame*)GetMainWnd())->GetProjectViewRef()->ClearProjectTree();

	// Close all documents
	std::vector<CView*> killViews;
	for( POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition(); pos != NULL; )
	{
		CDocTemplate* pTempl = AfxGetApp()->GetNextDocTemplate( pos );
		pTempl->CloseAllDocuments(true);
	}
	// Remove all existing simulators
	std::vector<PdcSimulatorID> simIds = m_pdcSimulatorManager->EnumeratePdcSimulators();
	for( std::vector<PdcSimulatorID>::iterator iter = simIds.begin(); iter != simIds.end(); ++iter )
		m_pdcSimulatorManager->DestroyPdcSimulator(*iter);

	
	

}

void StrongridSIM_App::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_PROJECT_VIEW_POPUP);
}

void StrongridSIM_App::LoadCustomState()
{
}

void StrongridSIM_App::SaveCustomState()
{
}

// StrongridSIM_App message handlers



