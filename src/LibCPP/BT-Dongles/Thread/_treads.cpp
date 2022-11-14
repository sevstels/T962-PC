//==============================================================================
//File name:    "usb_threads.cpp"
//Purpose:      Source File, USB communication
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "_thread.h"

//THREAD_PRIORITY_ABOVE_NORMAL   //на один пункт выше нормального
//THREAD_PRIORITY_BELOW_NORMAL   //на один пункт ниже нормального
//THREAD_PRIORITY_HIGHEST        //на два пункта выше нормального
//THREAD_PRIORITY_IDLE           //базовый приоритет равный 1
//THREAD_PRIORITY_LOWEST         //на два пункта ниже нормального
//THREAD_PRIORITY_NORMAL         //нормальный приоритет
//THREAD_PRIORITY_TIME_CRITICAL  //приоритет равный 15

//------------------------------------------------------------------------------ 
//Constructor 
//------------------------------------------------------------------------------
CThreadR::CThreadR()
{ 
  hThread = NULL;
}

//------------------------------------------------------------------------------
//Destructor 
//------------------------------------------------------------------------------
CThreadR::~CThreadR()
{
  Delete();
}

//------------------------------------------------------------------------------
//Function:	launch 
//------------------------------------------------------------------------------
void CThreadR::Create(LPVOID lParam)
{	
  //Stop, if thread exist 
  Delete();

  //----
  //ev_Thread_Quit.ResetEvent();

  hThread = CreateThread(NULL, 0U, RxThread, lParam, 
	                     THREAD_PRIORITY_TIME_CRITICAL, &id);
  
  //hThread->m_bAutoDelete = FALSE;
}

//------------------------------------------------------------------------------
//Function:	launch 
//------------------------------------------------------------------------------
void CThreadR::Launch(void)
{	  
  ResumeThread(hThread);
}

//------------------------------------------------------------------------------
//Function:	stop communication Threads
//------------------------------------------------------------------------------
void CThreadR::Delete(void)
{ 	
  //---- Stop Tx  
  if (hThread!=NULL)
	{	
		SuspendThread(hThread);
		//ev_Thread_Quit.SetEvent();
		//WaitForSingleObject(pThread->m_hThread, INFINITE);
		CloseHandle(hThread);
		//delete hThread;
		hThread = NULL;
	}	
}