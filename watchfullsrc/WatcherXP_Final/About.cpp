// About.cpp : implementation file
//

#include "stdafx.h"
#include "WatcherXP.h"
#include "About.h"


// About dialog
int About::lang=0;

IMPLEMENT_DYNAMIC(About, CDialog)
About::About(CWnd* pParent /*=NULL*/)
	: CDialog(About::IDD, pParent)
{
}

About::~About()
{
}

void About::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(About, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void About::setUI_EN(void)
{
	this->SetWindowText(_T("about"));
	this->SetDlgItemText(IDC_STATIC_VERSION,_T("WatcherFunction Version 1.1\r\n\r\nCopyright (C) 2004 to Yasser Almohammad"));
	this->SetDlgItemText(IDC_STATIC_ABOUT,_T("this program was made by: Yasser Almohammad, Senior at Damascus IT Faculty\r\n"));

}

void About::setUI_AR(void)
{
	this->SetWindowText(loadStr(IDS_HELP_ABOUT));
	this->SetDlgItemText(IDC_STATIC_VERSION,loadStr(IDS_ABOUT_VER));
	this->SetDlgItemText(IDC_STATIC_ABOUT,loadStr(IDS_ABOUT_ME));

}


// About message handlers

void About::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL About::OnInitDialog()
{
	CDialog::OnInitDialog();


	SetWindowLong(this->m_hWnd, GWL_EXSTYLE,GetWindowLong(this->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(RGB(10,10,10),0,LWA_COLORKEY);



	// TODO:  Add extra initialization here
	if(lang==0)
		this->setUI_EN();
	if(lang==1)
		this->setUI_AR();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

  LPCTSTR About::loadStr(UINT id)
  {
	  CString temp;
	  temp.LoadString(id);
	  strList.AddHead(temp); //this will make the variable alive when leaving the scope
	  return temp.GetString();
  }
