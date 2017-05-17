/*
MIT License

Copyright(c) 2017 wingkinl (Kenny Lau)

https://github.com/wingkinl/AutoCompleteDemo

History: 5/10/2017 Initial version

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// AutoCompleteWnd.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteWnd.h"
#include "AutoCompleteListCtrl.h"

#define AC_DEFAULT_MAX_VISIBLE_ITEM		10

#define AC_MIN_ALPHA					30
#define AC_ALPHA_DELTA					45
#define AC_TIMER_ALPHA_ID				0x1000
#define AC_TIMER_ALPHA_ELAPSE			25

#define AC_TIMER_TOOLTIP_ID				0x2000

#define IDC_AUTO_LIST_CTRL		100

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
	return 0 < nChar && nChar <= 0xFF && _istalnum((TCHAR)nChar);
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
			if ( nCurPos == PositionAfter(pInfo->nPosStartChar) )
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
			strText.AppendChar((TCHAR)pInfo->nChar);
	}
	if (bIsValid)
	{
		pInfo->nItemCount = bUpdateList ? UpdateFilteredList((LPCTSTR)strText, pInfo->nPreSelectItem) : -1;
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
				pInfo->nItemCount = UpdateFilteredList((LPCTSTR)pInfo->strStart, pInfo->nPreSelectItem);
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
	case ACCmdSelChange:
		return (LRESULT)HandleSelChange((AUTOCSELCHANGEINFO*)nmhdr);
	case ACCmdDrawIcon:
		return (LRESULT)DrawIcon((AUTOCDRAWICON*)nmhdr);
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
	int nMappedItem = GetMappedIndex(item.nItem);
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

int CWindowACImp::GetMappedIndex(int nItem) const
{
	int nMappedItem = nItem;
	if (nMappedItem < m_arrFilteredIndices.GetSize())
		nMappedItem = m_arrFilteredIndices[nMappedItem];
	return nMappedItem;
}

int CWindowACImp::UpdateFilteredList(LPCTSTR pszFilterText, int& nPreSelIndex)
{
	//int nOldMappedItem = GetMappedIndex(nPreSelIndex);
	m_arrFilteredIndices.RemoveAll();
	int nItemCount = GetTotalItemCount();
	if (!pszFilterText || !*pszFilterText)
		return 0;
	int nTextLen = (int)_tcslen(pszFilterText);
	for (int ii = 0; ii < nItemCount; ++ii)
	{
		LPCTSTR pszItemText = GetItemDisplayText(ii);
		bool bMatch = false;
		if (m_bFuzzyMatch)
		{
			LPCTSTR pszTextMatch = pszItemText;
			auto pfnStrChr = m_bMatchCase ? StrChr : StrChrI;
			for (int nChar = 0; nChar < nTextLen; ++nChar)
			{
				pszTextMatch = pfnStrChr(pszTextMatch, pszFilterText[nChar]);
				if (!pszTextMatch)
					break;
				if (*++pszTextMatch == _T('\0'))
				{
					if (nChar != nTextLen - 1)
						pszTextMatch = nullptr;
					break;
				}
			}
			bMatch = pszTextMatch != nullptr;
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
	int nEndCharPos = GetCaretPos();
	if (pInfo->bDropRestOfWord)
	{
		int nCurLine = LineFromChar(nEndCharPos);
		int nCurLineStartChar = LineIndex(nCurLine);
		auto nLineEndPos = GetLineEndPosition(nCurLine);

		ASSERT(nEndCharPos >= nCurLineStartChar);

		int nLineLen = LineLength(nCurLineStartChar);
		CString strLine;
		GetLineText(nCurLine, strLine, nLineLen);
		LPCTSTR pszLine = (LPCTSTR)strLine;

		int nCurCharOffset = nEndCharPos - nCurLineStartChar;
		nEndCharPos = nLineEndPos;
		for (int nCharPos = nCurCharOffset; nCharPos < nLineLen; ++nCharPos)
		{
			if (!IsValidChar(pszLine[nCharPos]))
			{
				nEndCharPos = nCurLineStartChar + nCharPos;
				break;
			}
		}
	}
	SetSel(pInfo->nPosStartChar, nEndCharPos, TRUE);
	CString strText = GetCompleteText(pInfo->strText, pInfo->nItem);
	ReplaceSel(strText);
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
		pInfo->strStart.Insert(0, (TCHAR)nChar);
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
	if (m_font.GetSafeHandle())
	{
		m_font.DeleteObject();
	}
	LOGFONT lf = {0};
	int style = STYLE_DEFAULT;
	_tcsncpy(lf.lfFaceName, m_pEdit->StyleGetFont(style), _countof(lf.lfFaceName));

	lf.lfCharSet	= (BYTE)m_pEdit->StyleGetCharacterSet(style);

	CClientDC dc(m_pEdit);
	//int nHeight		= -MulDiv(m_pEdit->StyleGetSize(style), dc.GetDeviceCaps(LOGPIXELSY), 72);
	//lf.lfHeight		= nHeight;
	lf.lfHeight = pInfo->nLineHeight;

	// NOTE: the weight here is NOT an accurate value!
	lf.lfWeight		= m_pEdit->StyleGetBold(style) ? FW_BOLD : FW_NORMAL;

	m_font.CreateFontIndirect(&lf);
	pInfo->hFont = (HFONT)m_font.GetSafeHandle();
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
		nEndCharPos = nLineEndPos;
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
	CString strText = GetCompleteText(pInfo->strText, pInfo->nItem);
	m_pEdit->ReplaceSel(strText);
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
/* CAutoCTooltipCtrl
/************************************************************************/
class CAutoCTooltipCtrl : public CMFCToolTipCtrl
{
public:
	CAutoCTooltipCtrl();
	~CAutoCTooltipCtrl();
public:
	BOOL Create(CWnd* pParentWnd, _In_ DWORD dwStyle, const AUTOCTOOLTIPINFO& tti);

	static void FillToolTipInfoWithDefault(AUTOCTOOLTIPINFO& tti);

	void Show(BOOL bShow = TRUE);
protected:
	BOOL OnDrawIcon(CDC* pDC, CRect rectImage) override;
	CSize GetIconSize() override;
protected:
	void FillInToolInfo(TOOLINFO& ti);
	CPoint CalcTrackPosition(int cx, int cy);
protected:
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnShow(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
protected:
	friend class CAutoCompleteWnd;
	int		m_nItem;
	CString	m_strLabel;
};

CAutoCTooltipCtrl::CAutoCTooltipCtrl()
{
	m_strLabel = _T("dummy");
}

CAutoCTooltipCtrl::~CAutoCTooltipCtrl()
{

}

BEGIN_MESSAGE_MAP(CAutoCTooltipCtrl, CMFCToolTipCtrl)
	ON_WM_MOUSEACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_NOTIFY_REFLECT(TTN_SHOW, &OnShow)
END_MESSAGE_MAP()

BOOL CAutoCTooltipCtrl::Create(CWnd* pParentWnd, _In_ DWORD dwStyle, const AUTOCTOOLTIPINFO& tti)
{
	BOOL bRet = CMFCToolTipCtrl::Create(pParentWnd, dwStyle);
	if (!bRet)
		return FALSE;
	m_Params.m_bDrawIcon = tti.m_bDrawIcon;
	m_Params.m_bDrawDescription = tti.m_bDrawDescription;
	m_Params.m_bBoldLabel = tti.m_bBoldLabel;
	m_Params.m_bDrawSeparator = tti.m_bDrawSeparator;
	m_Params.m_bVislManagerTheme = tti.m_bVislManagerTheme;
	m_Params.m_nMaxDescrWidth = tti.m_nMaxDescrWidth;
	m_Params.m_nGradientAngle = tti.m_nGradientAngle;
	m_Params.m_clrFill = tti.m_clrFill;
	m_Params.m_clrFillGradient = tti.m_clrFillGradient;
	m_Params.m_clrText = tti.m_clrText;
	m_Params.m_clrBorder = tti.m_clrBorder;

	if (tti.m_nInitialTime >= 0)
		SetDelayTime(TTDT_INITIAL, tti.m_nInitialTime);

	TOOLINFO ti = { 0 };
	FillInToolInfo(ti);
	SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);

	return TRUE;
}

void CAutoCTooltipCtrl::FillToolTipInfoWithDefault(AUTOCTOOLTIPINFO& tti)
{
	CMFCToolTipInfo param;
	tti.m_bEnable = FALSE;
	tti.m_nInitialTime = -1;

	tti.m_bDrawIcon = param.m_bDrawIcon;
	tti.m_bDrawDescription = param.m_bDrawDescription;
	tti.m_bBoldLabel = param.m_bBoldLabel;
	tti.m_bDrawSeparator = param.m_bDrawSeparator;
	tti.m_bVislManagerTheme = param.m_bVislManagerTheme;
	tti.m_nMaxDescrWidth = param.m_nMaxDescrWidth;
	tti.m_nGradientAngle = param.m_nGradientAngle;
	tti.m_clrFill = param.m_clrFill;
	tti.m_clrFillGradient = param.m_clrFillGradient;
	tti.m_clrText = param.m_clrText;
	tti.m_clrBorder = param.m_clrBorder;
}

void CAutoCTooltipCtrl::Show(BOOL bShow)
{
	if (!bShow)
	{
		SendMessage(TTM_TRACKACTIVATE, FALSE);
		return;
	}

	TOOLINFO ti = { 0 };
	FillInToolInfo(ti);
	SendMessage(TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
	//SendMessage(TTM_SETTOOLINFO, 0, (LPARAM)&ti);

	CRect rect;
	GetWindowRect(rect);
	CPoint pt  = CalcTrackPosition(rect.Width(), rect.Height());

	SendMessage(TTM_TRACKPOSITION, 0, MAKELPARAM(pt.x, pt.y));
	SendMessage(TTM_SETTOOLINFO, 0, (LPARAM)&ti);
}

CPoint CAutoCTooltipCtrl::CalcTrackPosition(int cx, int cy)
{
	CWnd* pWndOwner = GetOwner();
	CAutoCompleteWnd* pACWnd = (CAutoCompleteWnd*)pWndOwner;
	CRect rectWindow;
	pACWnd->GetItemRect(m_nItem, &rectWindow);
	CRect rectOwner;
	pACWnd->GetWindowRect(rectOwner);
	if (rectWindow.top < rectOwner.top)
		rectWindow.top = rectOwner.top;
	else if (rectWindow.top > rectOwner.bottom)
		rectWindow.top = rectOwner.bottom;

	CPoint pt(rectWindow.right, rectWindow.top);

	CRect rectScreen;

	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	if (GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi))
	{
		rectScreen = mi.rcWork;
	}
	else
	{
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0);
	}
	if (pt.x + cx > rectScreen.right)
		pt.x = rectOwner.left - cx;
	return pt;
}

void CAutoCTooltipCtrl::OnShow(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = 0;

	if (m_Params.m_bVislManagerTheme)
	{
		CMFCVisualManager::GetInstance()->GetToolTipInfo(m_Params);
		m_Params.m_bVislManagerTheme = TRUE;
	}

	if (m_Params.m_bBalloonTooltip)
	{
		return;
	}

	GetHotButton();

	m_sizeImage = m_Params.m_bDrawIcon ? GetIconSize() : CSize(0, 0);
	m_ptMargin = CPoint(6, 4);

	CRect rectMargin;
	GetMargin(rectMargin);

	CRect rectText;
	GetClientRect(rectText);

	CClientDC dc(this);
	CSize sizeText = OnDrawLabel(&dc, rectText, TRUE);

	int cx = sizeText.cx;
	int cy = sizeText.cy;

	CSize sizeDescr(0, 0);

	if (!m_Params.m_bDrawDescription || m_strDescription.IsEmpty())
	{
		cy = max(cy, m_sizeImage.cy);
	}
	else
	{
		sizeDescr = OnDrawDescription(&dc, rectText, TRUE);

		cy += sizeDescr.cy + 2 * m_ptMargin.y;
		cx = max(cx, sizeDescr.cx);
		cy = max(cy, m_sizeImage.cy);
	}

	if (m_sizeImage.cx > 0 && m_Params.m_bDrawIcon)
	{
		cx += m_sizeImage.cx + m_ptMargin.x;
	}

	cx += 2 * m_ptMargin.x;
	cy += 2 * m_ptMargin.y;

	const int nFixedWidth = GetFixedWidth();
	if (nFixedWidth > 0 && sizeDescr != CSize(0, 0))
	{
		cx = max(cx, nFixedWidth);
	}

	CPoint pt = CalcTrackPosition(cx, cy);

	SetWindowPos(NULL, pt.x, pt.y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

#if _MSC_VER >= 1900
	m_sizeCornerRadius = GetWindowTheme(GetSafeHwnd()) != NULL ? CSize(3, 3) : CSize(0, 0);
#endif _MSC_VER
	SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE | SWP_DRAWFRAME);
}

BOOL CAutoCTooltipCtrl::OnDrawIcon(CDC* pDC, CRect rectImage)
{
	CWnd* pWndOwner = GetOwner();
	CAutoCompleteWnd* pACWnd = (CAutoCompleteWnd*)pWndOwner;
	if (pACWnd)
		return pACWnd->DrawItemIcon(pDC, m_nItem, rectImage);
	return CMFCToolTipCtrl::OnDrawIcon(pDC, rectImage);
}

CSize CAutoCTooltipCtrl::GetIconSize()
{
	CWnd* pWndOwner = GetOwner();
	CAutoCompleteWnd* pACWnd = (CAutoCompleteWnd*)pWndOwner;
	if (pACWnd)
		return pACWnd->GetIconSize();
	return CMFCToolTipCtrl::GetIconSize();
}

void CAutoCTooltipCtrl::FillInToolInfo(TOOLINFO& ti)
{
	CWnd* pWndOwner = GetOwner();
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_TRACK | TTF_ABSOLUTE | TTF_IDISHWND;
	ti.lpszText = (LPTSTR)(LPCTSTR)m_strLabel;
	ti.hwnd = pWndOwner->GetSafeHwnd();
	ti.uId = (UINT_PTR)ti.hwnd;
}

int CAutoCTooltipCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return MA_NOACTIVATE;
}

void CAutoCTooltipCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	Show(FALSE);
}

void CAutoCTooltipCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	Show(FALSE);
}

void CAutoCTooltipCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	Show(FALSE);
}

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
	m_nAlpha = 255;
	m_bIncreaseAlpha = false;
	m_nAlphaTimer = 0;

	m_pFont = &GetGlobalData()->fontTooltip;
	m_nItemHeight = -1;

	m_pToolTipCtrl = nullptr;
	m_nToolTipTimer = 0;
	m_bUpdateToolTipOnScroll = true;

	m_szIcon = CSize(0, 0);
}

CAutoCompleteWnd::~CAutoCompleteWnd()
{
	ASSERT(GetActiveInstance());
	s_pInstance = nullptr;
}

BEGIN_MESSAGE_MAP(CAutoCompleteWnd, CAutoCompleteWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnListGetDispInfo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnListItemChange)
	ON_NOTIFY(NM_DBLCLK, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnListDblClk)
	ON_NOTIFY(LVN_ENDSCROLL, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnListEndScroll)
	ON_WM_TIMER()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_WM_MOUSEWHEEL()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CAutoCompleteWnd message handlers

BOOL CAutoCompleteWnd::Create(CWnd* pOwner, const AUTOCINITINFO& info)
{
	m_infoInit = info;
	if (m_infoInit.hFont)
		m_pFont = CFont::FromHandle(m_infoInit.hFont);
	BOOL bCreated = CAutoCompleteWndBase::Create(pOwner, info.posWordScreen);
	if (!bCreated)
		return FALSE;
	SetOwner(pOwner);
	if (info.pImageList)
		SetImageList(info.pImageList);
	else if (info.szIcons.cx && info.szIcons.cy)
		m_szIcon = info.szIcons;

	if (info.stToolTipInfo.m_bEnable)
	{
		m_pToolTipCtrl = new CAutoCTooltipCtrl;
		VERIFY(m_pToolTipCtrl->Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX, info.stToolTipInfo));
	}

	UpdateListItemCount(info.nItemCount);
	if (!info.bDummySelect)
	{
		int nNewSel = info.nPreSelectItem < info.nItemCount ? info.nPreSelectItem : 0;
		//m_listCtrl->SetCurSel(nNewSel);
		//m_listCtrl->EnsureVisible(nNewSel, FALSE);
		// prevent the tooltip from immediately showing
		m_bUpdateToolTipOnScroll = false;
		MoveSelection(nNewSel);
		m_bUpdateToolTipOnScroll = true;
	}

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

int CAutoCompleteWnd::GetCurSel() const
{
	int nCurSel = m_listCtrl ? m_listCtrl->GetCurSel() : 0;
	return nCurSel < 0 ? 0 : nCurSel;
}

void CAutoCompleteWnd::SetCurSel(int nItem)
{
	if (m_listCtrl)
		m_listCtrl->SetCurSel(nItem);
}

BOOL CAutoCompleteWnd::GetItemRect(int nItem, LPRECT rect)
{
	if (!m_listCtrl)
		return FALSE;
	GetWindowRect(rect);
	auto nRight = rect->right;
	m_listCtrl->GetItemRect(nItem, rect, LVIR_BOUNDS);
	m_listCtrl->ClientToScreen(rect);
	rect->right = nRight;
	return TRUE;
}

int CAutoCompleteWnd::MoveSelection(int nDelta)
{
	if (!m_listCtrl)
		return -1;
	int nTopItemIndex = GetTopIndex();
	int nOldSelItem = GetCurSel();
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

void CAutoCompleteWnd::DoAutoCompletion(int nEvent)
{
	AUTOCCOMPLETE info;
	PrepareNotifyHeader((AUTOCNMHDR*)&info);
	info.nPosStartChar = m_infoInit.nPosStartChar;
	info.bDropRestOfWord = m_infoInit.bDropRestOfWord;
	info.nItem = GetCurSel();
	info.strText = m_listCtrl->GetItemText(info.nItem, 0);
	info.eventFrom = (AUTOCCOMPLETE::Event)nEvent;
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
	CAutoCTooltipCtrl::FillToolTipInfoWithDefault(info.stToolTipInfo);
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

void CAutoCompleteWnd::OnListGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
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
	if ( (pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED) )
	{
		NotifySelChange(pNMLV->iItem);
	}
}

void CAutoCompleteWnd::OnListDblClk(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMITEMACTIVATE* pItemActivate = (NMITEMACTIVATE*)pNMHDR;
	if (pItemActivate->iItem >= 0)
	{
		DoAutoCompletion(AUTOCCOMPLETE::EventDblClk);
	}
}

void CAutoCompleteWnd::OnListEndScroll(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_bUpdateToolTipOnScroll && m_pToolTipCtrl->GetSafeHwnd())
	{
		m_pToolTipCtrl->Show();
	}
}

BOOL CAutoCompleteWnd::HandleKeyUpdateTransparency()
{
	if (GetKeyState(VK_SHIFT) < 0)
	{
		Close();
		return FALSE;
	}
	if (m_nAlpha > AC_MIN_ALPHA && m_nAlphaTimer == 0)
	{
		m_bIncreaseAlpha = false;
		m_nAlphaTimer = SetTimer(AC_TIMER_ALPHA_ID, AC_TIMER_ALPHA_ELAPSE, nullptr);
		m_nAlpha -= AC_ALPHA_DELTA;
		SetLayeredWindowAttributes(0, m_nAlpha, LWA_ALPHA);
		if (m_pToolTipCtrl->GetSafeHwnd())
		{
			m_pToolTipCtrl->SetLayeredWindowAttributes(0, m_nAlpha, LWA_ALPHA);
		}
	}
	return FALSE;
}

void CAutoCompleteWnd::KillToolTipTimer()
{
	if (m_nToolTipTimer)
	{
		KillTimer(m_nToolTipTimer);
		m_nToolTipTimer = 0;
	}
}

void CAutoCompleteWnd::NotifySelChange(int nItem)
{
	AUTOCSELCHANGEINFO info = { 0 };
	PrepareNotifyHeader((AUTOCNMHDR*)&info);
	info.nItem = nItem;
	GetItemRect(info.nItem, &info.rcItemScreen);
	NotifyOwner(ACCmdSelChange, (AUTOCNMHDR*)&info);
	if (m_pToolTipCtrl)
	{
		m_pToolTipCtrl->m_strLabel = info.strToolTipLabel;
		m_pToolTipCtrl->m_nItem = info.nItem;
		m_pToolTipCtrl->SetDescription(info.strToolTipDescription);
		UINT nElapse = (UINT)m_pToolTipCtrl->GetDelayTime(TTDT_INITIAL);
		m_nToolTipTimer = SetTimer(AC_TIMER_TOOLTIP_ID, nElapse, nullptr);
	}
}

void CAutoCompleteWnd::ShowToolTip()
{
	if (!m_pToolTipCtrl)
	{
		ASSERT(0);
		return;
	}
	m_pToolTipCtrl->Show(TRUE);
	KillToolTipTimer();
}

void CAutoCompleteWnd::UpdateTransparency()
{
	if ((m_bIncreaseAlpha && m_nAlpha < 255) || (!m_bIncreaseAlpha && m_nAlpha > AC_MIN_ALPHA))
	{
		m_nAlpha += m_bIncreaseAlpha ? AC_ALPHA_DELTA : -AC_ALPHA_DELTA;
		SetLayeredWindowAttributes(0, m_nAlpha, LWA_ALPHA);
		if (m_pToolTipCtrl->GetSafeHwnd())
			m_pToolTipCtrl->SetLayeredWindowAttributes(0, m_nAlpha, LWA_ALPHA);
	}
	if (m_bIncreaseAlpha ? (m_nAlpha == 255) : (m_nAlpha <= AC_MIN_ALPHA))
	{
		if (m_nAlphaTimer)
		{
			KillTimer(m_nAlphaTimer);
			m_nAlphaTimer = 0;
		}
	}
}

void CAutoCompleteWnd::OnTimer(UINT_PTR nIDEvent)
{
	CAutoCompleteWndBase::OnTimer(nIDEvent);
	switch (nIDEvent)
	{
	case AC_TIMER_ALPHA_ID:
		UpdateTransparency();
		break;
	case AC_TIMER_TOOLTIP_ID:
		ShowToolTip();
		break;
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

BOOL CAutoCompleteWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (GetKeyState(VK_CONTROL) < 0 || 
		GetKeyState(VK_SHIFT) < 0 ||
		GetKeyState(VK_MENU) < 0
		)
	{
		Close();
		return TRUE;
	}
	return FALSE;
}

void CAutoCompleteWnd::OnDestroy()
{
	KillToolTipTimer();
	if (m_pToolTipCtrl)
	{
		ASSERT_VALID(m_pToolTipCtrl);
		if (m_pToolTipCtrl->GetSafeHwnd())
		{
			m_pToolTipCtrl->Show(FALSE);
			m_pToolTipCtrl->DestroyWindow();
		}
		delete m_pToolTipCtrl;
		m_pToolTipCtrl = nullptr;
	}
	CAutoCompleteWndBase::OnDestroy();
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

	if (m_szIcon.cx)
	{
		CRect rectIcon = rect;
		rectIcon.left += g_szIconPadding.cx;
		rectIcon.right = rectIcon.left + m_szIcon.cx;
		DrawItemIcon(pDC, nRow, rectIcon);
	}

	DrawItemText(pDC, nRow, nItemState, rect);
}

BOOL CAutoCompleteWnd::DrawItemIcon(CDC* pDC, int nRow, CRect rect)
{
	BOOL bRet = FALSE;
	auto pImageList = m_listCtrl->GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		LVITEM item = { 0 };
		item.iItem = nRow;
		item.mask = LVIF_IMAGE;
		VERIFY(m_listCtrl->GetItem(&item));
		int nIcon = item.iImage;
 		CPoint pt = rect.TopLeft();
 		pt.y += (rect.Height() - m_szIcon.cy) / 2;
		UINT nIconStyle = ILD_TRANSPARENT;
		bRet = pImageList->Draw(pDC, nIcon, pt, nIconStyle);
	}
	else
	{
		AUTOCDRAWICON info;
		PrepareNotifyHeader((AUTOCNMHDR*)&info);
		info.hDC = pDC->GetSafeHdc();
		info.nItem = nRow;
		info.rect = rect;
		info.hIcon = nullptr;
		info.bAutoDestroyIcon = TRUE;
		bRet = NotifyOwner(ACCmdDrawIcon, (AUTOCNMHDR*)&info) != 0;
		if (bRet && info.hIcon)
		{
			CPoint pt = rect.TopLeft();
			pt.y += (rect.Height() - m_szIcon.cy) / 2;
			pDC->DrawIcon(pt, info.hIcon);
			if (info.hIcon)
				DestroyIcon(info.hIcon);
		}
	}
	return bRet;
}

void CAutoCompleteWnd::DrawItemText(CDC* pDC, int nRow, UINT nState, CRect& rect, BOOL bCalcOnly)
{
	CString str = m_listCtrl->GetItemText(nRow, 0);
	if (m_szIcon.cx)
		rect.left += g_szIconPadding.cx + m_szIcon.cx;
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
		DoAutoCompletion(AUTOCCOMPLETE::EventTab);
		break;
	case VK_RETURN:
		DoAutoCompletion(AUTOCCOMPLETE::EventEnter);
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
	info.nPreSelectItem = GetCurSel();
	info.bEatKey = FALSE;
	info.bClose = TRUE;
	NotifyOwner(ACCmdKey, (AUTOCNMHDR*)&info);
	if (info.bClose || info.nItemCount == 0)
	{
		Close();
	}
	else if (info.nItemCount > 0)
	{
		auto nOldSel = GetCurSel();
		UpdateListItemCount(info.nItemCount);
		auto nNewSel = info.nPreSelectItem < info.nItemCount ? info.nPreSelectItem : GetTopIndex();
		SetCurSel(nNewSel);
		m_listCtrl->EnsureVisible(nNewSel, FALSE);

		if (m_pToolTipCtrl)
			m_pToolTipCtrl->Show(FALSE);

		if (nOldSel == nNewSel && m_pToolTipCtrl->GetSafeHwnd())
		{
			// although the indices are the same but they could be different item!
			NotifySelChange(nNewSel);
		}
	}
	return info.bEatKey;
}

void CAutoCompleteWnd::Close()
{
	CAutoCompleteWndBase::Close();
}

CSize CAutoCompleteWnd::GetIconSize() const
{
	return m_szIcon;
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
		dc.SelectObject(pOldFont);
		m_nItemHeight = tm.tmHeight + g_szTextPadding.cy * 2;
		int nIconHeight = m_szIcon.cy + g_szIconPadding.cy * 2;
		if (nIconHeight > m_nItemHeight)
			m_nItemHeight = nIconHeight;
	}
	return m_nItemHeight;
}

BOOL CAutoCompleteWnd::ProcessMouseClick(UINT uiMsg, POINT pt, HWND hwnd)
{
	if (!HitTest(pt))
	{
		if (m_pToolTipCtrl->GetSafeHwnd())
		{
			CRect rectWindow;
			m_pToolTipCtrl->GetWindowRect(rectWindow);

			if (rectWindow.PtInRect(pt) )
			{
				return FALSE;
			}
		}
		Close();
	}
	return FALSE;
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
			if (m_nAlphaTimer == 0)
				m_nAlphaTimer = SetTimer(AC_TIMER_ALPHA_ID, AC_TIMER_ALPHA_ELAPSE, nullptr);
		}
		break;
	case WM_SYSKEYDOWN:
		Close();
		return -1;
	}
	return CAutoCompleteWndBase::WindowProc(message, wParam, lParam);
}
