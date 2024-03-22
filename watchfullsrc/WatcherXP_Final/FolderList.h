#pragma once
#include "afxwin.h"


// FolderList dialog

class FolderList : public CDialog
{
	DECLARE_DYNAMIC(FolderList)

public:
	FolderList(CWnd* pParent = NULL);   // standard constructor
	virtual ~FolderList();

// Dialog Data
	enum { IDD = IDD_FOLDERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString dir;
	CListBox folderList;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	CStringList list;
	afx_msg void OnBnClickedButton3();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	void setUI_EN(void);
	void setUI_AR(void);
	// 0 for english, 1 for arabic
	int lang;

	LPCTSTR loadStr(UINT id);
	CStringList strList;
};
