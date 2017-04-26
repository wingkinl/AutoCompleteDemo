#pragma once



// CAutoCompleteDemoFormView form view

class CAutoCompleteDemoFormView : public CFormView
{
	DECLARE_DYNCREATE(CAutoCompleteDemoFormView)

protected:
	CAutoCompleteDemoFormView();           // protected constructor used by dynamic creation
	virtual ~CAutoCompleteDemoFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTOCOMPLETEDEMOFORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};


