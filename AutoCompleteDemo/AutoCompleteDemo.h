
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

	CString			m_strPath;
	CStringArray	m_saTestList;
	int				m_nMaxTextLen;
	CImageList		m_imgList;
	bool			m_bMatchCase;
	bool			m_bFuzzyMatch;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnFileNew();
	afx_msg void OnAppAbout();

	afx_msg void OnFuzzyMatch();
	afx_msg void OnUpdateFuzzyMatch(CCmdUI* pCmdUI);
	afx_msg void OnMatchCase();
	afx_msg void OnUpdateMatchCase(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

extern CAutoCompleteDemoApp theApp;
