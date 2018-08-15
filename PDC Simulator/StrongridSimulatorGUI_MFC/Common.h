#pragma once

#include "stdafx.h"

namespace strongridgui
{	
	class SimException : public CException
	{
	public:
		SimException( CString message ) ;
		virtual BOOL GetErrorMessage(_Out_writes_z_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError,
			_Out_opt_ PUINT pnHelpContext = NULL) const;

	private:
		CString m_errormessage;
	};

	CString NumberToCStringASCII_i(int a);
	CString NumberToCStringASCII_f(float a);

	int GetEditTextAsInteger(CEdit& ce, int defaultValue);
	float GetEditTextAsFloat(CEdit& ce, float defaultValue);
	CString SimpleGetWindowText(CEdit& ce);
}