//==============================================================================
//File name:    "MainDlg.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "PIDPC.h"
#include "MainDlg.h"
#include "cmd.h"
#include "datatx.h"
#include <stdio.h>
#include "bt-dongle.h"
#include "app-setup.h"

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


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BT_AddrToTxt(CString *pTxt, long long Addr) 
{
  unsigned char buf[6];
  CString txt;

  for(int i = 0; i<6; i++) 
  {
	buf[5-i] = Addr;
	Addr >>= 8;

	if(i!=5)txt.Format("%X:", buf[5-i]);
	else txt.Format("%X", buf[5-i]);
	 
	*pTxt += txt;
  }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BT_AddrToStr(char *pAddr, long long Addr) 
{
  unsigned char buf[6];
  CString txt;

  for(int i = 0; i<6; i++) 
  {
	buf[5-i] = Addr;
	Addr >>= 8;

	if(i!=5)txt.Format("%X:", buf[5-i]);
	else txt.Format("%X", buf[5-i]);
	 
	//*pTxt += txt;
  }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMainDlg::BLE_UpdateDeviceList(void)
{ 
  int ret, index;
  CString txt, name, addr_txt, msg;
  int dev_count = BLE.Devices_BT_ADDR_str.size();
  
  msg.Format("Found devices: %d\r\n\r\n", dev_count);

  //------------------------------------------------------------
  //Если ни одно устройство еще не было подключено
  //------------------------------------------------------------
  if(BLE.dev_connected==0)
  {
  	//проверить адрес последнего подключенного
    ret = REG.GetTxtVar("BT_address", addr_txt);
	ret = -1;
	if(ret>0)
	{
	  //устройство ранее использовалось, адрес есть в памяти программы
	  //проверить, есть ли оно в списке обнаруженных устроиств
	  //получить индекс этого устройства в списке
	  index = BLE.FindAddrInList((char*)addr_txt.GetBuffer());
	  if(index>=0)
	  {  
		//Connect, устройство присутствует в эфире, подключить
        BLE.Connect((char*)addr_txt.GetBuffer());
		BLE.device_index = index;

		//ждем пока выполнит подключение, потом запросить сетап

	  }
	}
  }
  //------------------------------------------------------------
  
  //стереть список устройств
  m_combo_devices_list.ResetContent();

  //------------------------------------------------------------
  //Загрузить список устройств в контрол
  //------------------------------------------------------------
  for(int i=0; i<dev_count; i++)
  {
	//unsigned long long addr_dig = BLE.Devices_BT_ADDR.at(i);
	name = BLE.Devices_Name.at(i).c_str();	 
    //BT_AddrToTxt(&addr_txt, addr_dig); 
	//txt.Format("%s  %s", name, addr_txt);
    //m_devices_list.AddString(txt);
    m_combo_devices_list.AddString(name);
	
	//name 
	msg += name + ", BT Addr: ";
	msg += BLE.Devices_BT_ADDR_str.at(i).c_str();  
	txt.Format("%d", BLE.Devices_RSSI.at(i));
	msg += ", RSSI: " + txt + "dBm\r\n";
  }	
  
  pPage6->m_edit_info.SetWindowTextA(msg);

  //если хоть одно устройство есть в списке
  if(dev_count>0)
  {
	if(dev_count>0 && BLE.device_index>=0)
	{
	  //установить строку к показу 
      m_combo_devices_list.SetCurSel(BLE.device_index);
	}
	else 
	{
	  //если устройство не подключено
      m_combo_devices_list.SetCurSel(0);
	}
  }
}

  /*
  //если это первый запуск и устройство не было выбрано
  if(dev_selected==0)
  { 
	int ret = REG.GetTxtVar("BT_address", addr_txt);
	if(ret)
	{
	
	}
  }
  
  //Save settings to Registry
  REG.SetTxtVar("BT_address",  addr_txt);
  */

//------------------------------------------------------------------------------
//	BT ADDR: D1:A3:5D:1B:6E:11
//------------------------------------------------------------------------------
void CMainDlg::BLE_SelchangeDevice(int index)
{
  CString addr_txt, dig;
  //m_combo_devices_list.GetWindowTextA(txt);

  if(BLE.dev_connected==1)
  {
    BLE.Disconnect();
    //no devices
    Controls_Enable(0);
    LED_Control(0);
    Sleep(200);
  }

  //----
  addr_txt = BLE.Devices_BT_ADDR_str.at(index).c_str();
  
  /*
  std::vector <char> dev_addr;
  dev_addr = BLE.Devices_BT_ADDR.at(index);
 
  for(int i=0; i<6; i++) 
  {
	unsigned char byte = dev_addr.at(i);
	dig.Format("%0.2X", byte);
	txt += dig;
  } */
  
  //Save settings to Registry
  REG.SetTxtVar("BT_address",  addr_txt);
  
  //Connect
  BLE.Connect((char*)addr_txt.GetBuffer());
  ///BLE.Connect((char*)"5D1B6E110100");
  ///BLE.Connect((char*)"D1A35D1B6E11");
}

  /*
  //если это первый запуск и устройство не было выбрано
  if(dev_selected==0)
  { 
	int ret = REG.GetTxtVar("BT_address", addr_txt);
	if(ret)
	{
	
	
	}
  }
  
  //Save settings to Registry
  REG.SetTxtVar("BT_address",  addr_txt);
  */

 /*
 if(pBLE->dev_connected==0)
 {
    ///int ret = pBLE->Connect((char*)"D1A35D1B6E11");
    //pBLE->Connect((char*)"C6049C279ABD");
	 
    //F6:41:33:9A:73:C0 Terry
	//pBLE->Connect((char*)"F641339A73C0");
	//pBLE->Connect((char*)pBLE->DevicesI	
  }*/

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CMainDlg::BLE_Close(void)
{
  int ret;
  ret = BLE.WUSB.CloseDevice();
  BLE.WUSB.Close();
	
  return ret;
}

//-----------------------------------------------------------------------------
//Function:	ДОБАВИТЬ ОБРАБОТЧИКИ
//-----------------------------------------------------------------------------
int CMainDlg::BLE_ini(void)
{
  //----
  BLE.AddFilter_DeviceName("T-962");
  BLE.AddRxHandler(BLE_DataRx);
  BLE.AddBTeventsHandler(BLE_Event);
  
  return 1; 
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CMainDlg::BLE_Open(void)
{
  int find_adapters = BLE.FindAdapters();
  if(find_adapters<=0)
  {	  
	//стереть список устройств
    m_combo_bt_adapters.ResetContent();
	m_combo_devices_list.ResetContent();
    //----
	m_combo_bt_adapters.AddString("No BT adapters");
	m_combo_devices_list.AddString("Off");
	//----
	m_combo_bt_adapters.SetCurSel(0);
	m_combo_devices_list.SetCurSel(0);
	//----
	m_but_DevList.EnableWindow(FALSE);
	m_combo_devices_list.EnableWindow(FALSE);
	//----
	CString txt;
	txt = "Bluetooth adapter not found\r\n\r\n";
	txt += "1. Connect the BCM20702A0 BT USB dongle to PC\r\n";
	txt += "2. Install the WinUSB driver -> button About\r\n";
	txt += "3. Restart the program\r\n";
	pPage6->m_edit_info.SetWindowTextA(txt);
	//----
	return -1;
  }

  BT.SethBLE(&BLE);
  //----
  m_but_DevList.EnableWindow(TRUE);
  m_combo_devices_list.EnableWindow(TRUE);

  //если в настройках приложения адаптер выключен
  //if(APP.bt_enable==0) return -1; 

  //Получить список подключенных адаптеров
  //result = pBLE->GetAdaptersList(&m_combo_adapters, NULL);
  //result = BLE.WUSB.VIDs.size();
  
  int index=-1, match = 0;
  unsigned short vid = 0;
  unsigned short pid = 0;
  CString name;
  CString manuf;
  
  m_combo_bt_adapters.ResetContent();

  for(int i=0; i<BLE.adapters_number; i++)
  {
	match = 0;

	vid = BLE.WUSB.VIDs.at(i);
	pid = BLE.WUSB.PIDs.at(i);
	name = BLE.WUSB.Names.at(i).c_str();
	manuf = BLE.WUSB.Manufacturers.at(i).c_str();

	//----
	m_combo_bt_adapters.AddString(name);

    //найти последний использованый 
    if(APP.bt_last_vid==vid &&
	   APP.bt_last_pid==pid)
    {
  	  //если совпало
  	  match = 1;
	  index = i;
    }

	if(match)
	   break;
  }

  //Как работать с устройством USB через функции WinUSB 
  //http://microsin.net/programming/pc/how-to-access-a-usb-device-by-using-winusb-functions.html
  //https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/winusb-functions-for-pipe-policy-modification
  //https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/automatic-installation-of-winusb

  //последний адаптер не был найден и нет доступных адаптеров
  if(match==0 && find_adapters<1) return -1; //BLE.adapter_index
	   
  //подключить первый найденый адаптер
  m_combo_bt_adapters.SetCurSel(0);
  vid = BLE.WUSB.VIDs.at(0);
  pid = BLE.WUSB.PIDs.at(0);

  //----
  int result = BLE.AdapterOpen(vid, pid);
  //result = BLE.AdapterOpen(APP.PC_BT_last_vid,  APP.PC_BT_last_pid);
  //if no adapters
  if(result!=1)
  {
	CString msg;
	msg = "BT adapter hanging and requires a Reset\r\n\r\n";
	msg += "1. Close the application\r\n";
	msg += "2. Disconnect BT adapter and plug him back\r\n";
	msg += "3. Run application again";
	AfxMessageBox(_T(msg),  MB_ICONWARNING);
	return -1;
  }   
  
  //adapter ok
  //запомнить последний открытый адаптер
  APP.bt_last_vid = vid;
  APP.bt_last_pid = pid;
  CSETUP.Save();

  //----
  result = BLE.GetDevicesList(NULL, 0, 0);


  /*
  result = 1;
  int dev_ble_number;

  //Scan was ok
  if(result>0)
  {
    //Check device list
	dev_ble_number = (int)BLE.DevicesID.size();
  }
  else
  {
    //no BLE devices found
    dev_ble_number = 0; 
    ble_device = -1;
	return 0;
  }	  */
/*
  //----
  //Find last used device
  result = BLE.FindInList(ApSTP.last_connected_ble);
  if(result>0){BLE.Connect(ApSTP.last_connected_ble);}
  //need select new device from list
  else{}
  
  for(int i=0; i<BLE.BLE_DeviceName.GetCount(); i++)
  {
	CString str = BLE.BLE_DeviceName.GetAt(i);
	m_devices_list.AddString(str.GetBuffer());
  }	*/
   
  return 1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BT_VecAddrToStr(char *pAddr, std::vector <char> *pAdrr) 
{
  unsigned char buf;
  CString txt;

  for(int i = 0; i<6; i++) 
  {
//	buf = pAdrr->at[i];

	//if(i!=5)txt.Format("%X:", buf[5-i]);
	//else txt.Format("%X", buf[5-i]);
	 
	//*pTxt += txt;
  }
}

//------------------------------------------------------------------------------
//	BT ADDR: D1:A3:5D:1B:6E:11
//------------------------------------------------------------------------------
/*
void CMainDlg::BLE_SelchangeDevice(void)
{
 int index = m_combo_devices_list.GetCurSel();
 
 CString txt, dig;
 //m_combo_devices_list.GetWindowTextA(txt);

 if(BLE.dev_connected==1)
 {
   BLE.Disconnect();
   //no devices
   Controls_Enable(0);
   LED_Control(0);
   Sleep(200);
 }

 //----
 txt = BLE.Devices_BT_ADDR_str.at(index).c_str();
 /*
 std::vector <char> dev_addr;
 dev_addr = BLE.Devices_BT_ADDR.at(index);
 
 for(int i=0; i<6; i++) 
 {
	unsigned char byte = dev_addr.at(i);
	dig.Format("%0.2X", byte);
	txt += dig;
 }
*/
/*
 BLE.Connect((char*)txt.GetBuffer());
 ///BLE.Connect((char*)"5D1B6E110100");
 ///BLE.Connect((char*)"D1A35D1B6E11");


 /*
 if(pBLE->dev_connected==0)
 {
    ///int ret = pBLE->Connect((char*)"D1A35D1B6E11");
    //pBLE->Connect((char*)"C6049C279ABD");
	 
    //F6:41:33:9A:73:C0 Terry
	//pBLE->Connect((char*)"F641339A73C0");
	//pBLE->Connect((char*)pBLE->DevicesI	
  }
}  */

/*
m_list_devices.ResetContent();
  for(int i=0; i<length; i++)
  {
	CString txt, name, addr_txt;
	unsigned long long addr_dig = pBLE->Devices_BT_ADDR.at(i);
	name = pBLE->DevicesName.at(i).c_str();	 
    BT_AddrToTxt(&addr_txt, addr_dig); 

	txt.Format("%s  %s", name, addr_txt);
	m_list_devices.AddString(txt);
  }*/

//-----------------------------------------------------------------------------
//Function:
//-----------------------------------------------------------------------------
void CMainDlg::OnSelchangeBTAdapters()
{
  int index = m_combo_bt_adapters.GetCurSel();
  BLE_SelchangeAdapters(index);
}

//-----------------------------------------------------------------------------
//Function:
//-----------------------------------------------------------------------------
void CMainDlg::BLE_SelchangeAdapters(int index)
{
 int ret;

 //Если устройство было подключено
 if(BLE.dev_connected==1)
 {
   //отключить устройство
   BLE.Disconnect();
   //закрыть
   BLE.CloseUSB();
   Controls_Enable(0);
   LED_Control(0);
 }
 
 //если не было подключено ни одного адаптера
 if(BLE.adapter_opened==0)
 {
   //подключить адаптер
   ret = BLE_Open();
   //если адаптер успешно подключен
   if(ret)
   {
   	 //обновить список удаленных устройств
	 BLE_UpdateDeviceList();
   }

 }

}

//-----------------------------------------------------------------------------
//Function:	BLE Rx Handler
//-----------------------------------------------------------------------------
extern CMainDlg *pCMainDlg;
void CMainDlg::BLE_DataRx(char *pBuf, int length)
{ 
  pCMainDlg->DataRx(pBuf, length);
}

//-----------------------------------------------------------------------------
//Function:	BLE Events Handler
//-----------------------------------------------------------------------------  
void CMainDlg::BLE_Event(int event_id, char *pBuf, int length)
{
  //----
  if(event_id==BT_DEV_ARRIVED || event_id==BT_DEV_EXIT)
  {
    pCMainDlg->BLE_UpdateDeviceList();
	return;
  }

  //----
  if(event_id==BT_DEV_CONNECTED)
  {
  	//Device was connected	
	pCMainDlg->LED_Control(1);
	pCMainDlg->APP.bt_connected = 1;
	//если устройство подключено
    pCMainDlg->GetDeviceSetup();
	return;
  }
 
  //----
  if(event_id==BT_DEV_DISCONNECTED)
  {
  	//Device disconnected
	pCMainDlg->LED_Control(0);
	pCMainDlg->APP.bt_connected = 0;
  }
}
