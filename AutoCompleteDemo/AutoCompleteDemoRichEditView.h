
// AutoCompleteDemoRichEditView.h : interface of the CAutoCompleteDemoRichEditView class
//

#pragma once
#include "AutoCompleteDemoEdit.h"

class CAutoCompleteDemoCntrItem;

class CAutoCompleteDemoRichEditView : public CRichEditView
{
protected: // create from serialization only
	CAutoCompleteDemoRichEditView();
	DECLARE_DYNCREATE(CAutoCompleteDemoRichEditView)

// Attributes
public:
	CAutoCompleteDemoRichEditDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CAutoCompleteDemoRichEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CDemoEditACImp	m_acImp;
// Generated message map functions
protected:
	afx_msg void OnDestroy();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnACNotify(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AutoCompleteDemoRichEditView.cpp
inline CAutoCompleteDemoRichEditDoc* CAutoCompleteDemoRichEditView::GetDocument() const
   { return reinterpret_cast<CAutoCompleteDemoRichEditDoc*>(m_pDocument); }
#endif

