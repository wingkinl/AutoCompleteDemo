#pragma once


// CAutoCompleteListCtrl

typedef CListCtrl	CAutoCompleteListCtrlBase;

class CAutoCompleteListCtrl : public CAutoCompleteListCtrlBase
{
	DECLARE_DYNAMIC(CAutoCompleteListCtrl)
public:
	CAutoCompleteListCtrl();
	virtual ~CAutoCompleteListCtrl();
public:
	int MoveSelection(int nDelta);
	
	int GetCurSel() const;
	void SetCurSel(int nItem);

	int GetItemHeight() const;

	BOOL IsShowVScrollBar() const;
	void SetShowVScrollBar(BOOL bShow);
protected:
	void HitTestSelectItem(const POINT& point);
	void PostNcDestroy() override;
protected:
	BOOL	m_bShowVScroll;
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
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
protected:
	DECLARE_MESSAGE_MAP()
};


