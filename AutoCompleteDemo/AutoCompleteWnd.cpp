// AutoCompleteWnd.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteWnd.h"


// CAutoCompleteWnd

IMPLEMENT_DYNAMIC(CAutoCompleteWnd, CAutoCompleteWndBase)

CAutoCompleteWnd::CAutoCompleteWnd()
{

}

CAutoCompleteWnd::~CAutoCompleteWnd()
{
}


BEGIN_MESSAGE_MAP(CAutoCompleteWnd, CAutoCompleteWndBase)
END_MESSAGE_MAP()

// CAutoCompleteWnd message handlers

BOOL CAutoCompleteWnd::Create(CWnd* pOwner, POINT pos)
{
	DWORD dwStyle = WS_POPUP|WS_VISIBLE|WS_BORDER|WS_VSCROLL;
	dwStyle |= LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS;
	// the image list should be reused
	dwStyle |= LVS_SHAREIMAGELISTS;
	dwStyle |= LVS_NOCOLUMNHEADER;

	//dwStyle |= LVS_OWNERDATA;
	DWORD dwExStyle = WS_EX_NOPARENTNOTIFY|WS_EX_TOPMOST;
	CRect rect(0,0,1,1);
	BOOL bCreated = CWnd::CreateEx(0, WC_LISTVIEW, NULL,
		dwStyle,
		rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, NULL);
	if (bCreated)
	{
		SetExtendedStyle(LVS_EX_DOUBLEBUFFER);
	}
	return bCreated;
}


