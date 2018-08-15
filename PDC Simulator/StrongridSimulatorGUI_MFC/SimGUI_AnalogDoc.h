#pragma once

// SimGUI_AnalogDoc document

class SimGUI_AnalogDoc : public CDocument
{
	DECLARE_DYNCREATE(SimGUI_AnalogDoc)

public:
	SimGUI_AnalogDoc();
	virtual ~SimGUI_AnalogDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	NewDocState GetDocHandle() const;
	ManagedPdcSimulator* GetSimulator();
	BridgedPmuDataFeed* GetPmuDataFeed();

	CAnalogUnit* GetAnalogUnitRef();
	void SetAnalogName(CString phname);
	CString GetAnalogName();

protected:
	int m_pdcId;
	int m_pmuIdx;
	int m_analogIdx;
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
