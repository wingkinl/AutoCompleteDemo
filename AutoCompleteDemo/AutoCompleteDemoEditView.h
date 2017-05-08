#pragma once
#include "AutoCompleteDemoEdit.h"

// CAutoCompleteDemoEditView view

class CAutoCompleteDemoEditView : public CEditView
{
	DECLARE_DYNCREATE(CAutoCompleteDemoEditView)

protected:
	CAutoCompleteDemoEditView();           // protected constructor used by dynamic creation
	virtual ~CAutoCompleteDemoEditView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	CDemoEditACImp	m_acImp;
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnACNotify(WPARAM wp, LPARAM lp);
protected:
	DECLARE_MESSAGE_MAP()
};


