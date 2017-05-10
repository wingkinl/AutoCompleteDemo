// AutoCompleteDemoScintillaView.cpp : implementation file
//

#include "stdafx.h"
#ifdef _ENABLE_SCINTILLA_BUILD
#include "AutoCompleteDemoScintillaView.h"


// CAutoCompleteDemoScintillaView

IMPLEMENT_DYNCREATE(CAutoCompleteDemoScintillaView, CAutoCompleteDemoScintillaViewBase)

CAutoCompleteDemoScintillaView::CAutoCompleteDemoScintillaView()
{
	//m_acImp.m_pEdit = &GetCtrl();
}

CAutoCompleteDemoScintillaView::~CAutoCompleteDemoScintillaView()
{
}

BEGIN_MESSAGE_MAP(CAutoCompleteDemoScintillaView, CAutoCompleteDemoScintillaViewBase)
	//ON_MESSAGE(WM_AC_NOTIFY, &CAutoCompleteDemoScintillaView::OnACNotify)
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

class CAutoCompleteDemoScintillaCtrl : public CScintillaCtrl
{
public:
	CAutoCompleteDemoScintillaCtrl();
	~CAutoCompleteDemoScintillaCtrl();

protected:
	CDemoScintillaACImp	m_acImp;
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnACNotify(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};

CAutoCompleteDemoScintillaCtrl::CAutoCompleteDemoScintillaCtrl()
{
	m_acImp.m_pEdit = this;
}

CAutoCompleteDemoScintillaCtrl::~CAutoCompleteDemoScintillaCtrl()
{

}

BEGIN_MESSAGE_MAP(CAutoCompleteDemoScintillaCtrl, CScintillaCtrl)
	ON_WM_CHAR()
	ON_MESSAGE(WM_AC_NOTIFY, &CAutoCompleteDemoScintillaCtrl::OnACNotify)
END_MESSAGE_MAP()

void CAutoCompleteDemoScintillaCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CScintillaCtrl::OnChar(nChar, nRepCnt, nFlags);
	CAutoCompleteWnd::Activate(this, nChar);
}

LRESULT CAutoCompleteDemoScintillaCtrl::OnACNotify(WPARAM wp, LPARAM lp)
{
	return m_acImp.OnACNotify(wp, lp);
}

std::unique_ptr<CScintillaCtrl> CAutoCompleteDemoScintillaView::CreateControl()
{
#if _MSC_VER >= 1800
	return std::make_unique<CAutoCompleteDemoScintillaCtrl>();
#else
	return std::unique_ptr<CAutoCompleteDemoScintillaCtrl>(new CAutoCompleteDemoScintillaCtrl());
#endif
}

// void CAutoCompleteDemoScintillaView::OnCharAdded(SCNotification* pSCNotification)
// {
// 	CAutoCompleteDemoScintillaViewBase::OnCharAdded(pSCNotification);
// 	CAutoCompleteWnd::Activate(this, pSCNotification->ch);
// }

// CAutoCompleteDemoScintillaView message handlers

// LRESULT CAutoCompleteDemoScintillaView::OnACNotify(WPARAM wp, LPARAM lp)
// {
// 	return m_acImp.OnACNotify(wp, lp);
// }


#endif // _ENABLE_SCINTILLA_BUILD
// THE END OF FILE
//////////////////////////////////////////////////////////////////////////