
// StrongridSimulatorGUI_MFC.h : main header file for the StrongridSimulatorGUI_MFC application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include <list>
#include "SimGUI_PdcDoc.h"
#include "resource.h"       // main symbols
#include "DocState.h"
#include "PdcSimulatorManager.h"



// StrongridSIM_App:
// See StrongridSimulatorGUI_MFC.cpp for the implementation of this class
//

class StrongridSIM_App : public CWinAppEx
{
	NewDocState m_prevDocState;

public:
	StrongridSIM_App();
		
	PdcSimulatorManager* GetPdcSimManager();
	void OpenDocumentViewByPdcIndex(int pdcIdx);
	void OpenDocumentViewByPmuIndex(int pdcIdx, int pmuIdx);
	void OpenDocumentViewByPhasorIndex(int pdcIdx, int pmuIdx, int phIdx);
	void OpenDocumentViewByAnalogIndex(int pdcIdx, int pmuIdx, int angIdx);
	void OpenDocumentViewByDigitalIndex(int pdcIdx, int pmuIdx, int digIdx);
	NewDocState GetNewDocState(); // Retrieves result of previous "OpenDocumentViewBy*" call

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	void OnCreateNewSolution();
	void OnCreateNewPdcWindow();
	void OnSimulatorSolutionSave();
	void OnSimulatorSolutionSaveAs();
	void SerializeSolutionToArchive(CArchive& ca);
	void OnSimulatorSolutionOpen();
	void OnCloseSolution();

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	PdcSimulatorManager* m_pdcSimulatorManager;
	CString m_solutionFilePath;
	CString m_solutionFileName; 	
};

extern StrongridSIM_App theApp;
