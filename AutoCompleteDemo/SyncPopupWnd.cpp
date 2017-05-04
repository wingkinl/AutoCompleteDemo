// SyncPopupWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SyncPopupWnd.h"

#define DEFAULT_SYNC_WND_SIZE	100

IMPLEMENT_DYNAMIC(CSyncPopupWnd, CSyncPopupWndBase)

CSyncPopupWnd::CSyncPopupWnd()
{

}

CSyncPopupWnd::~CSyncPopupWnd()
{

}

CSyncPopupWnd* CSyncPopupWnd::m_pActivePopupWnd = nullptr;

CSyncPopupWnd* CSyncPopupWnd::GetSafeActivePopup()
{
	if (m_pActivePopupWnd != NULL && ::IsWindow(m_pActivePopupWnd->m_hWnd) && CWnd::FromHandlePermanent(m_pActivePopupWnd->m_hWnd) != NULL)
	{
		return m_pActivePopupWnd;
	}
	return NULL;
}

CSize CSyncPopupWnd::CalcSize() const
{
	return CSize(DEFAULT_SYNC_WND_SIZE, DEFAULT_SYNC_WND_SIZE);
}

void CSyncPopupWnd::PostNcDestroy()
{
	delete this;
}

BEGIN_MESSAGE_MAP(CSyncPopupWnd, CSyncPopupWndBase)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEACTIVATE()
	ON_WM_DESTROY()
	ON_WM_ACTIVATEAPP()
END_MESSAGE_MAP()

BOOL CSyncPopupWnd::Create(CWnd* pOwner, POINT pos)
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

BOOL CSyncPopupWnd::HitTest(const POINT& ptScreen) const
{
	CRect rectWindow;
	GetClientRect(rectWindow);
	ClientToScreen(rectWindow);

	if (!rectWindow.PtInRect(ptScreen))
	{
		return FALSE;
	}
	return TRUE;
}

void CSyncPopupWnd::Close()
{
	SendMessage(WM_CLOSE);
}

int CSyncPopupWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nRet = CSyncPopupWndBase::OnCreate(lpCreateStruct);
	if (nRet != 0)
		return nRet;
	m_pActivePopupWnd = this;
	CSize size = CalcSize();
	SetWindowPos(NULL, -1, -1, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	return 0;
}

BOOL CSyncPopupWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CSyncPopupWnd::OnPaint()
{
	CPaintDC dc(this);
}

int CSyncPopupWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return MA_NOACTIVATE;
}

void CSyncPopupWnd::OnDestroy()
{
	if (CSyncPopupWnd::m_pActivePopupWnd == this)
	{
		CSyncPopupWnd::m_pActivePopupWnd = NULL;
	}
	CSyncPopupWndBase::OnDestroy();
}

void CSyncPopupWnd::OnActivateApp(BOOL bActive, DWORD /*dwThreadID*/)
{
	if (!bActive)
	{
		PostMessage(WM_CLOSE);
	}
}
