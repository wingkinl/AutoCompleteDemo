// AutoCompleteDemoScintillaView.cpp : implementation file
//

#include "stdafx.h"
#ifdef _ENABLE_SCINTILLA_BUILD
#include "AutoCompleteDemoScintillaView.h"


// CAutoCompleteDemoScintillaView

IMPLEMENT_DYNCREATE(CAutoCompleteDemoScintillaView, CAutoCompleteDemoScintillaViewBase)

CAutoCompleteDemoScintillaView::CAutoCompleteDemoScintillaView()
{

}

CAutoCompleteDemoScintillaView::~CAutoCompleteDemoScintillaView()
{
}

BEGIN_MESSAGE_MAP(CAutoCompleteDemoScintillaView, CAutoCompleteDemoScintillaViewBase)
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
	CAutoCompleteDemoScintillaViewBase::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoCompleteDemoScintillaView::Dump(CDumpContext& dc) const
{
	CAutoCompleteDemoScintillaViewBase::Dump(dc);
}
#endif
#endif //_DEBUG


// CAutoCompleteDemoScintillaView message handlers


#endif // _ENABLE_SCINTILLA_BUILD
// THE END OF FILE
//////////////////////////////////////////////////////////////////////////