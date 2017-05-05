// AutoCompleteWnd.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteWnd.h"
#include "AutoCompleteListCtrl.h"

CEditACImp::CEditACImp(CEdit* pEdit)
{

}

CEditACImp::~CEditACImp()
{

}

BOOL CEditACImp::GetInitInfo(POINT& pos, CString& strWordBegin) const
{
	int nSelStartChar = 0, nSelEndChar = 0;
	m_pEdit->GetSel(nSelStartChar, nSelEndChar);

	int nCurLine = m_pEdit->LineFromChar(-1);
	int nCurLineStartChar = m_pEdit->LineIndex(nCurLine);
	
	ASSERT(nSelStartChar > nCurLineStartChar);

	int nLineLen = m_pEdit->LineLength(nCurLine);
	CString strLine;
	auto pszLine = strLine.GetBuffer(nLineLen);
	m_pEdit->GetLine(nCurLine, (LPTSTR)pszLine, nLineLen);
	strLine.ReleaseBuffer();

	int nCurCharOffset = nSelStartChar - nCurLineStartChar - 1;
	for (int nCharPos = nCurCharOffset; nCharPos >= 0; --nCharPos)
	{
		if ( !IsValidChar(pszLine[nCharPos]) )
			break;
		strWordBegin.Insert(0, pszLine[nCharPos]);
	}
	m_pEdit->ClientToScreen(&pos);
	return !strWordBegin.IsEmpty();
}


BOOL CEditACImp::IsValidChar(UINT nChar) const
{
	return _istalnum(nChar);
}

// CAutoCompleteWnd

IMPLEMENT_DYNAMIC(CAutoCompleteWnd, CAutoCompleteWndBase)

CAutoCompleteWnd::CAutoCompleteWnd()
{
	ASSERT(!GetActiveInstance());
	s_pInstance = this;
	m_listCtrl = nullptr;
	m_bDropRestOfWord = true;
}

CAutoCompleteWnd::~CAutoCompleteWnd()
{
	ASSERT(GetActiveInstance());
	s_pInstance = nullptr;
}

BEGIN_MESSAGE_MAP(CAutoCompleteWnd, CAutoCompleteWndBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CAutoCompleteWnd message handlers

BOOL CAutoCompleteWnd::Create(CWnd* pOwner, POINT pos)
{
	BOOL bCreated = CAutoCompleteWndBase::Create(pOwner, pos);
	if (!bCreated)
		return FALSE;
	SetOwner(pOwner);
	return bCreated;
}

void CAutoCompleteWnd::SetItemCount(int nItems)
{
	if (m_listCtrl)
		m_listCtrl->SetItemCount(nItems);
}

int CAutoCompleteWnd::GetItemCount() const
{
	if (m_listCtrl)
		return m_listCtrl->GetItemCount();
	return -1;
}

void CAutoCompleteWnd::SetImageList(CImageList* pImageList)
{
	if (m_listCtrl)
		m_listCtrl->SetImageList(pImageList, LVSIL_NORMAL);
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
			GetActiveInstance()->UpdateList(nChar);
			return TRUE;
		}
		// this is weird, the window should have been deactivated already,
		// better to investigate how is this possible
		ASSERT(0);
		GetActiveInstance()->Close();
	}

	AUTOCINITINFO info = {0};
	if ( !NotifyOwner(pOwner, ACCmdGetInitInfo, 0, (LPARAM)&info) || info.nListItems <= 0 )
		return FALSE;
	CAutoCompleteWnd* pACWnd = new CAutoCompleteWnd;
	BOOL bCreated = pACWnd->Create(pOwner, info.posScreen);
	if (bCreated)
	{
		if (info.pImageList)
			pACWnd->SetImageList(info.pImageList);
		pACWnd->SetItemCount(info.nListItems);
	}
	return bCreated;
}

BOOL CAutoCompleteWnd::Cancel()
{
	if (GetActiveInstance())
		GetActiveInstance()->Close();
	return TRUE;
}

LRESULT CAutoCompleteWnd::NotifyOwner(ACCmd cmd, WPARAM wp, LPARAM lp)
{
	auto pWndOwner = GetOwner();
	if (pWndOwner->GetSafeHwnd())
	{
		return NotifyOwner(pWndOwner, cmd, wp, lp);
	}
	return 0;
}

LRESULT CAutoCompleteWnd::NotifyOwner(CWnd* pWndOwner, ACCmd cmd, WPARAM wp, LPARAM lp)
{
	AUTOCNMHDR nmhdr = {0};
	nmhdr.wp = wp;
	nmhdr.lp = lp;
	return pWndOwner->SendMessage(WM_AC_NOTIFY, (WPARAM)cmd, (LPARAM)&nmhdr);
}

CAutoCompleteListCtrl* CAutoCompleteWnd::CreateListCtrl()
{
	return new CAutoCompleteListCtrl;
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
	BOOL bCreated = m_listCtrl->Create(dwStyle, rect, this, 1);
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

BOOL CAutoCompleteWnd::OnKey(UINT nChar)
{
	if (!m_listCtrl->GetSafeHwnd())
	{
		ASSERT(0);
		Close();
		return FALSE;
	}
	switch (nChar)
	{
	case VK_ESCAPE:
		Close();
		return TRUE;
	case VK_UP:
		m_listCtrl->MoveSelection(-1);
		return TRUE;
	case VK_DOWN:
		m_listCtrl->MoveSelection(1);
		return TRUE;
	case VK_PRIOR:
		m_listCtrl->MoveSelection(-m_listCtrl->GetVisibleRows());
		return TRUE;
	case VK_NEXT:
		m_listCtrl->MoveSelection(m_listCtrl->GetVisibleRows());
		return TRUE;
	case VK_TAB:
		return TRUE;
	case VK_RETURN:
		return TRUE;
	case VK_BACK:
		return TRUE;
	case VK_CONTROL:
		return TRUE;
	}
	
	if ('A' <= nChar && nChar <= 'Z')
	{
		if (GetKeyState(VK_SHIFT) < 0 
			|| GetKeyState(VK_CONTROL) < 0 
			|| GetKeyState(VK_MENU) < 0
			)
		{
			Close();
			return FALSE;
		}
		return TRUE;
	}
	Close();
	return FALSE;
}

void CAutoCompleteWnd::Close()
{
	CAutoCompleteWndBase::Close();
}

void CAutoCompleteWnd::UpdateList(UINT nChar)
{
	NotifyOwner(ACCmdUpdateList);
}

LRESULT CAutoCompleteWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		return OnKey((UINT)wParam) ? 0 : -1;
	case WM_SYSKEYDOWN:
		Close();
		return -1;
	}
	return CAutoCompleteWndBase::WindowProc(message, wParam, lParam);
}

