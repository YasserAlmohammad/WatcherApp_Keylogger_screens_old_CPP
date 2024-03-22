// Viewer.cpp : implementation file
//

#include "stdafx.h"
#include "WatcherXP.h"
#include "Viewer.h"


//global variables to controll the thread action
bool play=false; //start from begining
bool pause=true; //paly/pause from current position
bool next=false; //move 
bool pre=false;
bool stopThread=false;
bool stretch=true; //unstretched image is Guarantee to work best in full screen
int currentFrame=0;
int nextFrame=0;
int frame=0;
int oldFrame=0;
int i=0;
CStringArray fileList;
int count=0;

// Viewer dialog

IMPLEMENT_DYNAMIC(Viewer, CDialog)
Viewer::Viewer(CWnd* pParent /*=NULL*/)
	: CDialog(Viewer::IDD, pParent)
	, imagePath(_T("d:\\e\\"))
	, viewMsg(_T(""))
	,lang(0)
{
}

Viewer::~Viewer()
{
}

void Viewer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON8, playBtn);
	DDX_Control(pDX, IDC_BUTTON7, pauseBtn);
	DDX_Control(pDX, IDC_BUTTON5, nextBtn);
	DDX_Control(pDX, IDC_BUTTON4, prevBtn);
	DDX_Control(pDX, IDC_COMBO1, speed);
}


BEGIN_MESSAGE_MAP(Viewer, CDialog)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
END_MESSAGE_MAP()


// Viewer message handlers

UINT drawThread(LPVOID param){
	//first load file list
	Viewer* viewer=(Viewer*)param;
	while(!stopThread){
		if(!pause){
			++i;
			viewer->drawImage();
			
		}

		if(stopThread)
			return 0;
		if(viewer->speed.GetCurSel()==1)
			Sleep(100);
		if(viewer->speed.GetCurSel()==2)
			Sleep(500);
		//ELSE DON'T SLEEP
	}
	return 0;

}

void Viewer::OnBnClickedButton6()
{
	stopThread=true;
	OnOK();
}

void Viewer::OnBnClickedButton8()
{
	stopThread=false;
	play=true;
	pause=false;
	i=0;
	SetTimer(ID_TIMER1,1000,NULL);
	AfxBeginThread(drawThread,this);
	playBtn.EnableWindow(FALSE);
	nextBtn.EnableWindow(FALSE);
	prevBtn.EnableWindow(FALSE);
}

INT_PTR Viewer::DoModal()
{

	return CDialog::DoModal();
}

void Viewer::OnOK()
{
	stopThread=true;
	CDialog::OnOK();
}


BOOL Viewer::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	TRACE(_T("destroy\r\n"));
	stopThread=true;
	return CDialog::DestroyWindow();
}

//pause
void Viewer::OnBnClickedButton7()
{
	pause=!pause;
	//thread will respond
	if(pause){
		nextBtn.EnableWindow();
		prevBtn.EnableWindow();
	}
	else{
		nextBtn.EnableWindow(FALSE);
		prevBtn.EnableWindow(FALSE);
	}

}

//next
void Viewer::OnBnClickedButton5()
{
	if(pause){
		++i;
		drawImage();
	}

}

//prev
void Viewer::OnBnClickedButton4()
{
	if(pause){
		if(i>0)
			--i;
		drawImage();
	}
}

BOOL Viewer::OnInitDialog()
{
	CDialog::OnInitDialog();
//	fileList.SetSize(40);

	if(lang==0)
		this->SetUI_EN();
	if(lang==1)
		this->setUI_AR();
	//llist image file names once

	HANDLE h;
	WIN32_FIND_DATA info;
	h=FindFirstFile(this->imagePath+_T("*.jpg"),&info);

	if (h == INVALID_HANDLE_VALUE) {
		MessageBox(viewMsg,_T("viewer"),MB_OK);
		OnOK();
	}
	fileList.Add(this->imagePath+info.cFileName);
	while(FindNextFile(h,&info)){
		if(h==INVALID_HANDLE_VALUE)
			continue;
		fileList.Add(this->imagePath+info.cFileName);
	}
	FindClose(h);
	count=fileList.GetCount();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void Viewer::drawImage(void)
{
	CClientDC dc(this);
	RECT rect;

	if(i>=count)
		i=0;
	this->GetClientRect(&rect);
	CImage image;		
	CString imagePath=fileList.GetAt(i);
	HRESULT r=image.Load(imagePath);
	this->SetWindowText(imagePath);
	if(FAILED(r))
		return;
	CImageDC imageDC(image);
	if(stretch){
		int destWidth=rect.right-50;
		int destHeight=rect.bottom-50; //margins
		
		StretchBlt(dc,0,0,destWidth,destHeight,imageDC,0,0,image.GetWidth(),image.GetHeight(),SRCCOPY);
	}
	else
		BitBlt(dc,0,0,rect.right,rect.bottom,imageDC,0,0,SRCCOPY);
}

void Viewer::setUI_AR(void)
{

	//CLEAR LIST CONTENT
	strList.RemoveAll();

	viewMsg=loadStr( IDS_VIEWER_NOIMAGE);
	this->SetWindowText(loadStr( IDS_VIEWER_WT));
	this->SetDlgItemText(IDC_BUTTON8,loadStr( IDS_FILE_START));
	this->SetDlgItemText(IDC_BUTTON7,loadStr( IDS_VIEWER_PAUSE));
	this->SetDlgItemText(IDC_BUTTON6,loadStr( IDS_VIEWER_STOP));
	this->SetDlgItemText(IDC_BUTTON5,loadStr( IDS_VIEWER_NEXT));
	this->SetDlgItemText(IDC_BUTTON4,loadStr( IDS_VIEWER_PREV));
	this->SetDlgItemText(IDC_STATIC_INTERVAL,loadStr( IDS_VIEWER_INTERVAL));
	
	speed.Clear();
	speed.AddString(loadStr( IDS_VIEWER_MAXSPEED));
	speed.AddString(loadStr( IDS_VIEWER_100MILLIS));
	speed.AddString(loadStr( IDS_VIEWER_500MILLIS));

	speed.SetCurSel(0);
}


void Viewer::SetUI_EN(void)
{
	viewMsg=_T("there are no images to view");
	this->SetWindowText(_T("viewer"));
	this->SetDlgItemText(IDC_BUTTON8,_T("start"));
	this->SetDlgItemText(IDC_BUTTON7,_T("pause"));
	this->SetDlgItemText(IDC_BUTTON6,_T("stop"));
	this->SetDlgItemText(IDC_BUTTON5,_T("next"));
	this->SetDlgItemText(IDC_BUTTON4,_T("prev"));
	this->SetDlgItemText(IDC_STATIC_INTERVAL,_T("interval between two images"));
	
	speed.Clear();
	speed.AddString(_T("no time(max speed)"));
	speed.AddString(_T("100 milli seconds"));
	speed.AddString(_T("500 ms (half a second"));

	speed.SetCurSel(0);
}

  LPCTSTR Viewer::loadStr(UINT id)
  {
	  CString temp;
	  temp.LoadString(id);
	  strList.AddHead(temp); //this will make the variable alive when leaving the scope
	  return temp.GetString();
  }
