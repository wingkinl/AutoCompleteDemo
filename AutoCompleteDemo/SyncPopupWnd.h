#pragma once

#define _AC_DONT_USE_MFC_FP

#ifndef _AC_DONT_USE_MFC_FP
// implementation base on MFC feature pack
typedef CMFCPopupMenu	CSyncMFCFPPopupWndBase;

class CSyncMFCFPPopupWnd : public CSyncMFCFPPopupWndBase
{
	DECLARE_DYNAMIC(CSyncMFCFPPopupWnd)
public:
	CSyncMFCFPPopupWnd();
	virtual ~CSyncMFCFPPopupWnd();
public:
	virtual BOOL Create(CWnd* pOwner, POINT pos);
protected:
	virtual CSize CalcSize() const = 0;

	BOOL GetChildClientRect(CRect& rect);
protected:
	class CSyncMFCPopupMenuBar : public CMFCPopupMenuBar
	{
	public:
		CSize CalcSize(BOOL bVertDock) override;

		BOOL Create(CWnd* pParentWnd, DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE, UINT nID = AFX_IDW_TOOLBAR) override;
	};
	CSyncMFCPopupMenuBar m_wndSyncMenuBar;

	CMFCPopupMenuBar* GetMenuBar() override { return &m_wndSyncMenuBar; }
};

#define CSyncPopupWndBase	CSyncMFCFPPopupWnd
#else
// implementation that does not rely on MFC feature pack
typedef CWnd	CSyncNonMFCFPPopupWndBase;

class CSyncNonMFCFPPopupWnd : public CWnd
{
	DECLARE_DYNAMIC(CSyncNonMFCFPPopupWnd)
public:
	CSyncNonMFCFPPopupWnd();
	virtual ~CSyncNonMFCFPPopupWnd();
public:
	virtual BOOL Create(CWnd* pOwner, POINT pos);
protected:
	virtual CSize CalcSize() const = 0;
	BOOL GetChildClientRect(CRect& rect);
protected:
	void PostNcDestroy() override;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
protected:
	DECLARE_MESSAGE_MAP()
};

#define CSyncPopupWndBase	CSyncNonMFCFPPopupWnd
#endif // !_AC_DONT_USE_MFC_FP


class CSyncPopupWnd : public CSyncPopupWndBase
{
	DECLARE_DYNAMIC(CSyncPopupWnd)
public:
	CSyncPopupWnd();
	~CSyncPopupWnd();
public:
	CSize CalcSize() const override;
};