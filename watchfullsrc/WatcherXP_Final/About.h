#pragma once


// About dialog

class About : public CDialog
{
	DECLARE_DYNAMIC(About)

public:
	About(CWnd* pParent = NULL);   // standard constructor
	virtual ~About();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	void setUI_AR(void);
	void setUI_EN(void);
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	static int lang;

	LPCTSTR loadStr(UINT id);
	CStringList strList;

};
