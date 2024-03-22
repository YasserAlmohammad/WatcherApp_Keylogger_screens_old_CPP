// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WatcherXP.h"
#include "PasswordDlg.h"


// PasswordDlg dialog

IMPLEMENT_DYNAMIC(PasswordDlg, CDialog)
PasswordDlg::PasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PasswordDlg::IDD, pParent)
	, password(_T(""))
{
}

PasswordDlg::~PasswordDlg()
{
}

void PasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, password);
}


BEGIN_MESSAGE_MAP(PasswordDlg, CDialog)
END_MESSAGE_MAP()


// PasswordDlg message handlers

void PasswordDlg::OnOK()
{
	UpdateData();

	CDialog::OnOK();
}
