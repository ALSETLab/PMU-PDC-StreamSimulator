#pragma once
#include "afxbutton.h"
#include "afxwin.h"



// SimGUI_AnalogView form view

class SimGUI_AnalogView : public CFormView
{
public:
	static std::map<NewDocState, SimGUI_AnalogView*> ViewList;
	
	void HandleStateChange();

private:
	DECLARE_DYNCREATE(SimGUI_AnalogView)

protected:
	SimGUI_AnalogView();           // protected constructor used by dynamic creation
	virtual ~SimGUI_AnalogView();
	void SetDisplayHeader();

public:
	enum { IDD = IDD_ANALOG };
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
	CMFCButton m_btnRtdCommit;
	CMFCButton m_btnCfgCommit;
	CComboBox m_cbCfgType;
	CEdit m_txtCfgScalar;
	CButton m_btnRtdFind;
	CEdit m_txtRtdInputFile;
	CEdit m_txtRtdValue;
	afx_msg void OnBnClickedCfgCommit();
	afx_msg void OnBnClickedRdtCommit();
	afx_msg void OnBnClickedRtdLoad();
	afx_msg void OnBnClickedRadioInputType();
	int m_rbRtdInputType;
	CEdit m_txtCfgAnalogName;
	afx_msg void OnCfgChange();
	afx_msg void OnRtdChange();
};


