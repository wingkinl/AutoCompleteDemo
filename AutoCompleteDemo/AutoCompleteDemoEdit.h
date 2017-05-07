#pragma once
#include "AutoCompleteWnd.h"

struct AUTOCNMHDR;

class CDemoEditACImp : public CEditACImp
{
public:
	BOOL IsValidChar(UINT nChar) const override;

	int GetTotalItemCount() const override;

	LPCTSTR GetItemDisplayText(int nItem) const override;

	int UpdateFilteredList(LPCTSTR pszFilterText) override;
};

class CDemoACEdit : public CEdit
{
	DECLARE_DYNCREATE(CDemoACEdit)
public:
	CDemoACEdit();
	~CDemoACEdit();
public:
	bool m_bAutoDelete;
protected:
	void PostNcDestroy() override;
protected:
	CDemoEditACImp	m_acImp;
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnACNotify(WPARAM wp, LPARAM lp);
protected:
	DECLARE_MESSAGE_MAP()
};
