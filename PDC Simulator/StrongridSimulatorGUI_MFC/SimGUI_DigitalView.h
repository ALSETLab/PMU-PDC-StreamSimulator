#pragma once
#include "afxwin.h"
#include "afxbutton.h"



// SimGUI_DigitalView form view

class SimGUI_DigitalView : public CFormView
{
public:
	static std::map<NewDocState, SimGUI_DigitalView*> ViewList;
	
	void HandleStateChange();

private:
	DECLARE_DYNCREATE(SimGUI_DigitalView)

protected:
	SimGUI_DigitalView();           // protected constructor used by dynamic creation
	virtual ~SimGUI_DigitalView();
	void SetDisplayHeader();

public:
	enum { IDD = IDD_DIGITAL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_dbCfgNormalState;
	int m_rbRtdInputType;
	CEdit m_txtRtdFileInput;
	CButton m_btnRtdFind;
	CComboBox m_dbRtdValue;
	afx_msg void OnBnClickedCfgCommit();
	afx_msg void OnBnClickedRdtCommit();
	afx_msg void OnBnClickedRtdLoad();
	afx_msg void OnBnClickedRadioInputType();
	CMFCButton m_btnRtdCommit;
	CEdit m_txtCfgName;
	CMFCButton m_btnCfgCommit;
	afx_msg void OnCfgChange();
	afx_msg void OnRtdChange();
};


