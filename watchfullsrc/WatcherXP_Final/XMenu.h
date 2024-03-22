#pragma once
#include "afxwin.h"

#define MENU_BG_COLOR RGB(84,113,141)
#define MENU_TEXT_COLOR RGB(255,255,255)
#define MENU_TEXT_OVER_COLOR RGB( )
#define MENU_BG_OVER_COLOR RGB(101,84,141)
#define MENU_STROKE_COLOR RGB(143,236,253)
#define MENU_DISABLED_COLOR RGB(200,200,200)

class XMenu :
	public CMenu
{
public:
	XMenu(void);
	~XMenu(void);
	void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	int width;
	int height;
	void SetItemDim(int,int);
};
