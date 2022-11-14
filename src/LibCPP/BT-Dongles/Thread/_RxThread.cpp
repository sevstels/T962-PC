//==============================================================================
//File name:    "threads.cpp"
//Purpose:      Source File, USB communication
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "_thread.h"
#include <windows.h>

//------------------------------------------------------------------------------
//USB port,  Data input
//------------------------------------------------------------------------------ 
DWORD __stdcall RxThread(LPVOID lParam)
{ 	

  //CBLED *pBT = (CBLED *)lParam;
  unsigned char serv_notification_handle = 16;
///  unsigned char buf[512];
///  DWORD counter;
  //Get notified 10 times ...
  //DBG_TRACE("==== Watch for notifications and print them if arriving ====\n");
  ///DBG_TRACE("BT Rx Thread launched\n");
  Sleep(100);

  //===========================================
  //Read Loop
  //===========================================
  for(;;)
  {		
	Sleep(10000);  
    //pBT->Wait_Response();
    //pBT->Wait_Event();	  
	//ReadFile(pBT->hSerialPort, (unsigned char*)&buf, 1, &counter, NULL);
	//DBG_TRACE("BT Rx\n");
	  
	/*
    //We have to check for BGLib messages, this 
    //was previously hidden within the wait_for_... functions.
    if(pBT->Wait_Event())
    {
      DBG_TRACE("Error reading message\n");
    }

    //Check if everything is fine
    if(issetFlag(pBT->m_state, APP_ATTCLIENT_ERROR))
    {
      //pBT->Error();
	  DBG_TRACE("Error\n");
    }

    //Check if a notification is pending and if yes check 
    //for the handle of the battery service we are looking for
    if(issetFlag(pBT->m_state, APP_ATTCLIENT_NOTIFICATION_PENDING))
     {
       clearFlag(pBT->m_state, APP_ATTCLIENT_NOTIFICATION_PENDING);

       //We already know the handle of the custom battery 
       //service by looking it up with BLEGUI earlier
       //Now lets see if it is the handle we've waited for. 
       if(pBT->app_attclient.handle == serv_notification_handle)
        {
          //The service characteristics value is simply 
          //the battery voltage in 10th of a millivolt.
          //First put the 16-bit value together and then 
          //convert the value to volts.
          int voltage = 0;// (app_attclient.value.data[1] << 8) | 
                         //app_attclient.value.data[0];
      
          DBG_TRACE("[#] Notification - Battery Voltage: %1.3f Volt\n",
			     voltage * 0.0001);
        }
      }	  */

    }

  ///DBG_TRACE("BT Rx Thread closed\n");
  ExitThread(0u);
} 
