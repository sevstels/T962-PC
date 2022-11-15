 //=============================================================================
//File name:   "threads.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include <afxwin.h>
#include <afxcmn.h>
#include <afxmt.h>
#include "threads.h"

//THREAD_PRIORITY_ABOVE_NORMAL   //на один пункт выше нормального
//THREAD_PRIORITY_BELOW_NORMAL   //на один пункт ниже нормального
//THREAD_PRIORITY_HIGHEST        //на два пункта выше нормального
//THREAD_PRIORITY_IDLE           //базовый приоритет равный 1
//THREAD_PRIORITY_LOWEST         //на два пункта ниже нормального
//THREAD_PRIORITY_NORMAL         //нормальный приоритет
//THREAD_PRIORITY_TIME_CRITICAL  //приоритет равный 15

//----
CWinThread *pCMD_Thread = NULL; //указатель на функцию потока
CWinThread *pDATA_Thread = NULL; //указатель на функцию потока
CWinThread *pSYNC_Thread = NULL; //указатель на функцию потока

//----
CEvent ev_Quit[3];	//обьект класса событий

//----
extern DWORD HCI_RX_THREAD(LPVOID lParam);
extern DWORD DATA_RX_THREAD(LPVOID lParam);
extern DWORD SYNC_TX_THREAD(LPVOID lParam);

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Threads_Ini(void)
{
  pCMD_Thread = NULL;
  pDATA_Thread = NULL;
  pSYNC_Thread = NULL;
  
  //----
  ev_Quit[0].ResetEvent();
  ev_Quit[1].ResetEvent();
  ev_Quit[2].ResetEvent();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMD_ThreadLaunch(LPVOID lParam)
{
  if(pCMD_Thread!=NULL) return;
  
  ev_Quit[0].ResetEvent();
  
  //Restart all threads
  pCMD_Thread = AfxBeginThread((AFX_THREADPROC)HCI_RX_THREAD, 
                                lParam, THREAD_PRIORITY_BELOW_NORMAL);
  pCMD_Thread->m_bAutoDelete = FALSE;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void DATA_ThreadLaunch(LPVOID lParam)
{
  if(pDATA_Thread!=NULL) return;
  
  ev_Quit[1].ResetEvent();

  //start thread
  pDATA_Thread = AfxBeginThread((AFX_THREADPROC) DATA_RX_THREAD, 
                                 lParam, THREAD_PRIORITY_BELOW_NORMAL);
  pDATA_Thread->m_bAutoDelete = FALSE; 
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void SYNC_ThreadLaunch(LPVOID lParam)
{
  if(pSYNC_Thread!=NULL) return;
  
  ev_Quit[2].ResetEvent();

  //start thread
  pSYNC_Thread = AfxBeginThread((AFX_THREADPROC) SYNC_TX_THREAD, 
                                 lParam, THREAD_PRIORITY_TIME_CRITICAL);
  pSYNC_Thread->m_bAutoDelete = FALSE; 
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CMD_ThreadStop(void)
{
  if(pCMD_Thread==NULL) return;
  //----
  ev_Quit[0].SetEvent();
  WaitForSingleObject(pCMD_Thread->m_hThread, INFINITE);
  delete pCMD_Thread; pCMD_Thread = NULL;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void DATA_ThreadStop(void)
{
  if(pDATA_Thread==NULL) return;
  //----  
  ev_Quit[1].SetEvent(); 
  WaitForSingleObject(pDATA_Thread->m_hThread, INFINITE);
  delete pDATA_Thread; pDATA_Thread = NULL;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void SYNC_ThreadStop(void)
{
  if(pSYNC_Thread==NULL) return;
  //----  
  ev_Quit[2].SetEvent(); 
  WaitForSingleObject(pSYNC_Thread->m_hThread, INFINITE);
  delete pSYNC_Thread; pSYNC_Thread = NULL;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Threads_Launch(LPVOID lParam)
{
  //----
  CMD_ThreadLaunch(lParam);
  DATA_ThreadLaunch(lParam);
  SYNC_ThreadLaunch(lParam);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Threads_Release(void)
{
  //end all  
  //----
  SYNC_ThreadStop();
  DATA_ThreadStop();
  CMD_ThreadStop();
}
