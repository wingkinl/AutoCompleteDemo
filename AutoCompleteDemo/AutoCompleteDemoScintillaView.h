#pragma once

#ifdef _ENABLE_SCINTILLA_BUILD
#include "ScintillaWrapper\ScintillaDocView.h"
#include "AutoCompleteDemoEdit.h"
// CAutoCompleteDemoScintillaView view

typedef CScintillaView	CAutoCompleteDemoScintillaViewBase;

class CAutoCompleteDemoScintillaView : public CAutoCompleteDemoScintillaViewBase
{
	DECLARE_DYNCREATE(CAutoCompleteDemoScintillaView)

protected:
	CAutoCompleteDemoScintillaView();           // protected constructor used by dynamic creation
	virtual ~CAutoCompleteDemoScintillaView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	CDemoScintillaACImp	m_acImp;
protected:
	DECLARE_MESSAGE_MAP()
};


#endif // _ENABLE_SCINTILLA_BUILD