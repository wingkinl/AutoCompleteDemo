#pragma once

#include "SyncPopupWnd.h"

enum ACCmd
{
	ACCmdGetInitPos		= 0,

	ACCmdCustom			= 1024,
};

#define WM_AC_NOTIFY	(WM_APP+0xac)

struct AUTOCNMHDR
{
	WPARAM	wp;
	LPARAM	lp;
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

	static CAutoCompleteWnd*	GetActiveInstance();
	static BOOL					Activate(CWnd* pOwner);
	static BOOL					Cancel();
protected:
	BOOL CreateListCtrl();

	LRESULT NotifyOwner(ACCmd cmd, WPARAM wp = 0, LPARAM lp = 0);

	virtual BOOL GetInitPosition(CWnd* pOwner, POINT& pos) const;
private:
	BOOL GetInitPositionFromEdit(CWnd* pOwner, POINT& pos) const;
	BOOL GetInitPositionFromScintilla(CWnd* pOwner, POINT& pos) const;
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
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:
	DECLARE_MESSAGE_MAP()
};


