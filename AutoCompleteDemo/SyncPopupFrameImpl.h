#pragma once

class CSyncPopupFrameImpl
{
public:
	CSyncPopupFrameImpl(CFrameWnd* pFrame);
	virtual ~CSyncPopupFrameImpl();
public:
	BOOL PreTranslateMessage(MSG* pMsg);
protected:
	BOOL ProcessKeyboard(UINT message, int nKey);
	BOOL ProcessMouseClick(UINT uiMsg, POINT pt, HWND hwnd);
};

