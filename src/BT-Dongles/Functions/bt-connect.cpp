//==============================================================================
//File name:   "bt-connect.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:   (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <dbt.h>
#include "bt.h"
#include "parser.h"
#include "bt-dongle.h"
#include "bt-utils.h"

//---- Mem Leakage Debug
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//----

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---- If need Debug
//#define  BLE_DEBUG
#define  CMD_TIMEOUT 200

//------------------------------------------------------------------------------
//Open by BLE device address
//------------------------------------------------------------------------------
int CBTDONGLE::WaitEvData(void)
{
  ev_DATA.ResetEvent();
  long state = WaitForSingleObject(ev_DATA.m_hObject, 4000);
  if(state!=0)
  {
	//не получилось
	return -1;
  }
 
  return 1;
}

//------------------------------------------------------------------------------
//Open by BLE device address
//------------------------------------------------------------------------------
int CBTDONGLE::WaitEvBLE(void)
{
  ev_BLE.ResetEvent();
  long state = WaitForSingleObject(ev_BLE.m_hObject, 4000);
  if(state!=0)
  {
	//не получилось
	return -1;
  }
 
  return 1;
}

//------------------------------------------------------------------------------
//Open by BLE device address
//------------------------------------------------------------------------------
int CBTDONGLE::Connect(char *pDevAddress)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  //if already opened
  if(adapter_opened!=1 || pDevAddress==NULL) return -1;
  
  int length, result;
  char buf[256];
  unsigned short dev_handle;

  //HCI_CMD	33	Sent LE Set Scan Enable
  //HCI LE Set Scan Enable
  buf[0] = 0x0; //Scan Enable: false (0x00)
  buf[1] = 0x0; //Filter Duplicates: false (0x00)
  result = HCI_CMD(0x200c, buf, 2, NULL, 0, CMD_TIMEOUT);
  //if(result<0) return -1;

  //HCI Broadcom Unknown Command 0x0178 (opcode 0xFD78)
  buf[0] = 0x05;
  buf[1] = 0x01;
  result = HCI_CMD(0xFD78, buf, 2, buf, 12, CMD_TIMEOUT); 
  //if(result<0) return -1;

  //HCI	Sent LE Set Scan Parameters
  buf[0] = 0x00;                //Scan Type: Passive (0x00)
  buf[1] = 0xBD; buf[2] = 0x00; //Scan Interval: 189 (118,125 msec)
  buf[3] = 0x1D; buf[4] = 0x00; //Scan Window: 29 (18,125 msec)
  buf[5] = 0x00; //Own Address Type: Public Device Address (0x00)
  //Scan Filter Policy: Accept only advertisements from devices in the White List
  //Ignore directed advertisements not addressed to this device (0x01)
  buf[6] = 0x01;  
  result = HCI_CMD(0x200B, buf, 7, NULL, 0, CMD_TIMEOUT);
  //if(result<0) return -1;

  //HCI	Write Scan Enable	!!!!!!!! тут должно быть 2 байта
  buf[0] = 0x02; //Scan Enable: Inquiry Scan disabled/Page Scan enabled (0x02)
  result = HCI_CMD(0x0C1A, buf, 1, NULL, 0, CMD_TIMEOUT);
  //if(result<0) return -1;

  //HCI	Sent LE Create Connection
  //Command Opcode: LE Create Connection (0x200d)
  struct bt_hci_cmd_le_create_conn le_conn;
  memset(&le_conn, 0, sizeof(le_conn));

  //Command settings
  //Scan Interval: 18 (11,25 msec)
  le_conn.scan_interval = 0x12;
  //Scan Window: 18 (11,25 msec)
  le_conn.scan_window = 0x12;
  //Initiator Filter Policy: Use Peer Address (0x00)
  le_conn.filter_policy = 0x00;
  //Peer Address Type: Random Device Address (0x01)
  le_conn.peer_addr_type = 0x01;

  //Target MAC address d0:fd:00:a3:dd:5c
  for(int i=0; i<6; i++)
  {
	char txt[3] = {0,0,0};
	unsigned int value;
	memcpy(txt, pDevAddress, 2);
	//read value in any format
    result = sscanf(txt, "%x", &value);
	pDevAddress += 2;
	
	//if error Exit 
  	if(result<=0) return -1;
	
	//API needs address reversed
    ///reverseArray(le_conn.peer_addr, 6); 
	le_conn.peer_addr[5-i] = (unsigned char) value;
  }
  
  //Own Address Type: Public Device Address (0x00)
  le_conn.own_addr_type = 0x00;

  //===============================================
  //connIntervalmin = Conn_Interval_Min * 1.25 ms
  //Conn_Interval_Min range: 0x0006 to 0x0C80
  //---
  //Connection Interval Min: 22 (27,5 msec)
  ///le_conn.min_interval = 0x16;
  le_conn.min_interval = (unsigned short)((float)16.4/(float)1.25); //8     7.25 msec
  //===============================================

  //===============================================
  //connIntervalmax = Conn_Interval_Max * 1.25 ms
  //Conn_Interval_Max range: 0x0006 to 0x0C80
  //Connection Interval Max: 48 (60 msec) >> !!!!!!!!!!!! <<
  le_conn.max_interval =(unsigned short)((float)16.8/(float)1.25);	//12  27
  //===============================================	   
  //0x5CF3706479CA bcm
  //0x0007802f068f bled112

  //Connection Latency: 0 (number events)
  le_conn.latency = 0; //0ms
  //Supervision Timeout: 960 (9,6 sec)
  le_conn.supv_timeout = 0x03c0;
  //Min CE Length: 0 (0 msec)
  le_conn.min_ce_length = 0x0;
  //Max CE Length: 0 (0 msec)
  le_conn.max_ce_length = 0x0;
  //--
  TRACE("Target BT ADDR: ");
  printAddr6((unsigned char*)&le_conn.peer_addr,':'); 
  TRACE("\n");

  //HCI LE_Create_Connection						 
  result = HCI_CMD(0x200D, &le_conn, sizeof(le_conn), NULL, 0, CMD_TIMEOUT);
    
  //если команда была выполнена
  if(result>0)
  {
	//ждать Rcvd LE Meta (LE Connection Complete)
    if(WaitEvBLE()!=1)
	{
	  TRACE("Error > Event LE Connection Complete\n");
	  //не получилось приконнектиться
	  return -1;
	}
  }  
  //else return -1;

  TRACE("Dev connection handle: 0x%04X\n", dev_connection_handle);

  //HCI Sent Read Remote Version Information
  //struct bt_hci_cmd_read_remote_version rmt_ver;
  //Connection Handle: 0x0040
  buf[0] = dev_connection_handle; //0x40; 
  buf[1] = dev_connection_handle>>8;    //0x00;
  result = HCI_CMD(0x041d, buf, 2, NULL, 0, CMD_TIMEOUT);
  //if(result<0) return -1;

  //Status,
  //Connection_Handle,
  //Version,
  //Manufacturer_Name,
  //Subversion 

  //если команда была выполнена
  if(result>0)
  {
	//ждать Rcvd LE Meta (Read Remote Version Information)
	if(WaitEvBLE()!=1)
	{
	  TRACE("Error > Event Read Remote Version Information\n");
	  //не получилось приконнектиться
	  return -1;
	}
  } 

  //HCI	Write Scan Enable !!!!!!!! тут должно быть 2 байта
  buf[0] = 0x02; //Scan Enable: Inquiry Scan disabled/Page Scan enabled (0x02)
  result = HCI_CMD(0x0C1A, buf, 1, NULL, 0, CMD_TIMEOUT);
  //if(result<0) return -1;

  //HCI Sent LE Read Channel Map
  struct bt_hci_rsp_le_read_channel_map map;
  buf[0] = dev_connection_handle; //0x40; 
  buf[1] = dev_connection_handle>>8;    //0x00;
  result = HCI_CMD(0x2015, buf, 2, &map, sizeof(map), CMD_TIMEOUT);
  //if(result<0) return -1;

  //HCI Sent LE Read Remote Features
  buf[0] = dev_connection_handle;   //0x40; 
  buf[1] = dev_connection_handle>>8;//0x00;
  result = result = HCI_CMD(0x2016, buf, 2, NULL, 0, CMD_TIMEOUT);
  //if(result<0) return -1;

  //если команда была выполнена
  if(result>0)
  {
    //ждать Rcvd LE Meta (LE Read Remote Features Complete)
    if(WaitEvBLE()!=1)
    {
	 TRACE("Error > LE Read Remote Features\n");
     //не получилось приконнектиться
	 return -1;
    }
  }
  
  ///WUSB.FlushFifo();

  //host Sent
  //Bluetooth HCI ACL Packet
  //Endpoint: 0x02, Direction: OUT
  char tx0[] = {0x40, 0x00, 0x07, 0x00, 0x03, 0x00, 0x04, 0x00, 0x02, 0x0f, 0x02};
  HCI_DATA(tx0, sizeof(tx0));
  if(WaitEvData()<1)
  {
	TRACE("ACL TX Error > "); print_buffer((char*)tx0, 0, sizeof(tx0));
///	return -1;
  }

  //Endpoint: 0x02, Direction: OUT
  char tx1[] = {0x40, 0x00, 0x1b, 0x00, 0x17, 0x00, 0x04, 0x00, 
	            0x06, 0x01, 0x00, 0xff, 0xff, 0x00, 0x28, 0x9e,
                0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 
				0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e};

  HCI_DATA(tx1, sizeof(tx1));
  if(WaitEvData()!=1)
  {
	 TRACE("ACL TX Error > "); print_buffer((char*)tx1, 0, sizeof(tx1));
	 return -1;
  }
 
  char tx2[] = {0x40, 0x00, 0x0d, 0x00, 0x09, 0x00, 0x04, 0x00, 
	            0x06, 0x01, 0x00, 0xff, 0xff, 0x00, 0x28, 0x01, 0x18};
  HCI_DATA(tx2, sizeof(tx2));
  TRACE("ACL TX > "); print_buffer((char*)tx2, 0, sizeof(tx2));
  
  if(WaitEvData()!=1) 
	 return -1;

  char tx3[] = {0x40, 0x00, 0x0b, 0x00, 0x07, 0x00, 0x04, 
	            0x00, 0x08, 0x09, 0x00, 0xff, 0xff, 0x03, 0x28};

  HCI_DATA(tx3, sizeof(tx3));
  TRACE("ACL TX > "); print_buffer((char*)tx3, 0, sizeof(tx3));
  if(WaitEvData()!=1) 
	 return -1;


  //1801
  char tx4[] = {0x40, 0x00, 0x0b, 0x00, 0x07, 0x00, 0x04, 0x00, 
	            0x08, 0x0b, 0x00, 0xff, 0xff, 0x03, 0x28};
  
  HCI_DATA(tx4, sizeof(tx4));
  TRACE("ACL TX > "); print_buffer((char*)tx4, 0, sizeof(tx4));
  
  if(WaitEvData()!=1) 
	 return -1;

  //1803
  char tx5[] = {0x40, 0x00, 0x0b, 0x00, 0x07, 0x00, 0x04, 
	            0x00, 0x08, 0x0e, 0x00, 0xff, 0xff, 0x03, 0x28};
  HCI_DATA(tx5, sizeof(tx5));
  TRACE("ACL TX > "); print_buffer((char*)tx5, 0, sizeof(tx5));
  if(WaitEvData()!=1) 
	 return -1;

  //1806   
  char tx6[] = {0x40, 0x00, 0x0d, 0x00, 0x09, 0x00, 0x04, 0x00, 
	            0x06, 0x09, 0x00, 0xff, 0xff, 0x00, 0x28, 0x01, 0x18};
  HCI_DATA(tx6, sizeof(tx6));
  TRACE("ACL TX > "); print_buffer((char*)tx6, 0, sizeof(tx6));
  if(WaitEvData()!=1) 
	 return -1;

  //1808
  char tx7[] = {0x40, 0x00, 0x09, 0x00, 0x05, 0x00, 
	            0x04, 0x00, 0x04, 0x0c, 0x00, 0x0c, 0x00};
  HCI_DATA(tx7, sizeof(tx7));
  TRACE("ACL TX > "); print_buffer((char*)tx7, 0, sizeof(tx7));
  if(WaitEvData()!=1) 
	 return -1;

  //1811
  char tx8[] = {0x40, 0x00, 0x0d, 0x00, 0x09, 0x00, 0x04, 0x00, 
	            0x06, 0x01, 0x00, 0xff, 0xff, 0x00, 0x28, 0x00, 0x18};
  HCI_DATA(tx8, sizeof(tx8));
  TRACE("ACL TX > "); print_buffer((char*)tx8, 0, sizeof(tx8));
  if(WaitEvData()!=1) 
	 return -1; 

  //1813
  //HCI Sent LE Add Device To White List
  struct bt_hci_cmd_le_add_to_white_list w_list;
  w_list.addr_type = 0x01; //Address Type: Random Device Address (0x01)
  //BD_ADDR: ee:af:0a:7c:43:28 (ee:af:0a:7c:43:28)
  w_list.addr[0] = 0x01;	 
  buf[0] = 0x01; 
  buf[1] = 0x28; 
  buf[2] = 0x43; 
  buf[3] = 0x7c; 
  buf[4] = 0x0a; 
  buf[5] = 0xaf; 
  buf[6] = 0xee;
  //----
  result = HCI_CMD(0x2011, buf, 7, NULL, 0, CMD_TIMEOUT);
  //if(result<0) return -1;

  //HCI Sent LE Set Scan Enable		 
  buf[0] = 0x01; //Scan Enable: Scan Enable: true (0x01)
  buf[1] = 0x00; //Filter Duplicates: false (0x00)
  result = HCI_CMD(0x0C1A, buf, 2, NULL, 0, CMD_TIMEOUT);
  //if(result<0) return -1;

  //1821
  char tx9[] = {0x40, 0x00, 0x09, 0x00, 0x05, 0x00, 0x04, 
	            0x00, 0x12, 0x0c, 0x00, 0x01, 0x00};
  HCI_DATA(tx9, sizeof(tx9));
  TRACE("ACL TX > "); print_buffer((char*)tx9, 0, sizeof(tx9));
  if(WaitEvData()!=1) 
	 return -1;

  //1824
  char tx10[] = {0x40, 0x00, 0x0d, 0x00, 0x09, 0x00, 0x04, 0x00, 
	             0x06, 0x08, 0x00, 0xff, 0xff, 0x00, 0x28, 0x00, 0x18};
  HCI_DATA(tx10, sizeof(tx10));
  TRACE("ACL TX > "); print_buffer((char*)tx10, 0, sizeof(tx10));
  if(WaitEvData()!=1) 
	 return -1;

  //1826
  char tx11[] = {0x40, 0x00, 0x0b, 0x00, 0x07, 0x00, 0x04, 
	             0x00, 0x08, 0x09, 0x00, 0xff, 0xff, 0x03, 0x28};
  HCI_DATA(tx11, sizeof(tx11));
  TRACE("ACL TX > "); print_buffer((char*)tx11, 0, sizeof(tx11));
  if(WaitEvData()!=1) 
	 return -1;

  //1829
  char tx12[] = {0x40, 0x00, 0x0b, 0x00, 0x07, 0x00, 0x04, 0x00, 0x08, 0x0b, 0x00, 0xff, 0xff, 0x03, 0x28};
  HCI_DATA(tx12, sizeof(tx12));
  TRACE("ACL TX > "); print_buffer((char*)tx12, 0, sizeof(tx12));
  if(WaitEvData()!=1) 
	 return -1;

  //1831
  char tx13[] = {0x40, 0x00, 0x0b, 0x00, 0x07, 0x00, 0x04, 0x00, 0x08, 0x0e, 0x00, 0xff, 0xff, 0x03, 0x28};
  HCI_DATA(tx13, sizeof(tx13));
  TRACE("ACL TX > "); print_buffer((char*)tx13, 0, sizeof(tx13));
  if(WaitEvData()!=1) 
	 return -1;

  //1834
  char tx14[] = {0x40, 0x00, 0x0d, 0x00, 0x09, 0x00, 0x04, 0x00, 0x06, 0x01, 0x00, 0xff, 0xff, 0x00, 0x28, 0x13, 0x18};
  HCI_DATA(tx14, sizeof(tx14));
  TRACE("ACL TX > "); print_buffer((char*)tx14, 0, sizeof(tx14));
  if(WaitEvData()!=1) 
	 return -1;

  //1836
  char tx15[] = {0x40, 0x00, 0x0d, 0x00, 0x09, 0x00, 0x04, 0x00, 0x12, 0x0e, 0x00, 0x0f, 0xac, 0x01, 0x43, 0x0c, 0xf3};
  HCI_DATA(tx15, sizeof(tx15));
  TRACE("ACL TX > "); print_buffer((char*)tx15, 0, sizeof(tx15));
  if(WaitEvData()!=1) 
	 return -1;

  //1839
  char tx16[] = {0x40, 0x00, 0x0b, 0x00, 0x07, 0x00, 0x04, 0x00, 0x08, 0x01, 0x00, 0x07, 0x00, 0x03, 0x28};
  HCI_DATA(tx16, sizeof(tx16));
  TRACE("ACL TX > "); print_buffer((char*)tx16, 0, sizeof(tx16));
  if(WaitEvData()!=1) 
	 return -1;

  //1844
  char tx17[] = {0x40, 0x00, 0x0b, 0x00, 0x07, 0x00, 0x04, 0x00, 0x08, 0x07, 0x00, 0x07, 0x00, 0x03, 0x28};
  HCI_DATA(tx17, sizeof(tx17));
  TRACE("ACL TX > "); print_buffer((char*)tx17, 0, sizeof(tx17));
  if(WaitEvData()!=1) 
	 return -1;

  //1847
  char tx18[] = {0x40, 0x00, 0x07, 0x00, 0x03, 0x00, 0x04, 0x00, 0x0a, 0x03, 0x00};
  HCI_DATA(tx18, sizeof(tx18));
  TRACE("ACL TX > "); print_buffer((char*)tx18, 0, sizeof(tx18));
  if(WaitEvData()!=1) 
	 return -1;
  
  //1849
  char tx19[] = {0x40, 0x00, 0x07, 0x00, 0x03, 0x00, 0x04, 0x00, 0x0a, 0x05, 0x00};
  HCI_DATA(tx19, sizeof(tx19));
  TRACE("ACL TX > "); print_buffer((char*)tx19, 0, sizeof(tx19));
  if(WaitEvData()!=1) 
	 return -1;

  //1852
  char tx20[] = {0x40, 0x00, 0x07, 0x00, 0x03, 0x00, 0x04, 0x00, 0x0a, 0x07, 0x00};
  HCI_DATA(tx20, sizeof(tx20));
  TRACE("ACL TX > "); print_buffer((char*)tx20, 0, sizeof(tx20));
  if(WaitEvData()!=1) 
	 return -1;

  //1855
  char tx21[] = {0x40, 0x00, 0x0f, 0x00, 0x0b, 0x00, 0x04, 0x00, 0x12, 0x0e, 0x00, 0x0f, 0xac, 0x03, 0x41, 0x1a, 0x0d, 0x00, 0xd8};
  HCI_DATA(tx21, sizeof(tx21));
  TRACE("ACL TX > "); print_buffer((char*)tx21, 0, sizeof(tx21));
  if(WaitEvData()!=1) 
	 return -1;

  //40 00 0f 00 0b 00 04 00 12 0e 00 -> 0f ac 03 41 1a fc ff ea


  //
  memcpy(DEV_BT_ADDR, &le_conn.peer_addr, 6);
  dev_connected = 1;
  
  #ifdef BLE_DEBUG
  TRACE1("%s OK\n", __FUNCTION__);
  #endif
  
	   
  //Если приложение установило свой обработчик
  //послать сообщение приложению
  if(pBTEventsHandler!=NULL)
  {
	pBTEventsHandler(BT_DEV_CONNECTED, NULL, 0);
  }  
  
  /*
  struct bt_hci_rsp_read_clock clock;
  unsigned long old_time=0;
  unsigned int cnt = 0;
  float delta;
  int idelta;

  for(;;)
  {
    //читаем внутренние часы контроллера
    //HCI_Read_Clock: Connection_Handle Which_Clock  HCI Opcode: 0x1407
    buf[0] = dev_connection_handle; 
    buf[1] = dev_connection_handle>>8;
    buf[2] = 0x01; //0x00 = Local Clock, 0x01 = Piconet Clock
	
    result = HCI_CMD(BT_HCI_CMD_READ_CLOCK, buf, 3, &clock, sizeof(clock), CMD_TIMEOUT);
  
	if(result>0 && clock.status==0)
     {
	   delta = clock.clock - old_time;
	   delta *= 0.3125;// msec;
	   idelta = (int)delta;
	   old_time = clock.clock;

       TRACE("%d %d delta: %d\n", cnt, clock.clock, idelta);
     }
  
     cnt++;
     Sleep(100);
   } */


  return 1;
}

/*
Device Name: S3D VisionMax
BT ADDR: E1:5D:48:85:C0:A5
RSSI: -64 dBm

Glasses BT On/Off: 1
CBTDONGLE::Connect
Target BT ADDR: E1:5D:48:85:C0:A5
--> LE Connection Complete:
Status 0x00
Handle 0x0040
Target BT ADDR: E1:5D:48:85:C0:A5
Role 0x00
Connection Interval 33.75 msec
Master Clock Accuracy 0x05 ppm
Dev connection handle: 0x0040
--> Read Remote Version Information Complete:
--> Read Remote Features Complete:
ACL TX > Bytes:  40 00 07 00 03 00 04 00 | 02 0F 02                

ACL TX > 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F 
--------------------------------------------------------
Bytes:  40 00 1B 00 17 00 04 00 | 06 01 00 FF FF 00 28 9E 
Bytes:  CA DC 24 0E E5 A9 E0 93 | F3 A3 B5 01 00 40 6E    

ACL TX > 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F 
--------------------------------------------------------
Bytes:  40 00 0D 00 09 00 04 00 | 06 01 00 FF FF 00 28 01 
Bytes:  18                      |                         

ACL TX > Bytes:  40 00 0B 00 07 00 04 00 | 08 09 00 FF FF 03 28    

ACL TX > Bytes:  40 00 0B 00 07 00 04 00 | 08 0B 00 FF FF 03 28    

ACL TX > Bytes:  40 00 0B 00 07 00 04 00 | 08 0E 00 FF FF 03 28    

ACL TX > 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F 
--------------------------------------------------------
Bytes:  40 00 0D 00 09 00 04 00 | 06 09 00 FF FF 00 28 01 
Bytes:  18                      |                         

ACL TX > Bytes:  40 00 09 00 05 00 04 00 | 04 0C 00 0C 00          

ACL TX > 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F 
--------------------------------------------------------
Bytes:  40 00 0D 00 09 00 04 00 | 06 01 00 FF FF 00 28 00 
Bytes:  18                      |                         

ACL TX > Bytes:  40 00 09 00 05 00 04 00 | 12 0C 00 01 00          

ACL TX > 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F 
--------------------------------------------------------
Bytes:  40 00 0D 00 09 00 04 00 | 06 08 00 FF FF 00 28 00 
Bytes:  18                      |                         

ACL TX > Bytes:  40 00 0B 00 07 00 04 00 | 08 09 00 FF FF 03 28    

ACL TX > Bytes:  40 00 0B 00 07 00 04 00 | 08 0B 00 FF FF 03 28    

ACL TX > Bytes:  40 00 0B 00 07 00 04 00 | 08 0E 00 FF FF 03 28    

ACL TX > 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F 
--------------------------------------------------------
Bytes:  40 00 0D 00 09 00 04 00 | 06 01 00 FF FF 00 28 13 
Bytes:  18                      |                         

ACL TX > 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F 
--------------------------------------------------------
Bytes:  40 00 0D 00 09 00 04 00 | 12 0E 00 0F AC 01 43 0C 
Bytes:  F3                      |                         

ACL TX > Bytes:  40 00 RX < 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0B 00 0A 0B 0C 0D 0E 0F 
----07 -00 04 --00 | -08 01 --00 07 -00 03 --28    -

------------------------------------------
Bytes:  17 00 04 00 1B 0B 00 0F | AC 0C 00 27 11 D6 BB 55 
Bytes:  40 01 63 01 63 00 00 00 | 02 05 02                

RX < 
ADDR:	ACL TX > 00 Bytes:  01 40 02 00 0B 03 00 04 05 07 06 00 04 07 00 | | 08 08 07 00 09 07 0A 00 03 0B 0C 28    0D 

0E 0F 
--------------------------------------------------------
Bytes:  17 00 04 00 1B 0B 00 07 | 03 1A 41 00 00 00 00 00 
Bytes:  3F 0A D7 23 3C 00 00 00 | 00 00 10                

RX < Bytes:  ACL TX > Bytes:  09 00 40 00 07 04 00 00 1B 03 00 0B 04 00 00 | 00 0A 03 | 39 00       4F AB       71    
70    
      

ACL TX > Bytes:  40 00 07 00 03 00 04 00 | 0A 05 00                

ACL TX > Bytes:  40 00 07 00 03 00 04 00 | 0A 07 00                

ACL TX > 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F 
--------------------------------------------------------
Bytes:  40 00 0F 00 0B 00 04 00 | 12 0E 00 0F AC 03 41 1A 
Bytes:  0D 00 D8                |                         

CBTDONGLE::Connect OK
RX < 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F 
--------------------------------------------------------
Bytes:  17 00 04 00 1B 0B 00 0F | AC 0C 00 27 11 D6 BB 55 
Bytes:  40 01 63 01 63 0D 00 01 | 02 05 02                

RX < 
ADDR:	00 01 02 03 04 05 06 07 | 08 09 0A 0B 0C 0D 0E 0F 
--------------------------------------------------------
Bytes:  17 00 04 00 1B 0B 00 07 | 03 1A 41 00 00 00 00 00 
Bytes:  3F 0A D7 23 3C 00 00 00 | 00 00 10                

RX < Bytes:  09 00 04 00 1B 0B 00 00 | 39 4F AB 71 62          
*/


//------------------------------------------------------------------------------
//Event Connection Complete 0x03
//------------------------------------------------------------------------------
void CBTDONGLE::LE_Connection_Complete(unsigned char *pBuf, int length)
{	
  struct bt_hci_evt_le_conn_complete le_conn_compl;
  int len = sizeof(le_conn_compl);
  memcpy((void*)&le_conn_compl, pBuf, len); 
  
  TRACE("--> LE Connection Complete:\n");
  TRACE1("Status 0x%02X\n", le_conn_compl.status); //Status: Success (0x00)	 	
  TRACE1("Handle 0x%04X\n", le_conn_compl.handle);
  TRACE("Target BT ADDR: ");
  printAddr6((unsigned char*)&le_conn_compl.peer_addr,':'); 
  TRACE("\n");

  TRACE1("Role 0x%02X\n", le_conn_compl.role);
  TRACE1("Connection Interval %.2f msec\n", ((float)(le_conn_compl.interval))*1.25);
  TRACE1("Master Clock Accuracy 0x%02X ppm\n", le_conn_compl.clock_accuracy);//31-50 ppm (0x05)
  
  dev_connection_handle = le_conn_compl.handle;
  
  //----
  ev_BLE.PulseEvent();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------ 
void CBTDONGLE::LE_Read_Remote_Features_Complete(unsigned char *pBuf, int length)
{
  TRACE("--> Read Remote Features Complete:\n");

  //----
  ev_BLE.PulseEvent();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CBTDONGLE::Read_Remote_Version_Information_Complete(unsigned char *pBuf, 
	                                                     int length)
{
  TRACE("--> Read Remote Version Information Complete:\n");

  //----
  ev_BLE.PulseEvent();
}

//------------------------------------------------------------------------------
//Close Bluetooth connection
//------------------------------------------------------------------------------
int CBTDONGLE::Disconnect(void)
{ 
  #ifdef BLE_DEBUG
  TRACE1("%s", __FUNCTION__);
  #endif

  //if before no any opened 
  if(dev_connected==0){return 1;}
  
  struct bt_hci_cmd_disconnect cmd;

  //HCI Disconnect	
  //struct bt_hci_cmd_disconnect end;
  cmd.handle = dev_connection_handle>>8; //0x40; 
  cmd.handle = dev_connection_handle;    //0x00;
  //CONNECTION TERMINATED BY LOCAL HOST (0x16)
  cmd.reason = 0x16;
  //----
  HCI_CMD(BT_HCI_CMD_DISCONNECT, &cmd, 3, NULL, 0, CMD_TIMEOUT);
  
  #ifdef BLE_DEBUG  
  TRACE(" Target BT ADDR: ");
  printAddr6((unsigned char*)DEV_BT_ADDR,':'); 
  TRACE("\n");
  #endif

  dev_connection_handle = 0;
  dev_connected = 0;

  return 1;
}
