// AutoCompleteDemoEditView.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteDemoEditView.h"


// CAutoCompleteDemoEditView

IMPLEMENT_DYNCREATE(CAutoCompleteDemoEditView, CEditView)

CAutoCompleteDemoEditView::CAutoCompleteDemoEditView()
{

}

CAutoCompleteDemoEditView::~CAutoCompleteDemoEditView()
{
}

BEGIN_MESSAGE_MAP(CAutoCompleteDemoEditView, CEditView)
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
