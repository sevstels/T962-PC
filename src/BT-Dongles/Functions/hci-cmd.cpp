//==============================================================================
//File name:   ".cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "bt.h"
#include "trace.h"
#include "parser.h"
#include "bt-dongle.h"
#include "beacon_msg.h"

//------------------------------------------------------------------------------
//Function
//------------------------------------------------------------------------------
int CBTDONGLE::HCI_CMD(unsigned short opcode, //CMD opcode
                       const void *pTxBuf, 	  //CMD buffer
                       unsigned char TxLength,//CMD length
                       const void *pRxBuf, 	  //Reply buffer
                       int RxLength,          //Reply length
                       int RxTimeout)         //Reply wait timeout
{
  if(usb_opened==0) return -1; 
  
  int result, rx_accepted=0, any_accept_length=0;
  
  //add cmd code
  memcpy(HCI_buf, &opcode, 2);	
  
  //add command data
  if(pTxBuf!=NULL && TxLength>0) 
	 memcpy(&HCI_buf[3], pTxBuf, TxLength);
  else 
	 TxLength = 0;
   
  //cmd data lenght
  HCI_buf[2] = TxLength; 

  //----
  if(thead==1)
  {
	 ev_CMD.ResetEvent(); 
  }

  WUSB.ControlSend(WUSB.hUSB, (char*)HCI_buf, TxLength+3);

  //----
  if(thead==1)
  { 
	long state = WaitForSingleObject(ev_CMD.m_hObject, RxTimeout);
	if(state==0)
	{
	  if(RxLength!=0)
	    {memcpy((void*)pRxBuf, &HCI_buf[5], RxLength);}
	  return 1;
	}
	return -1;
  }
  
  memset(HCI_buf, 0, sizeof(HCI_buf));

  //[0]	- Event Code: Command Complete (0x0e)
  //[1]	- Parameter Total data Length
  //[2]	- Number of Allowed Command Packets
  //[3]	- opcode L
  //[4] - opcode H
  //[5] - Status: Success = 0x00

  //Число байт для считывания в буфер
  int rx_length = 6;
  if(RxLength>0){rx_length += RxLength-1;}

  //Wait answer packet from dongle
  result = WUSB.ControlRead(WUSB.hUSB, (char*)HCI_buf, 
	                        rx_length, rx_accepted, RxTimeout);

  //Check Rx										
  if(result<0) 
  {
	Show_Errors(result);
	//err
	return -1;
  }

  if(pRxBuf!=NULL && RxLength>0)
  {
	memcpy((void*)pRxBuf, &HCI_buf[5], RxLength);
  }

  //Exit OK, for unknown rx Length and defined length
  //if(result==0 && rx_accepted>0)
  if(result>=0)
  { 
	 result = CheckResponse(opcode, (unsigned char*)HCI_buf, rx_accepted);
	 if((result>0)/* && (RxLength+6==rx_accepted)*/) return 1; //ok
	 else
	 {
	   TRACE1("CMD 0x%04X ACK Error\n", opcode);
	   //err
	   return -1;
	 }
  }
  
  //Exit error
  return -1;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::HCI_FWR(unsigned short opcode, //CMD opcode
                       const void *pTxBuf, 	  //CMD buffer
                       unsigned char TxLength,//CMD length
                       const void *pRxBuf, 	  //Reply buffer
                       int RxLength,          //Reply length
                       int RxTimeout)         //Reply wait timeout
{
  if(usb_opened==0) return -1; 
  
  int result, rx_accepted=0, any_accept_length=0;
  
  //add cmd code
  memcpy(HCI_buf, &opcode, 2);	
  //cmd data lenght
  HCI_buf[2] = TxLength;
  
  //add command data
  memcpy(&HCI_buf[3], pTxBuf, TxLength);

  /*
  result = libusb_bulk_transfer(hUSB, //Указатель на устройство
								EP2_OUT,
								HCI_buf,   //буфер 
								TxLength+3,
								&rx_accepted,
								10);     //время ожидание реакции 
  */

  //----
  result = WUSB.BulkSend(WUSB.hUSB, EP2_OUT, (char*)HCI_buf, TxLength+3);

  //Check Tx
  if(result<0) 
  {
	Show_Errors(result);
	//err
	return -1;
  }

  memset(HCI_buf, 0, 256+16);

  //[0]	- Event Code: Command Complete (0x0e)
  //[1]	- Parameter Total data Length
  //[2]	- Number of Allowed Command Packets
  //[3]	- opcode L
  //[4] - opcode H
  //[5] - Status: Success = 0x00

  //Число байт для считывания в буфер
  int rx_length = 6;
  if(RxLength>0){rx_length += RxLength-1;}

  //Wait answer packet from dongle
  //If END point have type "interrupt", need read like interrupt
  /*
  result = libusb_interrupt_transfer(hUSB, EP1_IN,
								    (unsigned char*)HCI_buf, 
									 rx_length, 
									&rx_accepted, RxTimeout);
  */

  //----
  //result = USB.BulkRead(USB.hUSB, EP1_IN, (char*)HCI_buf, rx_length, rx_accepted);
  result = WUSB.ControlRead(WUSB.hUSB, (char*)HCI_buf, rx_length, rx_accepted, 100);
  
  //Check Rx										
  if(result<0) 
  {
	Show_Errors(result);
	//err
	return -1;
  }

  if(pRxBuf!=NULL && RxLength>0)
  {
	memcpy((void*)pRxBuf, &HCI_buf[5], RxLength);
  }

  //Exit OK, for unknown rx Length and defined length
  //if(result==0 && rx_accepted>0)
  if(result>=0)
  { 
	 result = CheckResponse(opcode, (unsigned char*)HCI_buf, rx_accepted);
	 if((result>0)/* && (RxLength+6==rx_accepted)*/) return 1; //ok
	 else
	 {
	   TRACE1("CMD 0x%04X ACK Error\n", opcode);
	   //err
	   return -1;
	 }
  }
  
  //Exit error
  return -1;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::HCI_DATA(const void *pTxBuf,    //buffer
                        unsigned char TxLength)//length
{
  if(usb_opened==0) return -1; 
  
  int result;
  
  //add command data
  memcpy(&HCI_buf[3], pTxBuf, TxLength);

  //----
  result = WUSB.BulkSend(WUSB.hUSB, EP2_OUT,(char*)pTxBuf, TxLength);
  
  //Check Tx
  if(result!=TxLength) 
  {
	Show_Errors(result);
	//err
	return -1;
  }

  return result;
}