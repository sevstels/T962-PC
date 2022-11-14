//==============================================================================
//File name:   "thread.h"
//Purpose:      Header File
//Version:      3.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _THREADR_H_
#define _THREADR_H_

#include <windows.h>

//------------------------------------------------------------------------------
//USB thread Functions
//------------------------------------------------------------------------------	
DWORD __stdcall RxThread(LPVOID lParam);

#ifdef  __cplusplus
extern "C" 
{
#endif //end __cplusplus

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class CThreadR
{
  //Construction
  public:
  CThreadR();
 ~CThreadR();

  //Operations
  public:
  void Create(LPVOID lParam);
  void Launch(void);
  void Delete(void);
  
  //CEvent ev_Quit;
  HANDLE hThread;
  DWORD id;

};

#ifdef __cplusplus
}
#endif //end __cplusplus

//------------------------------------------------------------------------------
#endif
