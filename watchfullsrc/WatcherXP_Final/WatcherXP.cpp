// WatcherXP.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WatcherXP.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWatcherXPApp

BEGIN_MESSAGE_MAP(CWatcherXPApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWatcherXPApp construction

CWatcherXPApp::CWatcherXPApp()
:mainDlg(NULL)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CWatcherXPApp object

CWatcherXPApp theApp;


// CWatcherXPApp initialization

BOOL CWatcherXPApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	mainDlg=new CWatcherXPDlg();
	mainDlg->Create(IDD_WATCHERXP_DIALOG);
	m_pMainWnd = &(*mainDlg);
	mainDlg->SetWindowPos(NULL,100,100,600,400,SWP_NOZORDER);
	if(mainDlg->options.isVisible)
		mainDlg->ShowWindow(SW_HIDE);
	else
		mainDlg->ShowWindow(SW_SHOW);
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}

int CWatcherXPApp::ExitInstance()
{
	if(mainDlg!=NULL)
		delete mainDlg;
	mainDlg=NULL;
	return CWinApp::ExitInstance();
}
