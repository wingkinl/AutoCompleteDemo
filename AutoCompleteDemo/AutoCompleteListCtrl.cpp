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

BEGIN_MESSAGE_MAP(CAutoCompleteListCtrl, CAutoCompleteListCtrlBase)
	ON_WM_CREATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
END_MESSAGE_MAP()

// CAutoCompleteListCtrl message handlers

int CAutoCompleteListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nRet = CAutoCompleteListCtrlBase::OnCreate(lpCreateStruct);
	if (nRet != 0)
		return nRet;

	InsertColumn(0, _T("Name"));
	InsertColumn(1, _T("Age"));

	InsertItem(0, _T("Tom"));
	SetItemText(0, 1, _T("12"));
	InsertItem(1, _T("John"));
	SetItemText(1, 1, _T("11"));
	InsertItem(2, _T("Emily"));
	SetItemText(2, 1, _T("10"));
	InsertItem(3, _T("Mike"));
	SetItemText(3, 1, _T("13"));

	SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	return 0;
}

int CAutoCompleteListCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// This prevents the view activating when the scrollbar is clicked
	return MA_NOACTIVATE;
}

void CAutoCompleteListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// We must take control of selection to prevent the ListBox activating
	// the popup
	int nItem = HitTest(point);
	if (nItem >= 0)
		SetItemState(nItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void CAutoCompleteListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{

}

void CAutoCompleteListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{

}

void CAutoCompleteListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{

}

void CAutoCompleteListCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	// disable the scroll wheel button click action
}



