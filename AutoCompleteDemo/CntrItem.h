
// CntrItem.h : interface of the CAutoCompleteDemoCntrItem class
//

#pragma once

class CAutoCompleteDemoRichEditDoc;
class CAutoCompleteDemoRichEditView;

class CAutoCompleteDemoCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CAutoCompleteDemoCntrItem)

// Constructors
public:
	CAutoCompleteDemoCntrItem(REOBJECT* preo = NULL, CAutoCompleteDemoRichEditDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer

// Attributes
public:
	CAutoCompleteDemoRichEditDoc* GetDocument()
		{ return reinterpret_cast<CAutoCompleteDemoRichEditDoc*>(CRichEditCntrItem::GetDocument()); }
	CAutoCompleteDemoRichEditView* GetActiveView()
		{ return reinterpret_cast<CAutoCompleteDemoRichEditView*>(CRichEditCntrItem::GetActiveView()); }

// Implementation
public:
	~CAutoCompleteDemoCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

