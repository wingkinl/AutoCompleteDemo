// AutoCompleteDemoFormDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteDemoFormDoc.h"


// CAutoCompleteDemoFormDoc

IMPLEMENT_DYNCREATE(CAutoCompleteDemoFormDoc, CDocument)

CAutoCompleteDemoFormDoc::CAutoCompleteDemoFormDoc()
{
}

BOOL CAutoCompleteDemoFormDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CAutoCompleteDemoFormDoc::~CAutoCompleteDemoFormDoc()
{
}


BEGIN_MESSAGE_MAP(CAutoCompleteDemoFormDoc, CDocument)
END_MESSAGE_MAP()


// CAutoCompleteDemoFormDoc diagnostics

#ifdef _DEBUG
void CAutoCompleteDemoFormDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoCompleteDemoFormDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CAutoCompleteDemoFormDoc serialization

void CAutoCompleteDemoFormDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif


// CAutoCompleteDemoFormDoc commands
