#include "stdafx.h"
#include "AutoCompleteDemoEdit.h"

/************************************************************************/
/* CDemoACEdit
/************************************************************************/

CDemoACEdit::CDemoACEdit()
{
	m_bAutoDelete = false;
	m_acImp.m_pEdit = this;
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

LRESULT CDemoACEdit::OnACNotify(WPARAM wp, LPARAM lp)
{
	return m_acImp.OnACNotify(wp, lp);
}
