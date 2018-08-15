#pragma once

// SimGUI_PhasorDoc document

class SimGUI_PhasorDoc : public CDocument
{
	DECLARE_DYNCREATE(SimGUI_PhasorDoc)

public:
	SimGUI_PhasorDoc();
	virtual ~SimGUI_PhasorDoc();
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

	CPhasorUnit* GetPhasorUnitRef();
	void SetPhasorName(CString phname);
	CString GetPhasorName();

protected:
	int m_pdcId;
	int m_pmuIdx;
	int m_phasorIdx;
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
