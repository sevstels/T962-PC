//==============================================================================
//File name:   "bt-devices.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:   (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <dbt.h>
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

//static CBTDONGLE *pCBLE;
static int count;

//------------------------------------------------------------------------------
//Find & Return BLE device list 
//------------------------------------------------------------------------------
int CBTDONGLE::GetDevicesList(HANDLE lpListBox, LPVOID pDevIDs, LPVOID pDevNames)
{  
  #ifdef BLE_DEBUG
  TRACE1("WinBLE: %s\n", __FUNCTION__);
  #endif

  //Info
  DevicesName.empty();
  DevicesID.empty();
  int result = 0;
			   /*
  //==== 64 Bit DLL ================================== 
  #ifdef _WIN64
  BLEManager *pBLE = (BLEManager*)hBLE;

  //Start scanning for devices (better to move somewhere else)
  result = pBLE->StartScan();
  if(result)
  {	
	//========================================================================
    //Wait scan start run, ms
    //========================================================================
	int timeout_ms = 200;
	ev_DeviceScan_Begin.ResetEvent();
	result = WaitForSingleObject(ev_DeviceScan_Begin.m_hObject, timeout_ms);
    //----
    switch(result)
    {   
      //произошел сбой 
	  //exit by error  
      case WAIT_FAILED:
///	  pBLE->StopScan();
      return -1;

      //ни один процесс не завершился в течение последних timeout_ms
      //срабатывает если адаптер не отвечает 
	  //exit by timeout
      case WAIT_TIMEOUT:
///	  pBLE->StopScan();
      return -1;

      //процесс с дескриптором h[0] завершился, OK 
      case WAIT_OBJECT_0:
      break;       
    } 
	
	//========================================================================
    //Wait scan completed, ms
    //========================================================================
	timeout_ms = 2000;
	ev_DeviceScan_Done.ResetEvent();
	result = WaitForSingleObject(ev_DeviceScan_Done.m_hObject, timeout_ms);
    //----
    switch(result)
    {   
      //произошел сбой 
	  //exit by error  
      case WAIT_FAILED:
	  result = -1;
      break;

      //ни один процесс не завершился в течение последних timeout_ms
      //срабатывает если адаптер не отвечает 
	  //exit by timeout
      case WAIT_TIMEOUT:
	  result = -1;
      break;

      //процесс с дескриптором h[0] завершился, OK 
      case WAIT_OBJECT_0:
	  result = 1;
      break;       
    }

///	pBLE->StopScan();
 	
	//-----
    if(result>0)
	{
	
	
	
	}
  }
  //ble_DeviceCount =	count;
  #endif
  //==================================================

  #ifdef BLE_DEBUG
  TRACE1("WinBLE: Detected %d BLE devices\n", result);
  #endif
  	*/
  return 1;
}

//------------------------------------------------------------------------------
//Find BLE device list 
//------------------------------------------------------------------------------  
int CBTDONGLE::FindInList(char *pStrDeviceID)
{
  #ifdef BLE_DEBUG
  TRACE1("WinBLE: %s\n", __FUNCTION__);
  #endif
  
  int found=0, length, result;
  int dev_count = (int)DevicesName.size();
  
  //----
  CString used_id(pStrDeviceID);
  length = used_id.GetLength();
  if(length<8 || length>64 || dev_count<1) return -1;

  //----
  for(int i=0; i<dev_count; i++)
  {
	std::string list_id = DevicesName.at(i);
	result = (int)list_id.find(used_id);
	if(result>=0){found = 1;}
  }

  #ifdef BLE_DEBUG
  TRACE1("WinBLE: Last BLE found: %d\n", found);
  #endif

  return found;
}

//------------------------------------------------------------------------------
//Find BLE device list 
//------------------------------------------------------------------------------  
int CBTDONGLE::FindAddrInList(char *pStrDevAddress)
{
  #ifdef BLE_DEBUG
  TRACE1("WinBLE: %s\n", __FUNCTION__);
  #endif
  
  unsigned char address[6];
  int found=0, length, result, index;
  
  //----
  CString str(pStrDevAddress);
  length = str.GetLength();
  if(length!=12) return -1;

  //Преобразуем в адрес для сравнения
  //Target MAC address d0:fd:00:a3:dd:5c
  for(int i=0; i<6; i++)
  {
	char txt[3] = {0,0,0};
	unsigned int value;
	memcpy(txt, pStrDevAddress, 2);
	//read value in any format
    result = sscanf(txt, "%x", &value);
	pStrDevAddress += 2;
	
	//if error Exit 
  	if(result<=0) return -1;

	address[i] = (unsigned char) value;
  }

  //посмотреть число устройств в списке 
  int dev_count = (int)Devices_BT_ADDR.size();
  
  //
  index = -1;

  //сравнить
  for(int i=0; i<dev_count; i++)
  {
	std::vector <char> list_addr;
	//извлечь адрес устройства из списка
	list_addr = Devices_BT_ADDR.at(i);
	
	unsigned char dev_a[6];
	dev_a[0] = list_addr.at(0);
	dev_a[1] = list_addr.at(1);
	dev_a[2] = list_addr.at(2);
	dev_a[3] = list_addr.at(3);
	dev_a[4] = list_addr.at(4);
	dev_a[5] = list_addr.at(5);

	int match = memcmp(address, dev_a, 6);
	 
	//Совпадение найдено
	if(match==0){index = i; break;}
  }

  #ifdef BLE_DEBUG
  TRACE1("WinBLE: Address Found, index: %d\n", index);
  #endif

  return index;
}

//------------------------------------------------------------------------------
//Find BLE device ID index in list
//------------------------------------------------------------------------------  
int CBTDONGLE::GetIdByName(char *pStrDevName)
{
  #ifdef BLE_DEBUG
  TRACE1("WinBLE: %s\n", __FUNCTION__);
  #endif
  
  int i, length, result;
  int dev_count = (int)DevicesName.size();
  
  //----
  std::string dev_mame(pStrDevName);
  length = (int)dev_mame.length();
  if(length<2 || length>32 || dev_count<1) return -1;

  //----
  for(i=0; i<dev_count; i++)
  {
	std::string dev_list = DevicesName.at(i);
	result = (int)dev_list.find(dev_mame);
	if(result>=0){break;}
  }

  #ifdef BLE_DEBUG
  TRACE1("WinBLE: BLE ID index found: %d\n", i);
  #endif

  return i;
}