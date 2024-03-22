// Options.cpp : implementation file
//

#include "stdafx.h"
#include "WatcherXP.h"
#include "Options.h"


// Options dialog

//IMPLEMENT_DYNAMIC(Options,CDialog)
Options::Options(CWnd* pParent /*=NULL*/)
	: CDialog(Options::IDD, pParent)
	, startUp(FALSE)
	, passwordEnable(FALSE)
	, password(_T(""))
	, passwordAgain(_T(""))
	, KeyStroke(TRUE)
	, WebAddress(TRUE)
	, WindowTitle(FALSE)
	, FileFolder(FALSE)
	, ScreenShots(FALSE)
	
	, isVisible(FALSE)
	, elapsed(60)
	, autoStart(TRUE)
	, lang(0)
	, passMsg(_T(""))
	, hotkey(0x41) //A
	, hotIndex(0)
{
	//this->loadConfig();
}

Options::~Options()
{
}

void Options::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, startUp);
	DDX_Check(pDX, IDC_CHECK3, passwordEnable);
	DDX_Text(pDX, IDC_EDIT1, password);
	DDX_Text(pDX, IDC_EDIT2, passwordAgain);
	DDX_Check(pDX, IDC_CHECK6, KeyStroke);
	DDX_Check(pDX, IDC_CHECK10, WebAddress);
	DDX_Check(pDX, IDC_CHECK7, WindowTitle);
	DDX_Check(pDX, IDC_CHECK8, FileFolder);
	DDX_Check(pDX, IDC_CHECK9, ScreenShots);
	DDX_Check(pDX, IDC_CHECK11, isVisible);
	DDX_Text(pDX, IDC_EDIT3, elapsed);
	DDX_Check(pDX, IDC_CHECK2, autoStart);
	DDX_Control(pDX, IDC_HOT, hotList);
}


BEGIN_MESSAGE_MAP(Options, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHOOSE_FOLDERS, OnBnClickedChooseFolders)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// Options message handlers

void Options::OnBnClickedButton1()
{
	//view Folder viewer
}

void Options::OnBnClickedCancel()
{
	OnCancel();
}

void Options::OnBnClickedOk()
{
	UpdateData();
	if(passwordEnable &&(password!=passwordAgain)){
		MessageBox(passMsg);
		password=_T("");
		passwordAgain=_T("");
		passwordEnable=false;
		UpdateData(FALSE);
		return;
	}

	hotIndex=hotList.GetCurSel();
	hotkey=hotList.GetItemData(hotIndex);
	OnOK();
}


// set arabic UI
void Options::setUI_EN(void)
{
	this->SetWindowText(_T("options"));
	this->SetDlgItemText(IDC_CHECK2,_T("auto start"));
	this->SetDlgItemText(IDC_CHECK1,_T("start when windows start") );
	this->SetDlgItemText(IDC_CHECK11,_T("start in hidden mode"));
	this->SetDlgItemText(IDC_CHECK3,_T("password protection"));
	this->SetDlgItemText(IDC_STATIC_ENTER,_T("eneter password"));
	this->SetDlgItemText(IDC_STATIC_CONFIRM,_T("confirm password"));
	this->SetDlgItemText(IDC_STATIC_HOT,_T("hot key setting :      ALT-CTRL-SHIFT"));	
	this->SetDlgItemText(IDC_STATIC_WATCH,_T("watch what?"));
	this->SetDlgItemText(IDC_CHECK6,_T("keystrokes"));
	this->SetDlgItemText(IDC_CHECK10,_T("I.E. web addresses"));
	this->SetDlgItemText(IDC_CHECK7,_T("window titles"));
	this->SetDlgItemText(IDC_CHECK8,_T("file folder changes"));
	this->SetDlgItemText(IDC_CHECK9,_T("screen shots"));
	this->SetDlgItemText(IDOK,_T("ok"));
	this->SetDlgItemText(IDC_CHOOSE_FOLDERS,_T("choose folders"));
	this->SetDlgItemText(IDCANCEL,_T("cancel"));
	this->SetDlgItemText(IDC_STATIC_SECOND,_T("second"));
	passMsg=_T("enter password correctly");

}

void Options::setUI_AR(void)
{
	strList.RemoveAll();
	this->SetWindowText(loadStr(IDS_VIEW_OPTIONS));
	this->SetDlgItemText(IDC_CHECK2,loadStr(IDS_OPTIONS_AUTOSTART));
	this->SetDlgItemText(IDC_CHECK1,loadStr(IDS_OPTIONS_STARTUP) );
	this->SetDlgItemText(IDC_CHECK11,loadStr(IDS_OPTIONS_AUTOHIDE));
	this->SetDlgItemText(IDC_CHECK3,loadStr(IDS_OPTIONS_PASS));
	this->SetDlgItemText(IDC_STATIC_ENTER,loadStr(IDS_OPTIONS_ENTERPASS));
	this->SetDlgItemText(IDC_STATIC_CONFIRM,loadStr(IDS_OPTIONS_CONFIRM));
	this->SetDlgItemText(IDC_STATIC_WATCH,loadStr(IDS_OPTIONS_WHAT));
	this->SetDlgItemText(IDC_STATIC_HOT,loadStr(IDS_OPTIONS_HOTKEY));	
	this->SetDlgItemText(IDC_CHECK6,loadStr(IDS_OPTIONS_KB));
	this->SetDlgItemText(IDC_CHECK10,loadStr(IDS_OPTIONS_IE));
	this->SetDlgItemText(IDC_CHECK7,loadStr(IDS_OPTIONS_WT));
	this->SetDlgItemText(IDC_CHECK8,loadStr(IDS_OPTIONS_FF));
	this->SetDlgItemText(IDC_CHECK9,loadStr(IDS_OPTIONS_SS));
	this->SetDlgItemText(IDOK,loadStr(IDS_OPTIONS_OK));
	this->SetDlgItemText(IDC_CHOOSE_FOLDERS,loadStr(IDS_OPTIONS_CHOOSEF));
	this->SetDlgItemText(IDCANCEL,loadStr(IDS_OPTIONS_CANCEL));
	this->SetDlgItemText(IDC_STATIC_SECOND,loadStr(IDS_OPTIONS_SECOND));
	passMsg=loadStr(IDS_OPTIONS_REPASS);
}

BOOL Options::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(lang==0){
		this->setUI_EN();
		folderList.lang=0;
	}
	if(lang==1){
		folderList.lang=1;
		this->setUI_AR();
	}


	//virtual key codes for some letters
	hotList.AddString(_T("A"));
	hotList.SetItemData(0,0x41);

	hotList.AddString(_T("B"));
	hotList.SetItemData(1,0x42);

	hotList.AddString(_T("C"));
	hotList.SetItemData(2,0x43);

	hotList.AddString(_T("D"));
	hotList.SetItemData(3,0x44);

	hotList.AddString(_T("E"));
	hotList.SetItemData(4,0x45);

	hotList.AddString(_T("F"));
	hotList.SetItemData(5,0x46);

	hotList.AddString(_T("G"));
	hotList.SetItemData(6,0x47);

	hotList.AddString(_T("H"));
	hotList.SetItemData(7,0x48);

	hotList.AddString(_T("I"));
	hotList.SetItemData(8,0x49);

	hotList.SetCurSel(hotIndex);




	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void Options::OnBnClickedChooseFolders()
{
	folderList.DoModal();
}

  LPCTSTR Options::loadStr(UINT id)
  {
	  CString temp;
	  temp.LoadString(id);
	  strList.AddHead(temp); //this will make the variable alive when leaving the scope
	  return temp.GetString();
  }
