#pragma once
#include "afxwin.h"
#include "afxvslistbox.h"
#include "afxbutton.h"


class SimGUI_PdcPmuList : public CVSListBox
{
public:
	void Initialize(SimGUI_PdcDoc* pdcDoc);

	virtual int AddItem(const CString& strIext, DWORD_PTR dwData = 0, int iIndex = -1);
	virtual BOOL RemoveItem(int iIndex);	
	virtual void SetItemText(int iIndex, const CString& strText);	
	virtual BOOL EditItem(int iIndex);
	virtual void OnEndEditLabel(LPCTSTR lpszLabel);	

private:
	SimGUI_PdcDoc* m_pdcDoc;
};

// SimGUI_PdcView form view
class SimGUI_PdcView : public CFormView
{	
public:
	static std::map<PdcSimulatorID, SimGUI_PdcView*> ViewList;

	DECLARE_DYNCREATE(SimGUI_PdcView)

protected:
	SimGUI_PdcView();           // protected constructor used by dynamic creation
	virtual ~SimGUI_PdcView();

public:
	enum { IDD = IDD_SIMGUI_PDCVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnInitialUpdate(); // called first time after construct
	void CommitDataFromGUI();
	void SetDisplayHeader( CString str );

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedToggleSim();
	afx_msg void OnBnClickedBtnStartrtdall();
	afx_msg void OnBnClickedBtnStoprtdAll();
	afx_msg void OnBnClickedBtnSendcfg2All();
	afx_msg void OnLvnItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
private:
	CEdit m_TimeBase;
public:
	CEdit m_dataRate;
	CEdit m_pdcId;
	CEdit m_listenPortTcp;
	SimGUI_PdcPmuList m_pmuList;
	CEdit m_txtPdcProjectName;
	afx_msg void OnEnChangeEditTimebase();
	afx_msg void OnBnClickedMfcbutton1();
	CMFCButton m_btnCfgCommit;
	afx_msg void OnChangeConfig();
};


