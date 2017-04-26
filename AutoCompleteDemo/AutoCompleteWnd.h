#pragma once

// CAutoCompleteWnd
class CAutoCompleteListCtrl;

typedef CMiniFrameWnd	CAutoCompleteWndBase;

class CAutoCompleteWnd : public CAutoCompleteWndBase
{
	DECLARE_DYNAMIC(CAutoCompleteWnd)

public:
	CAutoCompleteWnd();
	virtual ~CAutoCompleteWnd();
public:
	BOOL Create(CWnd* pOwner, POINT pos);
protected:
	BOOL CreateListCtrl();
protected:
	CAutoCompleteListCtrl*	m_listCtrl;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


