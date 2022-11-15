//==============================================================================
//File name:   "bt-adapters.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:   (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "bt-dongle.h"

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

//------------------------------------------------------------------------------
//Find & Return connected Bluetooth adapter list on 
//PC and put it to ComboBox control 
//------------------------------------------------------------------------------
int CBTDONGLE::GetAdaptersList(HANDLE lpComboBox, HANDLE pCStringArrayName)
{  
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
  if(dll_opened!=1) return 0; 

  //Info
  //AdaptersVID.empty(); //USB Vendor ID
  //AdaptersPID.empty(); //USB Product ID
  //AdaptersID.empty();
  //AdaptersName.empty();

  int ble_adapters_сount = 0; //pCyUSB->DeviceCount();
	/*
  //==== 64 Bit DLL ================================== 
  #ifdef _WIN64 
  //Get BT adapter status
  BLEManager *pBLE = (BLEManager*)hBLE;
  int adapters = pBLE->GetBluetooth();
   
  if(adapters>0)
  {
    //adapter ok
 	ble_adapters_сount = adapters;  
  	//HANDLE lpComboBox, HANDLE pCStringArrayName
  }
  else 
  {
    //if no adapters
  	ble_adapters_сount = 0;
	//HANDLE lpComboBox, HANDLE pCStringArrayName
  }  
  #endif	  */
  //==================================================
			 
  #ifdef BLE_DEBUG
  TRACE1("Detected %d BLE adapter\n", ble_adapters_сount);
  #endif
  
  return ble_adapters_сount;
}

//------------------------------------------------------------------------------
//Open by device name
//------------------------------------------------------------------------------
int CBTDONGLE::AdapterOpen(unsigned short vid, unsigned short pid)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
  
  if(WUSB.lib_opened!=1) return 0;
  int result = 0;
  int index;

  //найти в списке выбранный адаптер
  if(vid==0 && pid==0 && adapters_number>0)
  {  
	 index = 0;
	 //::AfxMessageBox(_T("Select BT Adapter from list"));
	 //return -1;
  }
  else
  {
	//index = (int) AdaptersVID.size;
	for(int i=0; i<adapters_number; i++)
	{
	  if(WUSB.VIDs.at(i) == vid &&
         WUSB.PIDs.at(i) == pid)
	  {
	     index = i;
		 result = 1;
	  }
	}

	//Если в списке нет использованого ранее 
	if(result==0)
	{
	  //использовать первый по списку
	  index = 0;
	 
	  //::AfxMessageBox(_T("Select BT Adapter from list"));
	  //return -1;
	}
  }
	
  //открыть последний использованный адаптер
  result = Dongle_Open(index);
  if(result!=1){adapter_opened = 0; return -1;}
  else {adapter_opened = 1;}

  //если успешно открыли, запомнить индекс
  adapter_index	= index;

  //----
  result = Dongle_WriteFirmware();
  if(result!=1)
  { 
	AfxMessageBox(_T("Write BT-dongle Firmware Error"),  MB_ICONERROR);
	return -1;
  }

  //----
  result = Dongle_BLE_ini();

 
  //----
  if(result>0)
  {
    Threads_Ini();

	//Запускаем процесс чтения данных
	ev_DATA.ResetEvent();
	DATA_ThreadLaunch(this);
    //ждем запуск процесса
    WaitForSingleObject(ev_DATA.m_hObject, 1000);

	//Запускаем процесс чтения евентов адаптера
    ev_CMD.ResetEvent();
    CMD_ThreadLaunch(this);
    //ждем запуск процесса
    WaitForSingleObject(ev_CMD.m_hObject, 1000);

	ev_SYNC.ResetEvent();
	SYNC_ThreadLaunch(this);
	//ждем запуск процесса
    WaitForSingleObject(ev_SYNC.m_hObject, 1000);
  }
  
  #ifdef BLE_DEBUG
  if(result>0)TRACE("Adapter opened\n");
  else TRACE("Adapter is not opened\n");
  #endif
  
  //adapter_opened
  return result;	
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CBTDONGLE::ChangeAdapter(int index)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
  
  AdapterClose();

  int result = 0;
 	  /*
  //найти в списке выбранный адаптер
	//index = (int) AdaptersVID.size;
	for(int i=0; i<adapters_number; i++)
	{
	  if(AdaptersVID.at(i) == vid &&
         AdaptersPID.at(i) == pid)
	  {
	     index = i;
	  }
	}
  }	*/

  //открыть адаптер
  result = Dongle_Open(index);
  if(result!=1){adapter_opened = 0; return -1;}
  else {adapter_opened = 1;}

  //если успешно открыли, запомнить индекс
  adapter_index	= index;

  //----
  result = Dongle_WriteFirmware();
  
  //----
  result = Dongle_BLE_ini();
   /*
  //----
  if(result>0)
  {
    Threads_ini();

	//Запускаем процесс чтения данных
	ev_DATA.ResetEvent();
	DATA_ThreadLaunch(this);
    //ждем запуск процесса
    WaitForSingleObject(ev_DATA.m_hObject, 5000);

	//Запускаем процесс чтения евентов адаптера
    ev_CMD.ResetEvent();
    CMD_ThreadLaunch(this);
    //ждем запуск процесса
    WaitForSingleObject(ev_CMD.m_hObject, 5000);
  }	 */
  
  #ifdef BLE_DEBUG
  if(result>0)TRACE("Adapter opened\n");
  else TRACE("Adapter is not opened\n");
  #endif
 	 //ЗАПОМНИТЬ СМЕНУ !!!!
  //adapter_opened
  return 1;	
}

//------------------------------------------------------------------------------
//Close Bluetooth Adapter
//------------------------------------------------------------------------------
void CBTDONGLE::AdapterClose(void)
{ 
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  if(WUSB.dev_opened==0) return;
  
  //if Dongle connected - disconnect RF link
  Disconnect();


  /*
  //make sure xfers are not active
  if(rx_xfer != NULL) libusb_cancel_transfer(rx_xfer);
  
  usb_opened = 0;
  dongle_ready = 0;

  if(hUSB != NULL)
  {
	libusb_release_interface(hUSB, usb_interface);
	//Reset dongle to initial state
    libusb_reset_device(hUSB);
  }
	
  libusb_close(hUSB);
  libusb_exit(NULL);
  hUSB = NULL;
   */

  adapter_opened = 0;
}
