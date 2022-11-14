//==============================================================================
//File name:   "le_functions.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "./Functions/bt.h"
#include "./Functions/utils.h"
#include "trace.h"
#include "bt-dongle.h"
#include "LE_test_protocol.h"

//typedef uint8_t addr[6];}bd_addr;

//------------------------------------------------------------------------------
//Function: LE Create Connection 
//------------------------------------------------------------------------------
int CBTDONGLE::LE_Connect(char freq, char cmd, unsigned char param)
{  
  if(dongle_ready != 1) return -1;
  
  int result;  
  unsigned char buf[256];
  bt_hci_cmd_le_create_conn le_conn;

  //Target MAC address d0:fd:00:a3:dd:5c 
  uint8_t target[6] = {0xd0, 0xfd, 0x00, 0xa3, 0xdd, 0x5c};

  //API needs address reversed
  reverseArray(target, sizeof(target));

  //command settings
  memset(&le_conn, 0, sizeof(le_conn));
  le_conn.peer_addr_type = 0x00;// Public Device Address gap_address_type_random;
  memcpy(&le_conn.peer_addr, target, sizeof(target));
  le_conn.scan_interval = 0x20;//Range: 0x0004 to 0x4000
  le_conn.scan_window = 0x4;//Range: 0x0004 to 0x4000
  le_conn.filter_policy = 0; //White list is not used to determine which advertiser to connect
  
  //Information parameter about the maximum length of connection event
  //needed for this LE connection.
  le_conn.max_ce_length = 0x100; 

  //Information parameter about the minimum length of connection
  //event needed for this LE connection.
  le_conn.min_ce_length = 0x04;

  le_conn.own_addr_type = 0x00; //Public Device Address
  le_conn.min_interval = 80;    //100ms
  le_conn.max_interval = 3200;  //1s
  le_conn.supv_timeout = 1000;  //10s
  le_conn.latency = 0;//0ms
  TRACE("Target BT_ADDR: 0x%012I64x\n\n", target);

  //HCI Opcode: 0x200D  "HCI_LE_Create_Connection"
  result = HCI_CMD(BT_HCI_CMD_LE_CREATE_CONN, &le_conn, sizeof(le_conn), buf, -1, 40);
  if(result<0) return -1;
  Sleep(20);
   



   /*
  //HCI_LE_Read_Local_Supported_Features, HCI Opcode: 0x2003
  result = HCI_CMD(BT_HCI_CMD_LE_READ_LOCAL_FEATURES, NULL, 0, buf, -1, 40);
  if(result<0) return -1;
  Sleep(20);
    
  //HCI_Read_BD_ADDR HCI Opcode: 0x1009
  memset(buf,0,sizeof(buf));
  result = HCI_CMD(BT_HCI_CMD_READ_BD_ADDR, NULL, 0, buf, -1, 40);
  if(result<0) return -1;
  //========= сохран€ем полученный адрес хоста
  //0x 3f f5 7c 00 23 70 e4
  memcpy(BD_ADDR, &buf[6], 6); 
  TRACE("Host BT_ADDR: 0x%012I64x\n\n", BD_ADDR); 
  //=============================================  
  Sleep(20);

  //HCI_Host_Buffer_Size  HCI Opcode: 0x0C33
  buf[0] = 0x9B; buf[1] = 0x06; buf[2] = 0xFF; buf[3] = 0x14; 
  buf[4] = 0x00; buf[5] = 0x0A; buf[6] = 0x00;
  result = HCI_CMD(BT_HCI_CMD_HOST_BUFFER_SIZE, buf, 7, buf, -1, 40);
  if(result<0) return -1;
  Sleep(20);
  */
  	/*
  //HCI_LE_Set_Advertising_Data	 HCI Opcode: 0x2008
  //buffer 32 bytes
  char dat[] = {0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	            0x08, 0x09, 0x0a, 0xc5, 0x6d, 0xb5, 0xdf, 0xfb, 
				0x48, 0xd2, 0xb0, 0x60, 0xd0, 0xf5, 0xa7, 0x10, 
				0x96, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xc5, 0x00};
  	*/	 /*
  //переложить данные в буфер вывода
  memset(&beacon_tx, 0, sizeof(beacon_tx));
  beacon_tx.cmd_length = 14; //not transmitted to device
  beacon_tx.cmd_header = CMD_BEACON;
  
  //ќ“ѕ–ј¬Ћя“№ ¬≈—№ “–≈Ѕ”≈ћџ… ѕј ≈“ = 32 октет!
  //int data_length;
  //if(usb_dev_vid==0x0A12) data_length = 32;
  //else data_length = sizeof(dat);
  
  //ѕерсонально дл€ CSR - ќ“ѕ–ј¬Ћя“№ ¬≈—№ “–≈Ѕ”≈ћџ… ѕј ≈“ = 32 октет!
  result = HCI_CMD(BT_HCI_CMD_LE_SET_ADV_DATA, &beacon_tx, 32, buf, -1, 40); 
  if(result<0) return -1;
  Sleep(20);   */
 /* 
  //HCI_LE_Set_Event_Mask
  SetEventMaskLE(mask);
  result = HCI_CMD(BT_HCI_CMD_LE_SET_EVENT_MASK, &mask, 8, buf, -1, 40);
  if(result<0) return -1;
  Sleep(20);
*/
/*
  /////////////////////////////////////////////////////////////////////////
  if(beacon_mode==1)
  {
   //Beacon Start
   //LE_Set_Advertise_Enable  HCI Opcode: 0x200A
   buf[0] = 0x01;
   result = HCI_CMD(BT_HCI_CMD_LE_SET_ADV_ENABLE, buf, 1, buf, -1, 40);
   if(result<0) return -1;
   //---- run запустилось
   //Sleep(20);  
   
   TRACE("BLE-advertising mode starting up\n");
  }
*/
  /*
  unsigned long time, old_time=0;
  unsigned int cnt = 0;
  float delta;
  int idelta;
  
  for(;;)
  {
    //читаем внутренние часы контроллера
    //HCI_Read_Clock: Connection_Handle Which_Clock  HCI Opcode: 0x1407
    buf[0] = 0x00; 
    buf[1] = 0x00; //handle 0 
    buf[2] = 0x00; //0x00 = Local Clock
    result = HCI_CMD(BT_HCI_CMD_READ_CLOCK, buf, 3, buf, 0x0E, 2);
  
    if(result>0 && buf[0]==0x0e && buf[1]==0x0c)
     {
       memcpy(&time, &buf[8], 4);
	   delta = time - old_time;
	   delta *= 0.3125;// msec;
	   idelta = (int)delta;
	   old_time = time;
       TRACE("%d %d delta: %d\n",cnt, time, idelta);
     }
  
     cnt++;
     Sleep(100);
   } 
  */
  /*
  //выгрести из контроллера все сообщени€ из канала 
  //Read answer packet from dongle
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										   buf, 250, &rx_accepted, 1000); 
 */
  TRACE("==== LE connection ready\n");
  printf("LE connection ready\n");

  return 1;  
}
