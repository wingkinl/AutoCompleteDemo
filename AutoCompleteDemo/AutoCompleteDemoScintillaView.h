#pragma once


// CAutoCompleteDemoScintillaView view

class CAutoCompleteDemoScintillaView : public CView
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

protected:
	DECLARE_MESSAGE_MAP()
};


