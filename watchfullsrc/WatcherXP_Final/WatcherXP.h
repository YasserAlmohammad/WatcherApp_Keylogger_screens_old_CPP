// WatcherXP.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "WatcherXPDlg.h"

// CWatcherXPApp:
// See WatcherXP.cpp for the implementation of this class
//

class CWatcherXPApp : public CWinApp
{
public:
	CWatcherXPApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
	CWatcherXPDlg* mainDlg;
	DECLARE_MESSAGE_MAP()

	
	virtual int ExitInstance();
};

extern CWatcherXPApp theApp;