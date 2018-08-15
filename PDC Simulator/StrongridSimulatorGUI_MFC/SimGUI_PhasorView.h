#pragma once
#include "afxwin.h"
#include "afxbutton.h"



// SimGUI_PhasorView form view

class SimGUI_PhasorView : public CFormView
{
public:
	static std::map<NewDocState, SimGUI_PhasorView*> ViewList;
	
	void HandleStateChange();

private:
	DECLARE_DYNCREATE(SimGUI_PhasorView)

protected:
	SimGUI_PhasorView();           // protected constructor used by dynamic creation
	virtual ~SimGUI_PhasorView();
	void SetDisplayHeader();

public:
	enum { IDD = IDD_PHASOR };
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
	// GUI elements
	CComboBox m_dbPhasorType;
	CEdit m_txtScalar;
	int m_inputGroup;
	CEdit m_txtInputFile;
	CButton m_btnFileLoad;
	CEdit m_txtConstInt; // *Real
	CEdit m_txtConstImag;
	CMFCButton m_btnRtdCommit;

	// Event handlers
	afx_msg void OnBnClickedLoadInputFile();
	afx_msg void OnBnClickedPhcfgcommit();
	afx_msg void OnBnClickedPhrdtcommit();
	afx_msg void OnPhasorInputChanged();
	CEdit m_phasorName;
	afx_msg void OnCfgChange();
	afx_msg void OnRtdChange();
	CMFCButton m_btnCfgCommit;
};


