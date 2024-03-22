// FolderList.cpp : implementation file
//

#include "stdafx.h"
#include "WatcherXP.h"
#include "FolderList.h"


// FolderList dialog

IMPLEMENT_DYNAMIC(FolderList, CDialog)
FolderList::FolderList(CWnd* pParent /*=NULL*/)
	: CDialog(FolderList::IDD, pParent)
	, dir(_T(""))
	, lang(0)
{
}

FolderList::~FolderList()
{
}

void FolderList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, dir);
	DDX_Control(pDX, IDC_LIST1, folderList);
}


BEGIN_MESSAGE_MAP(FolderList, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// FolderList message handlers

void FolderList::OnBnClickedButton2()
{
	UpdateData();
	/* we need to ckeck this befor we add the path:
	- general path reports inner paths so inner paths are deleted when a more
	  general path already exists-added i.e:
	  d:\   contains  d:\folder\folder2  this is why when the second path already exists
	  and the first path was newly selected the second path is deleted and d:\ is added
	  if d:\ is already in the list and the second path was added, it's ignored and not added
	  (two paths, one containing another reports the same report)
	  */
	for(int i=0;i<folderList.GetCount();i++){
		//check if list has a common path
		CString str;
		folderList.GetText(i,str);
		if(str.Find(dir)==0){ //the new path is more common
			folderList.DeleteString(i); //here we stop because we know no more coo\mmon exists
			folderList.AddString(dir);
			UpdateData(FALSE);
			return;
		}
		if(dir.Find(str)==0) // the new path is contained
			return;
	}
	folderList.AddString(dir);
	UpdateData(FALSE);
}

void FolderList::OnBnClickedButton1()
{
	CoInitialize(NULL);

	//show a browse folder dialog, this is a shell call throgh com intefaces
	//first fill a BROWSEINFO struct
	TCHAR* buffer=new TCHAR[MAX_PATH];
	buffer[0]=NULL;
	BROWSEINFO binfo;
	binfo.hwndOwner=this->m_hWnd;
	binfo.pidlRoot=NULL;
	binfo.pszDisplayName=buffer;
	binfo.lpszTitle=_T("browse");
	binfo.ulFlags=BIF_USENEWUI;
	binfo.lpfn=NULL;
	binfo.lParam=0;
	binfo.iImage=NULL;

	//get the location item
	LPITEMIDLIST itemID=SHBrowseForFolder(&binfo);

	TCHAR* path=new TCHAR[MAX_PATH]; //buffer
	//get the folder path from the ITEMIDLIST
	BOOL ok=SHGetPathFromIDList(itemID,path);


	//we must free the ITEMIDLIST using the IMalloc
	IMalloc* malloc=NULL;
	if(SHGetMalloc(&malloc)==NOERROR){
		malloc->Free(itemID);
		malloc->Release();
	}
	
	//int len=std::char_traits<char>::length(path);
	CString out=path;
	
	dir=out;
	UpdateData(FALSE);

}

void FolderList::OnBnClickedOk()
{
	UpdateData();
	list.RemoveAll();
	for(int i=0;i<folderList.GetCount();i++){
		CString str;
		folderList.GetText(i,str);
		list.AddHead(str);
	}
	OnOK();
}

void FolderList::OnBnClickedButton3()
{
	folderList.DeleteString(folderList.GetCurSel());
	UpdateData(FALSE);
}

BOOL FolderList::OnInitDialog()
{
	CDialog::OnInitDialog();

	int len=list.GetCount();
	if(len>0){
		POSITION pos=list.GetHeadPosition();
		int i=0;
		while(pos!=NULL)
			folderList.AddString(list.GetNext(pos));
	}

	if(lang==0)
		this->setUI_EN();
	if(lang==1)
		this->setUI_AR();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void FolderList::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void FolderList::setUI_EN(void)
{
	this->SetWindowText(_T("folders to select dialog"));
	this->SetDlgItemText(IDC_BUTTON1,_T("choose folder"));
	this->SetDlgItemText(IDC_BUTTON2,_T("add to list"));
	this->SetDlgItemText(IDC_BUTTON3,_T("remove from list"));
	this->SetDlgItemText(IDOK,_T("ok"));
	this->SetDlgItemText(IDCANCEL,_T("cancel"));
}

void FolderList::setUI_AR(void)
{
	strList.RemoveAll();
	this->SetWindowText(loadStr(IDS_FOLDERLIST_WT));
	this->SetDlgItemText(IDC_BUTTON1,loadStr(IDS_FOLDERLIST_CHOOSE));
	this->SetDlgItemText(IDC_BUTTON2,loadStr(IDS_FOLDERLIST_ADD));
	this->SetDlgItemText(IDC_BUTTON3,loadStr(IDS_FOLDERLIST_DEL));
	this->SetDlgItemText(IDOK,loadStr(IDS_OPTIONS_OK));
	this->SetDlgItemText(IDCANCEL,loadStr(IDS_OPTIONS_CANCEL));
}

  LPCTSTR FolderList::loadStr(UINT id)
  {
	  CString temp;
	  temp.LoadString(id);
	  strList.AddHead(temp); //this will make the variable alive when leaving the scope
	  return temp.GetString();
  }
