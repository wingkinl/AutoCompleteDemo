// AutoCompleteDemoFormView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AutoCompleteDemoFormView.h"


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
}

BEGIN_MESSAGE_MAP(CAutoCompleteDemoFormView, CFormView)
END_MESSAGE_MAP()


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
