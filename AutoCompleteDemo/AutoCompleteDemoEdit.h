#pragma once
#include "AutoCompleteWnd.h"
#include "AutoCompleteDemo.h"

struct AUTOCNMHDR;

template <typename ACImpBaseT>
class CDemoACImp : public ACImpBaseT
{
public:
	BOOL GetInitInfo(AUTOCINITINFO* pInfo) override;

	BOOL IsValidChar(UINT nChar) const override;

	int GetTotalItemCount() const override;

	int GetItemIconIndex(int nItem) const override;

	LPCTSTR GetItemDisplayText(int nItem) const override;

	int UpdateFilteredList(LPCTSTR pszFilterText) override;
};

template <typename ACImpBaseT>
BOOL CDemoACImp<ACImpBaseT>::GetInitInfo(AUTOCINITINFO* pInfo)
{
	if (!ACImpBaseT::GetInitInfo(pInfo))
		return FALSE;
	pInfo->pImageList = &theApp.m_imgList;
	return TRUE;
}

template <typename ACImpBaseT>
BOOL CDemoACImp<ACImpBaseT>::IsValidChar(UINT nChar) const
{
	if ( ACImpBaseT::IsValidChar(nChar) )
		return TRUE;
	switch (nChar)
	{
	case _T('<'):
	case _T('>'):
	case _T(':'):
	case _T('"'):
	case _T('/'):
	case _T('\\'):
	case _T('|'):
	case _T('?'):
	case _T('*'):
		return FALSE;
	}
	return TRUE;
}

template <typename ACImpBaseT>
int CDemoACImp<ACImpBaseT>::GetItemIconIndex(int nItem) const
{
	return (nItem % theApp.m_imgList.GetImageCount());
}

template <typename ACImpBaseT>
int CDemoACImp<ACImpBaseT>::GetTotalItemCount() const
{
	return (int)theApp.m_saTestList.GetCount();
}

template <typename ACImpBaseT>
LPCTSTR CDemoACImp<ACImpBaseT>::GetItemDisplayText(int nItem) const
{
	return (LPCTSTR)theApp.m_saTestList[nItem];
}

template <typename ACImpBaseT>
int CDemoACImp<ACImpBaseT>::UpdateFilteredList(LPCTSTR pszFilterText)
{
	m_bFuzzyMatch = theApp.m_bFuzzyMatch;
	m_bMatchCase = theApp.m_bMatchCase;
	return ACImpBaseT::UpdateFilteredList(pszFilterText);
}

typedef CDemoACImp<CEditACImp>		CDemoEditACImp;
typedef CDemoACImp<CScintillaACImp>	CDemoScintillaACImp;


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
