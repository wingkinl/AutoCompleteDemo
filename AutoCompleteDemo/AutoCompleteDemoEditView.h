#pragma once


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

protected:
	DECLARE_MESSAGE_MAP()
};


