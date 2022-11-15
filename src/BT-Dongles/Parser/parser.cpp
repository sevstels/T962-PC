//==============================================================================
//File name:    "parser.c"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru
//==============================================================================
#include "StdAfx.h"
#include "bt.h"
#include "hci.h"
#include "bt-dongle.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *hci_error_code_str[] = {
	"Success",
	"Unknown HCI Command",
	"Unknown Connection Identifier",
	"Hardware Failure",
	"Page Timeout",
	"Authentication Failure",
	"PIN or Key Missing",
	"Memory Capacity Exceeded",
	"Connection Timeout",
	"Connection Limit Exceeded",
	"Synchronous Connection to a Device Exceeded",
	"ACL Connection Already Exists",
	"Command Disallowed",
	"Connection Rejected due to Limited Resources",
	"Connection Rejected due to Security Reasons",
	"Connection Rejected due to Unacceptable BD_ADDR",
	"Connection Accept Timeout Exceeded",
	"Unsupported Feature or Parameter Value",
	"Invalid HCI Command Parameters",
	"Remote User Terminated Connection",
	"Remote Device Terminated Connection due to Low Resources",
	"Remote Device Terminated Connection due to Power Off",
	"Connection Terminated by Local Host",
	"Repeated Attempts",
	"Pairing Not Allowed",
	"Unknown LMP PDU",
	"Unsupported Remote Feature / Unsupported LMP Feature",
	"SCO Offset Rejected",
	"SCO Interval Rejected",
	"SCO Air Mode Rejected",
	"Invalid LMP Parameters / Invalid LL Parameters",
	"Unspecified Error",
	"Unsupported LMP Parameter Value / Unsupported LL Parameter Value",
	"Role Change Not Allowed",
	"LMP Response Timeout",
	"LMP Error Transaction Collision",
	"LMP PDU Not Allowed",
	"Encryption Mode Not Acceptable",
	"Link Key Can Not be Changed",
	"Requested QoS Not Supported",
	"Instant Passed",
	"Pairing with Unit Key Not Supported",
	"Different Transaction Collision",
	"Reserved",
	"QoS Unacceptable Parameter",
	"QoS Rejected",
	"Channel Classification Not Supported",
	"Insufficient Security",
	"Parameter out of Mandatory Range",
	"Reserved",
	"Role Switch Pending",
	"Reserved",
	"Reserved Slot Violation",
	"Role Switch Failed",
	"Extended Inquiry Response Too Large",
	"Simple Pairing Not Supported by Host",
	"Host Busy - Pairing",
	"Connection Rejected due to No Suitable Channel Found",
	"Controller Busy",
	"Unacceptable Connection Parameters",
	"Directed Advertising Timeout",
	"Connection Terminated Due to MIC Failure",
	"Connection Failed to be Established",
	"MAC Connection Failed",
};

#define BLE_DEBUG

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void print_buffer(char *pBuffer, int address, int page_size)
{
  long i, j;
  unsigned char data;
  int hsize;

  if(page_size < 1) return;

  if(page_size >=15) hsize = 16;
  else hsize = page_size;

  if(page_size>15)
  {
    //Print Address Header
    TRACE("\nADDR:\t");
    for (i=0; i<hsize; i++){if (i==8) TRACE("| "); TRACE1("%02X ",i);}
    TRACE("\n");

    //Print line
    for (i=0; i<8+(hsize<<1)+hsize; i++) TRACE("-");
    TRACE("\n");
  }

  for (i=0; i<page_size; i+=16)
   {
      //Print Address
    //printf("%06lX: ",i+address);
    TRACE("Bytes:  ");

    for (j=0; j<16; j++)
     {
       //Print div by 8 Bytes
       if (j==8 && hsize>7) TRACE("| ");

       if ((j+i)<page_size)
        {
         data = pBuffer[j+i];
         TRACE1("%02X ", data);
        }
       else
       TRACE("   ");
     }

    TRACE("\n"); //end row
  } 

   TRACE("\n"); //end row
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void wr_parser(void *pcmd_data, int cmd_size)
{
  unsigned short cmd_opcode;
  //char *pBuf = (char*) malloc(cmd_size+1);
  unsigned char pBuf[300];

  memset(pBuf, 0, cmd_size+1);

  memcpy(pBuf, pcmd_data, cmd_size);
  
  cmd_opcode = pBuf[1];
  cmd_opcode <<= 8;
  cmd_opcode |= pBuf[0];
  
  printf(" < Host send CMD: 0x%04X, data Sz=%d\n",cmd_opcode, pBuf[2]);
  fflush(stdout);
  print_buffer((char*)&pBuf[3], 0, pBuf[2]);

  //free(pBuf);
}

#define BT_H4_CMD_PKT	0x01
#define BT_H4_ACL_PKT	0x02
#define BT_H4_SCO_PKT	0x03
#define BT_H4_EVT_PKT	0x04

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int hci_cmd_read_parser(unsigned char *pData, int size)
{
  unsigned short opcode;
  unsigned char Buf[356+2];// = (char*) malloc(cmd_size+1);
  int temp = size;
  int length;

  if(size<1) return 0; 

  memset(Buf, 0, sizeof(Buf));
  memcpy(Buf, pData, size);

  opcode = Buf[4];
  opcode <<=8;
  opcode |= Buf[3];

  if(size!=temp)
  {
    //переполнение буфера
    temp++;  
  }

  //----------------------------------------------------------------------
  //HCI controller replay on CMD
  //----------------------------------------------------------------------
  //--- Check Event Code
  //Command Complete 0x0E Num_HCI_Command_Packets,
  //Command_Opcode, Return_Parameters
  if(Buf[0]==0x0E)// HCI_Command_Complete event
  {
     length = Buf[1];
     
     //-------- сохраняем полученные данные в буфер
     if(length>4) 
     {
       //сохраняем ответ контроллера (данные) в буфере
///       memset(ANSW_DATA, 0, sizeof(ANSW_DATA));
///       memcpy(ANSW_DATA, &Buf[6], length-4);
     }
     
	 TRACE1(" > Controller answ: Pack num: %d, Command 0x%04X  Status: %s\n", 
			    Buf[2], opcode, hci_error_code_str[Buf[5]]);
	 
	 print_buffer((char*)Buf, 0, size);

	 if(Buf[5]==0 || Buf[5]==1)
     {  
        //Status: Success (0x00)
        //command_ok = 1;
     }
     else
     {    
        //command_ok = -1;
     }

    return 0;
  }

  //----------------------------------------------------------------------
  //HCI controller event replay
  //----------------------------------------------------------------------
  //--- Check Event Code
  if(Buf[0]==0x0F)//HCI_command_Status event
  {
     length = Buf[1]; //parameter length
     opcode = Buf[5];
     opcode <<=8;
     opcode |= Buf[4];                      
     TRACE2(" > Controller set Status code: 0x%02X to Command 0x%04X\n", Buf[2], opcode);
     print_buffer((char*)Buf, 0, size);
     return 0; //read again
  }
  
  //----------------------------------------------------------------------
  //HCI controller replay
  //----------------------------------------------------------------------
    //--- Check Event Code
    if(Buf[0]==0xFF)//reserved for Vendor events
    {
     //length = Buf[1]; //parameter length
     //opcode = Buf[5];
     //opcode <<=8;
     //opcode |= Buf[4];                      
     TRACE1(" > Device Vendor Event: 0x%02X\n", Buf[0]);

	 print_buffer((char*)Buf, 0, size);
     return 0xFF; 
    }
  
    //----------------------------------------------------------------------
    //HCI controller replay
    //----------------------------------------------------------------------
    //--- Check Event Code
    if(Buf[0]==0x13)//Number Of Completed Packets
     {
     //length = Buf[1]; //parameter length
                     
     TRACE1(" > Number Of Completed Packets Event: 0x%02X\n", Buf[0]);
     print_buffer((char*)Buf, 0, size);
     
     TRACE1(" Number_of_Handles: %d\n",Buf[2]);
     
     opcode = Buf[4];
     opcode <<=8;
     opcode |= Buf[3];
     TRACE1(" Connection_Handle: %d\n",opcode);
     
     opcode = Buf[6];
     opcode <<=8;
     opcode |= Buf[5];
     TRACE1(" Completed_Packets: %d\n\n",opcode);	

     return 0; 
    }

  //----------------------------------------------------------------------
  //Известные HCI controller events
  //----------------------------------------------------------------------
  switch(Buf[0])
  {
     case 0x03:
     case 0x04:
     case 0x05:
     case 0x3E://BLE all events
     return Buf[0];
     
     default:
     break;
  }

  //----------------------------------------------------------------------
  //HCI other controller events
  //----------------------------------------------------------------------
  TRACE1(" > Unknown Controller Event: 0x%02X\n", Buf[0]);
  return Buf[0];
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int hci_acl_read_parser(unsigned char *pData, int size)
{
  int handler=0;
  char flip[256];
  memset (flip, 0, sizeof(flip));

  if((pData[4]==5)&&(pData[5]==0)&&(pData[6]==2))
     {
        TRACE("\n> Controller get Connection Announcement");
        TRACE1("\n> Battery level: %d persent\n\n", pData[size-1]);
        //call handler
        handler = 1;
     }
    else
     {	/*
        TRACE1("\n> Controller send ACL Data: Sz=%d\n", size);
        print_buffer((char*)&pData[0], 0, size);
        
        TRACE1("\n> Flip Data: Sz=%d\n", size);
        for(int i=0; i<size; i++)
        {
          flip[i] = pData[size-1-i];
        }
        print_buffer((char*)flip, 0, size);
		 */
        //послать ответ  RX: 00030401000000
        handler = pData[2];
     }

  return handler; 
}

//LOGICAL TRANSPORT ADDRESS (LT_ADDR)
#define LT_ADDR 0x01
#define PKT_TYPE 0x0008		/* 0x0008 = EDR + DM1, 0xff1e = BR only */
#define hci_dev NULL

static struct bt_hci_evt_conn_request conn_evt;
static struct bt_hci_evt_conn_complete conn_complete;
int ACL_ENABLE;

//BD_ADDR of the Host Device 
extern unsigned char BD_ADDR_host[6];
extern long long bt_addr_host; 
extern unsigned char BD_ADDR_device[6];
extern long long bt_addr_device;

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VendorIRQ_Connection_Request(unsigned char *pData, int size)
{
  char data[128]; long lap;
  unsigned char  uap; unsigned short nap;
  struct bt_hci_cmd_accept_conn_request cmd;
  memset(&cmd, 0, sizeof(cmd));
  memcpy(data, pData, size);
  memcpy((void*)&cmd, &data[3], 3+1+2);
  
  //LAP field: lower address part consisting of 24 bits
  //UAP field: upper address part consisting of 8 bits
  //NAP field: non-significant address part consisting of 16 bits
  //LAP - company_assigned
  //UAP NAP - company_id
  
  //LAP
  lap = data[3];
  lap <<=8;
  lap |= data[4];
  lap <<=8;
  lap |= data[5];
  
  //UAP
  uap = data[6];
  
  //NAP
  nap = data[7];
  nap <<= 8;
  nap |= data[8];
  
  //print_buffer((char*)pData, 0, size);

  TRACE("\n\tIncoming connection from 3D glasses\n\n");

  printf("\tBT Address (NAP+LAP) = Company ID\n\
          \tBT LAP addr: 0x%06x\n\
          \tBT UAP addr: 0x%02x\n\
          \tBT NAP addr: 0x%04x\n\n", lap, uap, nap);                                          
	
  //BD_ADDR of the Device to be connected
	//memcpy(cmd.bdaddr, conn_evt.bdaddr, 6); //6 bytes
	//cmd.role = 0x00; //Become the Master for this connection.
  cmd.role = 0x01; //Remain the Slave for this connection.

//  адрес не правильно вводится!
  char arr[] = {0, 0x60, 0, 0x51, 0, 0,  1};
  //HCI_Accept_Connection_Request, Return Parameters: None
///	bt_hci_send(hci_dev, BT_HCI_CMD_ACCEPT_CONN_REQUEST, 
   ///                   arr, sizeof(cmd), NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VendorIRQ_Lost3DSync(void)
{
  TRACE("\n\tBT controller lost 3D-sync signal\n\n");                                          
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//Connection Request Event Data:  BD_ADDR 6, Class_of_Device 3, Link_Type 1
/*struct bt_hci_evt_conn_request {
	uint8_t  bdaddr[6];
	uint8_t  dev_class[3];
	uint8_t  link_type;     1= ACL Connection requested */
void Connection_Request_0x04(unsigned char *pData, int size)
{
  struct bt_hci_cmd_accept_conn_request cmd;
  memcpy((void*)&conn_evt, &pData[2], 6+3+1);
  
  print_buffer((char*)pData, 0, size);

  TRACE("\n\tIncoming connection from 3D glasses\n\n");

  //BD_ADDR of the Device to be connected
  memcpy(cmd.bdaddr, conn_evt.bdaddr, 6); //6 bytes
  //cmd.role = 0x00; //Become the Master for this connection.
  cmd.role = 0x01; //Remain the Slave for this connection.

  //HCI_Accept_Connection_Request, Return Parameters: None
//  bt_hci_send(hci_dev, BT_HCI_CMD_ACCEPT_CONN_REQUEST, 
  //           &cmd, sizeof(cmd), NULL, NULL, NULL);
  
  
  //========= сохраняем полученный адрес
  //bt_addr_device = 0;
  //memcpy(&bt_addr_device, &pData[2], 6);
  //printf("\tDevice BT_ADDR: 0x%012I64x\n\n", bt_addr_device); 
  //====================================================
  
  //Send_Synchronization_Train();
}

//------------------------------------------------------------------------------
//Event Connection Complete 0x3E
//------------------------------------------------------------------------------
/*
Subevent_Code,
Status,
Connection_Handle,
Role,
Peer_Address_Type,
Peer_Address,
Conn_Interval,
Conn_Latency,
Supervision_Timeout,
Master_Clock_Accuracy
*/
	   /*
	uint8_t  num_reports;
	uint8_t  event_type;
	uint8_t  addr_type;
	uint8_t  addr[6];
	uint8_t  data_len;
	uint8_t  data[10];
		 */
void BLE_Connection_Completed_0x3E(unsigned char *pData, int size)
{
   //event_packet_handler((uint8_t*)pData, size);
  unsigned char num, length;
  unsigned char packet[256];
  char txt[128]; 
  int ret;
  unsigned char target1[6] = {0x00, 0x07, 0x80, 0x2f, 0x06, 0x8f};
  uint8_t addr[6];
  memset(packet, 0, 256);
  memcpy(packet, pData, size); 
  ///bd_addr_type_t addr_type;
  
  TRACE("\n\t-->BLE event: ");
  /*
  switch (packet[2])
  {
  case HCI_SUBEVENT_LE_LONG_TERM_KEY_REQUEST:
       printf("LONG TERM KEY REQUEST\n");	   
	   break;
  case HCI_SUBEVENT_LE_READ_REMOTE_USED_FEATURES_COMPLETE:
       printf("READ REMOTE USED FEATURES COMPLETE\n");		   
	   break;
  case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
       printf("CONN UPDATE COMPLETE\n");		   
	   break;

  case HCI_SUBEVENT_LE_ADVERTISING_REPORT:
	   printf("ADVERTISING REPORT\n");	
       //log_info("advertising report received");
       //if (hci_stack->le_scanning_state != LE_SCANNING) break;
       le_handle_advertisement_report(packet, size);
	   
	   //printf("\tNumReports: %d\n", packet[3]);
	   //struct bt_hci_evt_le_adv_report rep;
	   //num = packet[3];
	   //length = sizeof(rep);
	   //memset(&rep, 0, length);
	   //memcpy(&rep, &packet[3]+((num-1)*length), length);

	   printf("\tNumReports: %d, EvtType: %d, AddrType: %d, DLength: %d\n", 
		                         packet[3], packet[4], packet[5], packet[17]);
	   
	   bt_flip_addr(addr, &packet[6]);	   
	   printf("\tAddr: %s\n", bd_addr_to_str(addr));
	   
	   //попытаться подключиться к устройству 0x0007802f068f
	   ret = memcmp(addr, target1, 6);
	   if(ret==0)
	   {
	      printf("\tУстройство опознано: BLED112\n\n");
		  event_handler(packet, size);
	   }

	   if(packet[17]==0 || packet[17]>253) break;
	   memset(txt,0,sizeof(txt));
	   strncpy(txt, (const char*)&packet[18], packet[17]+2);
	   printf("\tData: %s\n\n", txt);	
	   printf("\tRSSI: %d dBm\n\n", (signed char)packet[0x1d]);

       break;
  
  case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
       // Connection management
       bt_flip_addr(addr, &packet[8]);
       addr_type = (bd_addr_type_t)packet[7];
       printf("\n\tBLE Connection completed\n\t(status=%u) type %u, %s\n\n", 
		           packet[3], addr_type, bd_addr_to_str(addr));
       
	   // LE connections are auto-accepted, so just create a connection if there isn't one already
       //conn = hci_connection_for_bd_addr_and_type(&addr, addr_type);
       /*
	   if (packet[3]){
       if (conn){
                            // outgoing connection failed, remove entry
                            linked_list_remove(&hci_stack->connections, (linked_item_t *) conn);
                            btstack_memory_hci_connection_free( conn );
                        }
                        // if authentication error, also delete link key
                        if (packet[3] == 0x05) {
                            hci_drop_link_key_for_bd_addr(&addr);
                        }
                        break;
                    }
                    if (!conn){
                        conn = create_connection_for_bd_addr_and_type(addr, addr_type);
                    }
                    if (!conn){
                        // no memory
                        break;
                    }
                    
                    conn->state = OPEN;
                    conn->con_handle = READ_BT_16(packet, 4);
                    
                    // TODO: store - role, peer address type, conn_interval, conn_latency, supervision timeout, master clock

                    // restart timer
                    // run_loop_set_timer(&conn->timeout, HCI_CONNECTION_TIMEOUT_MS);
                    // run_loop_add_timer(&conn->timeout);
                    
                    log_info("New connection: handle %u, %s", conn->con_handle, bd_addr_to_str(conn->address));
                    
                    hci_emit_nr_connections_changed();
                    break;




   break;


  }	     */

//  if(buf[2]==0x01) 
	//   printf("\n\tBLE Connection successfully completed\n\n");

  //memcpu(addr, buf[6], 6);

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Disconnection_Completed_0x05(void)
{
 /// ACL_ENABLE = 0;
  TRACE("\n\tDissconnection completed\n\n");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Send_Synchronization_Train(void)
{
	struct bt_hci_cmd_write_sync_train_params cmd;

	//printf("\n --------------- %s", __func__);

	//if (sync_train_active)
	//	return;
  TRACE("\n\tStarting new synchronization train\n\n");
  	   /*
  Broadcast_ini(NULL);

	cmd.min_interval = cpu_to_le16(0x0050);
	cmd.max_interval = cpu_to_le16(0x00a0);
	cmd.timeout = cpu_to_le32(0x0002ee00);		/* 120 sec */ /*
	cmd.service_data = LT_ADDR;
  
  
	bt_hci_send(hci_dev, BT_HCI_CMD_WRITE_SYNC_TRAIN_PARAMS,
					&cmd, sizeof(cmd), NULL, NULL, NULL);
  Sleep(10);
 
	bt_hci_send(hci_dev, BT_HCI_CMD_READ_SYNC_TRAIN_PARAMS, NULL, 0,
							NULL, NULL, NULL);
  Sleep(10);
  


  //HCI_Set_Connectionless_Slave_Broadcast

  //Connectionless slave broadcast mode
  //must be enabled on the BR/EDR Controller before this command may be used.

  //HCI_Start_Synchronization_Train
	bt_hci_send(hci_dev, BT_HCI_CMD_START_SYNC_TRAIN, NULL, 0,
							NULL, NULL, NULL);
  Sleep(10);
	*/
  //sync_train_active = true;
  ///BCS_ThreadLaunch();

}
			 
// DevicesName.push_back(rx_address);
/*
//------------------------------------------------------------------------------
//DataTypes
//------------------------------------------------------------------------------
int AdvertisingData(unsigned char *pBuf, int length)
{ /*
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  if(length<=2) return -1;

  unsigned short id;
  unsigned char data[4];
  int result = -1;

  for(;;)
  {
    //длина вложенного блока
	int data_length = *pBuf++;
	int data_type = *pBuf++;
	length -= data_length + 1;

    //-------------------------------------------------
    //Data Type:
    //-------------------------------------------------
    switch(data_type) 
    {	
	   //Manufacturer Specific: 
       case 0xFF:		/*
	   //Length: 30
	   //Company ID: Microsoft (0x0006)
	   memcpy(&id, pBuf, 2); pBuf +=2;
	   TRACE1("Company ID: 0x%04X\n", id);
	   data_length -= 2;

	   //----
	   TRACE("Data: ");
	   //Data: 01092002fa1637485d7312a05cc7897fcbfb9d3d8a45397c612457
	   for(int i=0; i<data_length-1; i++)
	   {
	   	 TRACE1("%02X", *pBuf++);
	   }
	   TRACE("\n");	 
       break;			  
	
	   //Type: Flags (0x01)
       case 0x01:
	   ///TRACE1("Flags: %02X\n", *pBuf++);
	   *pBuf++;
	   result = 1;
       break;	
	
	   //Type: Device Name (0x09)
       case 0x09:
	   TRACE("Device Name: ");
	   if(data_length>0)
	   {
		
		 char byte[2];
         for(int i=0; i<data_length-1; i++)
	     {
			byte[0] = *pBuf++;
			byte[1] = 0;
		    TRACE1("%s", byte);
	     }
	   }
	   TRACE("\n");
	   result = 1;
       break;	
	
	   //Event Type: Non-Connectable Undirected Advertising (0x03)
       case 0x03:

       break;	
	
	   //Event Type: Scan Response (0x04)
       case 0x04:

       break;

	   //----
       default:
	   length = 0;
	   break;
    }

   if(length <=0) 
	  break; 
  }

  return result;
}
*/

//------------------------------------------------------------------------------
//Забираем имя устройства 
//------------------------------------------------------------------------------
int CBTDONGLE::Parse_DeviceName(char *pBuf, int length, std::string *pName)
{
  std::string dev_name;
  dev_name.empty();

  if(length>0)
  {
	char byte;
    for(int i=0; i<length; i++)
	{
	  byte = *pBuf++;
	  dev_name.push_back(byte);
	}
  }
  else return -1;

  dev_name.push_back(0);
 *pName	=  dev_name;
  //TRACE1("Device Name: %s", dev_name.c_str());

  //Забираем адрес устройства 
//  unsigned char addr[6];
//  bt_flip_addr(addr, (uint8_t*)&le_report.addr);
/* 
  //Filtration
  count = Filter_DeviceName.size();
  for(int i=0; i<count; i++)
  {
    std::string name = Filter_DeviceName.at(i);
	int match = strcmp(dev_name.c_str(), name.c_str());
	if(match==0){result = 1; break;}
	else {result = 0;}
  }	
*/	
	/*
  if(result>0)
  {	
	//print_buffer((char*)pBuf, 0, length);
	
    //----
    std::string bd_addr;
	TRACE("BT ADDR: ");
    unsigned char addr[6];
    bt_flip_addr(addr, (uint8_t*)&le_report.addr);
    TRACE("%s\n", bd_addr_to_str(addr));
	
	devices_number = 1;
	
	unsigned long long rx_address = addr[5];
	rx_address <<= 8;
	rx_address |= addr[4];
 	rx_address <<= 8;
	rx_address |= addr[3];
	rx_address <<= 8;
	rx_address |= addr[2];
	rx_address <<= 8;
	rx_address |= addr[1];
	rx_address <<= 8;
	rx_address |= addr[0];


	//bd_addr.append("d");

	int length = Devices_BT_ADDR_str.size();
	if(length==0)
	{
	   DevicesName.push_back(dev_name);
///	   Devices_BT_ADDR_str.push_back(rx_address);

	   std::string temp;
	   BT_AddrToStr(temp, rx_address);
	   Devices_BT_ADDR_str.push_back(temp);

	   memcpy(DEV_BT_ADDR, addr, 6);
	   new_device = 1;
		 
	   //Если приложение установило свой обработчик
       //послать сообщение приложению
       if(pBTEventsHandler!=NULL)
       {
	     pBTEventsHandler(BT_DEV_ARRIVED, NULL, 0);
	   }
	}
	else
	{
	  int device_in_list = 0;

	  //ищем совпадение адреса в списке 
	  for(int i=0; i<length; i++)
	  {
///	    unsigned long long list_addr = Devices_BT_ADDR.at(i);
	///    if(list_addr==rx_address){ device_in_list = 1; break;}
	  }

	  //не было найдено, добавить
	  if(device_in_list==0)
	  {
		 DevicesName.push_back(dev_name);
///		 Devices_BT_ADDR.push_back(rx_address);
		 memcpy(DEV_BT_ADDR, addr, 6);
		 new_device = 1;
		  
		 //Если приложение установило свой обработчик
         //послать сообщение приложению
         if(pBTEventsHandler!=NULL)
         {
	       pBTEventsHandler(BT_DEV_ARRIVED, NULL, 0);
         }
	  }
	}
	
/*
	//----
	DEV_BT_ADDR[5] = 0xE1;
	DEV_BT_ADDR[4] = 0x5D;
	DEV_BT_ADDR[3] = 0x48;
	DEV_BT_ADDR[2] = 0x85;
	DEV_BT_ADDR[1] = 0xC0;
	DEV_BT_ADDR[0] = 0xA5;

	//---- C8:50:30:74:F9:26
	DEV_BT_ADDR[5] = 0xC8;
	DEV_BT_ADDR[4] = 0x50;
	DEV_BT_ADDR[3] = 0x30;
	DEV_BT_ADDR[2] = 0x74;
	DEV_BT_ADDR[1] = 0xF9;
	DEV_BT_ADDR[0] = 0x26;
*/
   /*
  //----
  signed char RSSI = (signed char)pBuf[length-1]; 
  Devices_RSSI.push_back(RSSI);
  TRACE("RSSI: %d dBm\n\n", RSSI);
   */
  return 1;
}

/*
//------------------------------------------------------------------------------
//DataTypes
//------------------------------------------------------------------------------
int AdvertisingDataOld(unsigned char *pBuf, int length)
{ /*
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
  */ /*
  if(length<=2) return -1;

  unsigned short id;
  unsigned char data[4];
  int result = -1;

  for(;;)
  {
    //длина вложенного блока
	int data_length = *pBuf++;
	int data_type = *pBuf++;
	length -= data_length + 1;

    //-------------------------------------------------
    //Data Type:
    //-------------------------------------------------
    switch(data_type) 
    {	
	   //Manufacturer Specific: 
       case 0xFF:		/*
	   //Length: 30
	   //Company ID: Microsoft (0x0006)
	   memcpy(&id, pBuf, 2); pBuf +=2;
	   TRACE1("Company ID: 0x%04X\n", id);
	   data_length -= 2;

	   //----
	   TRACE("Data: ");
	   //Data: 01092002fa1637485d7312a05cc7897fcbfb9d3d8a45397c612457
	   for(int i=0; i<data_length-1; i++)
	   {
	   	 TRACE1("%02X", *pBuf++);
	   }
	   TRACE("\n");	  */  /*
       break;			  
	
	   //Type: Flags (0x01)
       case 0x01:
	   ///TRACE1("Flags: %02X\n", *pBuf++);
	   *pBuf++;
	   result = 1;
       break;	
	
	   //Type: Device Name (0x09)
       case 0x09:
	   TRACE("Device Name: ");
	   if(data_length>0)
	   {
		 char byte[2];
         for(int i=0; i<data_length-1; i++)
	     {
			byte[0] = *pBuf++;
			byte[1] = 0;
		    TRACE1("%s", byte);
	     }
	   }
	   TRACE("\n");
	   result = 1;
       break;	
	
	   //Event Type: Non-Connectable Undirected Advertising (0x03)
       case 0x03:

       break;	
	
	   //Event Type: Scan Response (0x04)
       case 0x04:

       break;

	   //----
       default:
	   length = 0;
	   break;
    }

   if(length <=0) 
	  break; 
  }

  return result;
} */

//------------------------------------------------------------------------------
//DataTypes
//result = AdvertisingData(&pBuf[13], le_report.data_len);
//------------------------------------------------------------------------------
int CBTDONGLE::AdvertisingData(unsigned char *pBuf, int length)
{ /*
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
  */
  if(length<=2) return -1;

  unsigned short id;
  unsigned char data[4];
  int result = -1;
  int count;
  std::string dev_name;

  for(;;)
  {
    //длина вложенного блока
	int data_length = *pBuf++;
	int data_type = *pBuf++;
	length -= data_length + 1;
	

    //-------------------------------------------------
    //Data Type:
    //-------------------------------------------------
    switch(data_type) 
    {	
	   //Manufacturer Specific: 
       case 0xFF:		/*
	   //Length: 30
	   //Company ID: Microsoft (0x0006)
	   memcpy(&id, pBuf, 2); pBuf +=2;
	   TRACE1("Company ID: 0x%04X\n", id);
	   data_length -= 2;

	   //----
	   TRACE("Data: ");
	   //Data: 01092002fa1637485d7312a05cc7897fcbfb9d3d8a45397c612457
	   for(int i=0; i<data_length-1; i++)
	   {
	   	 TRACE1("%02X", *pBuf++);
	   }
	   TRACE("\n");	  */
       break;			  
	
	   //Type: Flags (0x01)
       case 0x01:
	   ///TRACE1("Flags: %02X\n", *pBuf++);
	   *pBuf++;
	   result = 1;
       break;	
	
	   //--------------------------------------------------
	   //Type: Device Name (0x09)
	   //--------------------------------------------------
       case 0x09:
	   /*
	   TRACE("Device Name: ");
	   if(data_length>0)
	   {
		
		 char byte[2];
         for(int i=0; i<data_length-1; i++)
	     {
			byte[0] = *pBuf++;
			byte[1] = 0;
			dev_name.push_back(byte[0]);
		    TRACE1("%s", byte);
	     }
	   }

	   dev_name.push_back(0);
	   TRACE("\n");

	   //Filtration
	   count = Filter_DeviceName.size();
	   for(int i=0; i<count; i++)
	   {
		  std::string name = Filter_DeviceName.at(i);
		  int match = strcmp(dev_name.c_str(), name.c_str());
		  if(match==0){result = 1; break;}
		  else {result = 0;}
	   } */

	   result = Parse_DeviceName((char*)pBuf, data_length, &dev_name);
//	   result = Parse_DeviceAddr((char*)pBuf, data_length, &dev_name);
	   TRACE1("Device Name: %s\n", dev_name.c_str());
       break;	
	
	   //Event Type: Non-Connectable Undirected Advertising (0x03)
       case 0x03:
       break;	
	
	   //Event Type: Scan Response (0x04)
       case 0x04:
       break;

	   //----
       default:
	   length = 0;
	   break;
    }

   if(length <=0) 
	  break; 
  }

  return result;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CBTDONGLE::BT_AddrToStr(std::string &adr_txt, char *pAddr) 
{
  unsigned char data;
  CString txt, addr;

  for(int i = 0; i<6; i++) 
  {
	data = *pAddr++;

	txt.Format("%0.2X", data);
	addr += txt;
  }

  adr_txt =	addr.GetBuffer();
}

//------------------------------------------------------------------------------
//Event Code 0x3E
//------------------------------------------------------------------------------
int CBTDONGLE::LE_AdvertisingReport(unsigned char *pBuf, int length)
{
  char data[256];
  memset(data, 0, 256);
  if(length>256) length = 256;
  memcpy(data, pBuf, length);

  int result, new_device = 0;
  struct bt_hci_evt_le_adv_report le_report;
  ///memcpy(&le_report, pBuf, sizeof(le_report));
  memcpy(&le_report, &pBuf[3], sizeof(le_report));

  //--------------------------------------------------
  //Type: Device Name (0x09)
  //--------------------------------------------------
  //тип данных 
  int data_type = le_report.Data[4];
  if(data_type != 0x09) return -1;

  //длина вложенного блока
  int name = -1;
  int name_length = le_report.Data[3]-1;
  std::string dev_name;
  name = Parse_DeviceName((char*)&data[0x12], name_length, &dev_name);
  TRACE1("Device Name: %s\n", dev_name.c_str());
  
  //Filtration
  int match = -1;
  int count = Filter_DeviceName.size();
  for(int i=0; i<count; i++)
  {
	std::string name = Filter_DeviceName.at(i);
    match = strcmp(dev_name.c_str(), name.c_str());
	if(match==0){break;}
  }
  
  //выход если имя не совпадает
  if(match!=0) return -1;

  //--------------------------------------------------
  //Device BD Address
  //--------------------------------------------------
  unsigned char bd_addr[6];
  bt_flip_addr(bd_addr, (uint8_t*)&le_report.Address);
  TRACE("Addr: %s\n", bd_addr_to_str(bd_addr));

  //--------------------------------------------------
  //Device RSSI
  //--------------------------------------------------
  //int data_length = le_report.Length_Data;
  signed char RSSI = data[length-1]; 
  //Devices_RSSI.push_back(RSSI);
  TRACE("RSSI: %d dBm\n\n", RSSI);  

  //--------------------------------------------------
  //Поиск устройства в списке
  //--------------------------------------------------
  match = -1;
  int devices_in_list = Devices_BT_ADDR.size();

  //ищем совпадение адреса в списке 
  for(int i=0; i<devices_in_list; i++)
  {
	 std::vector <char> dev_addr;
	 dev_addr = Devices_BT_ADDR.at(i);
	 
	 unsigned char dev_a[6];
	 dev_a[0] = dev_addr.at(0);
	 dev_a[1] = dev_addr.at(1);
	 dev_a[2] = dev_addr.at(2);
	 dev_a[3] = dev_addr.at(3);
	 dev_a[4] = dev_addr.at(4);
	 dev_a[5] = dev_addr.at(5);

	 match = memcmp(dev_a, bd_addr, 6);
	 //Совпадение найдено - не добавлять
	 if(match==0){return -1;}
  }

  //--------------------------------------------------
  //не было найдено, добавить
  //--------------------------------------------------
  //сохранить имя устройства
  Devices_Name.push_back(dev_name);
  
  //сохранить адрес устройства как вектор 
  std::vector <char> bd_a;
  bd_a.push_back(bd_addr[0]);
  bd_a.push_back(bd_addr[1]);
  bd_a.push_back(bd_addr[2]);
  bd_a.push_back(bd_addr[3]);
  bd_a.push_back(bd_addr[4]);
  bd_a.push_back(bd_addr[5]);

  Devices_BT_ADDR.push_back(bd_a);

  //сохранить уровень сигнала
  Devices_RSSI.push_back(RSSI);

  //сохранить адрес устройства как текст  
  std::string temp;
  BT_AddrToStr(temp, (char*)bd_addr);
  Devices_BT_ADDR_str.push_back(temp);
  
  //устройство добавлено
  new_device = 1;
		  
  //Если приложение установило свой обработчик
  //послать сообщение приложению
  if(pBTEventsHandler!=NULL)
  {
	 pBTEventsHandler(BT_DEV_ARRIVED, NULL, 0);
  }

  return new_device;
 
  /*
  result = -2;
  if(result>0)
  {	
	//print_buffer((char*)pBuf, 0, length);
	
    //----
    std::string bd_addr;
	TRACE("BT ADDR: ");
    unsigned char addr[6];
    bt_flip_addr(addr, (uint8_t*)&le_report.addr);
    TRACE("%s\n", bd_addr_to_str(addr));
	
	devices_number = 1;
	
	unsigned long long rx_address = addr[5];
	rx_address <<= 8;
	rx_address |= addr[4];
 	rx_address <<= 8;
	rx_address |= addr[3];
	rx_address <<= 8;
	rx_address |= addr[2];
	rx_address <<= 8;
	rx_address |= addr[1];
	rx_address <<= 8;
	rx_address |= addr[0];


	//bd_addr.append("d");

	int length = Devices_BT_ADDR_str.size();
	if(length==0)
	{
	   DevicesName.push_back(dev_name);
///	   Devices_BT_ADDR_str.push_back(rx_address);

	   std::string temp;
	   BT_AddrToStr(temp, rx_address);
	   Devices_BT_ADDR_str.push_back(temp);

	   memcpy(DEV_BT_ADDR, addr, 6);
	   new_device = 1;
		 
	   //Если приложение установило свой обработчик
       //послать сообщение приложению
       if(pBTEventsHandler!=NULL)
       {
	     pBTEventsHandler(BT_DEV_ARRIVED, NULL, 0);
	   }
	}
	else
	{
	  int device_in_list = 0;

	  //ищем совпадение адреса в списке 
	  for(int i=0; i<length; i++)
	  {
///	    unsigned long long list_addr = Devices_BT_ADDR.at(i);
	///    if(list_addr==rx_address){ device_in_list = 1; break;}
	  }

	  //не было найдено, добавить
	  if(device_in_list==0)
	  {
		 DevicesName.push_back(dev_name);
///		 Devices_BT_ADDR.push_back(rx_address);
		 memcpy(DEV_BT_ADDR, addr, 6);
		 new_device = 1;
		  
		 //Если приложение установило свой обработчик
         //послать сообщение приложению
         if(pBTEventsHandler!=NULL)
         {
	       pBTEventsHandler(BT_DEV_ARRIVED, NULL, 0);
         }
	  }
	}
	*/

	/*
	//----
    signed int RSSI = (signed char)pBuf[length-1];
    TRACE("RSSI: %d dBm\n\n", RSSI);
  }

/*
  //-------------------------------------------------
  //Events processing
  //-------------------------------------------------
  switch(event_type) 
  {	
	//Event Type: Connectable Undirected Advertising (0x00)
    case 0x00:

	//Data Type:
		//Manufacturer Specific

	//for(int i=0; i<){DataTypes(unsigned char *pBuf, int length)}
	//AdvertisingData(pBuf[8], le_report.data_len);

	//Advertising Data
	TRACE("RSSI: %d dBm\n\n", (signed char)pBuf[le_report.data_len]);
    break;			  
	
	//
    case 0x01:

    break;	
	
	//
    case 0x02:

    break;	
	
	//Event Type: Non-Connectable Undirected Advertising (0x03)
    case 0x03:

    break;	
	
	//Event Type: Scan Response (0x04)
    case 0x04:

    break;	
  }
*/

 //return new_device;
}

