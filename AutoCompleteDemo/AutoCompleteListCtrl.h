#pragma once

#ifndef ACLISTCTRL_EXT_CLASS
	#define ACLISTCTRL_EXT_CLASS
#endif

// CAutoCompleteListCtrl

typedef CListCtrl	CAutoCompleteListCtrlBase;

class ACLISTCTRL_EXT_CLASS CAutoCompleteListCtrl : public CAutoCompleteListCtrlBase
{
	DECLARE_DYNAMIC(CAutoCompleteListCtrl)
public:
	CAutoCompleteListCtrl();
	virtual ~CAutoCompleteListCtrl();
public:
	int MoveSelection(int nDelta);
	
	int GetCurSel() const;
	void SetCurSel(int nItem);

	BOOL IsShowVScrollBar() const;
protected:
	int HitTestSelectItem(const POINT& point);
	void PostNcDestroy() override;
protected:
	BOOL	m_bShowVScroll;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
protected:
	DECLARE_MESSAGE_MAP()
};


