// AutoCompleteWnd.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteWnd.h"
#include "AutoCompleteListCtrl.h"

#define AC_DEFAULT_MAX_VISIBLE_ITEM		10

#define AC_MIN_ALPHA					30
#define AC_ALPHA_DELTA					45
#define AC_TIMER_ALPHA_ID				0xabcd
#define AC_TIMER_ALPHA_ELAPSE			25

/************************************************************************/
/* CWindowACImp
/************************************************************************/
CWindowACImp::CWindowACImp()
{
	m_bMatchCase = false;
	m_bFuzzyMatch = true;
	m_bAllowCaretMove = true;
	m_bUpdateListAfterCaretMove = false;
}

CWindowACImp::~CWindowACImp()
{
	
}

BOOL CWindowACImp::IsValidChar(UINT nChar) const
{
	return 0 < nChar && nChar <= 0xFF && _istalnum(nChar);
}

BOOL CWindowACImp::HandleKey(AUTOCKEYINFO* pInfo)
{
	CString strText;
	BOOL bIsValid = TRUE;
	BOOL bAppendChar = FALSE;
	BOOL bUpdateList = TRUE;
	BOOL bCheckText = TRUE;
	int nMoveCaret = 0;
	auto nCurPos = GetCaretPos();
	switch (pInfo->nKey)
	{
	case VK_DELETE:
		bCheckText = FALSE;
		bUpdateList = FALSE;
		break;
	case VK_BACK:
		{
			if (nCurPos == pInfo->nPosStartChar + 1)
			{
				pInfo->bClose = TRUE;
				bIsValid = FALSE;
			}
		}
		break;
	case VK_HOME:
	case VK_END:
		bIsValid = FALSE;
		break;
	case VK_LEFT:
	case VK_RIGHT:
		bIsValid = m_bAllowCaretMove && !(GetKeyState(VK_CONTROL) < 0);
		if (bIsValid)
		{
			nMoveCaret = pInfo->nKey == VK_RIGHT ? 1 : -1;
			EditPosLen nNewPos = nCurPos;
			if (pInfo->nKey == VK_RIGHT)
			{
				nNewPos = PositionAfter(nCurPos);
				auto nLineEnd = GetLineEndPosition(pInfo->nStartLine);
				// if already the end of document, then no update
				if (nNewPos == nCurPos)
				{
					nMoveCaret = 0;
					bCheckText = FALSE;
				}
				else if (nNewPos > nLineEnd)
					bIsValid = FALSE;
			}
			else
			{
				nNewPos = PositionBefore(nCurPos);
				if (nNewPos < pInfo->nPosStartChar)
					bIsValid = FALSE;
			}
			nCurPos = nNewPos;
			bUpdateList = m_bUpdateListAfterCaretMove;
		}
		break;
	default:
		bIsValid = IsValidChar(pInfo->nChar);
		bAppendChar = TRUE;
		break;
	}
	if (bIsValid)
	{
		if (bCheckText && pInfo->nPosStartChar < nCurPos)
			GetRangeText(strText, pInfo->nPosStartChar, nCurPos);
		if (nMoveCaret > 0)
		{
			int nLen = strText.GetLength();
			for (int nn = nLen-nMoveCaret; nn >= 0; --nn)
			{
				if (!IsValidChar(strText[nn]))
				{
					bIsValid = FALSE;
					break;
				}
			}
		}
		if (pInfo->nKey == VK_BACK)
			strText.Delete(strText.GetLength()-1);
		else if (bAppendChar)
			strText.AppendChar(pInfo->nChar);
	}
	if (bIsValid)
	{
		pInfo->nItemCount = bUpdateList ? UpdateFilteredList((LPCTSTR)strText) : -1;
		pInfo->bClose = FALSE;
	}
	if (!bIsValid)
	{
		pInfo->bClose = TRUE;
	}
	pInfo->bEatKey = FALSE;

	return bIsValid;
}

LRESULT CWindowACImp::OnACNotify(WPARAM wp, LPARAM lp)
{
	ACCmd cmd = (ACCmd)wp;
	AUTOCNMHDR* nmhdr = (AUTOCNMHDR*)lp;

	switch (cmd)
	{
	case ACCmdGetInitInfo:
		{
			AUTOCINITINFO* pInfo = (AUTOCINITINFO*)nmhdr;
			BOOL bRet = GetInitInfo(pInfo);
			if (bRet)
			{
				pInfo->nItemCount = UpdateFilteredList((LPCTSTR)pInfo->strStart);
			}
			return bRet;
		}
	case ACCmdGetListDispInfo:
		return (LRESULT)GetDisplayInfo(nmhdr);
	case ACCmdKey:
		{
			AUTOCKEYINFO* pInfo = (AUTOCKEYINFO*)nmhdr;
			HandleKey(pInfo);
			return TRUE; // message processed
		}
	case ACCmdComplete:
		return (LRESULT)AutoComplete((AUTOCCOMPLETE*)nmhdr);
	}
	return 0;
}

LPCTSTR CWindowACImp::GetItemDisplayText(int nItem) const
{
	return nullptr;
}

int CWindowACImp::GetItemIconIndex(int nItem) const
{
	return nItem;
}

BOOL CWindowACImp::GetDisplayInfo(AUTOCNMHDR* nmhdr) const
{
	AUTOCDISPINFO* pInfo = (AUTOCDISPINFO*)nmhdr;
	AUTOCITEM& item = pInfo->item;
	int nMappedItem = item.nItem;
	if (nMappedItem < m_arrFilteredIndices.GetSize())
		nMappedItem = m_arrFilteredIndices[nMappedItem];
	if (item.mask & AUTOCITEM::ACIF_TEXT)
	{
		_tcsncpy(item.pszText, GetItemDisplayText(nMappedItem), item.cchTextMax);
	}
	if (item.mask & AUTOCITEM::ACIF_IMAGE)
	{
		item.nImage = GetItemIconIndex(nMappedItem);
	}
	return TRUE;
}

int CWindowACImp::UpdateFilteredList(LPCTSTR pszFilterText)
{
	m_arrFilteredIndices.RemoveAll();
	int nItemCount = GetTotalItemCount();
	if (pszFilterText && *pszFilterText)
	{
		int nTextLen = (int)_tcslen(pszFilterText);
		for (int ii = 0; ii < nItemCount; ++ii)
		{
			LPCTSTR pszItemText = GetItemDisplayText(ii);
			bool bMatch = false;
			if (m_bFuzzyMatch)
			{
				auto pfnStrChr = m_bMatchCase ? StrChr : StrChrI;
				for (int nChar = 0; nChar < nTextLen; ++nChar)
				{
					pszItemText = pfnStrChr(pszItemText, pszFilterText[nChar]);
					if (!pszItemText)
						break;
					if (*++pszItemText == _T('\0'))
					{
						if (nChar != nTextLen - 1)
							pszItemText = nullptr;
						break;
					}
				}
				bMatch = pszItemText != nullptr;
			}
			else
			{
				auto pfnCmp = m_bMatchCase ? _tcsnccmp : _tcsncicmp;
				bMatch = pfnCmp(pszItemText, pszFilterText, nTextLen) == 0;
			}
			if (bMatch)
			{
				m_arrFilteredIndices.Add(ii);
			}
		}
	}
	return (int)m_arrFilteredIndices.GetSize();
}

EditPosLen CWindowACImp::PositionBefore(EditPosLen nPos) const
{
	if (nPos == 0)
		return 0;
	return --nPos;
}

EditPosLen CWindowACImp::PositionAfter(EditPosLen nPos) const
{
	return ++nPos;
}

/************************************************************************/
/* CEditACImp
/************************************************************************/

CEditACImp::CEditACImp()
{
	m_bIsRichEdit = false;
}

CEditACImp::~CEditACImp()
{

}

BOOL CEditACImp::GetInitInfo(AUTOCINITINFO* pInfo)
{
	int nSelStartChar = 0, nSelEndChar = 0;
	GetSel(nSelStartChar, nSelEndChar);

	int nCurLine = LineFromChar(nSelStartChar);
	int nCurLineStartChar = LineIndex(nCurLine);
	
	ASSERT(nSelStartChar >= nCurLineStartChar);

	int nLineLen = LineLength(nCurLineStartChar);
	CString strLine;
	GetLineText(nCurLine, strLine, nLineLen);
	LPCTSTR pszLine = (LPCTSTR)strLine;

	pInfo->strStart.Empty();

	int nCurCharOffset = nSelStartChar - nCurLineStartChar - 1;
	int nCharPos = nCurCharOffset;
	for (; nCharPos >= 0; --nCharPos)
	{
		if (!IsValidChar(pszLine[nCharPos]))
		{
			break;
		}
		pInfo->strStart.Insert(0, pszLine[nCharPos]);
	}

	pInfo->nPosStartChar = (EditPosLen)(nCurLineStartChar + nCharPos + 1);
	pInfo->nStartStrLen = (EditPosLen)pInfo->strStart.GetLength();

	if (pInfo->nStartStrLen == 0)
		return FALSE;
	pInfo->nStartLine = nCurLine;

	// note: PosFromChar returns top-left corner of a given character
	pInfo->posWordScreen = PosFromChar(pInfo->nPosStartChar);
	m_pEdit->ClientToScreen(&pInfo->posWordScreen);

	if (m_pEdit->GetStyle() & ES_MULTILINE)
	{
		// PosFromChar gives us top-left corner, but we need bottom left!
		int nLineCount = GetLineCount();
		if (nLineCount > 1)
		{
			int nNextPrevLine = nCurLine < nLineCount - 1 ? nCurLine + 1 : nCurLine - 1;
			int nNextPrevLineCharPos = LineIndex(nNextPrevLine);
			CPoint posNextPrevLine = PosFromChar(nNextPrevLineCharPos);
			m_pEdit->ClientToScreen(&posNextPrevLine);
			pInfo->nLineHeight = abs(posNextPrevLine.y - pInfo->posWordScreen.y);
			pInfo->nLineHeight += 2;
		}
		else
		{
			TEXTMETRIC tm;
			CDC* pDC = m_pEdit->GetDC();
			pDC->GetTextMetrics(&tm);
			m_pEdit->ReleaseDC(pDC);
			pInfo->nLineHeight = tm.tmHeight;
		}
	}
	else
	{
		CRect rect;
		m_pEdit->GetWindowRect(rect);
		pInfo->nLineHeight = rect.bottom - pInfo->posWordScreen.y;
	}
	pInfo->posWordScreen.y += pInfo->nLineHeight;
	return !pInfo->strStart.IsEmpty();
}

BOOL CEditACImp::AutoComplete(AUTOCCOMPLETE* pInfo)
{
	int nEndChar = GetCaretPos();
	if (pInfo->bDropRestOfWord)
	{
		int nCurLine = LineFromChar(nEndChar);
		int nCurLineStartChar = LineIndex(nCurLine);

		ASSERT(nEndChar >= nCurLineStartChar);

		int nLineLen = LineLength(nCurLineStartChar);
		CString strLine;
		GetLineText(nCurLine, strLine, nLineLen);
		LPCTSTR pszLine = (LPCTSTR)strLine;

		int nCurCharOffset = nEndChar - nCurLineStartChar;
		for (int nCharPos = nCurCharOffset; nCharPos < nLineLen; ++nCharPos)
		{
			if (!IsValidChar(pszLine[nCharPos]))
			{
				nEndChar = nCurLineStartChar + nCharPos;
				break;
			}
		}
	}
	SetSel(pInfo->nPosStartChar, nEndChar, TRUE);
	ReplaceSel(pInfo->strText);
	return TRUE;
}

BOOL CEditACImp::GetRangeText(CString& strText, EditPosLen nStart, EditPosLen nEnd) const
{
	int nCurLine = LineFromChar(nStart);
	int nCurLineStartChar = LineIndex(nCurLine);

	ASSERT(nStart >= nCurLineStartChar);

	int nLineLen = LineLength(nCurLineStartChar);
	CString strLine;
	GetLineText(nCurLine, strLine, nLineLen);
	strText = strLine.Mid(nStart - nCurLineStartChar, nEnd - nStart);
	return TRUE;
}

EditPosLen CEditACImp::GetCaretPos() const
{
	int nSelStartChar = 0, nSelEndChar = 0;
	GetSel(nSelStartChar, nSelEndChar);
	return (EditPosLen)nSelStartChar;
}

int CEditACImp::LineFromPosition(EditPosLen nPos) const
{
	return LineFromChar(nPos);
}

EditPosLen CEditACImp::PositionAfter(EditPosLen nPos) const
{
	int nLen = m_pEdit->GetWindowTextLength();
	if (nPos <= nLen-1)
		++nPos;
	return nPos;
}

EditPosLen CEditACImp::GetLineEndPosition(int nLine) const
{
	int nCurLineStartChar = LineIndex(nLine);
	int nLineLen = LineLength(nCurLineStartChar);
	return nCurLineStartChar + nLineLen;
}

void CEditACImp::GetLineText(int nLineIndex, CString& strLine, int nLineLen) const
{
	auto pszLine = strLine.GetBufferSetLength(nLineLen + 1);
	*(LPWORD)pszLine = (WORD)nLineLen;
	GetLine(nLineIndex, (LPTSTR)pszLine);
	strLine.SetAt(nLineLen, _T('\0'));
	strLine.ReleaseBuffer(nLineLen);
}

/************************************************************************/
/* CScintillaACImp
/************************************************************************/
#ifdef _ENABLE_SCINTILLA_BUILD

#include SCINTILLA_CTRL_HEADER

CScintillaACImp::CScintillaACImp(CScintillaCtrl* pCtrl)
	: m_pEdit(pCtrl)
{

}

CScintillaACImp::~CScintillaACImp()
{

}

BOOL CScintillaACImp::GetInitInfo(AUTOCINITINFO* pInfo)
{
	auto nCurPos = m_pEdit->GetCurrentPos();
	int nCurLine = m_pEdit->LineFromPosition(nCurPos);
	auto nLineStartCharPos = m_pEdit->PositionFromLine(nCurLine);
	pInfo->strStart.Empty();
	pInfo->nPosStartChar = m_pEdit->PositionBefore(nCurPos);
	auto nCharPos = pInfo->nPosStartChar;
	while( nCharPos >= nLineStartCharPos )
	{
		auto nChar = m_pEdit->GetCharAt(nCharPos);
		if ( !IsValidChar((UINT)nChar) )
			break;
		pInfo->strStart.Insert(0, nChar);
		pInfo->nPosStartChar = nCharPos;
		if (nCharPos <= 0)
			break;
		nCharPos = m_pEdit->PositionBefore(nCharPos);
	}

	pInfo->nStartStrLen = (EditPosLen)pInfo->strStart.GetLength();

	if (pInfo->nStartStrLen == 0)
		return FALSE;
	pInfo->nStartLine = nCurLine;
	pInfo->posWordScreen.x = m_pEdit->PointXFromPosition(pInfo->nPosStartChar);
	pInfo->posWordScreen.y = m_pEdit->PointYFromPosition(pInfo->nPosStartChar);
	m_pEdit->ClientToScreen(&pInfo->posWordScreen);
	pInfo->nLineHeight = m_pEdit->TextHeight(nCurLine);
	pInfo->posWordScreen.y += pInfo->nLineHeight + 1;
	return TRUE;
}

BOOL CScintillaACImp::AutoComplete(AUTOCCOMPLETE* pInfo)
{
	int nEndCharPos = GetCaretPos();
	if (pInfo->bDropRestOfWord)
	{
		int nCurLine = m_pEdit->LineFromPosition(nEndCharPos);
		auto nLineEndPos = m_pEdit->GetLineEndPosition(nCurLine);
		auto nCharPos = nEndCharPos;
		auto nDocEnd = m_pEdit->GetLength();
		while (nCharPos < nLineEndPos)
		{
			auto nChar = m_pEdit->GetCharAt(nCharPos);
			if (!IsValidChar((UINT)nChar) || nCharPos >= nDocEnd)
			{
				nEndCharPos = nCharPos;
				break;
			}
			nCharPos = m_pEdit->PositionAfter(nCharPos);
		}
	}
	m_pEdit->SetSel(pInfo->nPosStartChar, nEndCharPos);
	m_pEdit->ReplaceSel(pInfo->strText);
	return TRUE;
}

BOOL CScintillaACImp::GetRangeText(CString& strText, EditPosLen nStart, EditPosLen nEnd) const
{
	Sci_TextRange tr;
	tr.chrg.cpMin = nStart;
	tr.chrg.cpMax = nEnd;
	char* pszBuffer = new char[nEnd-nStart+1];
	pszBuffer[0] = '\0';
	tr.lpstrText = pszBuffer;
	m_pEdit->GetTextRange(&tr);
	if (m_pEdit->GetCodePage() == SC_CP_UTF8)
	{
#ifdef _UNICODE
		strText = m_pEdit->UTF82W(pszBuffer, -1);
#else
		CStringW strW = m_pEdit->UTF82W(pszBuffer, -1);
		auto pszText = (LPCWSTR)strW;
		int nLength = -1;
		int nMBCSLength = WideCharToMultiByte(CP_ACP, 0, pszText, nLength, nullptr, 0, nullptr, nullptr);

		//If the calculated length is zero, then ensure we have at least room for a null terminator
		if (nMBCSLength == 0)
			nMBCSLength = 1;

		//Now recall with the buffer to get the converted text
		char* pszUTF8Text = strText.GetBuffer(nMBCSLength + 1); //include an extra byte because we may be null terminating the string ourselves
		int nCharsWritten = WideCharToMultiByte(CP_ACP, 0, pszText, nLength, pszUTF8Text, nMBCSLength, nullptr, nullptr);

		//Ensure we null terminate the text if WideCharToMultiByte doesn't do it for us
		if (nLength != -1)
		{
			AFXASSUME(nCharsWritten <= nMBCSLength);
			pszUTF8Text[nCharsWritten] = '\0';
		}
		strText.ReleaseBuffer();
#endif
	}
	else
	{
#ifdef _UNICODE
		int nChars = MultiByteToWideChar(CP_ACP, 0, pszBuffer, nEnd-nStart, nullptr, 0);
		auto pstr = strText.GetBuffer(nChars+1);
		MultiByteToWideChar(CP_ACP, 0, pszBuffer, nEnd - nStart, pstr, nChars);
		strText.ReleaseBuffer();
#else
		strText = pszBuffer;
#endif // _UNICODE
	}
	delete [] pszBuffer;
	return TRUE;
}

EditPosLen CScintillaACImp::GetCaretPos() const
{
	return (EditPosLen)m_pEdit->GetCurrentPos();
}

EditPosLen CScintillaACImp::PositionBefore(EditPosLen nPos) const
{
	return (EditPosLen)m_pEdit->PositionBefore(nPos);
}

EditPosLen CScintillaACImp::PositionAfter(EditPosLen nPos) const
{
	return (EditPosLen)m_pEdit->PositionAfter(nPos);
}

int CScintillaACImp::LineFromPosition(EditPosLen nPos) const
{
	return m_pEdit->LineFromPosition(nPos);
}

EditPosLen CScintillaACImp::GetLineEndPosition(int nLine) const
{
	return (EditPosLen)m_pEdit->GetLineEndPosition(nLine);
}
#endif // _ENABLE_SCINTILLA_BUILD

/************************************************************************/
/* CAutoCompleteWnd
/************************************************************************/

IMPLEMENT_DYNAMIC(CAutoCompleteWnd, CAutoCompleteWndBase)

CAutoCompleteWnd::CAutoCompleteWnd()
{
	ASSERT(!GetActiveInstance());
	s_pInstance = this;
	m_listCtrl = nullptr;
	m_infoInit.nMaxVisibleItems = AC_DEFAULT_MAX_VISIBLE_ITEM;
	m_nVisibleItems = AC_DEFAULT_MAX_VISIBLE_ITEM;
	m_nMaxItemWidth = 0;
	m_bReady = false;
	m_nAlpha = 255;
	m_bIncreaseAlpha = false;
	m_nAlphaTiimer = 0;

	m_pFont = &GetGlobalData()->fontTooltip;
	m_nItemHeight = -1;
}

CAutoCompleteWnd::~CAutoCompleteWnd()
{
	ASSERT(GetActiveInstance());
	s_pInstance = nullptr;
}

#define IDC_AUTO_LIST_CTRL	1

BEGIN_MESSAGE_MAP(CAutoCompleteWnd, CAutoCompleteWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnGetListDispInfo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnListItemChange)
	ON_NOTIFY(NM_DBLCLK, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnListDblClk)
	ON_WM_TIMER()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

// CAutoCompleteWnd message handlers

BOOL CAutoCompleteWnd::Create(CWnd* pOwner, const AUTOCINITINFO& info)
{
	BOOL bCreated = CAutoCompleteWndBase::Create(pOwner, info.posWordScreen);
	if (!bCreated)
		return FALSE;
	SetOwner(pOwner);
	m_infoInit = info;
	if (info.pImageList)
		SetImageList(info.pImageList);
	UpdateListItemCount(info.nItemCount);
	m_listCtrl->SetCurSel(info.nPreSelectItem < info.nItemCount ?  info.nPreSelectItem : 0);
	m_bReady = true;
	ModifyStyleEx(0, WS_EX_LAYERED);
	return bCreated;
}

void CAutoCompleteWnd::SetItemCount(int nItems)
{
	if (m_listCtrl)
	{
		DWORD dwFlags = LVSICF_NOSCROLL;
		m_listCtrl->SetItemCountEx(nItems, dwFlags);
	}
}

int CAutoCompleteWnd::GetItemCount() const
{
	if (m_listCtrl)
		return m_listCtrl->GetItemCount();
	return -1;
}

int CAutoCompleteWnd::GetVisibleItems() const
{
	return m_nVisibleItems;
}

int CAutoCompleteWnd::GetTopIndex() const
{
	int nTopIndex = -1;
	if (m_listCtrl)
	{
		nTopIndex = m_listCtrl->GetTopIndex();
	}
	// calling SetColumnWidth in response to WM_SIZE might cause GetTopIndex returns negative value!
	ASSERT(nTopIndex >= 0);
	return nTopIndex;
}

int CAutoCompleteWnd::MoveSelection(int nDelta)
{
	if (!m_listCtrl)
		return -1;
	int nTopItemIndex = GetTopIndex();
	int nOldSelItem = m_listCtrl->GetCurSel();
	int nCurSelItem = m_listCtrl->MoveSelection(nDelta);
	if (nCurSelItem < nTopItemIndex || nCurSelItem >= nTopItemIndex + GetVisibleItems())
	{
		//m_listCtrl->EnsureVisible(nCurSelItem, FALSE);
		CSize szScroll(0, GetItemHeight()* (nCurSelItem-nOldSelItem));
		m_listCtrl->Scroll(szScroll);
		RecalcSizeToFitList();
	}
	m_listCtrl->Invalidate();
	m_listCtrl->UpdateWindow();
	return nCurSelItem;
}

void CAutoCompleteWnd::DoAutoCompletion()
{
	AUTOCCOMPLETE info;
	PrepareNotifyHeader((AUTOCNMHDR*)&info);
	info.nPosStartChar = m_infoInit.nPosStartChar;
	info.bDropRestOfWord = m_infoInit.bDropRestOfWord;
	info.nItem = m_listCtrl->GetCurSel();
	info.strText = m_listCtrl->GetItemText(info.nItem, 0);
	m_listCtrl->GetItemRect(info.nItem, &info.rcItemScreen, LVIR_BOUNDS);
	m_listCtrl->ClientToScreen(&info.rcItemScreen);
	NotifyOwner(ACCmdComplete, (AUTOCNMHDR*)&info);
	Close();
}

void CAutoCompleteWnd::SetImageList(CImageList* pImageList)
{
	if (m_listCtrl)
	{
		m_listCtrl->SetImageList(pImageList, LVSIL_SMALL);
		IMAGEINFO ii = { 0 };
		pImageList->GetImageInfo(0, &ii);
		m_szIcon.cx = abs(ii.rcImage.right - ii.rcImage.left);
		m_szIcon.cy = abs(ii.rcImage.bottom - ii.rcImage.top);
	}
}

CAutoCompleteWnd* CAutoCompleteWnd::s_pInstance = nullptr;

CAutoCompleteWnd* CAutoCompleteWnd::GetActiveInstance()
{
	return s_pInstance;
}

BOOL CAutoCompleteWnd::IsActiveOwner(CWnd* pWnd)
{
	if (!GetSafeHwnd())
		return FALSE;
	return GetOwner()->GetSafeHwnd() == pWnd->GetSafeHwnd();
}

// this current implementation does not need nChar for now
BOOL CAutoCompleteWnd::Activate(CWnd* pOwner, UINT /*nChar*/)
{
	if ( GetActiveInstance() )
	{
		if (GetActiveInstance()->IsActiveOwner(pOwner) )
		{
			return TRUE;
		}
		// this is weird, the window should have been deactivated already,
		// better to investigate how is this possible
		ASSERT(0);
		GetActiveInstance()->Close();
	}

	AUTOCINITINFO info = {0};
	info.nMaxVisibleItems = AC_DEFAULT_MAX_VISIBLE_ITEM;
	info.bDropRestOfWord = TRUE;
	if ( NotifyOwnerImpl(pOwner, ACCmdGetInitInfo, (AUTOCNMHDR*)&info) == 0 || info.nItemCount <= 0 )
		return FALSE;
	CAutoCompleteWnd* pACWnd = new CAutoCompleteWnd;
	BOOL bCreated = pACWnd->Create(pOwner, info);
	return bCreated;
}

BOOL CAutoCompleteWnd::Show(CWnd* pOwner)
{
	return Activate(pOwner, (UINT)-1);
}

BOOL CAutoCompleteWnd::Cancel()
{
	if (GetActiveInstance())
		GetActiveInstance()->Close();
	return TRUE;
}

LRESULT CAutoCompleteWnd::NotifyOwner(ACCmd cmd, AUTOCNMHDR* pHdr) const
{
	auto pWndOwner = GetOwner();
	if (pWndOwner->GetSafeHwnd())
	{
		return NotifyOwnerImpl(pWndOwner, cmd, pHdr);
	}
	return 0;
}

LRESULT CAutoCompleteWnd::NotifyOwnerImpl(CWnd* pWndOwner, ACCmd cmd, AUTOCNMHDR* pHdr)
{
	return pWndOwner->SendMessage(WM_AC_NOTIFY, (WPARAM)cmd, (LPARAM)pHdr);
}

CAutoCompleteListCtrl* CAutoCompleteWnd::CreateListCtrl()
{
	return new CAutoCompleteListCtrl;
}

void CAutoCompleteWnd::PrepareNotifyHeader(AUTOCNMHDR* hdr)
{
	hdr->hwndFrom = m_hWnd;
}

int CAutoCompleteWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nRet = CAutoCompleteWndBase::OnCreate(lpCreateStruct);
	if (nRet != 0)
		return nRet;
	ASSERT(!m_listCtrl);
	m_listCtrl = CreateListCtrl();
	if (!m_listCtrl)
	{
		ASSERT(0);
		return 1;
	}
	DWORD dwStyle = WS_CHILD | WS_VSCROLL | WS_VISIBLE | WS_BORDER;
	dwStyle |= LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	// the image list should be reused
	dwStyle |= LVS_SHAREIMAGELISTS;
	dwStyle |= LVS_NOCOLUMNHEADER;
	dwStyle |= LVS_OWNERDATA;
	dwStyle |= LVS_OWNERDRAWFIXED;
	CRect rect;
	SetRectEmpty(rect);
	BOOL bCreated = m_listCtrl->Create(dwStyle, rect, this, IDC_AUTO_LIST_CTRL);
	if (!bCreated)
		return 1;
	if (m_infoInit.hFont)
		m_pFont = CFont::FromHandle(m_infoInit.hFont);
	m_listCtrl->SetFont(m_pFont);
	return 0;
}

void CAutoCompleteWnd::OnSize(UINT nType, int cx, int cy)
{
	CAutoCompleteWndBase::OnSize(nType, cx, cy);
	if (m_listCtrl->GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(rect);
		UINT uiSWPFlags = SWP_NOZORDER | SWP_NOACTIVATE;
		m_listCtrl->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), uiSWPFlags);
	}
}

void CAutoCompleteWnd::OnGetListDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	ASSERT(pDispInfo);

	LVITEM& listItem = pDispInfo->item;

	AUTOCDISPINFO info = { 0 };
	PrepareNotifyHeader((AUTOCNMHDR*)&info);
	AUTOCITEM& acItem = info.item;
	acItem.nItem = listItem.iItem;
	
	if (listItem.mask & LVIF_TEXT)
	{
		// See document for LVITEM:
		// the list-view control allows any length string to be stored as item text, only the first 260 TCHARs are displayed.
		// so better truncate the text here instead of leaving junk/gibberish string
		acItem.pszText = listItem.pszText;
		acItem.cchTextMax = listItem.cchTextMax;
		acItem.mask |= acItem.ACIF_TEXT;
	}

	if (listItem.mask & LVIF_IMAGE)
	{
		acItem.mask |= acItem.ACIF_IMAGE;
	}

	VERIFY( NotifyOwner(ACCmdGetListDispInfo, (AUTOCNMHDR*)&info) );

	if (listItem.mask & LVIF_IMAGE)
		listItem.iImage = acItem.nImage;

	*pResult = 0;
}

void CAutoCompleteWnd::OnListItemChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;
	if ( m_bReady && (pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED) )
	{
		AUTOCSELCHANGEINFO info = { 0 };
		PrepareNotifyHeader((AUTOCNMHDR*)&info);
		info.nItem = pNMLV->iItem;
		m_listCtrl->GetItemRect(info.nItem, &info.rcItemScreen, LVIR_BOUNDS);
		m_listCtrl->ClientToScreen(&info.rcItemScreen);
		NotifyOwner(ACCmdSelChange, (AUTOCNMHDR*)&info);
	}
}

void CAutoCompleteWnd::OnListDblClk(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMITEMACTIVATE* pItemActivate = (NMITEMACTIVATE*)pNMHDR;
	if (pItemActivate->iItem >= 0)
	{
		DoAutoCompletion();
	}
}

BOOL CAutoCompleteWnd::HandleKeyUpdateTransparency()
{
	if (GetKeyState(VK_SHIFT) < 0)
	{
		Close();
		return FALSE;
	}
	if (m_nAlpha > AC_MIN_ALPHA && m_nAlphaTiimer == 0)
	{
		m_bIncreaseAlpha = false;
		m_nAlphaTiimer = SetTimer(AC_TIMER_ALPHA_ID, AC_TIMER_ALPHA_ELAPSE, nullptr);
		m_nAlpha -= AC_ALPHA_DELTA;
		SetLayeredWindowAttributes(0, m_nAlpha, LWA_ALPHA);
	}
	return FALSE;
}

void CAutoCompleteWnd::OnTimer(UINT_PTR nIDEvent)
{
	CAutoCompleteWndBase::OnTimer(nIDEvent);
	if (nIDEvent != AC_TIMER_ALPHA_ID)
		return;

	if ( (m_bIncreaseAlpha && m_nAlpha < 255) || (!m_bIncreaseAlpha && m_nAlpha > AC_MIN_ALPHA) )
		SetLayeredWindowAttributes(0, m_nAlpha += m_bIncreaseAlpha ? AC_ALPHA_DELTA : -AC_ALPHA_DELTA, LWA_ALPHA);
	if (m_bIncreaseAlpha ? (m_nAlpha == 255) : (m_nAlpha <= AC_MIN_ALPHA))
	{
// 		if (!m_bIncreaseAlpha && GetKeyState(VK_CONTROL) < 0)
// 			return;
		if (m_nAlphaTiimer)
		{
			KillTimer(m_nAlphaTiimer);
			m_nAlphaTiimer = 0;
		}
	}
}

const CSize g_szTextPadding(4, 4);
const CSize g_szIconPadding(4, 0);	// always vertically centered

void CAutoCompleteWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (nIDCtl != IDC_AUTO_LIST_CTRL)
	{
		CAutoCompleteWndBase::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
		return;
	}
	lpMeasureItemStruct->itemHeight = GetItemHeight();
}

void CAutoCompleteWnd::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl != IDC_AUTO_LIST_CTRL)
	{
		CAutoCompleteWndBase::OnDrawItem(nIDCtl, lpDrawItemStruct);
		return;
	}
	DrawItem(lpDrawItemStruct);
}

void CAutoCompleteWnd::DrawItem(LPDRAWITEMSTRUCT pDIS)
{
	CDC* pDC = CDC::FromHandle(pDIS->hDC);

	UINT nItemState = pDIS->itemState;
	int nRow = (int)pDIS->itemID;
	CRect rect = pDIS->rcItem;
	BOOL bSelected = nItemState & ODS_SELECTED;
	COLORREF clrBkOrig = m_listCtrl->GetBkColor();
	COLORREF clrBk = clrBkOrig;
	if (bSelected)
		clrBk = GetSysColor(COLOR_MENUHILIGHT);

	// Background
	pDC->FillSolidRect(rect, clrBk);

	DrawItemIcon(pDC, nRow, rect);

	DrawItemText(pDC, nRow, nItemState, rect);
}

void CAutoCompleteWnd::DrawItemIcon(CDC* pDC, int nRow, CRect rect)
{
	auto pImageList = m_listCtrl->GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		LVITEM item = { 0 };
		item.iItem = nRow;
		item.mask = LVIF_IMAGE;
		VERIFY(m_listCtrl->GetItem(&item));
		int nIcon = item.iImage;
		rect.left += g_szIconPadding.cx;
		CPoint pt = rect.TopLeft();
		pt.y += (rect.Height() - m_szIcon.cy) / 2;
		UINT nIconStyle = ILD_TRANSPARENT;
		pImageList->Draw(pDC, nIcon, pt, nIconStyle);
	}
}

void CAutoCompleteWnd::DrawItemText(CDC* pDC, int nRow, UINT nState, CRect& rect, BOOL bCalcOnly)
{
	CString str = m_listCtrl->GetItemText(nRow, 0);
	auto pImageList = m_listCtrl->GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		rect.left += g_szIconPadding.cx + m_szIcon.cx;
	}
	rect.left += g_szTextPadding.cx;
	if (bCalcOnly)
	{
		CSize szText = pDC->GetTextExtent(str);
		rect.right = rect.left + szText.cx + g_szTextPadding.cx;
		return;
	}
	BOOL bSelected = nState & ODS_SELECTED;
	int nOldBKMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clrText = bSelected ? GetSysColor(COLOR_HIGHLIGHTTEXT) : m_listCtrl->GetTextColor();
	auto clrOldText = pDC->SetTextColor(clrText);
	
	UINT nFormat = DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX | DT_VCENTER;
	pDC->DrawText((LPCTSTR)str, &rect, nFormat);

	pDC->SetBkMode(nOldBKMode);
	pDC->SetTextColor(clrOldText);
}

BOOL CAutoCompleteWnd::OnKey(UINT nKey)
{
	// return TRUE to eat the key
	if (!m_listCtrl->GetSafeHwnd())
	{
		ASSERT(0);
		Close();
		return FALSE;
	}
	switch (nKey)
	{
	case VK_ESCAPE:
		Close();
		break;
	case VK_UP:
		MoveSelection(-1);
		break;
	case VK_DOWN:
		MoveSelection(1);
		break;
	case VK_PRIOR:
		MoveSelection(-GetVisibleItems());
		break;
	case VK_NEXT:
		MoveSelection(GetVisibleItems());
		break;
	case VK_TAB:
	case VK_RETURN:
		DoAutoCompletion();
		break;
	case VK_CONTROL:
		return HandleKeyUpdateTransparency();
	case VK_SHIFT:
		if (GetKeyState(VK_CONTROL) < 0)
		{
			Close();
			return FALSE;
		}
	case VK_CAPITAL:
	case VK_NUMLOCK:
	case VK_INSERT:
		break;
	default:
		return NotifyKey(nKey);
	}
	return TRUE;
}

BOOL CAutoCompleteWnd::NotifyKey(UINT nKey)
{
	AUTOCKEYINFO info = { 0 };
	PrepareNotifyHeader((AUTOCNMHDR*)&info);
	info.nKey = nKey;
	info.nChar = nKey;
	switch (nKey)
	{
	case VK_DELETE:
	case VK_BACK:
		break;
	default:
		{
			UINT nChar = info.nChar;
	#ifndef _UNICODE
			WORD wChar = 0;
			BYTE lpKeyState[256];
			::GetKeyboardState(lpKeyState);

			::ToAsciiEx(nChar, MapVirtualKey(nChar, 0), lpKeyState, &wChar, 1, ::GetKeyboardLayout(AfxGetThread()->m_nThreadID));

			info.nChar = (UINT)wChar;
	#else
			TCHAR szChar[2];
			memset(szChar, 0, sizeof(TCHAR) * 2);
			BYTE lpKeyState[256];
			ENSURE(::GetKeyboardState(lpKeyState));

			::ToUnicodeEx(nChar, MapVirtualKey(nChar, 0), lpKeyState, szChar, 2, 1, ::GetKeyboardLayout(AfxGetThread()->m_nThreadID));
			info.nChar = (UINT)szChar[0];
	#endif // _UNICODE
		}
		break;
	}

	info.nPosStartChar = m_infoInit.nPosStartChar;
	info.nStartLine = m_infoInit.nStartLine;
	info.nItemCount = -1;
	info.nPreSelectItem = m_listCtrl->GetCurSel();
	info.bEatKey = FALSE;
	info.bClose = TRUE;
	NotifyOwner(ACCmdKey, (AUTOCNMHDR*)&info);
	if (info.bClose || info.nItemCount == 0)
	{
		Close();
	}
	else if (info.nItemCount > 0)
	{
		UpdateListItemCount(info.nItemCount);
		m_listCtrl->SetCurSel(info.nPreSelectItem < info.nItemCount ? info.nPreSelectItem : GetTopIndex());
	}
	return info.bEatKey;
}

void CAutoCompleteWnd::Close()
{
	CAutoCompleteWndBase::Close();
}

void CAutoCompleteWnd::UpdateListItemCount(int nItemCount)
{
	SetItemCount(nItemCount);
	RecalcSizeToFitList();
}

static RECT _RectFromMonitor(HMONITOR hMonitor) {
	MONITORINFO mi = { 0 };
	mi.cbSize = sizeof(mi);
	if (GetMonitorInfo(hMonitor, &mi))
	{
		return mi.rcWork;
	}
	RECT rc = { 0, 0, 0, 0 };
	if (::SystemParametersInfoA(SPI_GETWORKAREA, 0, &rc, 0) == 0)
	{
		rc.left = 0;
		rc.top = 0;
		rc.right = 0;
		rc.bottom = 0;
	}
	return rc;
}

void CAutoCompleteWnd::RecalcSizeToFitList()
{
	int nItemCount = m_listCtrl->GetItemCount();
	int nTopItemIndex = GetTopIndex();
	//m_nMaxItemWidth = 0;
	m_nVisibleItems = min(m_infoInit.nMaxVisibleItems, nItemCount);
	CClientDC dc(m_listCtrl);
	auto pOldFont = dc.SelectObject(m_pFont);
	CRect rectClient;
	GetClientRect(rectClient);
	CRect rectLabel;
	for (int nItem = nTopItemIndex; nItem < nTopItemIndex + m_nVisibleItems; ++nItem)
	{
		rectLabel = rectClient;
		rectLabel.right = LONG_MAX;
		DrawItemText(&dc, nItem, 0, rectLabel, TRUE);
		m_nMaxItemWidth = max(m_nMaxItemWidth, rectLabel.right);
	}
	dc.SelectObject(pOldFont);

	int nVertSpacing = GetItemHeight();
	CPoint posClient(0,0);
	CSize szClient;
	szClient.cx = m_nMaxItemWidth;
	if (m_listCtrl->IsShowVScrollBar())
		szClient.cx += GetSystemMetrics(SM_CXVSCROLL);
	szClient.cy = nVertSpacing * m_nVisibleItems;
	UINT nFlags = SWP_NOACTIVATE;
	BOOL bVisible = IsWindowVisible();
	if (!bVisible)
		nFlags |= SWP_SHOWWINDOW;
	CWnd* pWndOwner = GetOwner();
	posClient = m_infoInit.posWordScreen;
	posClient.x -= rectLabel.left;
	HMONITOR hMonitor = MonitorFromPoint(posClient, MONITOR_DEFAULTTONEAREST);
	CRect rcPopupBounds = _RectFromMonitor(hMonitor);
	if (rcPopupBounds.Height() == 0)
		pWndOwner->GetClientRect(rcPopupBounds);
	if (posClient.y >= rcPopupBounds.bottom - szClient.cy &&  // Won't fit below.
		posClient.y >= (rcPopupBounds.bottom + rcPopupBounds.top) / 2)	// and there is more room above.
	{
		posClient.y = m_infoInit.posWordScreen.y - m_infoInit.nLineHeight - szClient.cy;
	}
	if (posClient.x + szClient.cx > rcPopupBounds.right)
		posClient.x -= posClient.x + szClient.cx - rcPopupBounds.right;
	CRect rect(posClient, szClient);
	AdjustWindowRect(&rect, WS_BORDER, FALSE);
	SetWindowPos(&wndTop, rect.left, rect.top, rect.Width(), rect.Height(), nFlags);
	m_listCtrl->SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
}

int CAutoCompleteWnd::GetItemHeight()
{
	if (m_nItemHeight < 0)
	{
		CClientDC dc(this);
		auto pOldFont = dc.SelectObject(m_pFont);
		TEXTMETRIC tm = { 0 };
		dc.GetTextMetrics(&tm);
		m_nItemHeight = tm.tmHeight + g_szTextPadding.cy * 2;
		int nIconHeight = m_szIcon.cy + g_szIconPadding.cy * 2;
		if (nIconHeight > m_nItemHeight)
			m_nItemHeight = nIconHeight;
	}
	return m_nItemHeight;
}

LRESULT CAutoCompleteWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		return OnKey((UINT)wParam) ? 0 : -1;
	case WM_KEYUP:
		if ((UINT)wParam == VK_CONTROL)
		{
			m_bIncreaseAlpha = true;
			if (m_nAlphaTiimer == 0)
				m_nAlphaTiimer = SetTimer(AC_TIMER_ALPHA_ID, AC_TIMER_ALPHA_ELAPSE, nullptr);
		}
		break;
	case WM_SYSKEYDOWN:
		Close();
		return -1;
	}
	return CAutoCompleteWndBase::WindowProc(message, wParam, lParam);
}
