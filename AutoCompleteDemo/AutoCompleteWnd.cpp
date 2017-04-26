// AutoCompleteWnd.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteWnd.h"
#include "AutoCompleteListCtrl.h"


// CAutoCompleteWnd

IMPLEMENT_DYNAMIC(CAutoCompleteWnd, CAutoCompleteWndBase)

CAutoCompleteWnd::CAutoCompleteWnd()
{
	m_listCtrl = nullptr;
}

CAutoCompleteWnd::~CAutoCompleteWnd()
{
}

BEGIN_MESSAGE_MAP(CAutoCompleteWnd, CAutoCompleteWndBase)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CAutoCompleteWnd message handlers

BOOL CAutoCompleteWnd::Create(CWnd* pOwner, POINT pos)
{
#if 0
	DWORD dwStyle = WS_POPUP|WS_BORDER|WS_VSCROLL;
	dwStyle |= LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS;
	// the image list should be reused
	dwStyle |= LVS_SHAREIMAGELISTS;
	dwStyle |= LVS_NOCOLUMNHEADER;

	//dwStyle |= LVS_OWNERDATA;
	DWORD dwExStyle = WS_EX_NOPARENTNOTIFY|WS_EX_TOPMOST;
	CRect rect(pos.x,pos.y,pos.x+100,pos.y+100);
	BOOL bCreated = CWnd::CreateEx(dwExStyle, WC_LISTVIEW, NULL,
		dwStyle,
		rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, NULL);
	if (bCreated)
	{
		SetExtendedStyle(LVS_EX_DOUBLEBUFFER);
		SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
	return bCreated;
#endif
	DWORD dwStyle = WS_POPUP;
	CRect rect(pos.x, pos.y, pos.x + 100, pos.y + 100);
	BOOL bCreated = CAutoCompleteWndBase::Create(NULL, _T(""), dwStyle, rect);
	if (bCreated)
	{
		SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
	return bCreated;
}

BOOL CAutoCompleteWnd::CreateListCtrl()
{
	m_listCtrl = new CAutoCompleteListCtrl;
	DWORD dwStyle = WS_CHILD | WS_BORDER | WS_VSCROLL;
	dwStyle |= LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	// the image list should be reused
	dwStyle |= LVS_SHAREIMAGELISTS;
	dwStyle |= LVS_NOCOLUMNHEADER;

	//dwStyle |= LVS_OWNERDATA;
	CRect rect;
	GetClientRect(rect);
	BOOL bCreated = m_listCtrl->Create(dwStyle, rect, this, 1);
	if (bCreated)
	{
		m_listCtrl->SetExtendedStyle(LVS_EX_DOUBLEBUFFER);
	}
	return bCreated;
}



int CAutoCompleteWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	CreateListCtrl();
	return 0;
}
