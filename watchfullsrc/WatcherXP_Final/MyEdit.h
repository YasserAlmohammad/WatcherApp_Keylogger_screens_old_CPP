#pragma once


// MyEdit

#define EDIT_BG_COLOR  RGB(104,140,176)
#define EDIT_TEXT_COLOR  RGB(255,255,255)

class MyEdit : public CEdit
{
	DECLARE_DYNAMIC(MyEdit)

public:
	MyEdit();
	virtual ~MyEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};


