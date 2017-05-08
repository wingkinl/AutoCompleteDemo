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
	ON_WM_CHAR()
	ON_MESSAGE(WM_AC_NOTIFY, &CAutoCompleteDemoScintillaView::OnACNotify)
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


void CAutoCompleteDemoScintillaView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CAutoCompleteDemoScintillaViewBase::OnChar(nChar, nRepCnt, nFlags);
	CAutoCompleteWnd::Activate(this, nChar);
}

void CAutoCompleteDemoScintillaView::OnCharAdded(SCNotification* pSCNotification)
{
	CAutoCompleteDemoScintillaViewBase::OnCharAdded(pSCNotification);
	CAutoCompleteWnd::Activate(this, pSCNotification->ch);
}

// CAutoCompleteDemoScintillaView message handlers

LRESULT CAutoCompleteDemoScintillaView::OnACNotify(WPARAM wp, LPARAM lp)
{
	return m_acImp.OnACNotify(wp, lp);
}


#endif // _ENABLE_SCINTILLA_BUILD
// THE END OF FILE
//////////////////////////////////////////////////////////////////////////