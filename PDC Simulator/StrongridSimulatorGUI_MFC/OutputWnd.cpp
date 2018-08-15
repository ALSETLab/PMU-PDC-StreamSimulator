
#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT OUTPUTWINDOW_INPUTQUEUE_UPDATE = (WM_USER + 1); //::RegisterWindowMessageA("OUTPUTWINDOW_INPUTQUEUE_UPDATE");

static int ACTIVITYLOG_MAX_ITEMS = 100;

void SynchronizedStringQueue::EnqueueString(CString str)
{
	MutexFragment mux(&m_mutex);
	m_stringQueue.push_back(str);
	mux.Finalize();
}

CString SynchronizedStringQueue::DequeueString()
{
	MutexFragment mux(&m_mutex);
	if( m_stringQueue.empty() ) return _T("--EMPTY--");
	CString tmp = *m_stringQueue.begin();
	m_stringQueue.pop_front();
	mux.Finalize();
	return tmp;
}

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}


void COutputWnd::PushOutputMessage(CString out)
{
	m_stringQueue.EnqueueString(out);
	PostMessage(OUTPUTWINDOW_INPUTQUEUE_UPDATE, 0, 0);
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(OUTPUTWINDOW_INPUTQUEUE_UPDATE, &COutputWnd::OnsStringQueueUpdate)
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndActivityLog.Create(dwStyle, rectDummy, &m_wndTabs, 2))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// Attach list windows to tab:
	bNameValid = strTabName.LoadString(IDS_ACTIVITYLOG);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndActivityLog, strTabName, (UINT)0);
	

	// Fill output tabs with some dummy text (nothing magic here)
	FillActivityLogWindow();

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

LRESULT COutputWnd::OnsStringQueueUpdate(WPARAM w, LPARAM l)
{
	// Drain the input queue
	while( true )
	{
		CString tmp = m_stringQueue.DequeueString();
		if( tmp == "--EMPTY--" ) break;
		m_wndActivityLog.AddString(tmp);
	}

	// make sure the list does not grow forever
	while( m_wndActivityLog.GetCount() > ACTIVITYLOG_MAX_ITEMS ) 
		m_wndActivityLog.DeleteString(0);

	// Scroll to bottom
	int nCount = m_wndActivityLog.GetCount();
	if (nCount > 0)
		m_wndActivityLog.SetCurSel(nCount - 1);

	return 1;
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillActivityLogWindow()
{
	m_wndActivityLog.AddString(_T("Strongrid PDC C37118 simulator ready."));	
}


void COutputWnd::UpdateFonts()
{
	m_wndActivityLog.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	MessageBox(_T("Copy output"));
}

void COutputList::OnEditClear()
{
	this->SetWindowTextW(_T(""));
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}
