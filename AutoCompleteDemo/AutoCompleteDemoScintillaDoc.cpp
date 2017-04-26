// AutoCompleteDemoScintillaDoc.cpp : implementation file
//

#include "stdafx.h"
#ifdef _ENABLE_SCINTILLA_BUILD
#include "AutoCompleteDemoScintillaDoc.h"


// CAutoCompleteDemoScintillaDoc

IMPLEMENT_DYNCREATE(CAutoCompleteDemoScintillaDoc, CAutoCompleteDemoScintillaDocBase)

CAutoCompleteDemoScintillaDoc::CAutoCompleteDemoScintillaDoc()
{
}

BOOL CAutoCompleteDemoScintillaDoc::OnNewDocument()
{
	if (!CAutoCompleteDemoScintillaDocBase::OnNewDocument())
		return FALSE;
	return TRUE;
}

CAutoCompleteDemoScintillaDoc::~CAutoCompleteDemoScintillaDoc()
{
}


BEGIN_MESSAGE_MAP(CAutoCompleteDemoScintillaDoc, CAutoCompleteDemoScintillaDocBase)
END_MESSAGE_MAP()


// CAutoCompleteDemoScintillaDoc diagnostics

#ifdef _DEBUG
void CAutoCompleteDemoScintillaDoc::AssertValid() const
{
	CAutoCompleteDemoScintillaDocBase::AssertValid();
}

#ifndef _WIN32_WCE
void CAutoCompleteDemoScintillaDoc::Dump(CDumpContext& dc) const
{
	CAutoCompleteDemoScintillaDocBase::Dump(dc);
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


#endif // _ENABLE_SCINTILLA_BUILD
// THE END OF FILE
//////////////////////////////////////////////////////////////////////////