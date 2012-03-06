// Snake HMI.h : PROJECT_NAME 應用程式的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號


// CSnakeHMIApp:
// 請參閱實作此類別的 Snake HMI.cpp
//

class CSnakeHMIApp : public CWinApp
{
public:
	CSnakeHMIApp();

// 覆寫
	public:
	virtual BOOL InitInstance();

// 程式碼實作

	DECLARE_MESSAGE_MAP()
};

extern CSnakeHMIApp theApp;