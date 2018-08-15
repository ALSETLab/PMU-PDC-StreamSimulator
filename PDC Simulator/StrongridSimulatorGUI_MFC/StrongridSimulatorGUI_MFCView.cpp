
// StrongridSimulatorGUI_MFCView.cpp : implementation of the StrongridSIM_View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "StrongridSimulatorGUI_MFC.h"
#endif

#include "StrongridSimulatorGUI_MFCDoc.h"
#include "StrongridSimulatorGUI_MFCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// StrongridSIM_View

IMPLEMENT_DYNCREATE(StrongridSIM_View, CView)

BEGIN_MESSAGE_MAP(StrongridSIM_View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &StrongridSIM_View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// StrongridSIM_View construction/destruction

StrongridSIM_View::StrongridSIM_View()
{
	// TODO: add construction code here

}

StrongridSIM_View::~StrongridSIM_View()
{
}

BOOL StrongridSIM_View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// StrongridSIM_View drawing

void StrongridSIM_View::OnDraw(CDC* /*pDC*/)
{
	StrongridSIM_Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// StrongridSIM_View printing


void StrongridSIM_View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL StrongridSIM_View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void StrongridSIM_View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void StrongridSIM_View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void StrongridSIM_View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void StrongridSIM_View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// StrongridSIM_View diagnostics

#ifdef _DEBUG
void StrongridSIM_View::AssertValid() const
{
	CView::AssertValid();
}

void StrongridSIM_View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

StrongridSIM_Doc* StrongridSIM_View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(StrongridSIM_Doc)));
	return (StrongridSIM_Doc*)m_pDocument;
}
#endif //_DEBUG


// StrongridSIM_View message handlers
