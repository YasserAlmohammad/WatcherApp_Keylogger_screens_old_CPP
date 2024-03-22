#pragma once
#include "afxwin.h"


// Viewer dialog

class Viewer : public CDialog
{
	DECLARE_DYNAMIC(Viewer)

public:
	Viewer(CWnd* pParent = NULL);   // standard constructor
	virtual ~Viewer();

// Dialog Data
	enum { IDD = IDD_VIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton8();
	virtual INT_PTR DoModal();
protected:
	virtual void OnOK();
public:
	virtual BOOL DestroyWindow();
	CString imagePath;
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
	virtual BOOL OnInitDialog();
	void drawImage(void);
	CButton playBtn;
	CButton pauseBtn;
	CButton nextBtn;
	CButton prevBtn;
	CComboBox speed;
	CString viewMsg;
	int lang;
	void setUI_AR(void);
	void SetUI_EN(void);

	LPCTSTR loadStr(UINT id);
	CStringList strList;

};
