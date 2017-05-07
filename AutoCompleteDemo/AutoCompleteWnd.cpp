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

CWindowACImp::CWindowACImp()
{
	m_bMatchCase = false;
	m_bFuzzyMatch = true;
}

CWindowACImp::~CWindowACImp()
{
	
}

BOOL CWindowACImp::IsValidChar(UINT nChar) const
{
	return _istalnum(nChar);
}

BOOL CWindowACImp::HandleKey(AUTOCKEYINFO* pInfo, CString& strText)
{
	BOOL bIsValid = TRUE;
	switch (pInfo->nKey)
	{
	case VK_DELETE:
		break;
	case VK_BACK:
		{
			auto nCaretPos = GetCaretPos();
			if (nCaretPos == pInfo->nPosStartChar + 1)
			{
				pInfo->bClose = TRUE;
				bIsValid = FALSE;
			}
		}
		break;
	default:
		bIsValid = IsValidChar(pInfo->nChar);
		break;
	}
	if (bIsValid)
	{
		GetRangeText(strText, pInfo->nPosStartChar, GetCaretPos());
		if (pInfo->nKey == VK_BACK)
			strText.Delete(strText.GetLength()-1);
		else
			strText.AppendChar(pInfo->nChar);
	}
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
			CString strText;
			BOOL bIsValid = HandleKey(pInfo, strText);
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
	return -1;
}

BOOL CWindowACImp::GetDisplayInfo(AUTOCNMHDR* nmhdr) const
{
	AUTOCDISPINFO* pInfo = (AUTOCDISPINFO*)nmhdr;
	AUTOCITEM& item = pInfo->item;
	if (item.mask & AUTOCITEM::ACIF_TEXT)
	{
		int nMappedItem = item.nItem;
		if (nMappedItem < m_arrFilteredIndices.GetSize())
			nMappedItem = m_arrFilteredIndices[nMappedItem];
		_tcsncpy(item.pszText, GetItemDisplayText(nMappedItem), item.cchTextMax);
	}
	if (item.mask & AUTOCITEM::ACIF_IMAGE)
	{
		item.nImaage = GetItemIconIndex(item.nItem);
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
	// note: PosFromChar returns top-left corner of a given character
	pInfo->posACWndScreen = PosFromChar(pInfo->nPosStartChar);
	m_pEdit->ClientToScreen(&pInfo->posACWndScreen);

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
			pInfo->posACWndScreen.y += abs(posNextPrevLine.y - pInfo->posACWndScreen.y);
		}
		else
		{
			TEXTMETRIC tm;
			CDC* pDC = m_pEdit->GetDC();
			pDC->GetTextMetrics(&tm);
			m_pEdit->ReleaseDC(pDC);
			pInfo->posACWndScreen.y += tm.tmHeight;
		}
	}
	else
	{
		CRect rect;
		m_pEdit->GetWindowRect(rect);
		pInfo->posACWndScreen.y = rect.bottom;
	}
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

void CEditACImp::GetLineText(int nLineIndex, CString& strLine, int nLineLen) const
{
	auto pszLine = strLine.GetBufferSetLength(nLineLen + 1);
	*(LPWORD)pszLine = (WORD)nLineLen;
	GetLine(nLineIndex, (LPTSTR)pszLine);
	strLine.SetAt(nLineLen, _T('\0'));
	strLine.ReleaseBuffer(nLineLen + 1);
}

// CAutoCompleteWnd

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
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnCustomDrawList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnGetListDispInfo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnListItemChange)
	ON_NOTIFY(NM_DBLCLK, IDC_AUTO_LIST_CTRL, &CAutoCompleteWnd::OnListDblClk)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CAutoCompleteWnd message handlers

BOOL CAutoCompleteWnd::Create(CWnd* pOwner, const AUTOCINITINFO& info)
{
	BOOL bCreated = CAutoCompleteWndBase::Create(pOwner, info.posACWndScreen);
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
	if (m_listCtrl)
		return m_listCtrl->GetTopIndex();
	return -1;
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
		// TODO
		//m_listCtrl->EnsureVisible(nCurSelItem, FALSE);
		CSize szScroll(0, m_listCtrl->GetItemHeight()* (nCurSelItem-nOldSelItem));
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
	NotifyOwner(ACCmdComplete, (AUTOCNMHDR*)&info);
	Close();
}

void CAutoCompleteWnd::SetImageList(CImageList* pImageList)
{
	if (m_listCtrl)
	{
		m_listCtrl->SetImageList(pImageList, LVSIL_NORMAL);
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

BOOL CAutoCompleteWnd::Activate(CWnd* pOwner, UINT nChar)
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
	DWORD dwStyle = WS_CHILD | WS_VSCROLL | WS_VISIBLE;
	dwStyle |= LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	// the image list should be reused
	dwStyle |= LVS_SHAREIMAGELISTS;
	dwStyle |= LVS_NOCOLUMNHEADER;
	dwStyle |= LVS_OWNERDATA;
	CRect rect;
	SetRectEmpty(rect);
	BOOL bCreated = m_listCtrl->Create(dwStyle, rect, this, IDC_AUTO_LIST_CTRL);
	if (!bCreated)
		return 1;

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

	listItem.iImage = acItem.nImaage;

	*pResult = 0;
}

void CAutoCompleteWnd::OnListItemChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;
	if ( m_bReady && (pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED) )
	{
		AUTOCNMHDR info = { 0 };
		PrepareNotifyHeader((AUTOCNMHDR*)&info);
		info.wp = (WPARAM)pNMLV->iItem;
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

void CAutoCompleteWnd::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	ENSURE(pNMHDR != NULL);
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

	switch (lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		{
			CDC* pDC = CDC::FromHandle(lplvcd->nmcd.hdc);
			CustomDrawListImpl(pDC, lplvcd);
			*pResult = CDRF_SKIPDEFAULT;
		}
		break;
	}
}

void CAutoCompleteWnd::CustomDrawListImpl(CDC* pDC, LPNMLVCUSTOMDRAW plvcd)
{
	NMLVCUSTOMDRAW lvcd = *plvcd;
	// MSDN says CDIS_SELECTED is not reliable so we need to 'correct' it
	UINT nState = m_listCtrl->GetItemState((int)plvcd->nmcd.dwItemSpec, LVIS_SELECTED);
	BOOL bSelected = nState & LVIS_SELECTED;
	CRect rect;
	int nRow = (int)plvcd->nmcd.dwItemSpec;
	m_listCtrl->GetItemRect(nRow, rect, LVIR_BOUNDS);
	COLORREF clrBkOrig = m_listCtrl->GetBkColor();
	COLORREF clrBk = clrBkOrig;
	if (bSelected)
		clrBk = GetSysColor(COLOR_MENUHILIGHT);

	// Background
	pDC->FillSolidRect(rect, clrBk);

	// Icon
	auto pImageList = m_listCtrl->GetImageList(LVSIL_NORMAL);
	if (pImageList)
	{
		LVITEM item = { 0 };
		item.mask = LVIF_IMAGE;
		VERIFY(m_listCtrl->GetItem(&item));
		int nIcon = item.iImage;
		CRect rcIcon;
		m_listCtrl->GetSubItemRect(nRow, 0, LVIR_ICON, rcIcon);
// 		rcIcon = rect;
// 		rcIcon.left += LIST_ITEM_GAP;
// 		rcIcon.right = rcIcon.left + m_szIcon.cx;
		CPoint pt = rcIcon.TopLeft();
		pt.x += (rcIcon.Width() - m_szIcon.cx) / 2;
		pt.y += (rcIcon.Height() - m_szIcon.cy) / 2;
		UINT nIconStyle = ILD_TRANSPARENT;
		pImageList->Draw(pDC, nIcon, pt, nIconStyle);
	}

	// Label
	OnDrawLabel(pDC, plvcd, nState, rect);
}

void CAutoCompleteWnd::OnDrawLabel(CDC* pDC, LPNMLVCUSTOMDRAW plvcd, UINT nState, CRect& rect, BOOL bCalcOnly)
{
	auto pImageList = m_listCtrl->GetImageList(LVSIL_NORMAL);
	int nRow = (int)plvcd->nmcd.dwItemSpec;
	BOOL bSelected = nState & LVIS_SELECTED;
	m_listCtrl->GetItemRect(nRow, rect, LVIR_LABEL);
// 	m_listCtrl->GetItemRect(nRow, rect, LVIR_BOUNDS);
// 	rect.left += LIST_ITEM_GAP + pImageList ? LIST_ITEM_ICON_LABEL_GAP : 0;
	if (bCalcOnly)
		rect.right = LONG_MAX;
	CString str = m_listCtrl->GetItemText(nRow, plvcd->iSubItem);
	UINT nFormat = DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX | DT_VCENTER;
	int nOldBKMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clrText = bSelected ? GetSysColor(COLOR_HIGHLIGHTTEXT) : m_listCtrl->GetTextColor();
	auto clrOldText = pDC->SetTextColor(clrText);
	if (bCalcOnly)
		nFormat |= DT_CALCRECT;
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

	info.nItemCount = -1;
	info.nPreSelectItem = m_listCtrl->GetCurSel();
	info.bEatKey = FALSE;
	info.bClose = TRUE;
	NotifyOwner(ACCmdKey, (AUTOCNMHDR*)&info);
	if (info.bClose || info.nItemCount == 0)
	{
		Close();
	}
	else
	{
		UpdateListItemCount(info.nItemCount);
		m_listCtrl->SetCurSel(info.nPreSelectItem < info.nItemCount ? info.nPreSelectItem : m_listCtrl->GetTopIndex());
	}
	return info.bEatKey;
}

void CAutoCompleteWnd::Close()
{
	CAutoCompleteWndBase::Close();
}

void CAutoCompleteWnd::UpdateListItemCount(int nItemCount)
{
	BOOL bShowVScroll = nItemCount > m_infoInit.nMaxVisibleItems;
	m_listCtrl->SetShowVScrollBar(bShowVScroll);
	SetItemCount(nItemCount);
	RecalcSizeToFitList();
}

void CAutoCompleteWnd::RecalcSizeToFitList()
{
	int nItemCount = m_listCtrl->GetItemCount();
	int nTopItemIndex = GetTopIndex();
	//m_nMaxItemWidth = 0;
	m_nVisibleItems = min(m_infoInit.nMaxVisibleItems, nItemCount);
	CClientDC dc(m_listCtrl);
	CRect rectLabel;
	for (int nItem = nTopItemIndex; nItem < nTopItemIndex + m_nVisibleItems; ++nItem)
	{
		NMLVCUSTOMDRAW lvcd = { 0 };
		lvcd.nmcd.dwItemSpec = nItem;
		OnDrawLabel(&dc, &lvcd, 0, rectLabel, TRUE);
		m_nMaxItemWidth = max(m_nMaxItemWidth, rectLabel.right);
	}

	int nVertSpacing = m_listCtrl->GetItemHeight();
	CPoint posWnd(0,0);
	CSize szWnd;
	szWnd.cx = m_nMaxItemWidth; // + GetSystemMetrics(SM_CYVSCROLL);
	szWnd.cy = nVertSpacing * m_nVisibleItems;
	UINT nFlags = SWP_NOACTIVATE;
	BOOL bVisible = IsWindowVisible();
	if (bVisible)
	{
		nFlags |= SWP_NOMOVE;
	}
	else
	{
		nFlags |= SWP_SHOWWINDOW;
		posWnd = m_infoInit.posACWndScreen;
		posWnd.x -= rectLabel.left;
	}
	SetWindowPos(&wndTop, posWnd.x, posWnd.y, szWnd.cx, szWnd.cy, nFlags);
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
