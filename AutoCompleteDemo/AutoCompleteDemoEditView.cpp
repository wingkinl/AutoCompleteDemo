// AutoCompleteDemoEditView.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteDemoEditView.h"


// CAutoCompleteDemoEditView

IMPLEMENT_DYNCREATE(CAutoCompleteDemoEditView, CEditView)

CAutoCompleteDemoEditView::CAutoCompleteDemoEditView()
{
	m_acImp.m_pEdit = &GetEditCtrl();
}

CAutoCompleteDemoEditView::~CAutoCompleteDemoEditView()
{
}

BEGIN_MESSAGE_MAP(CAutoCompleteDemoEditView, CEditView)
	ON_WM_CHAR()
	ON_MESSAGE(WM_AC_NOTIFY, &OnACNotify)
END_MESSAGE_MAP()


// CAutoCompleteDemoEditView diagnostics

#ifdef _DEBUG
void CAutoCompleteDemoEditView::AssertValid() const
{
	CEditView::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoCompleteDemoEditView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAutoCompleteDemoEditView message handlers

void CAutoCompleteDemoEditView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEditView::OnChar(nChar, nRepCnt, nFlags);
	CAutoCompleteWnd::Activate(this, nChar);
}

LRESULT CAutoCompleteDemoEditView::OnACNotify(WPARAM wp, LPARAM lp)
{
	return m_acImp.OnACNotify(wp, lp);
}
