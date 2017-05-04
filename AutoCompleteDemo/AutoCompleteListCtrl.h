#pragma once


// CAutoCompleteListCtrl

typedef CMFCListCtrl	CAutoCompleteListCtrlBase;

class CAutoCompleteListCtrl : public CAutoCompleteListCtrlBase
{
	DECLARE_DYNAMIC(CAutoCompleteListCtrl)
public:
	CAutoCompleteListCtrl();
	virtual ~CAutoCompleteListCtrl();
public:
	void MoveSelection(int nDelta);
	int GetVisibleRows() const;

	int GetCurSel() const;
	void SetCurSel(int nItem);
protected:
	void HitTestSelectItem(const POINT& point);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	DECLARE_MESSAGE_MAP()
};


