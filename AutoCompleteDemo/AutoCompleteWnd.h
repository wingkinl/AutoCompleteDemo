#pragma once

class CEditACImp
{
public:
	CEditACImp(CEdit* pEdit = nullptr);
	virtual ~CEditACImp();

	virtual BOOL GetInitInfo(POINT& pos, CString& strWordBegin) const;
	virtual BOOL IsValidChar(UINT nChar) const;
public:
	CEdit*	m_pEdit;
};

#include "SyncPopupWnd.h"

enum ACCmd
{
	ACCmdGetInitInfo		= 0,
	ACCmdUpdateList,

	ACCmdCustom			= 1024,
};

#define WM_AC_NOTIFY	(WM_APP+0xac)

struct AUTOCNMHDR
{
	WPARAM	wp;
	LPARAM	lp;
};

struct AUTOCINITINFO 
{
	POINT		posScreen;	// the initial position of the auto complete window
	int			nListItems;	// the number of items in the list
	CImageList*	pImageList;
};

typedef NMLVDISPINFO	NMACDISPINFO;

// CAutoCompleteWnd
class CAutoCompleteListCtrl;

typedef CSyncPopupWnd	CAutoCompleteWndBase;

class CAutoCompleteWnd : public CAutoCompleteWndBase
{
	DECLARE_DYNAMIC(CAutoCompleteWnd)
public:
	BOOL Create(CWnd* pOwner, POINT pos) override;

	BOOL IsActiveOwner(CWnd* pWnd);

	void SetItemCount(int nItems);
	int GetItemCount() const;

	void SetImageList(CImageList* pImageList);

	void Close() override;
public:
	static CAutoCompleteWnd*	GetActiveInstance();
	static BOOL					Activate(CWnd* pOwner, UINT nChar);
	static BOOL					Cancel();
protected:
	virtual CAutoCompleteListCtrl* CreateListCtrl();

	LRESULT NotifyOwner(ACCmd cmd, WPARAM wp = 0, LPARAM lp = 0);
	static LRESULT NotifyOwner(CWnd* pWndOwner, ACCmd cmd, WPARAM wp = 0, LPARAM lp = 0);

	virtual BOOL OnKey(UINT nChar);

	void UpdateList(UINT nChar);
protected:
	CAutoCompleteWnd();
	virtual ~CAutoCompleteWnd();
protected:
	static CAutoCompleteWnd*	s_pInstance;
	CAutoCompleteListCtrl*		m_listCtrl;
	bool						m_bDropRestOfWord;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
protected:
	DECLARE_MESSAGE_MAP()
};


