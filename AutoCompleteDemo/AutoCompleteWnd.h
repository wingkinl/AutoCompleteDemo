#pragma once

// CAutoCompleteWnd

typedef CMFCListCtrl	CAutoCompleteWndBase;

class CAutoCompleteWnd : public CAutoCompleteWndBase
{
	DECLARE_DYNAMIC(CAutoCompleteWnd)

public:
	CAutoCompleteWnd();
	virtual ~CAutoCompleteWnd();
public:
	BOOL Create(CWnd* pOwner, POINT pos);
protected:
protected:
	DECLARE_MESSAGE_MAP()
};


