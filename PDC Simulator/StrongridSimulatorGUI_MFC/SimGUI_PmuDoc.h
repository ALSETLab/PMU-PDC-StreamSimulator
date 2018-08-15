#pragma once

// SimGUI_PmuDoc document

class SimGUI_PmuDoc : public CDocument
{
	DECLARE_DYNCREATE(SimGUI_PmuDoc)

public:
	SimGUI_PmuDoc();
	virtual ~SimGUI_PmuDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	
public:	
	int m_simId;
	int m_pmuIdx;
	NewDocState m_pmuDocHandle;
	ManagedPdcSimulator* GetSimRef();

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
