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

IMPLEMENT_DYNCREATE(CDemoListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(CDemoListCtrl, CListCtrl)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, &OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &OnEndLabelEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CDemoListCtrl::InitList()
{
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
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDemoListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	CEdit* pEdit = GetEditControl();
	ASSERT(pEdit);
	CDemoEdit* pEditNew = new CDemoEdit;
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
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

CDemoEdit::CDemoEdit()
{
	m_bAutoDelete = false;
}

CDemoEdit::~CDemoEdit()
{

}

void CDemoEdit::PostNcDestroy()
{
	if (m_bAutoDelete)
		delete this;
}

IMPLEMENT_DYNCREATE(CDemoEdit, CEdit)

BEGIN_MESSAGE_MAP(CDemoEdit, CEdit)
	ON_WM_CHAR()
	ON_MESSAGE(WM_AC_NOTIFY, &CDemoEdit::OnACNotify)
END_MESSAGE_MAP()

void CDemoEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	CAutoCompleteWnd::Activate(this, nChar);
}

class CDemoEditACImp : public CEditACImp
{
public:
	BOOL IsValidChar(UINT nChar) const override;
};

BOOL CDemoEditACImp::IsValidChar(UINT nChar) const
{
	if (!CEditACImp::IsValidChar(nChar))
	{
		switch (nChar)
		{
		case _T('_'):
		case _T('$'):
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CDemoEdit::GetInitInfo(AUTOCNMHDR* nmhdr) const
{
	AUTOCINITINFO* pInfo = (AUTOCINITINFO*)nmhdr->lp;
	pInfo->nListItems = (int)theApp.m_saTestList.GetCount();

	CDemoEditACImp acImp;
	acImp.m_pEdit = (CEdit*)this;
	CString strWordBegin;
	BOOL bRet = acImp.GetInitInfo(pInfo->posScreen, strWordBegin);
	return bRet;
}

LRESULT CDemoEdit::OnACNotify(WPARAM wp, LPARAM lp)
{
	ACCmd cmd = (ACCmd)wp;
	AUTOCNMHDR* nmhdr = (AUTOCNMHDR*)lp;

	switch (cmd)
	{
	case ACCmdGetInitInfo:
		return GetInitInfo(nmhdr);
	}
	return 0;
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

