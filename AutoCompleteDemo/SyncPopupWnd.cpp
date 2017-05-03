// SyncPopupWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SyncPopupWnd.h"

#define DEFAULT_SYNC_WND_SIZE	100

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

BOOL CSyncMFCFPPopupWnd::GetChildClientRect(CRect& rect)
{
	CMFCPopupMenuBar* pMenuBar = GetMenuBar();
	ASSERT_VALID(pMenuBar);
	if (!::IsWindow(m_hWnd) || pMenuBar == NULL || !::IsWindow(pMenuBar->m_hWnd))
		return FALSE;
	pMenuBar->GetWindowRect(&rect);
	ScreenToClient(rect);
	return TRUE;
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

IMPLEMENT_DYNAMIC(CSyncNonMFCFPPopupWnd, CSyncNonMFCFPPopupWndBase)

CSyncNonMFCFPPopupWnd::CSyncNonMFCFPPopupWnd()
{

}

CSyncNonMFCFPPopupWnd::~CSyncNonMFCFPPopupWnd()
{

}

BOOL CSyncNonMFCFPPopupWnd::GetChildClientRect(CRect& rect)
{
	GetClientRect(rect);
	return TRUE;
}

void CSyncNonMFCFPPopupWnd::PostNcDestroy()
{
	delete this;
}

BEGIN_MESSAGE_MAP(CSyncNonMFCFPPopupWnd, CSyncNonMFCFPPopupWndBase)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

BOOL CSyncNonMFCFPPopupWnd::Create(CWnd* pOwner, POINT pos)
{
	UINT nClassStyle = CS_HREDRAW|CS_VREDRAW;
	CString strClassName = ::AfxRegisterWndClass(nClassStyle, ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
	DWORD dwExStyle = WS_EX_TOPMOST|WS_EX_NOPARENTNOTIFY;
	DWORD dwStyle = WS_POPUP|WS_BORDER;
	CRect rect(pos, CSize(DEFAULT_SYNC_WND_SIZE, DEFAULT_SYNC_WND_SIZE));
	BOOL bCreated = CreateEx(dwExStyle, strClassName, _T(""),
		dwStyle, rect, pOwner, 0);
	if (!bCreated)
		return FALSE;
	SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	return TRUE;
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

BOOL CSyncNonMFCFPPopupWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CSyncNonMFCFPPopupWnd::OnPaint()
{
	CPaintDC dc(this);
}

int CSyncNonMFCFPPopupWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return MA_NOACTIVATE;
}

#endif	// !_AC_DONT_USE_MFC_FP


IMPLEMENT_DYNAMIC(CSyncPopupWnd, CSyncPopupWndBase)

CSyncPopupWnd::CSyncPopupWnd()
{

}

CSyncPopupWnd::~CSyncPopupWnd()
{

}

CSize CSyncPopupWnd::CalcSize() const
{
	return CSize(DEFAULT_SYNC_WND_SIZE, DEFAULT_SYNC_WND_SIZE);
}

