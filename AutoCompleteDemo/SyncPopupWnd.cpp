// SyncPopupWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SyncPopupWnd.h"

#ifndef _AC_DONT_USE_MFC_FP
IMPLEMENT_DYNAMIC(CSyncMFCFPPopupWnd, CSyncMFCFPPopupWndBase)

CSyncMFCFPPopupWnd::CSyncMFCFPPopupWnd()
{
}

CSyncMFCFPPopupWnd::~CSyncMFCFPPopupWnd()
{
}

BOOL CSyncMFCFPPopupWnd::Create(CWnd* pOwner, POINT pos)
{
	BOOL bCreate = CSyncMFCFPPopupWndBase::Create(pOwner, pos.x, pos.y, (HMENU)nullptr);
	return bCreate;
}

CSize CSyncMFCFPPopupWnd::CSyncMFCPopupMenuBar::CalcSize(BOOL bVertDock)
{
	auto pParent = GetParent();
	CSyncMFCFPPopupWnd* pSyncWnd = DYNAMIC_DOWNCAST(CSyncMFCFPPopupWnd, pParent);
	return pSyncWnd->CalcSize();
}

BOOL CSyncMFCFPPopupWnd::CSyncMFCPopupMenuBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	dwStyle &= ~WS_VISIBLE;
	return CMFCPopupMenuBar::Create(pParentWnd, dwStyle, nID);
}

#else
// _AC_DONT_USE_MFC_FP

CSyncNonMFCFPPopupWnd::CSyncNonMFCFPPopupWnd()
{

}

CSyncNonMFCFPPopupWnd::~CSyncNonMFCFPPopupWnd()
{

}

BEGIN_MESSAGE_MAP(CSyncNonMFCFPPopupWnd, CSyncNonMFCFPPopupWndBase)
	ON_WM_CREATE()
END_MESSAGE_MAP()

BOOL CSyncNonMFCFPPopupWnd::Create(CWnd* pOwner, POINT pos)
{
	UINT nClassStyle = 0;
	CString strClassName = ::AfxRegisterWndClass(nClassStyle, ::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_BTNFACE + 1), NULL);
	DWORD dwExStyle = WS_EX_TOPMOST|WS_EX_NOPARENTNOTIFY;
	DWORD dwStyle = WS_POPUP;
	BOOL bCreated = CSyncNonMFCFPPopupWndBase::CreateEx(dwExStyle, strClassName, _T(""),
		dwStyle, rect, pOwner);
	if (!bCreated)
		return FALSE;
	SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	return TRUE;
}

void CSyncNonMFCFPPopupWnd::PostNcDestroy()
{
	delete this;
}

int CSyncNonMFCFPPopupWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nRet = CSyncNonMFCFPPopupWndBase::OnCreate(lpCreateStruct);
	if (nRet != 0)
		return nRet;
	CSize size = CalcSize();
	SetWindowPos(NULL, -1, -1, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	return 0;
}

#endif	// !_AC_DONT_USE_MFC_FP


IMPLEMENT_DYNAMIC(CSyncPopupWnd, CSyncPopupWndBase)

CSyncPopupWnd::CSyncPopupWnd()
{

}

CSyncPopupWnd::~CSyncPopupWnd()
{

}

#define DEFAULT_SYNC_WND_SIZE	100

CSize CSyncPopupWnd::CalcSize() const
{
	return CSize(DEFAULT_SYNC_WND_SIZE, DEFAULT_SYNC_WND_SIZE);
}

