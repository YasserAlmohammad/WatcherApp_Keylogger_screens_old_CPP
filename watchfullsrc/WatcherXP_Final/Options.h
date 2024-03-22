#pragma once
#include "afxcmn.h"
#include "folderlist.h"
#include "afxwin.h"


// Options dialog

class Options : public CDialog
{

public:
	Options(CWnd* pParent = NULL);   // standard constructor

	virtual ~Options();

// Dialog Data
	enum { IDD = IDD_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL startUp;
	BOOL passwordEnable;
	CString password;
	CString passwordAgain;
	afx_msg void OnBnClickedButton1();
	BOOL KeyStroke;
	BOOL WebAddress;
	BOOL WindowTitle;
	BOOL FileFolder;
	BOOL ScreenShots;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	BOOL isVisible;
	int elapsed;
	BOOL autoStart;
	// set arabic UI
	void setUI_AR(void);
	void setUI_EN(void);
	virtual BOOL OnInitDialog();
	// 0 for EN, 1 for AR
	int lang;
	CString passMsg;
	afx_msg void OnBnClickedChooseFolders();
	FolderList folderList;
	CComboBox hotList;
	int hotkey;
	int hotIndex;

	LPCTSTR loadStr(UINT id);
	CStringList strList;

};
