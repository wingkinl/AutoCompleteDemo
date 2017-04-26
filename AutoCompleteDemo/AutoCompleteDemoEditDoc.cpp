// AutoCompleteDemoEditDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteDemoEditDoc.h"


// CAutoCompleteDemoEditDoc

IMPLEMENT_DYNCREATE(CAutoCompleteDemoEditDoc, CDocument)

CAutoCompleteDemoEditDoc::CAutoCompleteDemoEditDoc()
{
}

BOOL CAutoCompleteDemoEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CAutoCompleteDemoEditDoc::~CAutoCompleteDemoEditDoc()
{
}


BEGIN_MESSAGE_MAP(CAutoCompleteDemoEditDoc, CDocument)
END_MESSAGE_MAP()


// CAutoCompleteDemoEditDoc diagnostics

#ifdef _DEBUG
void CAutoCompleteDemoEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoCompleteDemoEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CAutoCompleteDemoEditDoc serialization

void CAutoCompleteDemoEditDoc::Serialize(CArchive& ar)
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


// CAutoCompleteDemoEditDoc commands
