
// AutoCompleteDemoRichEditView.cpp : implementation of the CAutoCompleteDemoRichEditView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "AutoCompleteDemo.h"
#endif

#include "AutoCompleteDemoRichEditDoc.h"
#include "CntrItem.h"
#include "resource.h"
#include "AutoCompleteDemoRichEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoCompleteDemoRichEditView

IMPLEMENT_DYNCREATE(CAutoCompleteDemoRichEditView, CRichEditView)

BEGIN_MESSAGE_MAP(CAutoCompleteDemoRichEditView, CRichEditView)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CHAR()
	ON_MESSAGE(WM_AC_NOTIFY, &OnACNotify)
END_MESSAGE_MAP()

// CAutoCompleteDemoRichEditView construction/destruction

CAutoCompleteDemoRichEditView::CAutoCompleteDemoRichEditView()
{
	m_acImp.m_pEdit = (CEdit*)&GetRichEditCtrl();
}

CAutoCompleteDemoRichEditView::~CAutoCompleteDemoRichEditView()
{
}

BOOL CAutoCompleteDemoRichEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);
}

void CAutoCompleteDemoRichEditView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();


	// Set the printing margins (720 twips = 1/2 inch)
	SetMargins(CRect(720, 720, 720, 720));
}

void CAutoCompleteDemoRichEditView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
   CRichEditView::OnDestroy();
}


void CAutoCompleteDemoRichEditView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CAutoCompleteDemoRichEditView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CAutoCompleteDemoRichEditView diagnostics

#ifdef _DEBUG
void CAutoCompleteDemoRichEditView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CAutoCompleteDemoRichEditView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CAutoCompleteDemoRichEditDoc* CAutoCompleteDemoRichEditView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAutoCompleteDemoRichEditDoc)));
	return (CAutoCompleteDemoRichEditDoc*)m_pDocument;
}
#endif //_DEBUG


// CAutoCompleteDemoRichEditView message handlers
void CAutoCompleteDemoRichEditView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRichEditView::OnChar(nChar, nRepCnt, nFlags);
	CAutoCompleteWnd::Activate(this, nChar);
}

LRESULT CAutoCompleteDemoRichEditView::OnACNotify(WPARAM wp, LPARAM lp)
{
	return m_acImp.OnACNotify(wp, lp);
}