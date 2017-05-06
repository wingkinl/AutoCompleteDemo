// AutoCompleteDemoFormView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AutoCompleteDemoFormView.h"
#include "AutoCompleteWnd.h"
#include "AutoCompleteDemo.h"

CDemoListCtrl::CDemoListCtrl()
{

}

CDemoListCtrl::~CDemoListCtrl()
{

}

IMPLEMENT_DYNCREATE(CDemoListCtrl, CMFCListCtrl)

BEGIN_MESSAGE_MAP(CDemoListCtrl, CMFCListCtrl)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, &OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &OnEndLabelEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CDemoListCtrl::InitList()
{
	ModifyStyle(0, LVS_SHOWSELALWAYS);
	SetExtendedStyle(LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);
	InsertColumn(0, _T("Name"));
	InsertColumn(1, _T("Age"));

	InsertItem(0, _T("Tom"));
	SetItemText(0, 1, _T("12"));
	InsertItem(1, _T("John"));
	SetItemText(1, 1, _T("11"));
	InsertItem(2, _T("Emily"));
	SetItemText(2, 1, _T("10"));
	InsertItem(3, _T("Mike"));
	SetItemText(3, 1, _T("13"));

	SetColumnWidth(0, 100);
	SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
}

void CDemoListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_F2)
	{
		int nItem = GetNextItem(-1, LVNI_SELECTED);
		if (nItem >= 0)
			EditLabel(nItem);
	}
	CMFCListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDemoListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	CEdit* pEdit = GetEditControl();
	ASSERT(pEdit);
	CDemoEdit* pEditNew = new CDemoEdit;
	pEditNew->m_bAutoDelete = true;
	pEditNew->SubclassWindow(pEdit->GetSafeHwnd());
}

void CDemoListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = TRUE;
}

void CDemoListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nItem = HitTest(point);
	if (nItem >= 0)
	{
		EditLabel(nItem);
		return;
	}
	CMFCListCtrl::OnLButtonDblClk(nFlags, point);
}

/************************************************************************/
/* CDemoEdit
/************************************************************************/
CDemoEdit::CDemoEdit()
{
	m_bAutoDelete = false;
}

CDemoEdit::~CDemoEdit()
{

}

void CDemoEdit::PostNcDestroy()
{
	if (m_bAutoDelete)
		delete this;
}

IMPLEMENT_DYNCREATE(CDemoEdit, CEdit)

BEGIN_MESSAGE_MAP(CDemoEdit, CEdit)
	ON_WM_CHAR()
	ON_MESSAGE(WM_AC_NOTIFY, &CDemoEdit::OnACNotify)
END_MESSAGE_MAP()

void CDemoEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
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

BOOL CDemoEdit::GetACInitInfo(AUTOCNMHDR* nmhdr)
{
	AUTOCINITINFO* pInfo = (AUTOCINITINFO*)nmhdr;

	CDemoEditACImp acImp;
	acImp.m_pEdit = (CEdit*)this;
	BOOL bRet = acImp.GetInitInfo(pInfo);
	if (bRet)
	{
		pInfo->nListItems = UpdateFilteredList((LPCTSTR)pInfo->strStart);
	}
	return bRet;
}

BOOL CDemoEdit::GetACDisplayInfo(AUTOCNMHDR* nmhdr) const
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

BOOL CDemoEdit::HandleKey(AUTOCNMHDR* nmhdr)
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

BOOL CDemoEdit::AutoComplete(AUTOCNMHDR* nmhdr)
{
	AUTOCCOMPLETE* pInfo = (AUTOCCOMPLETE*)nmhdr;
	CDemoEditACImp acImp;
	acImp.m_pEdit = (CEdit*)this;
	acImp.AutoComplete(pInfo);
	return TRUE;
}

int CDemoEdit::UpdateFilteredList(LPCTSTR pszText)
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
				if (pszItemText++ >= pszItemText + strItemText.GetLength()-1)
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

LRESULT CDemoEdit::OnACNotify(WPARAM wp, LPARAM lp)
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

// CAutoCompleteDemoFormView

IMPLEMENT_DYNCREATE(CAutoCompleteDemoFormView, CFormView)

CAutoCompleteDemoFormView::CAutoCompleteDemoFormView()
	: CFormView(IDD_AUTOCOMPLETEDEMOFORMVIEW)
{

}

CAutoCompleteDemoFormView::~CAutoCompleteDemoFormView()
{
}

void CAutoCompleteDemoFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SINGLELINE, m_editSingleLine);
	DDX_Control(pDX, IDC_EDIT_MULTILINE, m_editMultiLine);
	DDX_Control(pDX, IDC_LIST1, m_listctrl);
}

BEGIN_MESSAGE_MAP(CAutoCompleteDemoFormView, CFormView)
END_MESSAGE_MAP()

BOOL CAutoCompleteDemoFormView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CAutoCompleteDemoFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();
	m_listctrl.InitList();
}

// CAutoCompleteDemoFormView diagnostics

#ifdef _DEBUG
void CAutoCompleteDemoFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoCompleteDemoFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAutoCompleteDemoFormView message handlers

