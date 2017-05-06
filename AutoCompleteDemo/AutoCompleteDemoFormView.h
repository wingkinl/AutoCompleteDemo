#pragma once
#include "afxwin.h"
#include "afxcmn.h"

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

struct AUTOCNMHDR;

class CDemoEdit : public CEdit
{
	DECLARE_DYNCREATE(CDemoEdit)
public:
	CDemoEdit();
	~CDemoEdit();
public:
	bool m_bAutoDelete;
protected:
	void PostNcDestroy() override;

	BOOL GetACInitInfo(AUTOCNMHDR* nmhdr);
	BOOL GetACDisplayInfo(AUTOCNMHDR* nmhdr) const;
	BOOL HandleKey(AUTOCNMHDR* nmhdr);
	BOOL AutoComplete(AUTOCNMHDR* nmhdr);
protected:
	int UpdateFilteredList(LPCTSTR pszText);
protected:
	CArray<int>		m_arrFilteredIndices;
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnACNotify(WPARAM wp, LPARAM lp);
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
	CDemoEdit m_editSingleLine;
	CDemoEdit m_editMultiLine;
	CDemoListCtrl m_listctrl;
};


