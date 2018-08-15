#include <deque>
#include "../StrongridSimBase/Mutex.h"

#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList window

class SynchronizedStringQueue
{
public:
	void EnqueueString(CString str);
	CString DequeueString();

private:
	std::deque<CString> m_stringQueue;
	strongridsim::Mutex m_mutex;
};

class COutputList : public CListBox
{
// Construction
public:
	COutputList();

// Implementation
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// Construction
public:
	COutputWnd();
	void PushOutputMessage(CString out);

	void UpdateFonts();

// Attributes
protected:
	CMFCTabCtrl	m_wndTabs;
	COutputList m_wndActivityLog;
	SynchronizedStringQueue m_stringQueue;

protected:
	void FillActivityLogWindow();
	void AdjustHorzScroll(CListBox& wndListBox);

// Implementation
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnsStringQueueUpdate(WPARAM w, LPARAM l);

	DECLARE_MESSAGE_MAP()
};

