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
	int			nListItems;		// the number of items in the list
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

	virtual BOOL GetInitInfo(AUTOCINITINFO* pInfo) const = 0;
	virtual BOOL IsValidChar(UINT nChar) const;

	virtual BOOL HandleKey(AUTOCKEYINFO* pInfo, CString& strText) const;
	virtual BOOL AutoComplete(AUTOCCOMPLETE* pInfo) const = 0;

	virtual BOOL GetRangeText(CString& strText, EditPosLen nStart, EditPosLen nEnd) const = 0;
	virtual EditPosLen GetCaretPos() const = 0;
};

class CEditACImp : public CWindowACImp
{
public:
	CEditACImp();
	virtual ~CEditACImp();

	virtual BOOL GetInitInfo(AUTOCINITINFO* pInfo) const;
	BOOL AutoComplete(AUTOCCOMPLETE* pInfo) const override;

	BOOL		GetRangeText(CString& strText, EditPosLen nStart, EditPosLen nEnd) const override;
	EditPosLen	GetCaretPos() const override;
public:
	CEdit*	m_pEdit;
};


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
	afx_msg void OnItemChangeList(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
protected:
	DECLARE_MESSAGE_MAP()
};


