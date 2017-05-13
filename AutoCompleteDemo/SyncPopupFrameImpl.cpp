#include "stdafx.h"
#include "SyncPopupFrameImpl.h"
#include "SyncPopupWnd.h"


CSyncPopupFrameImpl::CSyncPopupFrameImpl(CFrameWnd* pFrame)
{
}


CSyncPopupFrameImpl::~CSyncPopupFrameImpl()
{
}

BOOL CSyncPopupFrameImpl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_KEYUP:
		//-----------------------------------------
		// Pass keyboard action to the active menu:
		//-----------------------------------------
		if ( ProcessKeyboard(pMsg->message, (int)pMsg->wParam) )
		{
			return TRUE;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		{
			CPoint pt(AFX_GET_X_LPARAM(pMsg->lParam), AFX_GET_Y_LPARAM(pMsg->lParam));
			CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);

			if (pWnd != NULL && ::IsWindow(pMsg->hwnd))
			{
				pWnd->ClientToScreen(&pt);
			}

			if (ProcessMouseClick(pMsg->message, pt, pMsg->hwnd))
			{
				return TRUE;
			}

			if (!::IsWindow(pMsg->hwnd))
			{
				return TRUE;
			}
		}
		break;

	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
		if (ProcessMouseClick(pMsg->message, CPoint(AFX_GET_X_LPARAM(pMsg->lParam), AFX_GET_Y_LPARAM(pMsg->lParam)), pMsg->hwnd))
		{
			return TRUE;
		}
		break;

	case WM_MOUSEWHEEL:
		if (ProcessMouseWheel(pMsg->wParam, pMsg->lParam))
		{
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CSyncPopupFrameImpl::ProcessKeyboard(UINT message, int nKey)
{
	// If popup window is active, pass keyboard control to it
	CSyncPopupWnd* pActivePopupWnd = CSyncPopupWnd::GetSafeActivePopup();
	if (pActivePopupWnd != NULL)
	{
		auto res = pActivePopupWnd->SendMessage(message, nKey);
		// should return zero if it processes this message.
		return res == 0;
	}
	return FALSE;
}

BOOL CSyncPopupFrameImpl::ProcessMouseClick(UINT uiMsg, POINT pt, HWND hwnd)
{
	CSyncPopupWnd* pActivePopupWnd = CSyncPopupWnd::GetSafeActivePopup();
	if (pActivePopupWnd != NULL)
	{
		return pActivePopupWnd->ProcessMouseClick(uiMsg, pt, hwnd);
	}
	return FALSE;
}

BOOL CSyncPopupFrameImpl::ProcessMouseWheel(WPARAM wParam, LPARAM lParam)
{
	CSyncPopupWnd* pActivePopupWnd = CSyncPopupWnd::GetSafeActivePopup();
	if (pActivePopupWnd != NULL)
		return(BOOL)pActivePopupWnd->SendMessage(WM_MOUSEWHEEL, wParam, lParam);
	return FALSE;
}
