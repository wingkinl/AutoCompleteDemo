// AutoCompleteDemoFormView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AutoCompleteDemoFormView.h"

CDemoListCtrl::CDemoListCtrl()
{

}

CDemoListCtrl::~CDemoListCtrl()
{

}

void CDemoListCtrl::InitList()
{
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
	ON_EN_CHANGE(IDC_EDIT_MULTILINE, &OnEnChangeMulti)
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

void CAutoCompleteDemoFormView::OnEnChangeMulti()
{
	CPoint posCaret = m_editMultiLine.GetCaretPos();
	int nCurLine = m_editMultiLine.LineFromChar(-1);
	int nCharIndexCurLine = m_editMultiLine.LineIndex(nCurLine+1);
	CPoint pos = m_editMultiLine.PosFromChar(nCharIndexCurLine);
	pos.x = posCaret.x;
	m_editMultiLine.ClientToScreen(&pos);
	m_acWnd.Create(&m_editMultiLine, pos);
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

