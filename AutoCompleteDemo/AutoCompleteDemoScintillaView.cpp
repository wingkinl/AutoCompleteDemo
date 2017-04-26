// AutoCompleteDemoScintillaView.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteDemoScintillaView.h"


// CAutoCompleteDemoScintillaView

IMPLEMENT_DYNCREATE(CAutoCompleteDemoScintillaView, CView)

CAutoCompleteDemoScintillaView::CAutoCompleteDemoScintillaView()
{

}

CAutoCompleteDemoScintillaView::~CAutoCompleteDemoScintillaView()
{
}

BEGIN_MESSAGE_MAP(CAutoCompleteDemoScintillaView, CView)
END_MESSAGE_MAP()


// CAutoCompleteDemoScintillaView drawing

void CAutoCompleteDemoScintillaView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CAutoCompleteDemoScintillaView diagnostics

#ifdef _DEBUG
void CAutoCompleteDemoScintillaView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoCompleteDemoScintillaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAutoCompleteDemoScintillaView message handlers
