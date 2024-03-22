#pragma once


// PasswordDlg dialog

class PasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(PasswordDlg)

public:
	PasswordDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~PasswordDlg();

// Dialog Data
	enum { IDD = IDD_PASSWORD};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString password;
protected:
	virtual void OnOK();
};
