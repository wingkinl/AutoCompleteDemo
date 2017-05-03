#pragma once


// CAutoCompleteListCtrl

typedef CMFCListCtrl	CAutoCompleteListCtrlBase;

class CAutoCompleteListCtrl : public CAutoCompleteListCtrlBase
{
	DECLARE_DYNAMIC(CAutoCompleteListCtrl)
public:
	CAutoCompleteListCtrl();
	virtual ~CAutoCompleteListCtrl();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
protected:
	DECLARE_MESSAGE_MAP()
};


