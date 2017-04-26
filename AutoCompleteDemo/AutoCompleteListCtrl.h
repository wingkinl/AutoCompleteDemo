#pragma once


// CAutoCompleteListCtrl

typedef CMFCListCtrl	CAutoCompleteListCtrlBase;

class CAutoCompleteListCtrl : public CAutoCompleteListCtrlBase
{
	DECLARE_DYNAMIC(CAutoCompleteListCtrl)

public:
	CAutoCompleteListCtrl();
	virtual ~CAutoCompleteListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
};


