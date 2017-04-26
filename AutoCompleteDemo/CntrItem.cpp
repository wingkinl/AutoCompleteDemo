
// CntrItem.cpp : implementation of the CAutoCompleteDemoCntrItem class
//

#include "stdafx.h"
#include "AutoCompleteDemo.h"

#include "AutoCompleteDemoRichEditDoc.h"
#include "AutoCompleteDemoRichEditView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoCompleteDemoCntrItem implementation

IMPLEMENT_SERIAL(CAutoCompleteDemoCntrItem, CRichEditCntrItem, 0)

CAutoCompleteDemoCntrItem::CAutoCompleteDemoCntrItem(REOBJECT* preo, CAutoCompleteDemoRichEditDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
}

CAutoCompleteDemoCntrItem::~CAutoCompleteDemoCntrItem()
{
	// TODO: add cleanup code here
}


// CAutoCompleteDemoCntrItem diagnostics

#ifdef _DEBUG
void CAutoCompleteDemoCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CAutoCompleteDemoCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

