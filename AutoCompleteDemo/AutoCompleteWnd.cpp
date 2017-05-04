// AutoCompleteWnd.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteWnd.h"
#include "AutoCompleteListCtrl.h"


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

CAutoCompleteWnd* CAutoCompleteWnd::s_pInstance = nullptr;

CAutoCompleteWnd* CAutoCompleteWnd::GetActiveInstance()
{
	return s_pInstance;
}

BOOL CAutoCompleteWnd::Activate(CWnd* pOwner, UINT nChar)
{
	if ( GetActiveInstance() )
	{
	#ifdef _DEBUG
		auto pActualOwner = GetActiveInstance()->GetOwner();
		ASSERT(pActualOwner->GetSafeHwnd() == pOwner->GetSafeHwnd());
	#endif // _DEBUG
		return TRUE;
	}
	CAutoCompleteWnd* pACWnd = new CAutoCompleteWnd;
	POINT pos = {0};
	if ( !pACWnd->GetInitPosition(pOwner, pos) )
		pACWnd->NotifyOwner(ACCmdGetInitPos, 0, (LPARAM)&pos);
	return pACWnd->Create(pOwner, pos);
}

BOOL CAutoCompleteWnd::Cancel()
{
	if (!GetActiveInstance())
		return TRUE;
	return GetActiveInstance()->DestroyWindow();
}

LRESULT CAutoCompleteWnd::NotifyOwner(ACCmd cmd, WPARAM wp, LPARAM lp)
{
	auto pWndOwner = GetOwner();
	if (pWndOwner->GetSafeHwnd())
	{
		AUTOCNMHDR nmhdr = {0};
		nmhdr.wp = wp;
		nmhdr.lp = lp;
		return pWndOwner->SendMessage(WM_AC_NOTIFY, (WPARAM)cmd, (LPARAM)&nmhdr);
	}
	return 0;
}

BOOL CAutoCompleteWnd::GetInitPosition(CWnd* pOwner, POINT& pos) const
{
	ASSERT(pOwner->GetSafeHwnd());
#define WC_SCINTILLA _T("Scintilla")
	enum { MAX_CLASS_NAME = 256 };
	TCHAR szClassName[MAX_CLASS_NAME] = {0};
	GetClassName(pOwner->GetSafeHwnd(), szClassName, _countof(szClassName));
	if (_tcsnicmp(szClassName, WC_EDIT, MAX_CLASS_NAME) == 0)
	{
		return GetInitPositionFromEdit(pOwner, pos);
	}
	else if (_tcsnicmp(szClassName, WC_SCINTILLA, MAX_CLASS_NAME) == 0)
	{
		return GetInitPositionFromScintilla(pOwner, pos);
	}
	return FALSE;
}

BOOL CAutoCompleteWnd::GetInitPositionFromEdit(CWnd* pOwner, POINT& pos) const
{
	CEdit* pEdit = (CEdit*)pOwner;
	CPoint posCaret = pEdit->GetCaretPos();
	int nCurLine = pEdit->LineFromChar(-1);
	int nCharIndexCurLine = pEdit->LineIndex(nCurLine + 1);
	pos = pEdit->PosFromChar(nCharIndexCurLine);
	pos.x = posCaret.x;
	pEdit->ClientToScreen(&pos);
	return TRUE;
}

BOOL CAutoCompleteWnd::GetInitPositionFromScintilla(CWnd* pOwner, POINT& pos) const
{
	return FALSE;
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

	m_strValidChars.Empty();

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
	if ('A' <= nChar && nChar <= 'Z' || m_strValidChars.Find((TCHAR)nChar) >= 0 )
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