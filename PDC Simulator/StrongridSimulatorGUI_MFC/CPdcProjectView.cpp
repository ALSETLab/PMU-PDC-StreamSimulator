
#include "stdafx.h"
#include "MainFrm.h"
#include "CPdcProjectView.h"
#include "Resource.h"
#include "StrongridSimulatorGUI_MFC.h"

int WM_FORCEREBUILD = RegisterWindowMessage (_T("FORCEREBUILD"));





//
//BOOL TestTreeView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	if( wParam == NM_CLICK )
//		return CViewTree::OnNotify(wParam,lParam,pResult);
//	if( wParam != 1576236 && lParam != 4061060 )
//		return CViewTree::OnNotify(wParam,lParam,pResult);
//	else
//		return CViewTree::OnNotify(wParam,lParam,pResult);
//}

class CClassViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CClassView;

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPdcProjectView::CPdcProjectView()
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CPdcProjectView::~CPdcProjectView()
{
}

BEGIN_MESSAGE_MAP(CPdcProjectView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
	ON_REGISTERED_MESSAGE(WM_FORCEREBUILD, OnForcedRebuild)
	//ON_NOTIFY(TVN_SELCHANGED,2,OnClickNotify)	
	ON_NOTIFY(TVN_SELCHANGED,2,OnClickNotify)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView message handlers

int CPdcProjectView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	//const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}

	OnChangeVisualStyle();	

	// Fill in some static tree view data (dummy code, nothing magic here)
	//FillClassView();

	return 0;
}

void CPdcProjectView::ForceRebuild()
{
	m_wndClassView.DeleteAllItems();
	FillClassView();
}

void CPdcProjectView::ClearProjectTree()
{
	m_wndClassView.DeleteAllItems();
}

void CPdcProjectView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}



void CPdcProjectView::FillClassView()
{
	std::vector<PdcSimulatorID> simVec = theApp.GetPdcSimManager()->EnumeratePdcSimulators();

	for( int iPdcIndex = 0; iPdcIndex < simVec.size(); ++iPdcIndex )
	{
		ManagedPdcSimulator* sim = theApp.GetPdcSimManager()->GetPdcSimulator(simVec[iPdcIndex]);

		CString tmpPdcName;
		tmpPdcName.Format(_T("[%d] %s"), sim->getConfigurationRef()->SG_PdcId, sim->SimulatorProjectName() );

		UINT stateAndMask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
		HTREEITEM hRoot = m_wndClassView.InsertItem(stateAndMask, tmpPdcName, 0, 0, stateAndMask, stateAndMask, /* id */ simVec[iPdcIndex], 0, 0 );
		m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

		// Insert PMU'sl under the PDC
		for( int i = 0; i < sim->getConfigurationRef()->SG_PmuConfigArray.items.size(); ++i )
		{
			CPmuConfig* pmuCfg = sim->getConfigurationRef()->SG_PmuConfigArray.items[i];

			// Insert the PMU
			CString tmp;
			tmp.Format(_T("[%d] %s"), pmuCfg->SG_PmuIdCode, pmuCfg->SG_PmuStationName );
			HTREEITEM hClass = m_wndClassView.InsertItem(stateAndMask, tmp, 1, 1, stateAndMask, stateAndMask, /* id */ i, hRoot, 0 );
			
			// Insert phasors
			HTREEITEM hPhasors = m_wndClassView.InsertItem(stateAndMask, _T("Phasors"), 3, 3, stateAndMask, stateAndMask, /* id=1000 for ph */ 1000, hClass, 0 );
			for( int iPh = 0; iPh < pmuCfg->SG_PhasorNames.items.size(); ++iPh )
				m_wndClassView.InsertItem(stateAndMask,pmuCfg->SG_PhasorNames.items[iPh], 4, 4, stateAndMask, stateAndMask, /* id */ iPh, hPhasors, 0 );
			
			// Insert analog channels
			HTREEITEM hAnalog = m_wndClassView.InsertItem(stateAndMask, _T("Analog"), 3, 3, stateAndMask, stateAndMask, /* id=1001 for ang */ 1001, hClass, 0 );
			for( int iAng = 0; iAng < pmuCfg->SG_AnalogNames.items.size(); ++iAng )
				m_wndClassView.InsertItem(stateAndMask, pmuCfg->SG_AnalogNames.items[iAng], 4, 4, stateAndMask, stateAndMask, /* id */ iAng, hAnalog, 0 );

			// Insert digital channels
			HTREEITEM hDigital = m_wndClassView.InsertItem(stateAndMask, _T("Digital"), 3, 3, stateAndMask, stateAndMask, /* id=1001 for digital */ 1002, hClass, 0 );
			for( int iDig = 0; iDig < pmuCfg->SG_DigitalNames.items.size(); ++iDig )
				m_wndClassView.InsertItem(stateAndMask, pmuCfg->SG_DigitalNames.items[iDig], 4, 4, stateAndMask, stateAndMask, /* id */ iDig, hDigital, 0 );
		}
	}	
}

HTREEITEM GetPdcTreeItem( CTreeCtrl& tree, PdcSimulatorID pdcid )
{
	HTREEITEM hPdcTreeNode = tree.GetFirstVisibleItem();
	while( hPdcTreeNode != NULL )
	{
		int tmp = tree.GetItemData(hPdcTreeNode);
		if( tmp == pdcid ) break;
		hPdcTreeNode = tree.GetNextItem(hPdcTreeNode,TVGN_NEXT);
	}
	return hPdcTreeNode;
}

HTREEITEM GetPmuTreeItem( CTreeCtrl& tree, HTREEITEM hPdcTreeNode, int pmuIdx )
{
	if( hPdcTreeNode == NULL ) return NULL;
	
	HTREEITEM hPmuTreeNode = tree.GetChildItem(hPdcTreeNode);
	for( int i = 0; i < pmuIdx; ++i ) hPmuTreeNode = tree.GetNextItem(hPmuTreeNode, TVGN_NEXT );
	return hPmuTreeNode;
}

// CHNID: 0) phasors 1) analog 2) digital
HTREEITEM GetChannelTreeItem( CTreeCtrl& tree, HTREEITEM hPmuNode, int chnId, int channelIdx )
{
	if( hPmuNode == NULL ) return NULL;
	
	// Move to "phasors" / "analog" / "digital"
	HTREEITEM hChannelTypeItem = tree.GetChildItem(hPmuNode);
	for( int i = 0; i < chnId; ++i ) hChannelTypeItem = tree.GetNextItem(hChannelTypeItem, TVGN_NEXT );

	// Move to the correct channel by index
	HTREEITEM hChannel = tree.GetChildItem(hChannelTypeItem);
	for( int i = 0; i < channelIdx; ++i ) hChannel = tree.GetNextItem(hChannel, TVGN_NEXT );


	return hChannel;
}


void DeleteAllChildNodes( CTreeCtrl& tree, HTREEITEM hParent )
{
	if( hParent == NULL ) return;
	
	HTREEITEM hDeleteChild = tree.GetChildItem(hParent);
	while( hDeleteChild != NULL ) {
		tree.DeleteItem(hDeleteChild);
		hDeleteChild = tree.GetChildItem(hParent);
	}
}

void CPdcProjectView::RebuildPhasors( PdcSimulatorID pdcid, int pmuIdx)
{
	// Get quickref to PMU configuration
	CPmuConfig* refPmuCfg = theApp.GetPdcSimManager()->GetPdcSimulator(pdcid)->getConfigurationRef()->SG_PmuConfigArray.items[pmuIdx];

	// Find the correct PCD+PMU node
	HTREEITEM hPdcTreeNode = GetPdcTreeItem(m_wndClassView, pdcid);
	HTREEITEM hPmuTreeNode = GetPmuTreeItem(m_wndClassView, hPdcTreeNode, pmuIdx );

	// Find the PHASOR node (the first child of PMU node)
	HTREEITEM hPhasorNode = m_wndClassView.GetChildItem(hPmuTreeNode);
			
	// Delete all phasors under "hPhasorNode"
	DeleteAllChildNodes(m_wndClassView, hPhasorNode);

	// Insert phasors	
	UINT stateAndMask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;	
	for( int iPh = 0; iPh < refPmuCfg->SG_PhasorNames.items.size(); ++iPh )
		m_wndClassView.InsertItem(stateAndMask,refPmuCfg->SG_PhasorNames.items[iPh], 4, 4, stateAndMask, stateAndMask, /* id */ iPh, hPhasorNode, 0 );
}

void CPdcProjectView::RebuildAnalog( PdcSimulatorID pdcid, int pmuIdx)
{
	// Get quickref to PMU configuration
	CPmuConfig* refPmuCfg = theApp.GetPdcSimManager()->GetPdcSimulator(pdcid)->getConfigurationRef()->SG_PmuConfigArray.items[pmuIdx];

	// Find the correct PCD+PMU node
	HTREEITEM hPdcTreeNode = GetPdcTreeItem(m_wndClassView, pdcid);
	HTREEITEM hPmuTreeNode = GetPmuTreeItem(m_wndClassView, hPdcTreeNode, pmuIdx );

	// Find the ANALOG node (the first child of PMU node)
	HTREEITEM hAnalogNode = m_wndClassView.GetChildItem(hPmuTreeNode);
	for( int i = 0; i < 1; ++i ) hAnalogNode = m_wndClassView.GetNextItem(hAnalogNode, TVGN_NEXT);
			
	// Delete all phasors under "hPhasorNode"
	DeleteAllChildNodes(m_wndClassView, hAnalogNode);

	// Insert phasors	
	UINT stateAndMask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;	
	for( int iAng = 0; iAng < refPmuCfg->SG_AnalogNames.items.size(); ++iAng )
		m_wndClassView.InsertItem(stateAndMask,refPmuCfg->SG_AnalogNames.items[iAng], 4, 4, stateAndMask, stateAndMask, /* id */ iAng, hAnalogNode, 0 );
}

void CPdcProjectView::RebuildDigital( PdcSimulatorID pdcid, int pmuIdx)
{
	// Get quickref to PMU configuration
	CPmuConfig* refPmuCfg = theApp.GetPdcSimManager()->GetPdcSimulator(pdcid)->getConfigurationRef()->SG_PmuConfigArray.items[pmuIdx];

	// Find the correct PCD+PMU node
	HTREEITEM hPdcTreeNode = GetPdcTreeItem(m_wndClassView, pdcid);
	HTREEITEM hPmuTreeNode = GetPmuTreeItem(m_wndClassView, hPdcTreeNode, pmuIdx );

	// Find the Digital node (the third child of PMU node)
	HTREEITEM hDigitalNode = m_wndClassView.GetChildItem(hPmuTreeNode);
	for( int i = 0; i < 2; ++i ) hDigitalNode = m_wndClassView.GetNextItem(hDigitalNode, TVGN_NEXT);
			
	// Delete all phasors under "hPhasorNode"
	DeleteAllChildNodes(m_wndClassView, hDigitalNode);

	// Insert phasors	
	UINT stateAndMask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;	
	for( int iAng = 0; iAng < refPmuCfg->SG_DigitalNames.items.size(); ++iAng )
		m_wndClassView.InsertItem(stateAndMask,refPmuCfg->SG_DigitalNames.items[iAng], 4, 4, stateAndMask, stateAndMask, /* id */ iAng, hDigitalNode, 0 );
}

void CPdcProjectView::UpdatePdcName( PdcSimulatorID pdcId)
{	
	// Find the correct PCD node and simulator ref
	HTREEITEM hPdcTreeNode = GetPdcTreeItem(m_wndClassView, pdcId);
	ManagedPdcSimulator* pdcSim = theApp.GetPdcSimManager()->GetPdcSimulator(pdcId);

	// Update tree item text
	CString tmpPdcName;
	tmpPdcName.Format(_T("[%d] %s"), pdcSim->getConfigurationRef()->SG_PdcId, pdcSim->SimulatorProjectName() );
	m_wndClassView.SetItemText(hPdcTreeNode, tmpPdcName);
}

void CPdcProjectView::UpdatePmuName( PdcSimulatorID pdcId, int pmuIdx )
{
	// Find the correct PCD+PMU node and simulator+pmu ref
	HTREEITEM hPdcTreeNode = GetPdcTreeItem(m_wndClassView, pdcId);
	HTREEITEM hPmuTreeNode = GetPmuTreeItem(m_wndClassView, hPdcTreeNode, pmuIdx );
	ManagedPdcSimulator* pdcSim = theApp.GetPdcSimManager()->GetPdcSimulator(pdcId);
	CPmuConfig* pmuCfg = pdcSim->getConfigurationRef()->SG_PmuConfigArray.items[pmuIdx];

	// Update tree item text
	CString tmp;
	tmp.Format(_T("[%d] %s"), pmuCfg->SG_PmuIdCode, pmuCfg->SG_PmuStationName );
	m_wndClassView.SetItemText(hPmuTreeNode, tmp);
}


void CPdcProjectView::UpdatePhasorName( PdcSimulatorID pdcId, int pmuIdx, int phasorIndex )
{	
	// Find the correct PCD+PMU node and simulator+pmu ref
	HTREEITEM hPhasorTreeNode = GetChannelTreeItem(m_wndClassView, GetPmuTreeItem(m_wndClassView, GetPdcTreeItem(m_wndClassView, pdcId), pmuIdx ), 0, phasorIndex );
	CPmuConfig* pmuCfg = theApp.GetPdcSimManager()->GetPdcSimulator(pdcId)->getConfigurationRef()->SG_PmuConfigArray.items[pmuIdx];

	// Update tree item text
	m_wndClassView.SetItemText(hPhasorTreeNode, pmuCfg->SG_PhasorNames.items[phasorIndex]);
}

void CPdcProjectView::UpdateAnalogName( PdcSimulatorID pdcId, int pmuIdx, int analogIndex )
{
	// Find the correct PCD+PMU node and simulator+pmu ref
	HTREEITEM hAnalogTreeNode = GetChannelTreeItem(m_wndClassView, GetPmuTreeItem(m_wndClassView, GetPdcTreeItem(m_wndClassView, pdcId), pmuIdx ), 1, analogIndex );
	CPmuConfig* pmuCfg = theApp.GetPdcSimManager()->GetPdcSimulator(pdcId)->getConfigurationRef()->SG_PmuConfigArray.items[pmuIdx];

	// Update tree item text
	m_wndClassView.SetItemText(hAnalogTreeNode, pmuCfg->SG_AnalogNames.items[analogIndex]);
}

void CPdcProjectView::UpdateDigitalName( PdcSimulatorID pdcId, int pmuIdx, int digitalIndex )
{
	// Find the correct PCD+PMU node and simulator+pmu ref
	HTREEITEM hDigitalTreeNode = GetChannelTreeItem(m_wndClassView, GetPmuTreeItem(m_wndClassView, GetPdcTreeItem(m_wndClassView, pdcId), pmuIdx ), 2, digitalIndex );
	CPmuConfig* pmuCfg = theApp.GetPdcSimManager()->GetPdcSimulator(pdcId)->getConfigurationRef()->SG_PmuConfigArray.items[pmuIdx];

	// Update tree item text
	m_wndClassView.SetItemText(hDigitalTreeNode, pmuCfg->SG_DigitalNames.items[digitalIndex]);
}


void CPdcProjectView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void CPdcProjectView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = 0; //m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndClassView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CPdcProjectView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CPdcProjectView::OnSort(UINT id)
{
}

void CPdcProjectView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CPdcProjectView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("Add member function..."));
}

void CPdcProjectView::OnClassAddMemberVariable()
{
	// TODO: Add your command handler code here
}

void CPdcProjectView::OnClassDefinition()
{
	// TODO: Add your command handler code here
}

void CPdcProjectView::OnClassProperties()
{
	// TODO: Add your command handler code here
}

void CPdcProjectView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPdcProjectView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void CPdcProjectView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);
}

LRESULT CPdcProjectView::OnForcedRebuild(WPARAM wParam, LPARAM lParam)
{
	return 0;
}



afx_msg void CPdcProjectView::OnClickNotify(NMHDR * pNotifyStruct, LRESULT * result )
{
	CMainFrame* mainFrame = (CMainFrame*)GetParentOwner();
	HTREEITEM hti = m_wndClassView.GetSelectedItem();
	if( hti == NULL ) return;
	//CString blah = m_wndClassView.GetItemText(hti);
	
	// Count the levels
	int levels = 0;
	HTREEITEM parent = hti;
	while( parent != NULL ) {
		parent = m_wndClassView.GetParentItem(parent);
		++levels;
	}

	if( levels == 1 ) // Topmost = PDC
	{
		DWORD_PTR pdcIdentifier = m_wndClassView.GetItemData(hti);
		theApp.OpenDocumentViewByPdcIndex(pdcIdentifier);
	}
	else if( levels == 2 ) // PMU
	{
		// Get PMU index + PDC index
		DWORD_PTR pmuIdx = m_wndClassView.GetItemData(hti);
		DWORD_PTR pdcIdentifier = m_wndClassView.GetItemData(m_wndClassView.GetParentItem(hti));

		// Open a PMU window
		theApp.OpenDocumentViewByPmuIndex(pdcIdentifier, pmuIdx);
	}
	else if( levels == 4 ) // PHASOR/ANALOG/DIGITAL 
	{
		// Unwind the tree and get ref to each successive parent node 
		HTREEITEM hChn = hti;
		HTREEITEM hChnHeader = m_wndClassView.GetParentItem(hChn);
		HTREEITEM hPmu = m_wndClassView.GetParentItem(hChnHeader);
		HTREEITEM hPdc = m_wndClassView.GetParentItem(hPmu);

		// Get PMU index + PDC index
		DWORD_PTR channelIdx = m_wndClassView.GetItemData(hChn);
		DWORD_PTR chnHdrId = m_wndClassView.GetItemData(hChnHeader);
		DWORD_PTR pmuIdx = m_wndClassView.GetItemData(hPmu);
		DWORD_PTR pdcIdentifier = m_wndClassView.GetItemData(hPdc);

		// Figure out which type of channel it is
		if( chnHdrId == 1000 )
			theApp.OpenDocumentViewByPhasorIndex(pdcIdentifier, pmuIdx, channelIdx);
		else if( chnHdrId == 1001 )
			theApp.OpenDocumentViewByAnalogIndex(pdcIdentifier, pmuIdx, channelIdx );
		else if( chnHdrId == 1002 )
			theApp.OpenDocumentViewByDigitalIndex(pdcIdentifier, pmuIdx, channelIdx );
	}
}


