#pragma once

#ifndef SYNCPOPUPWND_EXT_CLASS
	#define SYNCPOPUPWND_EXT_CLASS
#endif

typedef CWnd	CSyncPopupWndBase;

class SYNCPOPUPWND_EXT_CLASS CSyncPopupWnd : public CWnd
{
	DECLARE_DYNAMIC(CSyncPopupWnd)
public:
	CSyncPopupWnd();
	virtual ~CSyncPopupWnd();
public:
	virtual BOOL Create(CWnd* pOwner, POINT pos);

	BOOL HitTest(const POINT& ptScreen) const;
	virtual BOOL ProcessMouseClick(UINT uiMsg, POINT pt, HWND hwnd);

	virtual void Close();
public:
	static CSyncPopupWnd* GetActivePopup() { return m_pActivePopupWnd; }
	static CSyncPopupWnd* GetSafeActivePopup();
protected:
	void PostNcDestroy() override;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
protected:
	DECLARE_MESSAGE_MAP()

	static CSyncPopupWnd* m_pActivePopupWnd;
};
