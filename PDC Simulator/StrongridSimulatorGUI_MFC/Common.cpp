
#include "stdafx.h"
#include "Common.h"

using namespace strongridgui;

SimException::SimException( CString message ) 
{
	m_errormessage = message;
}

BOOL SimException::GetErrorMessage(_Out_writes_z_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError,
	_Out_opt_ PUINT pnHelpContext) const
{
	return true;
}


CString strongridgui::NumberToCStringASCII_i(int a)
{
	CString tmp;
	tmp.Format(_T("%d"), a);
	return tmp;
}

CString strongridgui::NumberToCStringASCII_f(float a)
{
	CString tmp;
	tmp.Format(_T("%.2f"), a);
	return tmp;
}


int strongridgui::GetEditTextAsInteger(CEdit& ce, int defaultValue)
{
	CString str;
	ce.GetWindowTextW(str);
	return _ttoi(str);
}

float strongridgui::GetEditTextAsFloat(CEdit& ce, float defaultValue)
{
	CString str;
	ce.GetWindowTextW(str);
	return _tstof(str);
}

CString strongridgui::SimpleGetWindowText(CEdit& ce)
{
	CString tmp;
	ce.GetWindowTextW(tmp);
	return tmp;
}

