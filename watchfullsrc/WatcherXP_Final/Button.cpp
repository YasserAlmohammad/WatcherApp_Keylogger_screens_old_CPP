// Button.cpp : implementation file
//

#include "stdafx.h"
#include "Button.h"

// Button

IMPLEMENT_DYNAMIC(Button, CWnd)
Button::Button()
: stretchImg(false)
{
	state=NORMAL;
	x=y=width=height=0;
	stretchImg=false;
	in=true;
	enabled=TRUE;
	color=RGB(255,255,255);
	label=_T("");
	oColor=TEXT_COLOR_OVER;
	dColor=TEXT_COLOR;
	xColor=TEXT_COLOR;
	nColor=TEXT_COLOR;


}

Button::~Button()
{
	
}


BEGIN_MESSAGE_MAP(Button, CWnd)
//	ON_MESSAGE(WM_LBUTTONDOWN,onMouseDown)
//	ON_MESSAGE(WM_LBUTTONUP,onMouseUp)
	ON_MESSAGE(WM_MOUSELEAVE,onMouseLeave)
//	ON_MESSAGE(WM_MOUSEMOVE,onMouseMove)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL Button::Create(LPCTSTR lpszClassName,LPCTSTR lpszWindowName,DWORD dwStyle,
					const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext){
	x=rect.left;
	y=rect.top;
	width=rect.right;
	height=rect.bottom;
	label=lpszWindowName;
	
	return CWnd::Create(NULL,NULL,0,rect,pParentWnd,nID);

}


bool Button::SetImages(UINT normalID,UINT overID,UINT downID,UINT disabledID,bool stretch,COLORREF clr){
	try{
	//	SetWindowLong(this->m_hWnd, GWL_EXSTYLE,GetWindowLong(this->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//	SetLayeredWindowAttributes(RGB(240,240,240),0,LWA_COLORKEY);	
		//delete images if they already exist
		over.Destroy();
		down.Destroy();
		normal.Destroy();
		disabled.Destroy();

		if(overID!=0)
			over.LoadFromResource(AfxGetInstanceHandle(),overID);
	//	down.LoadFromResource(AfxGetInstanceHandle(),downID);
		normal.LoadFromResource(AfxGetInstanceHandle(),normalID);
	//	disabled.LoadFromResource(AfxGetInstanceHandle(),disabledID);
		color=clr;

		if(!stretchImg){
			width=normal.GetWidth();
			height=normal.GetHeight();
			this->SetWindowPos(&wndTop,x,y,width,height,0);
		}
		track.cbSize=sizeof(TRACKMOUSEEVENT);
		track.dwFlags=TME_LEAVE;
		track.hwndTrack=this->m_hWnd;
		track.dwHoverTime=400;
		
	}
	catch(...)
	{ 
		return false;
	}

	this->SetLabelColor(TEXT_COLOR,TEXT_COLOR_OVER,TEXT_COLOR,TEXT_COLOR);
	return true;
}

// Button message handlers

/*
LONG Button::onMouseDown(WPARAM wParam, LPARAM lParam){
	state=DOWN;
	Invalidate();
	
	//to track mouce movement
//	TrackMouseEvent(&track);	


	return 0;
}

LONG Button::onMouseUp(WPARAM wParam, LPARAM lParam){ 
	state=OVER;
	Invalidate();
	NMHDR* hdr=new NMHDR;
	hdr->code=NM_CLICK;  //send Click notfication to parent
	hdr->hwndFrom=this->m_hWnd;
	hdr->idFrom=this->GetDlgCtrlID();
	this->GetParent()->PostMessage(WM_NOTIFY,this->GetDlgCtrlID(),reinterpret_cast<LPARAM>(hdr));

	
	return 0;
}

LONG Button::onMouseMove(WPARAM w,LPARAM l){
	if(in){ //only one time through mouse enter the button is redrawn
		state=OVER;
		Invalidate();
		TrackMouseEvent(&track); //track leaving		
		in=false;
	}
	return 0;
	
}
*/

LONG Button::onMouseLeave(WPARAM wParam, LPARAM lParam){
	state=NORMAL;
	Invalidate();
	in=true;
/*
	NMHDR hdr;
	hdr.code=BTN_MOUSE_LEAVE;  //send leave notfication to parent
	hdr.hwndFrom=this->m_hWnd;
	hdr.idFrom=this->GetDlgCtrlID();
	this->GetParent()->PostMessage(WM_NOTIFY,this->GetDlgCtrlID(),reinterpret_cast<LPARAM>(&hdr));	
*/
	return 0;
}


BOOL Button::EnableWindow(BOOL enable){
	enabled=!enabled;
	if(!enabled){ //means it was enabled
		state=DISABLED;
		Invalidate();
	}
	else{
		state=NORMAL;
		Invalidate();
	}
	return !enabled;
}

void Button::setTransparancyColor(COLORREF RGBval)
{
	color=RGBval;
}

BOOL Button::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	over.Destroy();
	down.Destroy();
	normal.Destroy();
	disabled.Destroy();

	return CWnd::DestroyWindow();
}

void Button::SetLabelColor(COLORREF normalClr,COLORREF overClr,COLORREF downClr,COLORREF disabledClr){
	nColor=normalClr;
	oColor=overClr;
	dColor=downClr;
	xColor=disabledClr;
}


void Button::OnSetFocus(CWnd* pOldWnd)
{
//	CWnd::OnSetFocus(pOldWnd);
//	state=OVER;
//	Invalidate();
	
}

void Button::OnKillFocus(CWnd* pNewWnd)
{
//	CWnd::OnKillFocus(pNewWnd);
//	state=NORMAL;
//	Invalidate();
	
}

void Button::OnLButtonDown(UINT nFlags, CPoint point)
{
	state=DOWN;
//	Invalidate();
	
	CWnd::OnLButtonDown(nFlags, point);
}

void Button::OnLButtonUp(UINT nFlags, CPoint point)
{
	state=OVER;
//	Invalidate();

	NMHDR hdr;
	hdr.code=BTN_CLICK;  //send Click notfication to parent
	hdr.hwndFrom=this->m_hWnd;
	hdr.idFrom=this->GetDlgCtrlID();
	this->GetParent()->SendMessage(WM_NOTIFY,this->GetDlgCtrlID(),reinterpret_cast<LPARAM>(&hdr));	

	CWnd::OnLButtonUp(nFlags, point);
}

void Button::OnMouseMove(UINT nFlags, CPoint point)
{
	if(in){ //only one time through mouse enter the button is redrawn
		state=OVER;
		Invalidate();
		TrackMouseEvent(&track); //track leaving		
		in=false;

	}

	CWnd::OnMouseMove(nFlags, point);
}
/*
BOOL Button::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//CBrush b(RGB(59,93,153));
	CBrush b(RGB(240,240,240));
	
	RECT rect;
	this->GetClientRect(&rect);
	pDC->FillRect(&rect,&b);

	return 1;
//	return -1;//CWnd::OnEraseBkgnd(pDC);
}
*/
void Button::OnPaint()
{
	CPaintDC dc(this);
	if(normal.GetWidth()<=0)
		return;
	COLORREF oldColor;
	dc.SetBkMode(TRANSPARENT);
	switch(state){
		case NORMAL:
				normal.TransparentBlt(dc,CRect(0,0,width,height),color);
				//draw the caption with the specified color
				oldColor=dc.SetTextColor(nColor);
				dc.DrawText(label,CRect(0,0,width,height),DT_CENTER|DT_NOCLIP|DT_SINGLELINE|DT_VCENTER);
			dc.SetTextColor(oldColor);
			break;
		case OVER:

			if(!over.IsNull())
				over.TransparentBlt(dc,CRect(0,0,width,height),color);
			else
				normal.TransparentBlt(dc,CRect(0,0,width,height),color);
							//draw the caption with the specified color
			oldColor=dc.SetTextColor(oColor);
			dc.DrawText(label,CRect(0,0,width,height),DT_CENTER|DT_NOCLIP|DT_SINGLELINE|DT_VCENTER);
			dc.SetTextColor(oldColor);
			break;
			/*
		case DOWN:
			if(down.GetWidth()>0)
				down.TransparentBlt(dc,CRect(0,0,width,height),color);
			else
				normal.TransparentBlt(dc,CRect(0,0,width,height),color);
				//draw the caption with the specified color
				oldColor=dc.SetTextColor(dColor);
				dc.DrawText(label,CRect(0,0,width,height),DT_CENTER|DT_NOCLIP|DT_SINGLELINE|DT_VCENTER);
			break;
		case DISABLED:
			if(disabled.GetWidth()>0)
				disabled.TransparentBlt(dc,CRect(0,0,width,height),color);
			else
				normal.TransparentBlt(dc,CRect(0,0,width,height),color);
				//draw the caption with the specified color
				oldColor=dc.SetTextColor(xColor);
				dc.DrawText(label,CRect(0,0,width,height),DT_CENTER|DT_NOCLIP|DT_SINGLELINE|DT_VCENTER);
			break;
			*/
	}
	
	
}

void Button::setLabel(CString l)
{
	label=l;
	this->SetWindowText(l);
	Invalidate();
}
