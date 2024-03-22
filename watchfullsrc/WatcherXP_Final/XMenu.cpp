#include "StdAfx.h"
#include "xmenu.h"

XMenu::XMenu(void)
:width(0),
height(0)
{
}

XMenu::~XMenu(void)
{
}

void XMenu::SetItemDim(int w,int h){
	width=w;
	height=h;
}

void XMenu::DrawItem(LPDRAWITEMSTRUCT ps)
{
	CDC* pDC = CDC::FromHandle(ps->hDC);
	CString str = (LPCTSTR)ps->itemData; //item data

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(MENU_TEXT_COLOR);

	if(ps->itemAction & ODA_DRAWENTIRE){ //when calling TrackPopupMenu for example		
		pDC->FillRect(&ps->rcItem, &CBrush(MENU_BG_COLOR));
		pDC->DrawText(str,&ps->rcItem,DT_CENTER|DT_VCENTER);

		if(ps->itemID==-1){ //separator
			pDC->SetDCPenColor(RGB(255,255,255));
			CRect r(ps->rcItem.left,ps->rcItem.bottom-(ps->rcItem.bottom-ps->rcItem.top)/2,
				ps->rcItem.right,ps->rcItem.bottom-(ps->rcItem.bottom-ps->rcItem.top)/2);
			r.InflateRect(0,0,0,1);
			pDC->FillRect(&r, &CBrush(MENU_TEXT_COLOR));
		//	pDC->MoveTo(ps->rcItem.left,ps->rcItem.bottom-(ps->rcItem.bottom-ps->rcItem.top)/2);
		//	pDC->LineTo(ps->rcItem.right,ps->rcItem.bottom-(ps->rcItem.bottom-ps->rcItem.top)/2);
		}
	}


	if(ps->itemID==-1) //separator mark
		return;

	if(ps->itemState & ODS_GRAYED){
		TRACE(_T("grayed %d\r\n"),ps->itemID);
		pDC->SetTextColor(RGB(0,0,0));
		pDC->FillRect(&ps->rcItem, &CBrush(MENU_DISABLED_COLOR));
		pDC->DrawText(str,&ps->rcItem,DT_CENTER|DT_VCENTER);
		return;
	}

	if((ps->itemAction & ODA_SELECT) && (ps->itemState & ODS_SELECTED)){
		//item is seletcted
		TRACE(_T("select %d\r\n"),ps->itemID);
		CRect r=ps->rcItem;
		r.DeflateRect(1,1,1,1);
		
		pDC->FillRect(&ps->rcItem, &CBrush(MENU_BG_OVER_COLOR));
		pDC->DrawText(str,&ps->rcItem,DT_CENTER|DT_VCENTER);
		pDC->FrameRect(&ps->rcItem,&CBrush(MENU_STROKE_COLOR));
	}
	else{ //draw defualt state
		TRACE(_T("defualt %d\r\n"),ps->itemID);
		pDC->SetTextColor(MENU_TEXT_COLOR);
		pDC->FillRect(&ps->rcItem, &CBrush(MENU_BG_COLOR));
		pDC->DrawText(str,&ps->rcItem,DT_CENTER|DT_VCENTER);
	}

}

//this method is called once per menu item view life
void XMenu::MeasureItem(LPMEASUREITEMSTRUCT i/*lpMeasureItemStruct*/)
{
		CString str=(LPCTSTR)i->itemData;
		CSize textSize=CWnd::GetDesktopWindow()->GetDC()->GetTextExtent(str);
		if(textSize.cx<width)
			textSize.cx=width;
		if(textSize.cy<height)
			textSize.cy=height;
		i->itemHeight=textSize.cy;
		i->itemWidth=textSize.cx;

		if(i->itemID==-1){
			i->itemHeight=10;
		}
	
}
