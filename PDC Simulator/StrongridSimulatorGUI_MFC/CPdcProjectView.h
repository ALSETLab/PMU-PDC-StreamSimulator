
#pragma once

#include "ViewTree.h"
#include "PdcSimulatorManager.h"

using namespace strongridgui;

class CClassToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};


class CPdcProjectView : public CDockablePane
{
public:
	CPdcProjectView();
	virtual ~CPdcProjectView();

	void AdjustLayout();
	void OnChangeVisualStyle();
	void ForceRebuild();
	void ClearProjectTree();
	void RebuildPhasors( PdcSimulatorID pdcid, int pmuIdx );
	void RebuildAnalog( PdcSimulatorID pdcid, int pmuIdx );
	void RebuildDigital( PdcSimulatorID pdcid, int pmuIdx );
	void UpdatePdcName( PdcSimulatorID pdcId);
	void UpdatePmuName( PdcSimulatorID pdcId, int pmuIdx );
	void UpdatePhasorName( PdcSimulatorID pdcId, int pmuIdx, int phasorIndex );
	void UpdateAnalogName( PdcSimulatorID pdcId, int pmuIdx, int analogIndex );
	void UpdateDigitalName( PdcSimulatorID pdcId, int pmuIdx, int digitalIndex );

protected:
	//CClassToolBar m_wndToolBar;
	CTreeCtrl m_wndClassView;
	CImageList m_ClassViewImages;
	UINT m_nCurrSort;

	void FillClassView();

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClassAddMemberFunction();
	afx_msg void OnClassAddMemberVariable();
	afx_msg void OnClassDefinition();
	afx_msg void OnClassProperties();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);
	afx_msg LRESULT OnForcedRebuild(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClickNotify(NMHDR * pNotifyStruct, LRESULT * result );

	DECLARE_MESSAGE_MAP()
};

