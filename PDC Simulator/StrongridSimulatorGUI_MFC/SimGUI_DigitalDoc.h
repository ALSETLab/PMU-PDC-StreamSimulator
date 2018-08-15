#pragma once

// SimGUI_DigitalDoc document

class SimGUI_DigitalDoc : public CDocument
{
	DECLARE_DYNCREATE(SimGUI_DigitalDoc)

public:
	SimGUI_DigitalDoc();
	virtual ~SimGUI_DigitalDoc();
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

	CDigitalUnitBit* GetDigitalUnitBitRef();
	void SetDigitalName(CString phname);
	CString GetDigitalName();

protected:
	int m_pdcId;
	int m_pmuIdx;
	int m_digitalIdx;
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
