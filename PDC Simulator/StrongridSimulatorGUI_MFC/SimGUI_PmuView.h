#pragma once
#include "afxwin.h"
#include "afxvslistbox.h"
#include "afxbutton.h"
#include "afxfontcombobox.h"


class SimGUI_PhasorList : public CVSListBox
{
public:
	void Initialize(PdcSimulatorID pdcId, int pmuIdx, CPmuConfig* pmucfg);

	virtual int AddItem(const CString& strIext, DWORD_PTR dwData = 0, int iIndex = -1);
	virtual BOOL RemoveItem(int iIndex);	
	virtual void SetItemText(int iIndex, const CString& strText);	
	virtual BOOL EditItem(int iIndex);
	virtual void OnEndEditLabel(LPCTSTR lpszLabel);	

private:
	PdcSimulatorID m_pdcId;
	int m_pmuIdx;
	CPmuConfig* m_pmuCfg;
};

class SimGUI_AnalogList : public CVSListBox
{
public:
	void Initialize(PdcSimulatorID pdcId, int pmuIdx, CPmuConfig* pmucfg);

	virtual int AddItem(const CString& strIext, DWORD_PTR dwData = 0, int iIndex = -1);
	virtual BOOL RemoveItem(int iIndex);	
	virtual void SetItemText(int iIndex, const CString& strText);	
	virtual BOOL EditItem(int iIndex);
	virtual void OnEndEditLabel(LPCTSTR lpszLabel);	

private:
	PdcSimulatorID m_pdcId;
	int m_pmuIdx;
	CPmuConfig* m_pmuCfg;
};

class SimGUI_DigitalList : public CVSListBox
{
public:
	void Initialize(PdcSimulatorID pdcId, int pmuIdx, CPmuConfig* pmucfg);

	virtual int AddItem(const CString& strIext, DWORD_PTR dwData = 0, int iIndex = -1);
	virtual BOOL RemoveItem(int iIndex);	
	virtual void SetItemText(int iIndex, const CString& strText);	
	virtual BOOL EditItem(int iIndex);
	virtual void OnEndEditLabel(LPCTSTR lpszLabel);	

private:
	PdcSimulatorID m_pdcId;
	int m_pmuIdx;
	CPmuConfig* m_pmuCfg;
};


// SimGUI_PmuView form view

class SimGUI_PmuView : public CFormView
{
public:	
	static std::map<NewDocState, SimGUI_PmuView*> ViewList;

	void OnSimulatorEnabled();
	void OnSimulatorDisabled();
	void SetGuiState();

	DECLARE_DYNCREATE(SimGUI_PmuView)

protected:
	SimGUI_PmuView();           // protected constructor used by dynamic creation
	virtual ~SimGUI_PmuView();
	void SetDisplayHeader();

public:
	enum { IDD = IDD_SIMGUI_PMUVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:	
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	ManagedPdcSimulator* GetSimulatorRef();
	CPmuConfig* GetPmuConfigRef();

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_stationName;
	CEdit m_IdCode;
	CEdit m_cfgCngCnt;
	SimGUI_PhasorList m_phasorList;
	SimGUI_AnalogList m_analogList;
	SimGUI_DigitalList m_digitalList;
	int m_phasorFormatRect;
	int m_phasorTypeInt;
	int m_analogTypeInt;
	int m_freqDataTypeInt;
	CMFCButton m_commit;
	afx_msg void OnBnClickedCommit();
	afx_msg void OnBnClickedCommitRealtimeDataParams();
	CButton m_rtdChkClockSynced;
	CButton m_rtdChkDataSort;
	CButton m_rtdChkTriggerDetected;
	CButton m_rtdChkCfgCngPending;
	CButton m_rtdChkDataModified;
	CComboBox m_rtdCbDataErr;
	CComboBox m_rtdCbTriggerReason;
	CComboBox m_rtdCbTimeQuality;
	CComboBox m_rtdCbTimeUnlocked;
	CComboBox m_nomFreq;
	CEdit m_frequency;
	CEdit m_deltaFrequency;
	afx_msg void OnCfgChange();
};


