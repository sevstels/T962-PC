//==============================================================================
//File name:   "threads.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _HCI_THREADS_WIN_H_
#define _HCI_THREADS_WIN_H_

extern CEvent ev_Quit[]; 

#ifdef __cplusplus
extern "C"
{
#endif

 void Threads_Ini(void);
 void Threads_Launch(LPVOID lParam);
 void Threads_Release(void);

 void CMD_ThreadLaunch(LPVOID lParam);
 void DATA_ThreadLaunch(LPVOID lParam);
 void SYNC_ThreadLaunch(LPVOID lParam);

 void CMD_ThreadStop(void);
 void DATA_ThreadStop(void);
 void SYNC_ThreadStop(void);

#ifdef __cplusplus
}
#endif

#endif //
