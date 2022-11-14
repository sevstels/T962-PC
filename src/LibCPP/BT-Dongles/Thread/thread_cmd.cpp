//==============================================================================
//File name:   "thread_cmd.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include <afxwin.h>
#include <afxcmn.h>
#include <afxmt.h>
#include "bt-dongle.h"
#include "parser.h"
#include "hci.h"

 /*
//==============================================================================
//Thread: CMD Event
//==============================================================================
//Thread function, обьявление функций потоков
DWORD HCI_EVENT_RX_THREAD(LPVOID lParam);
DWORD HCI_EVENT_RX_THREAD(LPVOID lParam)
{
  struct usb_dev_handle *handl;
  int device_event, size;
  unsigned char Buffer[256];
  BOOL result;
  HANDLE hTimer = NULL;
  DWORD dwObjects;
  HANDLE Handles[2];
  hTimer = CreateWaitableTimer(NULL, FALSE, _T("USB Tick timer"));
  Handles[0] = hTimer;
  Handles[1] = ev_CMDQuit.operator HANDLE();
  handl = (struct usb_dev_handle*)lParam;

  //Synchronisation objects                                    
  ResetEvent(Handles[0]);
  ResetEvent(Handles[1]);

  //Set the timer
  //относительное время
  LARGE_INTEGER lPeriod;
  int ms = 1; //было 10
  lPeriod.QuadPart = - ((int)ms * 10000);
  //result = SetWaitableTimer(hTimer, &lPeriod, ms, TimerAPCProc, NULL, FALSE);
  result = SetWaitableTimer(hTimer, &lPeriod, ms, NULL, NULL, FALSE);
  //for (;;) SleepEx(INFINITE, TRUE);
  if(result==FALSE)
  {
	printf("\nSet CMD Timer Error! Exit"); 
    CancelWaitableTimer(hTimer);
    CloseHandle (hTimer);
      
    //End Tread
    AfxEndThread(0, TRUE);
    return 0;
  }      
  
  //---- launch Tread loop 
  for(;;)
  {
    //waiting event`s
	dwObjects = WaitForMultipleObjects(2, Handles, FALSE, INFINITE);
    		
    switch (dwObjects)
	{			
	  //------ Timer tick for read HCI device
	  case WAIT_OBJECT_0:
			   
      //device_event = tick_handler_usb_interrupt_read(handl);
      //if(handl==NULL) break;
      
      size = usb_interrupt_read(handl, EP1_IN, (char*)Buffer, 256, 0);
      if(size<1) 
		  break;

      //memset(&Buffer[size], 0, sizeof(Buffer)-size);
      device_event = hci_cmd_read_parser(Buffer, size);
      if(device_event<=0) 
		  break;
      
      //-------------------------------------------------
	  //Events processing
	  //-------------------------------------------------
	  switch (device_event) 
	  {	
		//device send data
        case 0xFF:   
        //Beep(1000, 10);
        if(Buffer[1]==0x12 && Buffer[2]==0x29) VendorIRQ_Connection_Request(Buffer, size);
        if(Buffer[1]==0x01 && Buffer[2]==0x21) VendorIRQ_Lost3DSync();
        break;			  
        
        case 0x04:   
        Connection_Request_0x04(Buffer, size);
        break;
          
		case 0x03:
        Connection_Completed_0x03(Buffer, size);
		break;

        case 0x05:
        Disconnection_Completed_0x05();  
        break;
        
		case 0x3E:
		BLE_Connection_Completed_0x3E(Buffer, size);
        break;

        /// Builk data read
        case 0x100:
        //size = usb_bulk_read(handl, EP2_IN, (char*)BBuffer, 256, 0);
        //hci_cmd_rd_parser((void*)BBuffer, 1000+size);
        break;
     }
  
    //================
    break; //WAIT_OBJECT_0:
			
    //synchro event from ev_Quit
    case WAIT_OBJECT_0+1:
    //------ others case
    default:
	break;
   }
  }
  
  //Release timer
  CancelWaitableTimer(hTimer);
  CloseHandle (hTimer);
      
  //End Tread
  AfxEndThread(0, TRUE);
  return 0; 
} 
*/

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int hci_parser2(unsigned char *pData, int size)
{
  int handler=0;
  char flip[256];
  memset (flip, 0, sizeof(flip));

  if((pData[4]==5)&&(pData[5]==0)&&(pData[6]==2))
     {
        TRACE("\n> Controller get 3DG Connection Announcement");
        TRACE1("\n> Battery level: %d persent\n\n", pData[size-1]);
       //call handler
     handler = 1;
     }
    else
     {
        TRACE1("\n> Controller send ACL Data: Sz=%d\n", size);
        print_buffer((char*)&pData[0], 0, size);
        
        TRACE1("\n> Flip Data: Sz=%d\n", size);
        for(int i=0; i<size; i++)
        {
          flip[i] = pData[size-1-i];
        }
        print_buffer((char*)flip, 0, size);

        //послать ответ  RX: 00030401000000
        handler = 2;
     }

  return handler; 
}

//==============================================================================
//Thread: CMD Events
//==============================================================================
//Thread function, обьявление функций потоков
DWORD HCI_RX_THREAD(LPVOID lParam)
{
  long object;
  unsigned long rx_accepted;
  int device_event, ret, bResult, dwError, exit = 0;
  unsigned char buf[256];
  unsigned char event_code;
  unsigned char subevent_code;
  unsigned char data_length;
  const unsigned int buf_length = sizeof(buf);
  
  //----
  CBTDONGLE *pClass;
  pClass = (CBTDONGLE*)lParam;
    
  // This is synchronous I/O. Since we always open I/O items for
  // overlapped I/O we're obligated to always provide OVERLAPPED
  // structure to read / write routines.
  OVERLAPPED OVRLPD;
  ZeroMemory(&OVRLPD, sizeof(OVRLPD));
  OVRLPD.hEvent = CreateEvent(NULL, TRUE, FALSE, "ReadUSBPipeCMD");
  OVRLPD.Internal = STATUS_PENDING;
  OVRLPD.Offset = 0;
  OVRLPD.OffsetHigh = 0;
  
  //----
  HANDLE Handles[] = {OVRLPD.hEvent, ev_Quit[0]};

  //флаг поток открыт 
  pClass->thead = 1;

  //Перед запуском цикла послать подтверждение
  pClass->ev_CMD.PulseEvent();

  //---- launch Tread loop 
  for(;;)
  { 
	next:
	ret = WinUsb_ReadPipe(pClass->WUSB.hUSB, EP1_IN, 
			              buf, buf_length, &rx_accepted, &OVRLPD);

	//ошибка при инициализации чтения
	if(ret)
	{
	  //Something went wrong.  Return the Windows error code
      dwError = GetLastError();
	  
	  //Something went wrong - abort the transfer
      WinUsb_AbortPipe(pClass->WUSB.hUSB, EP1_IN);
	  
	  goto next;
	}

	//waiting event`s
    object = WaitForMultipleObjects(2, Handles, FALSE, INFINITE);

	if(object==0)
	{
	   //The overlapped IO request completed so check to see how many bytes we got
       bResult = WinUsb_GetOverlappedResult(pClass->WUSB.hUSB, &OVRLPD, &rx_accepted, FALSE);
       if(bResult)
       {
          dwError = ERROR_SUCCESS;
		  if(pClass->thead==1)
	      {
		    //if(rx_accepted>sizeof(pClass->HCI_buf)) 
		    //   rx_accepted = sizeof(pClass->HCI_buf);
		    memset(pClass->HCI_buf, 0, sizeof(pClass->HCI_buf));
		    memcpy(pClass->HCI_buf, buf, rx_accepted); 
		    pClass->ev_CMD.PulseEvent();
	      }
          //memset(&Buffer[size], 0, sizeof(Buffer)-size);
          //device_event = hci_acl_read_parser(buf, rx_accepted);

	      //----	 
	      //[0] - Event Code: Command Complete (0x0e)
          //[1] - Parameter Total data Length
          //[2] - Number of Allowed Command Packets
          //[3] - opcode L
          //[4] - opcode H
          //[5] - Status: Success = 0x00
	      event_code = buf[0];
	      data_length = buf[1]; 
	      subevent_code = buf[2];

		  //-------------------------------------------------
	      //Events processing
	      //-------------------------------------------------
	      switch(event_code) 
	      {					   /*
		   //device send data
           case 0xFF:
           if(buf[1]==0x12 && buf[2]==0x29) VendorIRQ_Connection_Request(buf, rx_accepted);
           if(buf[1]==0x01 && buf[2]==0x21) VendorIRQ_Lost3DSync();
           break;				  */ 
        	
           //----
	       case 0x3E:   
           //-----
		   switch(subevent_code) 
	        {	
	          //Sub Event: LE Advertising Report (0x02)
			  //HCI_EVT	59	Rcvd LE Meta (LE Advertising Report)
		      case 0x02:
              pClass->LE_AdvertisingReport(buf, rx_accepted);
			  break;			  
			  
			  //Sub Event: LE Connection Complete (0x01)
			  //HCI_EVT	48 Rcvd LE Meta (LE Connection Complete)
		      case 0x01:
              pClass->LE_Connection_Complete(&buf[3], data_length);
			  break;
			  
			  //Sub Event: LE Read Remote Features Complete (0x04)
		      case 0x04:
              pClass->LE_Read_Remote_Features_Complete(&buf[3], data_length);
			  break;

			  //----
			  default:
		      break;
			}
		   break;			   
        
		  //Event Code: Read Remote Version Information Complete (0x0c)
		  case 0x0C:
  		  pClass->Read_Remote_Version_Information_Complete(&buf[3], data_length);
		  break;
		 }  
	    }
      }


      //-------------------------------------------------
	  //Exit event processing
	  //-------------------------------------------------
      if(object>=WAIT_OBJECT_0+1)
	  {
	    exit = 1;
      }

	  if(exit==1) break;
	}
	/*
	switch (objects)
	{
	  //-------------------------------------------------
	  //Data event processing
	  //-------------------------------------------------
	  case WAIT_OBJECT_0:
	   //----
	   //Read answer packet from dongle
	    //----
	    if(rx_accepted==0) 
		    break;

	    if(pClass->thead==1)
	    {
		  //if(rx_accepted>sizeof(pClass->HCI_buf)) 
		  //   rx_accepted = sizeof(pClass->HCI_buf);
		  memset(pClass->HCI_buf, 0, sizeof(pClass->HCI_buf));
		  memcpy(pClass->HCI_buf, buf, rx_accepted); 
		  pClass->ev_CMD.PulseEvent();
	    }
        //memset(&Buffer[size], 0, sizeof(Buffer)-size);
        //device_event = hci_acl_read_parser(buf, rx_accepted);

	    //----	 
	    //[0] - Event Code: Command Complete (0x0e)
        //[1] - Parameter Total data Length
        //[2] - Number of Allowed Command Packets
        //[3] - opcode L
        //[4] - opcode H
        //[5] - Status: Success = 0x00
	    event_code = buf[0];
	    data_length = buf[1]; 
	    subevent_code = buf[2];
	 
	    //-------------------------------------------------
	    //Events processing
	    //-------------------------------------------------
	    switch(event_code) 
	     {					   /*
		   //device send data
           case 0xFF:
           if(buf[1]==0x12 && buf[2]==0x29) VendorIRQ_Connection_Request(buf, rx_accepted);
           if(buf[1]==0x01 && buf[2]==0x21) VendorIRQ_Lost3DSync();
           break;				  */ 
        	/*
           //----
	       case 0x3E:   
           //-----
		   switch(subevent_code) 
	        {	
	          //Sub Event: LE Advertising Report (0x02)
			  //HCI_EVT	59	Rcvd LE Meta (LE Advertising Report)
		      case 0x02:
              pClass->LE_AdvertisingReport(buf, rx_accepted);
			  break;			  
			  
			  //Sub Event: LE Connection Complete (0x01)
			  //HCI_EVT	48 Rcvd LE Meta (LE Connection Complete)
		      case 0x01:
              pClass->LE_Connection_Complete(&buf[3], data_length);
			  break;
			  
			  //Sub Event: LE Read Remote Features Complete (0x04)
		      case 0x04:
              pClass->LE_Read_Remote_Features_Complete(&buf[3], data_length);
			  break;

			  //----
			  default:
		      break;
			}
		   break;			   
        
		  //Event Code: Read Remote Version Information Complete (0x0c)
		  case 0x0C:
  		  pClass->Read_Remote_Version_Information_Complete(&buf[3], data_length);
		  break;
           
		  /*
          case 0x05:
          Disconnection_Completed_0x05();  
          break;
        
		  case 0x3E:
		  BLE_Connection_Completed_0x3E(buf, rx_accepted);
          break;

          /// Builk data read
          case 0x100:
          //size = usb_bulk_read(handl, EP2_IN, (char*)BBuffer, 256, 0);
          //hci_cmd_rd_parser((void*)BBuffer, 1000+size);
          break;*/	/*
        }
	  break;

	  //-------------------------------------------------
	  //Exit event processing
	  //-------------------------------------------------
	  case WAIT_OBJECT_0+1:
	  exit = 1;
      break;
	}


	//---------------
    if(ret<0)
    {
      //An error occurred or the read will complete asynchronously.
      //Which is it?
      dwError = GetLastError();
 
      //Check for error cases other than the one we expect
      if(dwError == ERROR_IO_PENDING)
      {	/*
        //The IO is pending so wait for it to complete or for a timeout to occur.
        phSignals[0] = psDevInfo->hReadEvent;
        phSignals[1] = hBreak;
        dwError = WaitForMultipleObjects(hBreak ? 2 : 1, phSignals, FALSE, ulTimeoutmS);
 		*/
		/*
	    //waiting event`s
        objects = WaitForMultipleObjects(2, Handles, FALSE, INFINITE);

        //At this stage, one of three things could have occurred
        //Either we read a packet or we timed out or the break signal was detected. Which was it?
        if(dwError == WAIT_OBJECT_0)
        {
		   

          //The overlapped IO request completed so check to see how many bytes we got
          bResult = WinUsb_GetOverlappedResult(pClass->WUSB.hUSB, &OVRLPD, &rx_accepted, FALSE);
          if(bResult)
          {
             dwError = ERROR_SUCCESS;
          }
          else
          {
             //Something went wrong.  Return the Windows error code
             dwError = GetLastError();
          }
         }
        else
        {
          //Something went wrong - abort the transfer
          WinUsb_AbortPipe(pClass->WUSB.hUSB, EP1_IN);
 
          //Was the break event signalled?
          if(dwError==(WAIT_OBJECT_0 + 1))
          {
            //The break event was signalled - abort the read and return
            dwError = WAIT_OBJECT_0;
          }
        }
      }
	}




	//---------------
	/*

    //waiting event`s
    objects = WaitForMultipleObjects(2, Handles, FALSE, INFINITE);
    
	switch (objects)
	{
	  //-------------------------------------------------
	  //Data event processing
	  //-------------------------------------------------
	  case WAIT_OBJECT_0:
	   //----
	   //Read answer packet from dongle
	    //----
	    if(rx_accepted==0) 
		    break;

	    if(pClass->thead==1)
	    {
		  //if(rx_accepted>sizeof(pClass->HCI_buf)) 
		  //   rx_accepted = sizeof(pClass->HCI_buf);
		  memset(pClass->HCI_buf, 0, sizeof(pClass->HCI_buf));
		  memcpy(pClass->HCI_buf, buf, rx_accepted); 
		  pClass->ev_CMD.PulseEvent();
	    }
        //memset(&Buffer[size], 0, sizeof(Buffer)-size);
        //device_event = hci_acl_read_parser(buf, rx_accepted);

	    //----	 
	    //[0] - Event Code: Command Complete (0x0e)
        //[1] - Parameter Total data Length
        //[2] - Number of Allowed Command Packets
        //[3] - opcode L
        //[4] - opcode H
        //[5] - Status: Success = 0x00
	    event_code = buf[0];
	    data_length = buf[1]; 
	    subevent_code = buf[2];
	 
	    //-------------------------------------------------
	    //Events processing
	    //-------------------------------------------------
	    switch(event_code) 
	     {					   /*
		   //device send data
           case 0xFF:
           if(buf[1]==0x12 && buf[2]==0x29) VendorIRQ_Connection_Request(buf, rx_accepted);
           if(buf[1]==0x01 && buf[2]==0x21) VendorIRQ_Lost3DSync();
           break;				  */ 
        	   /*
           //----
	       case 0x3E:   
           //-----
		   switch(subevent_code) 
	        {	
	          //Sub Event: LE Advertising Report (0x02)
			  //HCI_EVT	59	Rcvd LE Meta (LE Advertising Report)
		      case 0x02:
              pClass->LE_AdvertisingReport(buf, rx_accepted);
			  break;			  
			  
			  //Sub Event: LE Connection Complete (0x01)
			  //HCI_EVT	48 Rcvd LE Meta (LE Connection Complete)
		      case 0x01:
              pClass->LE_Connection_Complete(&buf[3], data_length);
			  break;
			  
			  //Sub Event: LE Read Remote Features Complete (0x04)
		      case 0x04:
              pClass->LE_Read_Remote_Features_Complete(&buf[3], data_length);
			  break;

			  //----
			  default:
		      break;
			}
		   break;			   
        
		  //Event Code: Read Remote Version Information Complete (0x0c)
		  case 0x0C:
  		  pClass->Read_Remote_Version_Information_Complete(&buf[3], data_length);
		  break;
           
		  /*
          case 0x05:
          Disconnection_Completed_0x05();  
          break;
        
		  case 0x3E:
		  BLE_Connection_Completed_0x3E(buf, rx_accepted);
          break;

          /// Builk data read
          case 0x100:
          //size = usb_bulk_read(handl, EP2_IN, (char*)BBuffer, 256, 0);
          //hci_cmd_rd_parser((void*)BBuffer, 1000+size);
          break;*/	/*
        }
	  break;

	  //-------------------------------------------------
	  //Exit event processing
	  //-------------------------------------------------
	  case WAIT_OBJECT_0+1:
	  exit = 1;
      break;
	}

	*/
	/*

	if(exit==1) 
		 break;	
  }  */ 
  
  #ifdef BLE_DEBUG
  TRACE1("%s Close\n", __FUNCTION__);
  #endif
  
  CloseHandle(OVRLPD.hEvent);
  OVRLPD.hEvent = NULL;

  //End Tread
  AfxEndThread(0, TRUE);
  return 0; 
}	