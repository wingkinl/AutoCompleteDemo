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

void CAutoCompleteListCtrl::MoveSelection(int nDelta)
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
}

int CAutoCompleteListCtrl::GetVisibleRows() const
{
	return 10;
}

int CAutoCompleteListCtrl::GetCurSel() const
{
	return GetNextItem(-1, LVNI_SELECTED);
}

void CAutoCompleteListCtrl::SetCurSel(int nItem)
{
	SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

void CAutoCompleteListCtrl::HitTestSelectItem(const POINT& point)
{
	// We must take control of selection to prevent this window activating
	// the popup
	int nItem = HitTest(point);
	if (nItem >= 0)
		SetCurSel(nItem);
}

BEGIN_MESSAGE_MAP(CAutoCompleteListCtrl, CAutoCompleteListCtrlBase)
	ON_WM_CREATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CAutoCompleteListCtrl message handlers

int CAutoCompleteListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nRet = CAutoCompleteListCtrlBase::OnCreate(lpCreateStruct);
	if (nRet != 0)
		return nRet;

	SetExtendedStyle(LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT);

	InsertColumn(0, _T("Name"));

	int nItem = 0;
	InsertItem(nItem++, _T("Tom"));
	InsertItem(nItem++, _T("John"));
	InsertItem(nItem++, _T("Emily"));
	InsertItem(nItem++, _T("May"));
	InsertItem(nItem++, _T("Mike"));
	InsertItem(nItem++, _T("Morgan"));
	InsertItem(nItem++, _T("Steve"));

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

void CAutoCompleteListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
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

void CAutoCompleteListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CAutoCompleteListCtrlBase::OnSize(nType, cx, cy);
	SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}


