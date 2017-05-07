// AutoCompleteDemoFormView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AutoCompleteDemoFormView.h"
#include "AutoCompleteWnd.h"
#include "AutoCompleteDemo.h"

CDemoListCtrl::CDemoListCtrl()
{

}

CDemoListCtrl::~CDemoListCtrl()
{

}

IMPLEMENT_DYNCREATE(CDemoListCtrl, CMFCListCtrl)

BEGIN_MESSAGE_MAP(CDemoListCtrl, CMFCListCtrl)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, &OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &OnEndLabelEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CDemoListCtrl::InitList()
{
	ModifyStyle(0, LVS_SHOWSELALWAYS);
	SetExtendedStyle(LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);
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

	SetColumnWidth(0, 100);
	SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
}

void CDemoListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_F2)
	{
		int nItem = GetNextItem(-1, LVNI_SELECTED);
		if (nItem >= 0)
			EditLabel(nItem);
	}
	CMFCListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDemoListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	CEdit* pEdit = GetEditControl();
	ASSERT(pEdit);
	CDemoACEdit* pEditNew = new CDemoACEdit;
	pEditNew->m_bAutoDelete = true;
	pEditNew->SubclassWindow(pEdit->GetSafeHwnd());
}

void CDemoListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = TRUE;
}

void CDemoListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nItem = HitTest(point);
	if (nItem >= 0)
	{
		EditLabel(nItem);
		return;
	}
	CMFCListCtrl::OnLButtonDblClk(nFlags, point);
}


// CAutoCompleteDemoFormView

IMPLEMENT_DYNCREATE(CAutoCompleteDemoFormView, CFormView)

CAutoCompleteDemoFormView::CAutoCompleteDemoFormView()
	: CFormView(IDD_AUTOCOMPLETEDEMOFORMVIEW)
{

}

CAutoCompleteDemoFormView::~CAutoCompleteDemoFormView()
{
}

void CAutoCompleteDemoFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SINGLELINE, m_editSingleLine);
	DDX_Control(pDX, IDC_EDIT_MULTILINE, m_editMultiLine);
	DDX_Control(pDX, IDC_LIST1, m_listctrl);
}

BEGIN_MESSAGE_MAP(CAutoCompleteDemoFormView, CFormView)
END_MESSAGE_MAP()

BOOL CAutoCompleteDemoFormView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CAutoCompleteDemoFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();
	m_listctrl.InitList();
}

// CAutoCompleteDemoFormView diagnostics

#ifdef _DEBUG
void CAutoCompleteDemoFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoCompleteDemoFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAutoCompleteDemoFormView message handlers

