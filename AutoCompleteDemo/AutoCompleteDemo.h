
// AutoCompleteDemo.h : main header file for the AutoCompleteDemo application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CAutoCompleteDemoApp:
// See AutoCompleteDemo.cpp for the implementation of this class
//

class CAutoCompleteDemoApp : public CWinAppEx
{
public:
	CAutoCompleteDemoApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	BOOL  m_bHiColorIcons;
#ifdef _ENABLE_SCINTILLA_BUILD
	HINSTANCE m_hSciDLL;
#endif // _ENABLE_SCINTILLA_BUILD
	bool	m_bDuringInit;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnFileNew();
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAutoCompleteDemoApp theApp;
