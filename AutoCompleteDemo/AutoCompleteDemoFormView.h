#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CDemoListCtrl : public CListCtrl
{
public:
	CDemoListCtrl();
	~CDemoListCtrl();
public:
	void InitList();
protected:
private:
};

// CAutoCompleteDemoFormView form view

class CAutoCompleteDemoFormView : public CFormView
{
	DECLARE_DYNCREATE(CAutoCompleteDemoFormView)

protected:
	CAutoCompleteDemoFormView();           // protected constructor used by dynamic creation
	virtual ~CAutoCompleteDemoFormView();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
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
	virtual void OnInitialUpdate(); // called first time after construct
	afx_msg void OnEnChangeMulti();

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editSingleLine;
	CEdit m_editMultiLine;
	CDemoListCtrl m_listctrl;
};


