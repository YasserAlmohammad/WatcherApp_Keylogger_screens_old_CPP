#pragma once
#include "atlimage.h"
#include "atltypes.h"

#define DTC RGB(0,0,0) //defult text color
#define BTN_CLICK WM_USER+2000
#define BTN_MOUSE_IN WM_USER+2001
#define BTN_MOUSE_LEAVE WM_USER+2002

#define TEXT_COLOR RGB(255,255,255)
#define TEXT_COLOR_OVER RGB(155,0,0)

class Button : public CWnd
{
	DECLARE_DYNAMIC(Button)

public:
	Button();
	virtual ~Button();

	virtual BOOL Create(LPCTSTR lpszClassName,LPCTSTR lpszWindowName,DWORD dwStyle,
   const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	bool SetImages(UINT normal/*this is required*/,UINT over,UINT down,UINT disabled,bool stretch=false,COLORREF clr=RGB(255,255,255));
	void SetLabelColor(COLORREF normalClr=DTC,COLORREF overClr=DTC,COLORREF downClr=DTC ,COLORREF disabledClr=DTC);


	BOOL EnableWindow(BOOL enable=TRUE);
	void setTransparancyColor(COLORREF clr=RGB(255,255,255));


	enum STATE{
		NORMAL,
		DOWN,
		OVER,
		DISABLED
	} state;

protected:
	CImage over;
	CImage down;
	CImage normal;
	CImage disabled;

	//button position
	int x;
	int y;
	int width;
	int height;
	bool in;
	bool enabled;
	bool stretchImg;//if the image is not streched the button will
	TRACKMOUSEEVENT track;
	COLORREF color; //transparany color, defualt is white: RGB(255,255,255)
	COLORREF nColor; //normal state text color
	COLORREF oColor; //over state text colr
	COLORREF dColor; //down
	COLORREF xColor; //disabled

	CString label;

	DECLARE_MESSAGE_MAP()

	afx_msg LONG onMouseLeave(WPARAM wParam, LPARAM lParam);
	
public:
	virtual BOOL DestroyWindow();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	
//	BOOL OnEraseBkgnd(CDC* pDC);
	void setLabel(CString l);
	
};


