#pragma once

#ifndef SYNCPOPUPFRAMEIMPL_EXT_CLASS
	#define SYNCPOPUPFRAMEIMPL_EXT_CLASS
#endif

class SYNCPOPUPFRAMEIMPL_EXT_CLASS CSyncPopupFrameImpl
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

