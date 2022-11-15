//==============================================================================
//File name:   "bt-dongles.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "./Functions/bt.h"
#include "trace.h"
#include "bt-dongles.h"
#include "cmd.h"

#ifdef _WIN64
//--- win64
#ifdef NDEBUG
#pragma comment(lib,"./LibUSB/x64/libusb-1.0.lib") //64bit Win
#else
#pragma comment(lib,"./LibUSB/x64dbg/libusb-1.0.lib")//64bit Win
#endif
#else
//--- win32
#ifdef NDEBUG
#pragma comment(lib,"./LibUSB/x86/libusb-1.0.lib") //32bit Win
#else
#pragma comment(lib,"./LibUSB/x86dbg/libusb-1.0.lib")//32bit Win
#endif 
#endif //end _WIN64

CBTDG *pBTDG;

//------------------------------------------------------------------------------ 
//Constructor 
//------------------------------------------------------------------------------
CBTDG::CBTDG()
{ 
  hUSB = NULL;
  ctx = NULL;
  rx_xfer = NULL;
  usb_dev_vid = 0;
  usb_dev_pid = 0;
  usb_dev_ready = 0;
  usb_open = 0;
  usb_interface = 0;
  usb_configuration = 1;
  beacon_mode = 0;
  memset(&descr_dev, 0, sizeof(descr_dev));
  memset(&descr_conf, 0, sizeof(descr_conf));
}

//------------------------------------------------------------------------------
//Destructor 
//------------------------------------------------------------------------------
CBTDG::~CBTDG()
{
  Close();
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDG::Find_Dongles(void)
{
  struct libusb_device **usb_list = NULL;
  struct libusb_device_descriptor desc;
  int usb_dev_num, i, r, ret, bt_adapter = 0;
  int bt_devs[] = {0,0,0,0,0,0,0,0};

  usb_dev_num = libusb_get_device_list(ctx, &usb_list);
  for(i=0; i<usb_dev_num; ++i)
  {
	r = libusb_get_device_descriptor(usb_list[i], &desc);
	if(r<0)
	{
	  fprintf(stderr, "couldn't get usb descriptor for dev #%d!\n", i);
	}
	  /*
	if((desc.idVendor == TC13_VENDORID && desc.idProduct == TC13_PRODUCTID)
		|| (desc.idVendor == U0_VENDORID && desc.idProduct == U0_PRODUCTID)
		|| (desc.idVendor == U1_VENDORID && desc.idProduct == U1_PRODUCTID))
		{
			ubertooth_devs[ubertooths] = i;
			ubertooths++;
		}
	} */

	if(/*desc.bDeviceClass == LIBUSB_CLASS_WIRELESS &&*/ 
	  (desc.idVendor==0x0A12 || desc.idVendor==0x0A5C))
	  {
		bt_devs[bt_adapter] = i;
		bt_adapter++;
	  }
	} 
	
    if(bt_adapter==1)
	{
		ret = libusb_open(usb_list[bt_devs[0]], &hUSB);
		if(ret<0)
		 {
			libusb_free_device_list(usb_list, usb_dev_num);
			Show_Errors(ret); 
			hUSB = NULL; 
			return -1;
		 }
	}
	else 
	if(bt_adapter == 0) 
	{
		TRACE("Error: USB BT-adapter not found!\n");
		libusb_free_device_list(usb_list, usb_dev_num);
		return 0;
	}
	else 
	{ 
	  if(bt_adapter < 0) 
	  {
		
		fprintf(stderr, "multiple devices found! Use '-U' to specify device number\n");
		unsigned char serial[17], r;
	   /*
		for(i=0 ; i<bt_adapter; ++i)
		{
			libusb_get_device_descriptor(usb_list[bt_devs[i]], &desc);
			ret = libusb_open(usb_list[bt_devs[i]], &devh);
			if(ret)
			{
				fprintf(stderr, "  Device %d: ", i);
				Show_Errors(ret);
			}
				
			else 
			{
				r = cmd_get_serial(devh, serial);
				if(r==0) 
				{
					fprintf(stderr, "  Device %d: ", i);
					print_serial(serial, stderr);
				}
				libusb_close(devh);
			}
		  }
		  hUSB = NULL;
		} 
	  else 
	  {
		ret = libusb_open(usb_list[ubertooth_devs[ubertooth_device]], &devh);
		if(ret)
		{
			Show_Errors(ret);
			hUSB = NULL;
		}	 */
	   }  
	}

  libusb_free_device_list(usb_list, usb_dev_num);
  return 1;    
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDG::Open(void)
{  
  TRACE("==== Open USB dongle BT-adapter ====\n");
  
  std::string msg;

  libusb_setlocale("en");
  //setlocale(LC_ALL, "Russian");

  int result = libusb_init(&ctx);
  if(result<0) 
  {
     msg = "LibUSB init failed! (do you use ver: 1.0?)\n";
     
     #ifdef _CONSOLE 
	 fprintf(stderr, msg);
     #else
     ::MessageBox(NULL, msg.c_str(),"Error:", MB_ICONERROR|MB_TASKMODAL);    
     #endif

	 return -1;
  }
  
  #ifdef _DEBUG 
  libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);
  #endif

  result = Find_Dongles();
  if (result<=0) 
  {
	 msg = "Can`t open Bluetooh dongle\n";
	 #ifdef _CONSOLE 
	 fprintf(stderr, msg);
     #else
     ::MessageBox(NULL, msg.c_str(),"Error:", MB_ICONERROR|MB_TASKMODAL);    
     #endif

	 Close();
	 return -1;
  }	

  /* //driver is free? 
  result = libusb_kernel_driver_active(hUSB, usb_interface); 
  if(result < 0)
  {
	Show_Errors(result);

	//detach if busy
	result = libusb_detach_kernel_driver(hUSB, usb_interface); 
	if(result < 0)
	 {
	   Show_Errors(result);
	   fprintf(stderr, "usb_kernel_driver busy code: %d\n", result);
	   fprintf(stderr, libusb_strerror((libusb_error)result));
	   Close();
	   TRACE("Error open USB dongle\n");
	   return -1;
	 }
  }	*/
  	
  //Reset dongle to initial state
  result = libusb_reset_device(hUSB);
  if(result < 0)
  {
	fprintf(stderr, "Device Reset error, code: %d\n", result);
	fprintf(stderr, libusb_strerror((libusb_error)result));
	Close();
	return -1;
  }

  //wait while device hardware rebooted
  Sleep(400);

  //===================================================================  
  //GET DESCRIPTOR`s
  //===================================================================
  result = libusb_get_descriptor(hUSB, LIBUSB_DT_DEVICE, 0, 
	                            (unsigned char*)&descr_dev, 
								 LIBUSB_DT_DEVICE_SIZE);

  if(result < 0)
   {
	 //fprintf(stderr, "usb_set_configuration error code: %d\n", result);
	 //fprintf(stderr, libusb_strerror((libusb_error)result));
	 Close();
	 return -1;
   }

  result = libusb_get_descriptor(hUSB, LIBUSB_DT_CONFIG, 0, 
	                            (unsigned char*)&descr_conf,
								 LIBUSB_DT_CONFIG_SIZE);
  if(result < 0)
   {
	 //fprintf(stderr, "usb_set_configuration error code: %d\n", result);
	 //fprintf(stderr, libusb_strerror((libusb_error)result));
	 Close();
	 return -1;
   }
  
  //===================================================================
  //SET CONFIGURATION
  //===================================================================
  //const int configuration = 1;
  result = libusb_set_configuration(hUSB, usb_configuration); 
  if(result < 0)
  {
	fprintf(stderr, "usb_set_configuration error code: %d\n", result);
	fprintf(stderr, libusb_strerror((libusb_error)result));
	Close();
	return -1;
  }

  //====================================================================
  //SET INTERFACE  reserve access to device
  //====================================================================
  result = libusb_claim_interface(hUSB, usb_interface); //0
  if(result < 0)
  {
	fprintf(stderr, "usb_claim_interface error code: %d\n", result);
	fprintf(stderr, libusb_strerror((libusb_error)result));
	Close();
	return -1;
  }
  
  TRACE("USB dongle opened\n");
  usb_open = 1;

  return 1;  
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------   
void CBTDG::Close(void)
{
  //if Dongle connected - disconnect RF link
  Disconnect();

  //make sure xfers are not active
  if(rx_xfer != NULL) libusb_cancel_transfer(rx_xfer);
  
  usb_open = 0;

  if(hUSB != NULL)
  {
	libusb_release_interface(hUSB, usb_interface);
	//Reset dongle to initial state
    libusb_reset_device(hUSB);
  }
	
  libusb_close(hUSB);
  libusb_exit(NULL);
  hUSB = NULL;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDG::Ini(void)
{  
  TRACE("==== Ini USB dongle ====\n");
  
  struct bt_hci_cmd_le_create_conn le_conn;
  struct bt_hci_cmd_le_set_adv_parameters le_param;
  struct bt_hci_cmd_set_slave_broadcast cmd;

  int result, length, rx_accepted;
  unsigned long addr; 
  unsigned char sspmode = 0x01;
  long long mask;
  unsigned char buf[256];
  unsigned char byte;
  char *pString;

  /////////////////////////////////////////////////////////////////////

  //HCI_Reset cmd:0x0C03 ret:0e 04 01 03 0c 00
  result = HCI_TxCMD(BT_HCI_CMD_RESET, NULL, 0, buf, -1, 500);
  if(result<0) return -1;
  Sleep(20);
  
  //
  //
  //

  //HCI_Read_Buffer_Size,   HCI Opcode: 0x1005
  result = HCI_TxCMD(BT_HCI_CMD_READ_BUFFER_SIZE, NULL, 0, buf, -1, 100);
  if(result<0) return -1;
  Sleep(20);

  //HCI_LE_Read_Buffer_Size  HCI Opcode: 0x2002
  result = HCI_TxCMD(BT_HCI_CMD_LE_READ_BUFFER_SIZE, NULL, 0, buf, -1, 100);
  if(result<0) return -1;
  Sleep(20);
 
  //HCI Opcode: 0x1003  "Read_Local_Supported_Features"
  result = HCI_TxCMD(BT_HCI_CMD_READ_LOCAL_FEATURES, NULL, 0, buf, -1, 100);
  if(result<0) return -1;
  Sleep(20);

  //HCI_LE_Read_Local_Supported_Features, HCI Opcode: 0x2003
  result = HCI_TxCMD(BT_HCI_CMD_LE_READ_LOCAL_FEATURES, NULL, 0, buf, -1, 40);
  if(result<0) return -1;
  Sleep(20);
    
  //HCI_Read_BD_ADDR HCI Opcode: 0x1009
  memset(buf,0,sizeof(buf));
  result = HCI_TxCMD(BT_HCI_CMD_READ_BD_ADDR, NULL, 0, buf, -1, 40);
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
  result = HCI_TxCMD(BT_HCI_CMD_HOST_BUFFER_SIZE, buf, 7, buf, -1, 40);
  if(result<0) return -1;
  Sleep(20);
  
  //Advertising Parameters
  memset(&le_param, 0, sizeof(le_param));
  le_param.min_interval	= (unsigned short)((100)/0.625);  //160=100ms
  le_param.max_interval	= (unsigned short)((100)/0.625);  //
  //0x03=ADV_NONCONN_IND
  le_param.type = 0x00;        
  //0=Public Device Address
  le_param.own_addr_type = 0x0;	
  //Public Device Address
  le_param.direct_addr_type = 0;
  //le_param.direct_addr = 0x8e89bed6;  
  //7=37-39, all channels enabled, 1=37 use
  le_param.channel_map = 0x01;   
  //Allow Scan Request from Any, Allow Connect Request from Any (default)
  le_param.filter_policy = 0x0;	 

  //HCI_LE_Set_Advertising_Parameters  HCI Opcode: 0x2006
  result = HCI_TxCMD(BT_HCI_CMD_LE_SET_ADV_PARAMETERS, &le_param, sizeof(le_param), buf, -1, 40);
  if(result<0) return -1;
  Sleep(20);
  	/*
  //HCI_LE_Set_Advertising_Data	 HCI Opcode: 0x2008
  //buffer 32 bytes
  char dat[] = {0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	            0x08, 0x09, 0x0a, 0xc5, 0x6d, 0xb5, 0xdf, 0xfb, 
				0x48, 0xd2, 0xb0, 0x60, 0xd0, 0xf5, 0xa7, 0x10, 
				0x96, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xc5, 0x00};
  	*/
  //переложить данные в буфер вывода
  memset(&beacon_tx, 0, sizeof(beacon_tx));
  beacon_tx.cmd_length = 14; //not transmitted to device
  beacon_tx.cmd_header = CMD_BEACON;
  
  //ќ“ѕ–ј¬Ћя“№ ¬≈—№ “–≈Ѕ”≈ћџ… ѕј ≈“ = 32 октет!
  //int data_length;
  //if(usb_dev_vid==0x0A12) data_length = 32;
  //else data_length = sizeof(dat);
  
  //ѕерсонально дл€ CSR - ќ“ѕ–ј¬Ћя“№ ¬≈—№ “–≈Ѕ”≈ћџ… ѕј ≈“ = 32 октет!
  result = HCI_TxCMD(BT_HCI_CMD_LE_SET_ADV_DATA, &beacon_tx, 32, buf, -1, 40); 
  if(result<0) return -1;
  Sleep(20);
 /* 
  //HCI_LE_Set_Event_Mask
  SetEventMaskLE(mask);
  result = HCI_TxCMD(BT_HCI_CMD_LE_SET_EVENT_MASK, &mask, 8, buf, -1, 40);
  if(result<0) return -1;
  Sleep(20);
*/

  /////////////////////////////////////////////////////////////////////////
  if(beacon_mode==1)
  {
   //Beacon Start
   //LE_Set_Advertise_Enable  HCI Opcode: 0x200A
   buf[0] = 0x01;
   result = HCI_TxCMD(BT_HCI_CMD_LE_SET_ADV_ENABLE, buf, 1, buf, -1, 40);
   if(result<0) return -1;
   //---- run запустилось
   //Sleep(20);  
   
   TRACE("BLE-advertising mode starting up\n");
  }

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
    result = HCI_TxCMD(BT_HCI_CMD_READ_CLOCK, buf, 3, buf, 0x0E, 2);
  
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

  //выгрести из контроллера все сообщени€ из канала 
  //Read answer packet from dongle
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										   buf, 250, &rx_accepted, 1000); 

  TRACE("==== USB dongle launched\n");
  usb_dev_ready = 1;
  
  return 1;  
}

//------------------------------------------------------------------------------
//Time delay calculation in uS
//------------------------------------------------------------------------------
unsigned long CBTDG::Calc_Delay(LARGE_INTEGER *pTimeStart, LARGE_INTEGER *pTimeEnd)
{  
  double freq = (double)Frequency.QuadPart;
  double time  = (double)(pTimeEnd->QuadPart - pTimeStart->QuadPart);
  time /= (double) Frequency.QuadPart;
  time *= 1000000;

  return (unsigned long)time;
}

//------------------------------------------------------------------------------
//Current Time calculation in uS
//------------------------------------------------------------------------------
unsigned long CBTDG::Calc_Time(LARGE_INTEGER *pTime)
{  
  double freq = (double)Frequency.QuadPart;
  double time  = (double)pTime->QuadPart;
  time /= freq;
  time *= 1000;

  return (unsigned long)time;
}
//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDG::Read_Message(void) 
{
  return 0;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDG::Wait_Response(void) 
{ 
  return APP_OK;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDG::Wait_Event(void)
{  
  return APP_OK;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------ 
inline void CBTDG::Error(void) 
{
  std::string msg, err;
  msg  = "Critical unexpected error occurred: ";
  msg += "Err23\n";
  msg += "Compare result code with error codes list\n";
  
  ::MessageBox(NULL, msg.c_str(), "Error:", 
               MB_ICONERROR|MB_TASKMODAL);
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
void CBTDG::Show_Errors(int error_code)
{
	const char *error_name = "";

	switch (error_code) {
		case LIBUSB_ERROR_TIMEOUT:
			error_name="Exit by Timeout";
			break;
		case LIBUSB_ERROR_NO_DEVICE:
			error_name="No Device";
			break;
		case LIBUSB_ERROR_ACCESS:
			error_name="Insufficient Permissions";
			break;
		case LIBUSB_ERROR_OVERFLOW:
			break;
		case LIBUSB_ERROR_PIPE:
			error_name="Pipe error";
			break;
		default:
			error_name="Command Error";
			break;
	}
 
  TRACE("LibUSB err: %d, %s\n", error_code, error_name);
  //fprintf(stderr,"libUSB Error: %s: %s (%d)\n", error_name, error_hint,);
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
inline int CBTDG::CheckResponse(int tx_opcode, unsigned char *pBuf, int length)
{
  int result, answer_length;
  unsigned short opcode;
  
  if(length<1) return -1; 

  //длина данных в переданном пакете  msg.c_str()
  answer_length = pBuf[1];
  if(answer_length<4)
  { 
	 TRACE("CheckResponse: paket length Error, Opcode: 0x=%X!\n\n", tx_opcode);
	 ::MessageBox(NULL, "CheckResponse paket length Error","Error:", 
		          MB_ICONERROR|MB_TASKMODAL);   
	 return -1;
  }

  //----------------------------------------------------------------------
  //HCI controller replay on host CMD
  //----------------------------------------------------------------------
  //--- Check Event Code
  //Command Complete 0x0E Num_HCI_Command_Packets,
  //Command_Opcode, Return_Parameters
  // 0x0E - HCI_Command_Complete event occured
  if(pBuf[0]==0x0E)
   {
	 //Read CMD opcode
     opcode = pBuf[4];
     opcode <<=8;
     opcode |= pBuf[3];

	 //Check CMD status, if = 0 - OK
	 if(pBuf[5]==0)
	   {                      
	      //TRACE("CMD HCI Opcode: 0x%04X OK\n", opcode);
		  return 1;
	   }
	   else
	   {    
          TRACE("CMD HCI Opcode: 0x%04X Error!\n\n", opcode);
		  return -1;
	   }  
	}
  
  //----------------------------------------------------------------------
  //HCI controller event replay
  //----------------------------------------------------------------------
  //--- Check Event Code
  if(pBuf[0]==0x0F)//HCI_command_Status event
   { /*
     length = Buf[1]; //parameter length
     opcode = Buf[5];
     opcode <<=8;
     opcode |= Buf[4];                      
	 */
	 TRACE(" > Controller set Status code: 0x%02X to Command 0x%04X\n", pBuf[2], opcode);
     //print_buffer((char*)Buf, 0, size);
     return -1; //read again
   }
  
  //----------------------------------------------------------------------
  //HCI controller replay
  //----------------------------------------------------------------------
	//--- Check Event Code
	if(pBuf[0]==0xFF)//reserved for Vendor events
	 {
     //length = Buf[1]; //parameter length
     //opcode = Buf[5];
     //opcode <<=8;
     //opcode |= Buf[4];                      
	  TRACE(" > Device Vendor Event: 0x%02X\n", pBuf[0]);
      //print_buffer((char*)Buf, 0, size);

      //return 0xFF; 
	 }
  
	//----------------------------------------------------------------------
    //HCI controller replay
    //----------------------------------------------------------------------
	//--- Check Event Code
	if(pBuf[0]==0x13)//Number Of Completed Packets
	 {
     //length = Buf[1]; //parameter length
                     
	 TRACE(" > Number Of Completed Packets Event: 0x%02X\n", pBuf[0]);
     //print_buffer((char*)Buf, 0, size);
	 
	 TRACE(" Number_of_Handles: %d\n",pBuf[2]);
	 
	 opcode = pBuf[4];
     opcode <<=8;
     opcode |= pBuf[3];
	 TRACE(" Connection_Handle: %d\n",opcode);
	 
	 opcode = pBuf[6];
     opcode <<=8;
     opcode |= pBuf[5];
	 TRACE(" Completed_Packets: %d\n\n",opcode);	

     return -1; 
	}

  //----------------------------------------------------------------------
  //»звестные HCI controller events
  //----------------------------------------------------------------------
  switch(pBuf[0])
  {
     case 0x03:
     case 0x04:
     case 0x05:
     case 0x3E://BLE all events
     //return pBuf[0];
     
     default:
     break;
  }

  //----------------------------------------------------------------------
  //HCI other controller events
  //----------------------------------------------------------------------
  TRACE(" > Unknown Controller Event: 0x%02X\n", pBuf[0]);
  //return Buf[0];


  /*
  //Exit OK for unknown rx Length 
  if(result==0 && any_accept_length==1) 
	 return rx_accepted;

  //Exit OK
  if(result==0 && rx_accepted>0) 
	 return rx_accepted;
  */
  //Exit error
  return -1;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDG::HCI_TxCMD(unsigned short opcode,  //CMD opcode
                     const void *pTxBuf, 	 //CMD buffer
                     unsigned char TxLength, //CMD length
                     const void *pRxBuf, 	 //Reply buffer
                     int RxLength,  //Reply length
                     int RxTimeout) //Reply wait timeout
{
  if(hUSB==NULL) return -1; 
  
  int result, rx_accepted=0, any_accept_length=0;
  unsigned char TxRxBuf[360];

  //add cmd code
  memcpy(TxRxBuf, &opcode, 2);	

  //add command data
  if(pTxBuf!=NULL && TxLength>0) memcpy(&TxRxBuf[3], pTxBuf, TxLength);
  else TxLength = 0;
  
  //cmd data lenght
  TxRxBuf[2] = TxLength;
  //opcode lenght
  TxLength += 3;

  //Transmit packet to dongle
  result = libusb_control_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE |
	                                     LIBUSB_REQUEST_TYPE_CLASS | 
										 LIBUSB_ENDPOINT_OUT,
										 0, //set/get test
										 0, //test type								 
										 usb_interface, //interface id 
										 TxRxBuf, TxLength, 0);
  //Check Tx
  if(result<0) 
  {
	Show_Errors(result);
	return -1;
  }

  //задержка дл€ ресета тк железка уходит в перезагрузку
  if(opcode==BT_HCI_CMD_RESET) Sleep(1000);

  //if Rx not defined 
  if(pRxBuf==NULL || RxLength==0)
   { 
	 if(RxTimeout!=0) 
		Sleep(RxTimeout);  
	 return result; 
   }
 
  if(RxLength==-1){RxLength	= 256; any_accept_length = 1;}

  //Wait answer packet from dongle
  //If END point have type "interrupt", need read like interrupt
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										  (unsigned char*)pRxBuf, 
										   RxLength, &rx_accepted, RxTimeout); 
  //Check Rx
  if(result<0) 
  {
	Show_Errors(result);
	return -1;
  }
  
  //Exit OK, for unknown rx Length and defined length
  if(result==0 && (any_accept_length==1 || rx_accepted>0))
  { 
	 result = CheckResponse(opcode, (unsigned char*)pRxBuf, rx_accepted);
	 if(result>0) return rx_accepted;
	 else return -1;
  }
  
  //Exit error
  return -1;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------   
int CBTDG::Connect(void)
{
  
  return 1;
}

//------------------------------------------------------------------------------
//Function: Write value by handle
//------------------------------------------------------------------------------
int CBTDG::DataTx(char handle, char *pBuf, int length)
{
  return 1;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------   
int CBTDG::Disconnect(void)
{ 
  //HCI_Reset cmd:0x0C03 ret:0e 04 01 03 0c 00
  HCI_TxCMD(BT_HCI_CMD_RESET, NULL, 0, NULL, 0, 0);
  
  /*
  ThreadRx.Delete();

  //Disconnect
  TRACE("==== Disconnect from target ====\n");
  TRACE("PC -> ble_cmd_connection_disconnect\n");
  
  //Cmd
  ble_cmd_connection_disconnect(app_connection.handle);
  if (Wait_Response()!= APP_OK)
   {
     Error(); return -1;
   }

  Wait_Event();  */
  return 1;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
void CBTDG::SetEventMask(long long &mask)
{ 
  mask = 0;
  //mask |= 0x0000000000000001; //Inquiry Complete Event
  //mask |= 0x0000000000000002; //Inquiry Result Event
  mask |= 0x0000000000000004; //Connection Complete Event
  mask |= 0x0000000000000008; //Connection Request Event
  mask |= 0x0000000000000010; //Disconnection Complete Event
  //mask |= 0x0000000000000020; //Authentication Complete Event
  //mask |= 0x0000000000000040; //Remote Name Request Complete Event
  //mask |= 0x0000000000000080; //Encryption Change Event

  //mask |= 0x0000000000000100; //Change Connection Link Key Complete Event
  //mask |= 0x0000000000000200; //Master Link Key Complete Event
  //mask |= 0x0000000000000400; //Read Remote Supported Features Complete Event
  //mask |= 0x0000000000000800; //Read Remote Version Information Complete Event
  //mask |= 0x0000000000001000; //QoS Setup Complete Event
  //mask |= 0x0000000000002000; //Reserved
  //mask |= 0x0000000000004000; //Reserved
  mask |= 0x0000000000008000; //Hardware Error Event

  //mask |= 0x0000000000010000; //Flush Occurred Event
  //mask |= 0x0000000000020000; //Role Change Event
  //mask |= 0x0000000000040000;// Reserved
  mask |= 0x0000000000080000; //Mode Change Event
  //mask |= 0x0000000000100000; //Return Link Keys Event
  //mask |= 0x0000000000200000; //PIN Code Request Event
  //mask |= 0x0000000000400000; //Link Key Request Event
  //mask |= 0x0000000000800000; //Link Key Notification Event
  //mask |= 0x0000000001000000; //Loopback Command Event
  mask |= 0x0000000002000000; //Data Buffer Overflow Event
  //mask |= 0x0000000004000000; //Max Slots Change Event
  //mask |= 0x0000000008000000; //Read Clock Offset Complete Event
  //mask |= 0x0000000010000000; //Connection Packet Type Changed Event
  //mask |= 0x0000000020000000; //QoS Violation Event
  //mask |= 0x0000000040000000; //Page Scan Mode Change Event [deprecated]
  //mask |= 0x0000000080000000; //Page Scan Repetition Mode Change Event
  //mask |= 0x0000000100000000; //Flow Specification Complete Event
  //mask |= 0x0000000200000000; //Inquiry Result with RSSI Event
  //mask |= 0x0000000400000000; //Read Remote Extended Features Complete Event
  //mask |= 0x0000080000000000; //Synchronous Connection Complete Event
  //mask |= 0x0000100000000000; //Synchronous Connection Changed Event
  //mask |= 0x0000200000000000; //Sniff Subrating Event
  //mask |= 0x0000400000000000; //Extended Inquiry Result Event
  //mask |= 0x0000800000000000; //Encryption Key Refresh Complete Event
  //mask |= 0x0001000000000000; //IO Capability Request Event
  //mask |= 0x0002000000000000; //IO Capability Request Reply Event
  //mask |= 0x0004000000000000; //User Confirmation Request Event
  //mask |= 0x0008000000000000; //User Passkey Request Event
  //mask |= 0x0010000000000000; //Remote OOB Data Request Event
  //mask |= 0x0020000000000000; //Simple Pairing Complete Event
  //mask |= 0x0080000000000000; //Link Supervision Timeout Changed Event
  //mask |= 0x0100000000000000; //Enhanced Flush Complete Event
  //mask |= 0x0400000000000000; //User Passkey Notification Event
  //mask |= 0x0800000000000000; //Keypress Notification Event
  //mask |= 0x1000000000000000; //Remote Host Supported Features Notification Event
  //mask |= 0x2000000000000000; //LE Meta-Event
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------     
void CBTDG::SetEventMaskPage2(long long &mask)
{
 mask = 0;
 //mask |= 0x0000000000000001; //Physical Link Complete Event
 //mask |= 0x0000000000000002; //Channel Selected Event
 //mask |= 0x0000000000000004; //Disconnection Physical Link Event
 //mask |= 0x0000000000000008; //Physical Link Loss Early Warning Event
 //mask |= 0x0000000000000010; //Physical Link Recovery Event
 //mask |= 0x0000000000000020; //Logical Link Complete Event
 //mask |= 0x0000000000000040; //Disconnection Logical Link Complete Event
 //mask |= 0x0000000000000080; //Flow Spec Modify Complete Event
 //mask |= 0x0000000000000100; //Number of Completed Data Blocks Event
 //mask |= 0x0000000000000200; //AMP Start Test Event
 //mask |= 0x0000000000000400; //AMP Test End Event
 //mask |= 0x0000000000000800; //AMP Receiver Report Event
 //mask |= 0x0000000000001000; //Short Range Mode Change Complete Event
 //mask |= 0x0000000000002000; //AMP Status Change Event
 //mask |= 0x0000000000004000; //Triggered Clock Capture Event
 mask |= 0x0000000000008000; //Synchronization Train Complete Event
 //mask |= 0x0000000000010000; //Synchronization Train Received Event
 //mask |= 0x0000000000020000; //Connectionless Slave Broadcast Receive Event
 mask |= 0x0000000000040000; //Connectionless Slave Broadcast Timeout Event
 //mask |= 0x0000000000080000; //Truncated Page Complete Event
 mask |= 0x0000000000100000; //Slave Page Response Timeout Event
 //mask |= 0x0000000000200000; //Connectionless Slave Broadcast Channel Map Change Event
 //mask |= 0x0000000000400000; //Inquiry Response Notification Event
 //mask |= 0x0000000000800000; //Authenticated Payload Timeout Expired Event
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------     
void CBTDG::SetEventMaskLE(long long &mask)
{
 mask = 0; //No LE events specified
 mask |= 0x0000000000000001; //LE Connection Complete Event
 mask |= 0x0000000000000002; //LE Advertising Report Event
 //mask |= 0x0000000000000004; //LE Connection Update Complete Event
 //mask |= 0x0000000000000008; //LE Read Remote Used Features Complete Event
 //mask |= 0x0000000000000010; //LE Long Term Key Request Event
 //mask |= 0x000000000000001F; //Default
}
