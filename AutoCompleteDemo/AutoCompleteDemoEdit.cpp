#include "stdafx.h"
#include "AutoCompleteDemoEdit.h"
#include "AutoCompleteWnd.h"
#include "AutoCompleteDemo.h"

CDemoACEdit::CDemoACEdit()
{
	m_bAutoDelete = false;
}

CDemoACEdit::~CDemoACEdit()
{

}

void CDemoACEdit::PostNcDestroy()
{
	if (m_bAutoDelete)
		delete this;
}

IMPLEMENT_DYNCREATE(CDemoACEdit, CEdit)

BEGIN_MESSAGE_MAP(CDemoACEdit, CEdit)
	ON_WM_CHAR()
	ON_MESSAGE(WM_AC_NOTIFY, &CDemoACEdit::OnACNotify)
END_MESSAGE_MAP()

void CDemoACEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	CAutoCompleteWnd::Activate(this, nChar);
}

class CDemoEditACImp : public CEditACImp
{
public:
	BOOL IsValidChar(UINT nChar) const override;
};

BOOL CDemoEditACImp::IsValidChar(UINT nChar) const
{
	if (!CEditACImp::IsValidChar(nChar))
	{
		switch (nChar)
		{
		case _T('_'):
		case _T('$'):
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CDemoACEdit::GetACInitInfo(AUTOCNMHDR* nmhdr)
{
	AUTOCINITINFO* pInfo = (AUTOCINITINFO*)nmhdr;

	CDemoEditACImp acImp;
	acImp.m_pEdit = (CEdit*)this;
	BOOL bRet = acImp.GetInitInfo(pInfo);
	if (bRet)
	{
		pInfo->nItemCount = UpdateFilteredList((LPCTSTR)pInfo->strStart);
	}
	return bRet;
}

BOOL CDemoACEdit::GetACDisplayInfo(AUTOCNMHDR* nmhdr) const
{
	AUTOCDISPINFO* pInfo = (AUTOCDISPINFO*)nmhdr;
	AUTOCITEM& item = pInfo->item;
	if (item.mask & AUTOCITEM::ACIF_TEXT)
	{
		int nItem = item.nItem;
		if (nItem < m_arrFilteredIndices.GetSize())
			nItem = m_arrFilteredIndices[nItem];
		_tcsncpy(item.pszText, (LPCTSTR)theApp.m_saTestList[nItem], item.cchTextMax);
	}
	if (item.mask & AUTOCITEM::ACIF_IMAGE)
	{
		item.nImaage = -1;
	}
	return TRUE;
}

BOOL CDemoACEdit::HandleKey(AUTOCNMHDR* nmhdr)
{
	AUTOCKEYINFO* pInfo = (AUTOCKEYINFO*)nmhdr;
	CDemoEditACImp acImp;
	acImp.m_pEdit = (CEdit*)this;

	CString strText;
	BOOL bIsValid = acImp.HandleKey(pInfo, strText);
	if (bIsValid)
	{
		pInfo->nItemCount = UpdateFilteredList((LPCTSTR)strText);
		pInfo->bClose = FALSE;
	}
	if (!bIsValid)
	{
		pInfo->bClose = TRUE;
	}
	pInfo->bEatKey = FALSE;
	return TRUE;
}

BOOL CDemoACEdit::AutoComplete(AUTOCNMHDR* nmhdr)
{
	AUTOCCOMPLETE* pInfo = (AUTOCCOMPLETE*)nmhdr;
	CDemoEditACImp acImp;
	acImp.m_pEdit = (CEdit*)this;
	acImp.AutoComplete(pInfo);
	return TRUE;
}

int CDemoACEdit::UpdateFilteredList(LPCTSTR pszText)
{
	m_arrFilteredIndices.RemoveAll();
	int nItemCount = (int)theApp.m_saTestList.GetCount();
	if (pszText && *pszText)
	{
		int nTextLen = (int)_tcslen(pszText);
		for (int ii = 0; ii < nItemCount; ++ii)
		{
			CString strItemText = theApp.m_saTestList[ii];
			LPCTSTR pszItemText = (LPCTSTR)strItemText;
			for (int nChar = 0; nChar < nTextLen; ++nChar)
			{
				pszItemText = StrChrI(pszItemText, pszText[nChar]);
				if (!pszItemText)
					break;
				if (pszItemText++ >= pszItemText + strItemText.GetLength() - 1)
				{
					pszItemText = nullptr;
					break;
				}
			}
			if (pszItemText)
			{
				m_arrFilteredIndices.Add(ii);
			}
		}
	}
	return (int)m_arrFilteredIndices.GetSize();
}

LRESULT CDemoACEdit::OnACNotify(WPARAM wp, LPARAM lp)
{
	ACCmd cmd = (ACCmd)wp;
	AUTOCNMHDR* nmhdr = (AUTOCNMHDR*)lp;

	switch (cmd)
	{
	case ACCmdGetInitInfo:
		return (LRESULT)GetACInitInfo(nmhdr);
	case ACCmdGetListDispInfo:
		return (LRESULT)GetACDisplayInfo(nmhdr);
	case ACCmdKey:
		return (LRESULT)HandleKey(nmhdr);
	case ACCmdComplete:
		return (LRESULT)AutoComplete(nmhdr);
	}
	return 0;
}
