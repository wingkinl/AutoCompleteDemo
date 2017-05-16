#pragma once

#include "SyncPopupWnd.h"

#ifndef ACWND_EXT_CLASS
	#define ACWND_EXT_CLASS
#endif

enum ACCmd
{
	ACCmdGetInitInfo,		// return non-zero to show the window
	ACCmdGetListDispInfo,
	ACCmdSelChange,
	ACCmdKey,
	ACCmdComplete,
	ACCmdDrawIcon,

	ACCmdCustom			= 1024,
};

#define WM_AC_NOTIFY	(WM_APP+0xac)

struct AUTOCNMHDR
{
	HWND	hwndFrom;
	WPARAM	wp;
	LPARAM	lp;
};

class CAutoCTooltipCtrl;

typedef int		EditPosLen;

struct AUTOCTOOLTIPINFO
{
	BOOL	m_bEnable;
	int		m_nInitialTime;	// negative value means don't modify it

	BOOL m_bDrawIcon;
	BOOL m_bDrawDescription;
	BOOL m_bBoldLabel;
	BOOL m_bDrawSeparator;
	BOOL m_bVislManagerTheme;
	int  m_nMaxDescrWidth;
	int  m_nGradientAngle;	// 0 - 360, -1 -default (vertical)
	COLORREF m_clrFill;
	COLORREF m_clrFillGradient;
	COLORREF m_clrText;
	COLORREF m_clrBorder;
};

struct AUTOCINITINFO 
{
	AUTOCNMHDR	hdr;
	POINT		posWordScreen;	// bottom-left corner of the first character
	long		nLineHeight;	// the height of the current line
	int			nItemCount;		// the number of items in the list
	int			nPreSelectItem;	// the item to be selected when display
	int			nMaxVisibleItems;
	int			nStartLine;		// the line index
	EditPosLen	nPosStartChar;	// the position of the first character in editor
	EditPosLen	nStartStrLen;	// the pre-entered length of characters
	CString		strStart;
	BOOL		bDropRestOfWord;
	HFONT		hFont;			// the font for drawing list items
	CImageList*	pImageList;		// the image list for displaying icon
	SIZE		szIcons;		// used only when non-zero values returned AND pImageList is NULL
	AUTOCTOOLTIPINFO stToolTipInfo;
};

struct AUTOCDRAWICON 
{
	AUTOCNMHDR	hdr;
	HDC			hDC;
	int			nItem;
	RECT		rect;
	// the owner can finish drawing itself or return the icon for the
	// window to draw it
	HICON		hIcon;
	BOOL		bAutoDestroyIcon;
};

struct AUTOCITEM
{
	enum MaskType
	{
		ACIF_TEXT = 0x00000001,
		ACIF_IMAGE = 0x00000002,
	};
	UINT	mask;
	int		nItem;
	int		nImage;
	LPTSTR	pszText;
	int		cchTextMax;
};

struct AUTOCDISPINFO 
{
	AUTOCNMHDR	hdr;
	AUTOCITEM	item;
};

struct AUTOCKEYINFO 
{
	AUTOCNMHDR	hdr;
	EditPosLen	nPosStartChar;
	int			nStartLine;
	UINT		nKey;
	UINT		nChar;

	int			nItemCount;	// negative value means don't update list
	int			nPreSelectItem;	// the item to be selected when display
	BOOL		bEatKey;	// eat the char
	BOOL		bClose;		// whether to close the window
};

struct AUTOCSELCHANGEINFO 
{
	AUTOCNMHDR	hdr;
	int			nItem;
	RECT		rcItemScreen;
	// these are applicable only when tooltip was enabled
	CString		strToolTipLabel;
	CString		strToolTipDescription;
};

struct AUTOCCOMPLETE 
{
	AUTOCNMHDR	hdr;
	EditPosLen	nPosStartChar;
	BOOL		bDropRestOfWord;
	int			nItem;
	CString		strText;
};

class ACWND_EXT_CLASS CWindowACImp
{
public:
	CWindowACImp();
	virtual ~CWindowACImp();

	virtual BOOL GetInitInfo(AUTOCINITINFO* pInfo) = 0;
	virtual BOOL IsValidChar(UINT nChar) const;

	virtual BOOL HandleKey(AUTOCKEYINFO* pInfo);
	virtual BOOL AutoComplete(AUTOCCOMPLETE* pInfo) = 0;

	virtual BOOL GetRangeText(CString& strText, EditPosLen nStart, EditPosLen nEnd) const = 0;
	virtual EditPosLen GetCaretPos() const = 0;

	virtual BOOL HandleSelChange(AUTOCSELCHANGEINFO* pSelChangeInfo) {return FALSE;}

	virtual BOOL DrawIcon(AUTOCDRAWICON* pDrawIcon)	{return FALSE;}

	virtual LRESULT OnACNotify(WPARAM wp, LPARAM lp);

	virtual int GetTotalItemCount() const = 0;

	virtual LPCTSTR GetItemDisplayText(int nItem) const;
	virtual int GetItemIconIndex(int nItem) const;

	virtual BOOL GetDisplayInfo(AUTOCNMHDR* nmhdr) const;

	virtual int GetMappedIndex(int nItem) const;

	virtual int UpdateFilteredList(LPCTSTR pszFilterText, int& nPreSelIndex);

	virtual CString GetCompleteText(const CString& strText, int nItem) const {return strText;}

	virtual EditPosLen PositionBefore(EditPosLen nPos) const;
	virtual EditPosLen PositionAfter(EditPosLen nPos) const;
	virtual int LineFromPosition(EditPosLen nPos) const = 0;
	virtual EditPosLen GetLineEndPosition(int nLine) const = 0;
protected:
	CArray<int>		m_arrFilteredIndices;
public:
	bool			m_bMatchCase;
	bool			m_bFuzzyMatch;
	bool			m_bAllowCaretMove;	// caret move won't close the window
	bool			m_bUpdateListAfterCaretMove;
};

//template <typename EditT>
class ACWND_EXT_CLASS CEditACImp : public CWindowACImp
{
public:
	CEditACImp();
	virtual ~CEditACImp();

	BOOL GetInitInfo(AUTOCINITINFO* pInfo) override;
	BOOL AutoComplete(AUTOCCOMPLETE* pInfo) override;

	BOOL		GetRangeText(CString& strText, EditPosLen nStart, EditPosLen nEnd) const override;
	EditPosLen	GetCaretPos() const override;
	int			LineFromPosition(EditPosLen nPos) const override;
	EditPosLen PositionAfter(EditPosLen nPos) const override;
	EditPosLen GetLineEndPosition(int nLine) const override;
protected:
	inline int GetLineCount() const
	{
		if (m_bIsRichEdit)
			return ((CRichEditCtrl*)m_pEdit)->GetLineCount();
		return ((CEdit*)m_pEdit)->GetLineCount();
	}

	inline void GetSel(int& nStartChar, int& nEndChar) const
	{
		if (m_bIsRichEdit)
		{
			long nStart, nEnd;
			((CRichEditCtrl*)m_pEdit)->GetSel(nStart, nEnd);
			nStartChar = nStart; nEndChar = nEnd;
		}
		else
			((CEdit*)m_pEdit)->GetSel(nStartChar, nEndChar);
	}

	inline void SetSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE)
	{
		if (m_bIsRichEdit)
			((CRichEditCtrl*)m_pEdit)->SetSel(nStartChar, nEndChar);
		else
			((CEdit*)m_pEdit)->SetSel(nStartChar, nEndChar);
	}

	inline void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE)
	{
		if (m_bIsRichEdit)
			((CRichEditCtrl*)m_pEdit)->ReplaceSel(lpszNewText, bCanUndo);
		else
			((CEdit*)m_pEdit)->ReplaceSel(lpszNewText, bCanUndo);
	}

	inline CPoint PosFromChar(UINT nChar) const
	{
		if (m_bIsRichEdit)
			return ((CRichEditCtrl*)m_pEdit)->PosFromChar(nChar);
		return ((CEdit*)m_pEdit)->PosFromChar(nChar);
	}

	inline int LineFromChar(int nIndex = -1) const
	{
		if (m_bIsRichEdit)
			return (int)((CRichEditCtrl*)m_pEdit)->LineFromChar(nIndex);
		return ((CEdit*)m_pEdit)->LineFromChar(nIndex);
	}

	inline int LineIndex(int nLine = -1) const
	{
		if (m_bIsRichEdit)
			return ((CRichEditCtrl*)m_pEdit)->LineIndex(nLine);
		return ((CEdit*)m_pEdit)->LineIndex(nLine);
	}

	inline int LineLength(int nLine = -1) const
	{
		if (m_bIsRichEdit)
			return ((CRichEditCtrl*)m_pEdit)->LineLength(nLine);
		return ((CEdit*)m_pEdit)->LineLength(nLine);
	}

	inline int GetLine(int nIndex, LPTSTR lpszBuffer) const
	{
		if (m_bIsRichEdit)
			return ((CRichEditCtrl*)m_pEdit)->GetLine(nIndex, lpszBuffer);
		return ((CEdit*)m_pEdit)->GetLine(nIndex, lpszBuffer);
	}

	void GetLineText(int nLineIndex, CString& strLine, int nLineLen) const;
public:
	CWnd*	m_pEdit;
	bool	m_bIsRichEdit;
};

#ifdef _ENABLE_SCINTILLA_BUILD
class CScintillaCtrl;

class ACWND_EXT_CLASS CScintillaACImp : public CWindowACImp
{
public:
	CScintillaACImp(CScintillaCtrl* pCtrl = nullptr);
	virtual ~CScintillaACImp();

	BOOL GetInitInfo(AUTOCINITINFO* pInfo) override;
	BOOL AutoComplete(AUTOCCOMPLETE* pInfo) override;

	BOOL		GetRangeText(CString& strText, EditPosLen nStart, EditPosLen nEnd) const override;
	EditPosLen	GetCaretPos() const override;

	EditPosLen PositionBefore(EditPosLen nPos) const override;
	EditPosLen PositionAfter(EditPosLen nPos) const override;
	int			LineFromPosition(EditPosLen nPos) const override;
	EditPosLen GetLineEndPosition(int nLine) const override;
public:
	CScintillaCtrl*	m_pEdit;
	CFont			m_font;
};
#endif // _ENABLE_SCINTILLA_BUILD


// CAutoCompleteWnd
class CAutoCompleteListCtrl;

typedef CSyncPopupWnd	CAutoCompleteWndBase;

class ACWND_EXT_CLASS CAutoCompleteWnd : public CAutoCompleteWndBase
{
	DECLARE_DYNAMIC(CAutoCompleteWnd)
public:
	BOOL Create(CWnd* pOwner, const AUTOCINITINFO& info);

	BOOL IsActiveOwner(CWnd* pWnd);

	void SetItemCount(int nItems);
	int GetItemCount() const;

	int GetVisibleItems() const;
	int GetTopIndex() const;

	int GetCurSel() const;
	void SetCurSel(int nItem);

	BOOL GetItemRect(int nItem, LPRECT rect);

	int MoveSelection(int nDelta);

	void DoAutoCompletion();

	void SetImageList(CImageList* pImageList);

	void Close() override;

	CSize GetIconSize() const;
public:
	static CAutoCompleteWnd*	GetActiveInstance();
	static BOOL					Activate(CWnd* pOwner, UINT nChar);
	static BOOL					Show(CWnd* pOwner);
	static BOOL					Cancel();
protected:
	virtual CAutoCompleteListCtrl* CreateListCtrl();

	void PrepareNotifyHeader(AUTOCNMHDR* hdr);

	LRESULT NotifyOwner(ACCmd cmd, AUTOCNMHDR* pHdr) const;
	static LRESULT NotifyOwnerImpl(CWnd* pWndOwner, ACCmd cmd, AUTOCNMHDR* pHdr);

	virtual BOOL OnKey(UINT nKey);

	BOOL NotifyKey(UINT nKey);

	void UpdateListItemCount(int nItemCount);

	void RecalcSizeToFitList();

	int GetItemHeight();

	BOOL ProcessMouseClick(UINT uiMsg, POINT pt, HWND hwnd) override;

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	BOOL DrawItemIcon(CDC* pDC, int nRow, CRect rect);
	void DrawItemText(CDC* pDC, int nRow, UINT nState, CRect& rect, BOOL bCalcOnly = FALSE);

	BOOL HandleKeyUpdateTransparency();
	void UpdateTransparency();

	void KillToolTipTimer();
	void NotifySelChange(int nItem);
	void ShowToolTip();
protected:
	CAutoCompleteWnd();
	virtual ~CAutoCompleteWnd();
protected:
	static CAutoCompleteWnd*	s_pInstance;
	CAutoCompleteListCtrl*		m_listCtrl;
	AUTOCINITINFO				m_infoInit;
	CSize						m_szIcon;
	int							m_nVisibleItems;
	int							m_nMaxItemWidth;

	BYTE						m_nAlpha;
	bool						m_bIncreaseAlpha;
	UINT_PTR					m_nAlphaTimer;

	CFont*						m_pFont;
	int							m_nItemHeight;

	friend class CAutoCTooltipCtrl;
	CAutoCTooltipCtrl*			m_pToolTipCtrl;
	UINT_PTR					m_nToolTipTimer;
	bool						m_bUpdateToolTipOnScroll;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnListGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListItemChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListEndScroll(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg void OnDestroy();

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
protected:
	DECLARE_MESSAGE_MAP()
};


