// MyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "WatcherXP.h"
#include "MyEdit.h"


// MyEdit

IMPLEMENT_DYNAMIC(MyEdit, CEdit)
MyEdit::MyEdit()
{
}

MyEdit::~MyEdit()
{
}


BEGIN_MESSAGE_MAP(MyEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// MyEdit message handlers


HBRUSH MyEdit::CtlColor(CDC* pDc, UINT /*nCtlColor*/)
{
	// TODO:  Change any attributes of the DC here
	TRACE(_T("ctrl color\r\n"));
	pDc->SetTextColor(EDIT_TEXT_COLOR);
	pDc->SetBkColor(EDIT_BG_COLOR); //this will assure to paint only one line bg
	pDc->SelectObject(&CBrush(EDIT_BG_COLOR)); //this will assure to paint the whole bg
	// TODO:  Return a non-NULL brush if the parent's handler should not be called
	return CBrush(EDIT_BG_COLOR);
}
