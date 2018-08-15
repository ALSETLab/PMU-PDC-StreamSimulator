
// StrongridSimulatorGUI_MFCView.h : interface of the StrongridSIM_View class
//

#pragma once


class StrongridSIM_View : public CView
{
protected: // create from serialization only
	StrongridSIM_View();
	DECLARE_DYNCREATE(StrongridSIM_View)

// Attributes
public:
	StrongridSIM_Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~StrongridSIM_View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in StrongridSimulatorGUI_MFCView.cpp
inline StrongridSIM_Doc* StrongridSIM_View::GetDocument() const
   { return reinterpret_cast<StrongridSIM_Doc*>(m_pDocument); }
#endif

