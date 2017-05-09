// AutoCompleteListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteDemo.h"
#include "AutoCompleteListCtrl.h"


// CAutoCompleteListCtrl

IMPLEMENT_DYNAMIC(CAutoCompleteListCtrl, CAutoCompleteListCtrlBase)

CAutoCompleteListCtrl::CAutoCompleteListCtrl()
{

}

CAutoCompleteListCtrl::~CAutoCompleteListCtrl()
{
}

int CAutoCompleteListCtrl::MoveSelection(int nDelta)
{
	int nCurSel = GetCurSel();
	nCurSel += nDelta;
	if (nCurSel < 0 )
		nCurSel = 0;
	else
	{
		int nItemCount = GetItemCount();
		if (nCurSel >= nItemCount)
			nCurSel = nItemCount-1;
	}
	SetCurSel(nCurSel);
	return nCurSel;
}

int CAutoCompleteListCtrl::GetCurSel() const
{
	return GetNextItem(-1, LVNI_SELECTED);
}

void CAutoCompleteListCtrl::SetCurSel(int nItem)
{
	SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

int CAutoCompleteListCtrl::GetItemHeight() const
{
	int nHorzSpacing = 0, nVertSpacing = 0;
	GetItemSpacing(TRUE, &nHorzSpacing, &nVertSpacing);
	return nVertSpacing;
}

BOOL CAutoCompleteListCtrl::IsShowVScrollBar() const
{
	return m_bShowVScroll;
}

void CAutoCompleteListCtrl::SetShowVScrollBar(BOOL bShow)
{
	m_bShowVScroll = bShow;
}

int CAutoCompleteListCtrl::HitTestSelectItem(const POINT& point)
{
	// We must take control of selection to prevent this window activating
	// the popup
	int nItem = HitTest(point);
	if (nItem >= 0)
		SetCurSel(nItem);
	return nItem;
}

void CAutoCompleteListCtrl::PostNcDestroy()
{
	delete this;
}

BEGIN_MESSAGE_MAP(CAutoCompleteListCtrl, CAutoCompleteListCtrlBase)
	ON_WM_CREATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()

// CAutoCompleteListCtrl message handlers

int CAutoCompleteListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nRet = CAutoCompleteListCtrlBase::OnCreate(lpCreateStruct);
	if (nRet != 0)
		return nRet;

	SetExtendedStyle(LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT);

 	InsertColumn(0, _T("Value"));
 	SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	return 0;
}

int CAutoCompleteListCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// This prevents the view activating when the scrollbar is clicked
	return MA_NOACTIVATE;
}

void CAutoCompleteListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	HitTestSelectItem(point);
}

void CAutoCompleteListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{

}

void CAutoCompleteListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	HitTestSelectItem(point);
}

void CAutoCompleteListCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{

}

void CAutoCompleteListCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	// disable the scroll wheel button click action
}

void CAutoCompleteListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	DWORD dwRemove = WS_HSCROLL;
	DWORD dwAdd = 0;
	if (IsShowVScrollBar())
		dwAdd |= WS_VSCROLL;
	else
		dwRemove |= WS_VSCROLL;
	ModifyStyle(dwRemove, dwAdd);
	CAutoCompleteListCtrlBase::OnNcCalcSize(bCalcValidRects, lpncsp);
}

