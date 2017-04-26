// AutoCompleteDemoScintillaDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AutoCompleteDemoScintillaDoc.h"


// CAutoCompleteDemoScintillaDoc

IMPLEMENT_DYNCREATE(CAutoCompleteDemoScintillaDoc, CDocument)

CAutoCompleteDemoScintillaDoc::CAutoCompleteDemoScintillaDoc()
{
}

BOOL CAutoCompleteDemoScintillaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CAutoCompleteDemoScintillaDoc::~CAutoCompleteDemoScintillaDoc()
{
}


BEGIN_MESSAGE_MAP(CAutoCompleteDemoScintillaDoc, CDocument)
END_MESSAGE_MAP()


// CAutoCompleteDemoScintillaDoc diagnostics

#ifdef _DEBUG
void CAutoCompleteDemoScintillaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoCompleteDemoScintillaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CAutoCompleteDemoScintillaDoc serialization

void CAutoCompleteDemoScintillaDoc::Serialize(CArchive& ar)
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


// CAutoCompleteDemoScintillaDoc commands
