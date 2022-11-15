//==============================================================================
//File name:   "thread_data.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include <afxwin.h>
#include <afxcmn.h>
#include <afxmt.h>
#include "hci.h"
#include "l2cap.h"
#include "parser.h"
#include "bt-dongle.h"

//==============================================================================
//Thread function: Monitoring data
//==============================================================================
DWORD DATA_RX_THREAD(LPVOID lParam)
{
  long objects;
  unsigned long rx_accepted;
  int device_event, ret=0, length, exit = 0;
  unsigned short conn_handle;
  unsigned char buf[512];
  const unsigned int buf_length = sizeof(buf);

  //----
  CBTDONGLE *pClass;
  pClass = (CBTDONGLE*)lParam;
  
  // This is synchronous I/O. Since we always open I/O items for
  // overlapped I/O we're obligated to always provide OVERLAPPED
  // structure to read / write routines.
  OVERLAPPED OVRLPD;
  ZeroMemory(&OVRLPD, sizeof(OVRLPD));
  OVRLPD.hEvent = CreateEvent(NULL, TRUE, FALSE, "ReadUSBPipeDATA");
  OVRLPD.Internal = STATUS_PENDING;

  //----
  HANDLE Handles[] = {OVRLPD.hEvent, ev_Quit[1]};
  
  //Перед запуском цикла послать подтверждение
  pClass->ev_DATA.PulseEvent();

  //---- launch Tread loop 
  for(;;)
  {  
    //----
    ret = WinUsb_ReadPipe(pClass->WUSB.hUSB, EP2_IN, 
			              buf, buf_length, &rx_accepted, &OVRLPD);
    
	//waiting event`s
    objects = WaitForMultipleObjects(2, Handles, FALSE, INFINITE);

	switch (objects)
	{
	  //-------------------------------------------------
	  //Data event processing
	  //-------------------------------------------------
	  case WAIT_OBJECT_0:	
	  //Read data packet from dongle
      /* ret = libusb_bulk_transfer(pClass->hUSB, EP2_IN,
							   buf, 256, &rx_accepted, 5000); */
	  //----
	  if(rx_accepted>0)
	  {
	    //---- Bluetooth HCI ACL Packet layer
	    conn_handle = buf[1];
	    conn_handle <<= 8;
	    conn_handle |= buf[0];
	    conn_handle &= 0x0FFF;

	    //---- .... 0000 0100 0000 = Connection Handle: 0x040
	    if(pClass->dev_connection_handle == conn_handle)
	    {
	      //---- ACL packet data length
	      length = buf[3];
	      length <<= 8;
	      length |= buf[2];
		
		  memset(pClass->L2CAP_buf, 0, sizeof(pClass->L2CAP_buf));
	      memcpy(pClass->L2CAP_buf, &buf[4], length);
		
		  //---- Enter to L2CAP layer
	      pClass->L2CAP_Decoder(length);
	    }
	    
		///TRACE("ACL RX < "); print_buffer((char*)buf, 0, rx_accepted);
	    ///pClass->ev_DATA.PulseEvent();
	  }
	  break;

	  //-------------------------------------------------
	  //Exit event processing
	  //-------------------------------------------------
	  case WAIT_OBJECT_0+1:
	  exit = 1;
      break;
	}

	if(exit==1) 
	   break;
  }

  #ifdef BLE_DEBUG
  TRACE1("%s Close\n", __FUNCTION__);
  #endif
       
  CloseHandle(OVRLPD.hEvent);
  OVRLPD.hEvent = NULL;

  //---- End Tread
  AfxEndThread(0, TRUE);
  return 0;
}

 /*
  //---- launch Tread loop 
  for(;;)
	{
       size = usb_bulk_read(handl, EP2_IN, (char*)buf, 256, 0);
       if(size>1)device_event = hci_acl_read_parser(buf, size);

	   //		
	   if(device_event==2)
	   {   
		 memset(buf,0,sizeof(buf));
		  /*
		 Sleep(20);	
		 //послать ответ  TX: 00 03 04 01 00 00 00
		 buf[0] = 0; buf[1] = 3; buf[2] = 4; buf[3] = 1; buf[4] = 0; buf[5] = 0; buf[6] = 0xaa;
		 usb_bulk_write(handl, EP2_OUT, (char*)buf, 7, 0);
		 printf("Send TX:1\n");	fflush(stdout); 
		  */
///		 struct l2cap_channel_t cr;

/*		 static void att_read_type_rsp(const struct l2cap_frame *frame)
        {
	       const struct bt_l2cap_att_read_group_type_rsp *pdu = frame->data;

	       print_field("Attribute data length: %d", pdu->length);
	       print_data_list("Attribute data list", pdu->length,
					frame->data + 1, frame->size - 1);
        }	*/

/*

Example for L2CAP Connect Response Packet

(All Values are in Hex)
L2CAP Connect response

0c 00 01 00 03 01 08 00 40 00 40 00 00 00 00 00

0c 00 - Payload length
01 00 - SIG Command (Connection Oriented)
03 - UcCode (Connect Rsp)
01 - Identifier (This should be the same value that is received in the Identifier from L2cap Connect Request)
08 00 - Length
40 00 - Local Channel Identifier
40 00 - Remote Channel Identifier (same value recieved from Master)
00 00 - Result
00 00 - Status
*/	  /*
		 char t[] ={ 0x0c,0x00,0x01,0x00,0x03,0x01,0x08,0x00,0x40,0x00,0x40,0x00,0x00,0x00,0x00,0x00};
		 ret = usb_bulk_write(handl, EP2_OUT, (char*)t, sizeof(t), 100);
		 /*
		 //Sleep(20);
		 //послать ответ  TX: 80 08 04 02 00 01 00 07 00 02 00 18
		 buf[0] = 0x80; buf[1] = 8; buf[2] = 4; buf[3] = 2; buf[4] = 0; buf[5] = 1; buf[6] = 0;
		 buf[7] = 7; buf[8] = 0; buf[9] = 2; buf[10] = 0; buf[11] = 0x18;
		 ret = usb_bulk_write(handl, EP2_OUT, (char*)buf, 12, 100);
		 printf("Send TX:2 sz=%d\n", ret);	fflush(stdout);	  */
		  /*
		 Sleep(3000);
		 //послать ответ  TX: 80 08 04 02 00 08 00 08 00 02 01 18
		 buf[0] = 0x80; buf[1] = 8; buf[2] = 4; buf[3] = 2; buf[4] = 0; buf[5] = 8; buf[6] = 0;
		 buf[7] = 8; buf[8] = 0; buf[9] = 2; buf[10] = 1; buf[11] = 0x18;
		 usb_bulk_write(handl, EP2_OUT, (char*)buf,12, 0);
		 printf("Send TX:3\n");	fflush(stdout);

		 Sleep(3000);
		 //послать ответ  TX: 80 16 04 02 00 09 00 ff ff 10 9e ca dc 24 0e e5 a9 e0 93 f3 a3 b5 01 00 40 6e
		 buf[0] = 0x80;  buf[1] = 0x16;  buf[2] = 4;     buf[3] = 2;     buf[4] = 0;     buf[5] = 9; buf[6] = 0;
		 buf[7] = 0xff;  buf[8] = 0xff;  buf[9] = 0x10;  buf[10] = 0x9e; buf[11] = 0xca; buf[12] = 0xdc; buf[13] = 0x24;
		 buf[14] = 0x0e; buf[15] = 0xe5; buf[16] = 0xa9; buf[17] = 0xe0; buf[18] = 0x93; buf[19] = 0xf3; buf[20] = 0xa3;
		 buf[21] = 0xb5; buf[22] = 0x01; buf[23] = 0x00; buf[24] = 0x40; buf[25] = 0x6e;
		 usb_bulk_write(handl, EP2_OUT, (char*)buf, 26, 0);
		 printf("Send TX:4\n");	fflush(stdout);

		 Sleep(3000);
		 //послать ответ  TX: 80 05 04 01 00 00 00 00 00
		 buf[0] = 0x80; buf[1] = 0x05; buf[2] = 0x04; buf[3] = 0x01; buf[4] = 0; buf[5] = 9; buf[6] = 0; buf[7] = 0; buf[8] = 0;
		 usb_bulk_write(handl, EP2_OUT, (char*)buf, 9, 0);
		 printf("Send TX:5\n");	fflush(stdout);    */
/*	   }

	   if(ev_ACLQuit.m_hObject) break;
    }
  */  




/*

   //---- launch Tread loop 
  for(;;)
	{
    //waiting event`s
	dwObjects = WaitForMultipleObjects(2, Handles, FALSE, INFINITE);
    		
    switch (dwObjects)
	{			
		//------ Timer tick for read HCI device
		case WAIT_OBJECT_0:
       
		//if(ACL_ENABLE)
		{ 	
		 device_event = 0;
         size = usb_bulk_read(handl, EP2_IN, (char*)Buffer, 256, 0);
         if(size>1)device_event = hci_acl_read_parser(Buffer, size);
       
   
      //-------------------------------------------------
			//Events processing
			//-------------------------------------------------
			switch (device_event) 
			{	
				//device send broadcast data
			  case 0x01:   
        ///Send_broadcast_pack();
        break;
          
				case 0x03:
        //Connection_Completed_0x03(Buffer, size);
				break;

        case 0x05:
        //Beep(200, 100);
        //Disconnection_Complete_0x05();  
        break;
        
		   } 
       
       } 
  
			//================
			break; //WAIT_OBJECT_0:
			
			//------ synchro event from ev_Quit
			case WAIT_OBJECT_0+1:
			//------ others case
			default:

      //Release timer
///Exit: CancelWaitableTimer(hTimer);
///	    CloseHandle (hTimer);
      
      //---- End Tread
      AfxEndThread(0, TRUE);
			return 0; 
		}
	}   
}*/