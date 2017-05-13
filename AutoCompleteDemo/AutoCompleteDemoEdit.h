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

	LPCTSTR GetItemDisplayText(int nItem) const override;

	int UpdateFilteredList(LPCTSTR pszFilterText, int& nPreSelIndex) override;

	BOOL HandleSelChange(AUTOCSELCHANGEINFO* pSelChangeInfo) override;
};

template <typename ACImpBaseT>
BOOL CDemoACImp<ACImpBaseT>::GetInitInfo(AUTOCINITINFO* pInfo)
{
	if (!ACImpBaseT::GetInitInfo(pInfo))
		return FALSE;
	pInfo->pImageList = &theApp.m_imgList;
	pInfo->stToolTipInfo.m_bEnable = TRUE;
	return TRUE;
}

template <typename ACImpBaseT>
BOOL CDemoACImp<ACImpBaseT>::IsValidChar(UINT nChar) const
{
	if ( ACImpBaseT::IsValidChar(nChar) )
		return TRUE;
	if (nChar == 0 || nChar > 0xFF)
		return FALSE;
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
	case _T(' '):
	case _T('\t'):
		return FALSE;
	}
	return TRUE;
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
int CDemoACImp<ACImpBaseT>::UpdateFilteredList(LPCTSTR pszFilterText, int& nPreSelIndex)
{
	m_bFuzzyMatch = theApp.m_bFuzzyMatch;
	m_bMatchCase = theApp.m_bMatchCase;
	if ((int)_tcslen(pszFilterText) > theApp.m_nMaxTextLen)
		return 0;
	return ACImpBaseT::UpdateFilteredList(pszFilterText, nPreSelIndex);
}

template <typename ACImpBaseT>
BOOL CDemoACImp<ACImpBaseT>::HandleSelChange(AUTOCSELCHANGEINFO* pSelChangeInfo)
{
	pSelChangeInfo->strToolTipLabel = GetItemDisplayText(GetMappedIndex(pSelChangeInfo->nItem));
	CString strFilePath = theApp.m_strPath + _T("\\") + pSelChangeInfo->strToolTipLabel;
	pSelChangeInfo->strToolTipDescription = strFilePath;
	return TRUE;
}

typedef CDemoACImp<CEditACImp>		CDemoEditACImp;
#ifdef _ENABLE_SCINTILLA_BUILD
typedef CDemoACImp<CScintillaACImp>	CDemoScintillaACImp;
#endif // _ENABLE_SCINTILLA_BUILD


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
