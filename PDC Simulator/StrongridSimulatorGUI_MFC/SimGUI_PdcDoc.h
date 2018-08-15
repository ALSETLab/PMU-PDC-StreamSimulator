#pragma once
#include "PdcSimulatorManager.h"

using namespace strongridgui;


class SimGUI_PdcDoc : public CDocument
{
	DECLARE_DYNCREATE(SimGUI_PdcDoc)

public:
	SimGUI_PdcDoc();
	virtual ~SimGUI_PdcDoc();
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
	PdcSimulatorID m_pdcSimId;

	void StartSimulator();
	void StopSimulator();
	void SendConfigurationCfg2AllClients();
	void StartRTDAllClients();
	void StopRTDAllClients();
	bool GetSimulatorIsActive();
	ManagedPdcSimulator* GetSimRef();


protected:
	virtual BOOL OnNewDocument();
	
	DECLARE_MESSAGE_MAP()
};
