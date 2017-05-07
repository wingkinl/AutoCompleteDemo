#pragma once

struct AUTOCNMHDR;

class CDemoACEdit : public CEdit
{
	DECLARE_DYNCREATE(CDemoACEdit)
public:
	CDemoACEdit();
	~CDemoACEdit();
public:
	bool m_bAutoDelete;
protected:
	void PostNcDestroy() override;

	BOOL GetACInitInfo(AUTOCNMHDR* nmhdr);
	BOOL GetACDisplayInfo(AUTOCNMHDR* nmhdr) const;
	BOOL HandleKey(AUTOCNMHDR* nmhdr);
	BOOL AutoComplete(AUTOCNMHDR* nmhdr);
protected:
	int UpdateFilteredList(LPCTSTR pszText);
protected:
	CArray<int>		m_arrFilteredIndices;
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnACNotify(WPARAM wp, LPARAM lp);
protected:
	DECLARE_MESSAGE_MAP()
};
