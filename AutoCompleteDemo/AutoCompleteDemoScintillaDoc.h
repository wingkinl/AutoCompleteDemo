#pragma once

#ifdef _ENABLE_SCINTILLA_BUILD
#include "ScintillaWrapper\ScintillaDocView.h"
// CAutoCompleteDemoScintillaDoc document

typedef CScintillaDoc	CAutoCompleteDemoScintillaDocBase;

class CAutoCompleteDemoScintillaDoc : public CAutoCompleteDemoScintillaDocBase
{
	DECLARE_DYNCREATE(CAutoCompleteDemoScintillaDoc)

public:
	CAutoCompleteDemoScintillaDoc();
	virtual ~CAutoCompleteDemoScintillaDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};


#endif // _ENABLE_SCINTILLA_BUILD