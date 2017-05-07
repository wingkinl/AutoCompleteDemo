#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "AutoCompleteDemoEdit.h"

class CDemoListCtrl : public CMFCListCtrl
{
	DECLARE_DYNCREATE(CDemoListCtrl)
public:
	CDemoListCtrl();
	~CDemoListCtrl();
public:
	void InitList();
protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
protected:
	DECLARE_MESSAGE_MAP()
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

	DECLARE_MESSAGE_MAP()
public:
	CDemoACEdit m_editSingleLine;
	CDemoACEdit m_editMultiLine;
	CDemoListCtrl m_listctrl;
};


