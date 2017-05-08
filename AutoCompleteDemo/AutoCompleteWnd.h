#pragma once

#include "SyncPopupWnd.h"

enum ACCmd
{
	ACCmdGetInitInfo,		// return non-zero to show the window
	ACCmdGetListDispInfo,
	ACCmdSelChange,
	ACCmdKey,
	ACCmdComplete,

	ACCmdCustom			= 1024,
};

#define WM_AC_NOTIFY	(WM_APP+0xac)

struct AUTOCNMHDR
{
	HWND	hwndFrom;
	WPARAM	wp;
	LPARAM	lp;
};

typedef int		EditPosLen;

struct AUTOCINITINFO 
{
	AUTOCNMHDR	hdr;
	POINT		posACWndScreen;	// the initial position of the auto complete window
	int			nItemCount;		// the number of items in the list
	int			nPreSelectItem;	// the item to be selected when display
	int			nMaxVisibleItems;
	EditPosLen	nPosStartChar;	// the position of the first character in editor
	EditPosLen	nStartStrLen;	// the pre-entered length of characters
	CString		strStart;
	BOOL		bDropRestOfWord;
	CImageList*	pImageList;		// the image list for displaying icon
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
	int		nImaage;
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
	UINT		nKey;
	UINT		nChar;

	int			nItemCount;	// negative value means don't update list
	int			nPreSelectItem;	// the item to be selected when display
	BOOL		bEatKey;	// eat the char
	BOOL		bClose;		// whether to close the window
};

struct AUTOCCOMPLETE 
{
	AUTOCNMHDR	hdr;
	EditPosLen	nPosStartChar;
	BOOL		bDropRestOfWord;
	int			nItem;
	CString		strText;
};

class CWindowACImp
{
public:
	CWindowACImp();
	virtual ~CWindowACImp();

	virtual BOOL GetInitInfo(AUTOCINITINFO* pInfo) = 0;
	virtual BOOL IsValidChar(UINT nChar) const;

	virtual BOOL HandleKey(AUTOCKEYINFO* pInfo, CString& strText);
	virtual BOOL AutoComplete(AUTOCCOMPLETE* pInfo) = 0;

	virtual BOOL GetRangeText(CString& strText, EditPosLen nStart, EditPosLen nEnd) const = 0;
	virtual EditPosLen GetCaretPos() const = 0;

	virtual LRESULT OnACNotify(WPARAM wp, LPARAM lp);

	virtual int GetTotalItemCount() const = 0;

	virtual LPCTSTR GetItemDisplayText(int nItem) const;
	virtual int GetItemIconIndex(int nItem) const;

	virtual BOOL GetDisplayInfo(AUTOCNMHDR* nmhdr) const;

	virtual int UpdateFilteredList(LPCTSTR pszFilterText);
protected:
	CArray<int>		m_arrFilteredIndices;
public:
	bool			m_bMatchCase;
	bool			m_bFuzzyMatch;
};

//template <typename EditT>
class CEditACImp : public CWindowACImp
{
public:
	CEditACImp();
	virtual ~CEditACImp();

	BOOL GetInitInfo(AUTOCINITINFO* pInfo) override;
	BOOL AutoComplete(AUTOCCOMPLETE* pInfo) override;

	BOOL		GetRangeText(CString& strText, EditPosLen nStart, EditPosLen nEnd) const override;
	EditPosLen	GetCaretPos() const override;
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

class CScintillaACImp : public CWindowACImp
{
public:
	CScintillaACImp(CScintillaCtrl* pCtrl = nullptr);
	virtual ~CScintillaACImp();

	BOOL GetInitInfo(AUTOCINITINFO* pInfo) override;
	BOOL AutoComplete(AUTOCCOMPLETE* pInfo) override;

	BOOL		GetRangeText(CString& strText, EditPosLen nStart, EditPosLen nEnd) const override;
	EditPosLen	GetCaretPos() const override;
public:
	CScintillaCtrl*	m_pCtrl;
};
#endif // _ENABLE_SCINTILLA_BUILD


// CAutoCompleteWnd
class CAutoCompleteListCtrl;

typedef CSyncPopupWnd	CAutoCompleteWndBase;

class CAutoCompleteWnd : public CAutoCompleteWndBase
{
	DECLARE_DYNAMIC(CAutoCompleteWnd)
public:
	BOOL Create(CWnd* pOwner, const AUTOCINITINFO& info);

	BOOL IsActiveOwner(CWnd* pWnd);

	void SetItemCount(int nItems);
	int GetItemCount() const;

	int GetVisibleItems() const;
	int GetTopIndex() const;

	int MoveSelection(int nDelta);

	void DoAutoCompletion();

	void SetImageList(CImageList* pImageList);

	void Close() override;
public:
	static CAutoCompleteWnd*	GetActiveInstance();
	static BOOL					Activate(CWnd* pOwner, UINT nChar);
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

	void CustomDrawListImpl(CDC* pDC, LPNMLVCUSTOMDRAW plvcd);
	void OnDrawLabel(CDC* pDC, LPNMLVCUSTOMDRAW plvcd, UINT nState, CRect& rect, BOOL bCalcOnly = FALSE);

	BOOL HandleKeyUpdateTransparency();
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
	bool						m_bReady;

	BYTE						m_nAlpha;
	bool						m_bIncreaseAlpha;
	UINT_PTR					m_nAlphaTiimer;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetListDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListItemChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListDblClk(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
protected:
	DECLARE_MESSAGE_MAP()
};


