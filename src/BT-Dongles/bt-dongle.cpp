//==============================================================================
//File name:   "bt-dongles.cpp"
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
#include <algorithm>

CBTDONGLE *pDNGL;

//------------------------------------------------------------------------------ 
//Constructor 
//------------------------------------------------------------------------------
CBTDONGLE::CBTDONGLE()
{ 
  pSyncBuf = NULL;
  pRXhandler = NULL;
  pBTEventsHandler = NULL; 
  thead = 0;
  
  //----
  sync_ready = 0;
  sync_length = 0; 
  sync_completed = 0;

  //----
  Last_Opened_DeviceID.empty();

  //----
  dll_opened = 0;
  usb_opened = 0;
  dev_connected = 0;
  dev_selected = 0;
  device_index = -1;
  dev_connection_handle = 0;
  devices_number = 0;
  adapter_vid = 0;
  adapter_pid = 0;
  adapter_model = -1;
  adapter_index = -1;
  adapters_number = 0;
  adapter_opened = 0;
  dongle_ready = 0;
  usb_interface = 0;
  usb_configuration = 1;
  
  //----
  memset(HCI_buf, 0, sizeof(HCI_buf));
  memset(L2CAP_buf, 0, sizeof(L2CAP_buf));
///  memset(&descr_dev, 0, sizeof(descr_dev));
///  memset(&descr_conf, 0, sizeof(descr_conf));
  
  //----
  Dongle_Supported();
  Filter_DeviceName.empty();
}

//------------------------------------------------------------------------------
//Destructor 
//------------------------------------------------------------------------------
CBTDONGLE::~CBTDONGLE()
{
  WUSB.CloseDevice();
  WUSB.Close();
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
int CBTDONGLE::AddRxHandler(void(*pRXcallback)(char*, int))
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  //Add Rx callback
  pRXhandler = pRXcallback;
  //----
  if(pRXhandler!=NULL) return 1;
  else return -1;
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
int CBTDONGLE::AddBTeventsHandler(void(*pEventsCallback)(int, char*, int))
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  //Add Events callback
  pBTEventsHandler = pEventsCallback;
  //----
  if(pEventsCallback!=NULL) return 1;
  else return -1;
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
int CBTDONGLE::AddFilter_DeviceName(char *pDeviceName)
{
   std::string filter(pDeviceName);
   Filter_DeviceName.push_back(filter);
   return Filter_DeviceName.size();
}

//------------------------------------------------------------------------------
//Static callback to update device list
//------------------------------------------------------------------------------
void Callback_BTDeviceScan(const char* deviceName, const char* deviceId, 
	                       bool remove, void *pContext)
{
  #ifdef BLE_DEBUG
  //----	  ошибка  bool remove - проинвертировать!!!
  char *pAction;
  if(remove==false) pAction = "Discovered";
  else pAction = "Losted";
  //----
  TRACE2("%s: %s", __FUNCTION__, pAction);
  TRACE2(" - %s, Id: %s\n", deviceName, deviceId);
  #endif
  	/*
  //==== 64 Bit DLL ==================================  
  #ifdef _WIN64
  CWINBLE *pCBLE = (CWINBLE*)pContext;

  //фильтр по допустимым именам устройств
  std::string dev_name(deviceName);

  int find = 0;
  int name_count = (int)pCBLE->FilterDevName.size();
  for(int i=0; i<name_count; i++)
  {
	int result = (int)dev_name.find(pCBLE->FilterDevName.at(i));
	if(result>=0){find=1; break;}
 
  }

  //выход если устройство не совпадает
  if(find==0) return;

  //---- добавить устройство
  if(remove==false) 
  {
	//----
	pCBLE->DevicesName.push_back(deviceName);
	pCBLE->DevicesID.push_back(deviceId);
	pCBLE->bt_devices_number++;
	//хоть одни очки были задетектированы
	if(pCBLE->pBT_glasses_opened!=NULL)
	  *pCBLE->pBT_glasses_opened = 1;
  }
  else 
  {
    //устройство потеряно для обнаружения
    //ищем его в списке устройств и удаляем
	int id_count = (int)pCBLE->DevicesID.size();
    for(int i=0; i<id_count; i++)
    {
	  int result = (int)pCBLE->DevicesID.at(i).find(deviceId);
	  if(result>=0)
	  {
		//найдено успешно, удалить из массива
		pCBLE->DevicesName.erase(pCBLE->DevicesName.begin()+i);
		pCBLE->DevicesID.erase(pCBLE->DevicesID.begin()+i);
		pCBLE->bt_devices_number--;
		if(pCBLE->bt_devices_number<0)
		   pCBLE->bt_devices_number=0;
		break;
	  }
    }
  }

  //Если приложение установило свой обработчик
  //послать сообщение приложению
  if(pCBLE->pBTEventsHandler!=NULL)
  {
	pCBLE->pBTEventsHandler(remove, NULL, 0);
  }
  #endif
  //==================================================
  */
}

//------------------------------------------------------------------------------
//Callback for adapter results
//------------------------------------------------------------------------------
static void Callback_BTAdapterEvent(const char* name, const char* id, 
	                                bool remove, void *pContext)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
  				/*
  //==== 64 Bit DLL ==================================  
  #ifdef _WIN64 
  CWINBLE *pCBLE = (CWINBLE*)pContext;
  if(remove==false)
  {
    pCBLE->BT_AdapterVID.Add(23);   //USB Vendor ID
    pCBLE->BT_AdapterPID.Add(23);   //USB Product ID
    pCBLE->BT_AdapterID.Add(id);    //Address
	pCBLE->BT_AdaptersName.push_back(name);//Name
	pCBLE->bt_adapters_number++;
  }
  #endif
  //==================================================	*/
}

//------------------------------------------------------------------------------
//BLE status callback
//Поменять на Callback_DeviceStatus
//добавить сюда какое конкретно id успешно подключилось!!!
//------------------------------------------------------------------------------
static void Callback_BTAdapterStatus(int adapterstatus, bool connectstatus, 
	                                 void *pContext)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
  	  /*
  //==== 64 Bit DLL ==================================    
  #ifdef _WIN64 
  CWINBLE *pCBLE = (CWINBLE*)pContext;
  //состояние адаптера
  pCBLE->adapter_opened = adapterstatus;
  TRACE1("BT Adapter status: %d\n", adapterstatus);

  //состояние устройства
  pCBLE->dev_opened = connectstatus;
  TRACE1("BT Dev conn status: %d\n", (int)connectstatus);

  if(pCBLE->dev_opened==1)
  {
	//находим последнее прибывшее устройство 
	int index = (int) pCBLE->DevicesID.size();
	pCBLE->Last_OpenedDeviceID = pCBLE->DevicesID.at(index-1);
  }
  #endif
  //==================================================	 */
}

//------------------------------------------------------------------------------
//Callback to udpate scan status
//------------------------------------------------------------------------------
static void Callback_BTDeviceScanEvent(bool scanning, void *pContext)
{
  #ifdef BLE_DEBUG
  TRACE2("%s %d\n", __FUNCTION__, scanning);
  #endif
   	 /*
  //==== 64 Bit DLL ==================================  
  #ifdef _WIN64 
  CWINBLE *pCBLE = (CWINBLE*)pContext;
  if(scanning==true) 
	 pCBLE->ev_DeviceScan_Begin.SetEvent();
  else 
	 pCBLE->ev_DeviceScan_Done.SetEvent();
  #endif
  //==================================================	*/
}
 /*
//------------------------------------------------------------------------------
//Callback for scan results
//------------------------------------------------------------------------------
static void Callback_ScanDevice(const char* name, const char* id,
	                            bool remove, void *pContext)
{
  #ifdef BLE_DEBUG
  TRACE1("WinBLE: %s\n", __FUNCTION__);
  #endif
  
  CWINBLE *pCBLE = (CWINBLE*)pContext;
  if(remove==false)
  {
    pCBLE->BT_AdapterVID.Add(23);   //USB Vendor ID
    pCBLE->BT_AdapterPID.Add(23);   //USB Product ID
    pCBLE->BT_AdapterID.Add(id);    //Address
    pCBLE->BT_AdapterName.Add(name);//Name
	pCBLE->bt_adapters_number++;
  }
} */

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
int CBTDONGLE::FindAdapters(void)
{ 
  //if(WUSB.lib_opened==1) return 1;
  
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
  
  //получаем список всех устройств usb
  int result = WUSB.Open();
  if(result<1) return -1;
  
  //выбираем только BLE
  result = Find_Dongles();
  
  adapters_number = result;
    
  return result;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
void CBTDONGLE::Close(void)
{
  //if Device connected - disconnect RF link
  Disconnect();
  CloseUSB();
  Threads_Release();
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::Find_Dongles(void)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
   
  int result;

  //локальные буферы только для поиска
  std::vector <unsigned short> VID; //USB Vendor ID
  std::vector <unsigned short> PID; //USB Product ID
  std::vector <long long> ID;
  std::vector <std::string> Manufacturer;
  std::vector <std::string> Name;
  std::vector <std::string> DevPatch;

  //----
  VID.empty();
  PID.empty();
  ID.empty();
  Manufacturer.empty();
  Name.empty();
  DevPatch.empty();

  //------------------------------------------------------------------
 /* #ifdef USE_LIBUSB
  struct libusb_device_descriptor desc;
  struct libusb_config_descriptor *config;
  int usb_dev_num, i, ret, adapters_number = 0;
  char txt[256];
  
  //получить список подключенных устройств
  usb_dev_num = libusb_get_device_list(ctx, &usb_device_list);
  for(i=0; i<usb_dev_num; ++i)
  {
	ret = libusb_get_device_descriptor(usb_device_list[i], &desc);
	if(ret<0)
	{
	  fprintf(stderr, "couldn't get usb descriptor for dev #%d!\n", i);
	}
	
	//проверка на уже известные адаптеры
	for(int i=0; i<ВT_DNGL.numbers; i++)
	{
      //проверка совпадения
	  if(desc.idVendor == ВT_DNGL.vid[i] &&
		 desc.idProduct == ВT_DNGL.pid[i])
	  {
	    //USB Vendor ID
		VID.push_back((int)desc.idVendor); 
        //USB Product ID
		PID.push_back((int)desc.idProduct);
		//----
		ID.push_back(desc.iSerialNumber);
		//----
		Manufacturer.push_back(ВT_DNGL.manufacturer[i]);
		Name.push_back(ВT_DNGL.name[i]);
		//----
		adapters_number++;
	  }
	}

	 /*
	//libusb_get_bus_number(usb_device[i]); 
	//libusb_get_device_address(usb_device[i]);
	if(desc.bDeviceClass == LIBUSB_CLASS_WIRELESS ||
	   desc.bDeviceClass == LIBUSB_CLASS_VENDOR_SPEC)
    //(desc.idVendor==0x0A12 || desc.idVendor==0x0A5C)
	  {
        
		//USB Vendor ID
		VID.push_back((int)desc.idVendor); 
        //USB Product ID
		PID.push_back((int)desc.idProduct);

		ret = libusb_get_active_config_descriptor(usb_device_list[i], &config);

		//открываем чтоб получить инфо
		ret = libusb_open(usb_device_list[i], &hUSB);  
		if(ret==0)
	    {
		  //---- 1
		  memset(txt, 0, sizeof(txt));
		  ret = libusb_get_string_descriptor_ascii(hUSB, 1, 
			          (unsigned char*)txt, sizeof(txt)); 
		  if(ret>0){Manufacturer.push_back(txt);} 
		  else {Manufacturer.push_back(" ");}
		  
		  //---- 2	USB Dev Name
		  memset(txt,0,sizeof(txt));
		  ret = libusb_get_string_descriptor_ascii(hUSB, 2, 
			          (unsigned char*)txt, sizeof(txt)); 
		  if(ret>0){Name.push_back(txt);} 
		  else {Name.push_back(" ");}			 

		  //---- 3  Product ID
		  memset(txt,0,sizeof(txt));
		  ret = libusb_get_string_descriptor_ascii(hUSB, 3, 
			          (unsigned char*)txt, sizeof(txt)); 
		  if(ret>0){Serial.push_back(txt);} 
		  else {Serial.push_back(" ");}	
		   
		  libusb_close(hUSB); 
		  hUSB = NULL; 
		}
		else
		{
		  Manufacturer.push_back(" ");
		  Name.push_back(" ");			 
		  Serial.push_back(" ");
		  //Show_Errors(ret);
		}  

		adapters_number++;
	  }		
	} 
			 */
/*  //-----
  libusb_free_device_list(usb_device_list, usb_dev_num);
  usb_device_list = NULL;
  #endif		 */
  //-------------------------------------------------------
	
  CString msg, txt;
  ///txt.Format("Find dongles: %d", WUSB.PIDs.size());
  ///AfxMessageBox(_T(txt));
  ///txt.Empty();

  //----
  int adapters = 0;
  int number = WUSB.PIDs.size();

  //перебираем найденные адаптеры
  for(int n=0; n<number; n++)
  {
	
	///txt.Format("VID: 0x%X, PID: 0x%X\r\n", WUSB.VIDs.at(n), WUSB.PIDs.at(n));
	///msg += txt;

	//проверка на уже известные адаптеры
	for(int i=0; i<ВT_DNGL.numbers; i++)
	{
   
	  //проверка совпадения
	  if(WUSB.VIDs.at(n) == ВT_DNGL.vid[i] &&
		 WUSB.PIDs.at(n) == ВT_DNGL.pid[i])
	  {
	    //USB Vendor ID
		VID.push_back((int)WUSB.VIDs.at(n)); 
        //USB Product ID
		PID.push_back((int)WUSB.PIDs.at(n));
		//----
		ID.push_back(WUSB.IDs.at(n));
		//----
		Manufacturer.push_back(ВT_DNGL.manufacturer[i]);
		Name.push_back(ВT_DNGL.name[i]);
		DevPatch.push_back(WUSB.DevPatchs.at(n));
		 
		///AfxMessageBox(_T("VID&PID match"));

		//----
		adapters++;
	  }
	}
  }
   
  ///AfxMessageBox(_T(msg));

  #ifdef BLE_DEBUG
  if(adapters==0){TRACE("BLE adapter not found\n");}
  else{TRACE1("Found %d BLE adapter\n", adapters);}   
  #endif
  
  adapters_number = adapters;
  
  //перезагрузить базу
  WUSB.VIDs = VID;
  WUSB.PIDs = PID;
  WUSB.IDs = ID;
  WUSB.Manufacturers = Manufacturer;
  WUSB.Names = Name;
  WUSB.DevPatchs = DevPatch; 
  
  return adapters;    
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::OpenUSB(void)
{  
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  int result;
  std::string msg;

  result = WUSB.GetDeviceList();

  //=============================================================
  #ifdef USE_LIBUSB
  libusb_setlocale("en");
  //setlocale(LC_ALL, "Russian");

  result = libusb_init(&ctx);
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

  adapters_number = Find_Dongles();
  //::MessageBox(NULL, msg.c_str(),"Error:", MB_ICONERROR|MB_TASKMODAL);
  #endif
  //=============================================================


  dll_opened = 1;
  return 1;  
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
void CBTDONGLE::CloseUSB(void)
{
  //make sure xfers are not active
  ///if(rx_xfer != NULL) libusb_cancel_transfer(rx_xfer);
  
  usb_opened = 0;
  dongle_ready = 0;

 /// if(hUSB != NULL)
  {
    //флаг закрыть потоки
	Threads_Release();
	
	//Reset dongle to initial state
    ///libusb_reset_device(hUSB);
	///libusb_release_interface(hUSB, usb_interface);
  }
	
  ///libusb_close(hUSB);
  ///libusb_exit(NULL);
 // hUSB = NULL;
}
	  /*
    //HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\USB\Vid_0403&Pid_6001\FTYJB6P6
    CString RegPath, DeviceInstanceId, DeviceUniqueID;
    RegPath = "SYSTEM\\CurrentControlSet\\Enum\\USB\\";
    
    //---- Get Vid Pid string
    ///DeviceInstanceId = "Vid_0403&Pid_6001"; 
    DeviceInstanceId = msg;
    length = DeviceInstanceId.GetLength();
    pos = DeviceInstanceId.Find("Vid_", 0);
    DeviceInstanceId = DeviceInstanceId.Right(length-pos);
    length = DeviceInstanceId.GetLength();
    pos = DeviceInstanceId.Find("\\", 0);
    
	//---- Get serial number
    ///DeviceUniqueID = "FTYJB6P6";
    DeviceUniqueID = DeviceInstanceId.Right(length-(pos+1));
    
	//---- Complete Vid Pid
    DeviceInstanceId.Delete(pos, length-pos);
    
    //Assemble full patch to key
    RegPath += DeviceInstanceId;
    RegPath += "\\";
    RegPath += DeviceUniqueID;

    //RegPath = _T("SYSTEM\\CurrentControlSet\\Enum\\USB\\Vid_0403&Pid_6001\\FTYJB6P6\\");
    //LPCTSTR pth = _T("SYSTEM\\CurrentControlSet\\Enum\\USB\\Vid_0403&Pid_6001\\FTYJB6P6");
  
    CRegKey regkey;
    long status = regkey.Open(HKEY_LOCAL_MACHINE, RegPath, KEY_READ);
    if(status == ERROR_SUCCESS)
     { /*	
       char buffer[16];
       const char name[] = "LocationInformation";
       unsigned long bufSize = sizeof(buffer);
       memset(buffer, 0, bufSize);
       
       //Get adapter name
       status = regkey.QueryStringValue(name, buffer, &bufSize); 
       regkey.Close(); */  /*
	   status = ERROR_SUCCESS;
       if(status == ERROR_SUCCESS)
        {   
          //send info to usb class, if class exist
          if(pCAN!=NULL)
           { 
             //adapter IDs
             pCAN->USB.HotPlug_serial = DeviceUniqueID;
           } 

          if(EventType == DBT_DEVICEARRIVAL) 
           { 
             if(pCAN!=NULL)
                result = pCAN->USB.AdapterAttached();
           }
          if(EventType == DBT_DEVICEREMOVECOMPLETE)
           {  
             if(pCAN!=NULL)
                result = pCAN->USB.AdapterRemoved(); 
           }
        }
     }
  }
  */
//------------------------------------------------------------------------------
//Function
//https://coderoad.ru/11190241/%D0%9F%D1%80%D0%BE%D0%B2%D0%B5%D1%80%D1%8C%D1%82%D0%B5-%D1%83%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BB%D0%B5%D0%BD-%D0%BB%D0%B8-%D0%B4%D1%80%D0%B0%D0%B9%D0%B2%D0%B5%D1%80-%D0%B4%D0%BB%D1%8F-%D1%83%D1%81%D1%82%D1%80%D0%BE%D0%B9%D1%81%D1%82%D0%B2%D0%B0-my-USB
//https://coderoad.ru/28559450/%D0%9A%D0%B0%D0%BA-%D1%8F-%D0%BC%D0%BE%D0%B3%D1%83-%D1%83%D0%B7%D0%BD%D0%B0%D1%82%D1%8C-%D1%83%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BB%D0%B5%D0%BD-%D0%BB%D0%B8-%D0%B4%D1%80%D0%B0%D0%B9%D0%B2%D0%B5%D1%80-%D1%83%D1%81%D1%82%D1%80%D0%BE%D0%B9%D1%81%D1%82%D0%B2%D0%B0-USB-%D0%B7%D0%B0%D0%BF%D1%80%D0%BE%D1%81%D0%B8%D0%B2-%D1%80%D0%B5%D0%B5%D1%81%D1%82%D1%80
//этот
//https://coderoad.ru/27144063/%D0%9F%D1%80%D0%BE%D0%B2%D0%B5%D1%80%D1%8C%D1%82%D0%B5-%D1%81%D1%83%D1%89%D0%B5%D1%81%D1%82%D0%B2%D1%83%D0%B5%D1%82-%D0%BB%D0%B8-%D0%B4%D1%80%D0%B0%D0%B9%D0%B2%D0%B5%D1%80-Windows-%D0%B4%D0%BB%D1%8F-%D0%B4%D0%B0%D0%BD%D0%BD%D0%BE%D0%B3%D0%BE-%D1%83%D1%81%D1%82%D1%80%D0%BE%D0%B9%D1%81%D1%82%D0%B2%D0%B0-ID
//------------------------------------------------------------------------------
int CBTDONGLE::CheckDriver(int vid, int pid, long dev_num)
{
  CString vid_txt, pid_txt;
  vid_txt.Format("%0.4X", vid);
  pid_txt.Format("%0.4X", pid);

  //HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\USB\Vid_0403&Pid_6001\FTYJB6P6
  std::string RegPath, DeviceInstanceId, DeviceUniqueID;
  RegPath = "SYSTEM\\CurrentControlSet\\Enum\\USB\\";
  
  //0A5C&PID_21E8";
  RegPath += "VID_" +  vid_txt;         
  RegPath += "&PID_" + pid_txt;        
  
  RegPath += "\\";
  //RegPath += "5CF3706479CA";

  CRegKey regkey;
  long status = regkey.Open(HKEY_LOCAL_MACHINE, RegPath.c_str(), KEY_READ);
  if(status != ERROR_SUCCESS) return -1; 

  //Папка драйвера найдена, Читаем имя первой вложенной папки
  CString sResult;
  DWORD dwLength = MAX_PATH;
  status = regkey.EnumKey(0, sResult.GetBuffer(MAX_PATH), &dwLength);
  
  regkey.Close(); 
  
  if(status != ERROR_SUCCESS) return -1;

  //добавляем папку в путь
  RegPath += sResult;

  //открываем заново
  status = regkey.Open(HKEY_LOCAL_MACHINE, RegPath.c_str(), KEY_READ);
  if(status != ERROR_SUCCESS) return -1; 

  //----
  char buffer[16];
  unsigned long bufSize = sizeof(buffer);
  memset(buffer, 0, bufSize);
       
  //Get adapter driver name
  status = regkey.QueryStringValue("Service", buffer, &bufSize); 
  regkey.Close();
  
  //тут имя драйвера
  std::string txt(buffer); 
  std::transform(txt.begin(), txt.end(), txt.begin(), tolower);

  //проверка совпадения драйвера
  int result = (int)txt.find("winusb");
  if(result>=0){return 1;}

  //не установлен
  return 0;

  /*  
  //---- Get Vid Pid string
  ///DeviceInstanceId = "Vid_0403&Pid_6001"; 
  DeviceInstanceId = msg;
  length = DeviceInstanceId.GetLength();
  pos = DeviceInstanceId.Find("Vid_", 0);
  DeviceInstanceId = DeviceInstanceId.Right(length-pos);
  length = DeviceInstanceId.GetLength();
  pos = DeviceInstanceId.Find("\\", 0);
  */

  /* 
  if(hKey)
  {
    if(win32::registry::read(hKey, L"ProviderName", 
		                     desc.DriverProviderName, false) != ERROR_SUCCESS)
    return -1;

    desc.InstalledDriverRegFolder = regFolder;

    std::wstring val;
    if(win32::registry::read(hKey, L"DriverVersion", val, false) == ERROR_SUCCESS) desc.Version = val;
        rval = 1;
  }
     */
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::Dongle_Open(int index)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  int result;

  //Проверить наличие драйвера
  std::wstring driver;
  result = CheckDriver(WUSB.VIDs.at(index),
	                   WUSB.PIDs.at(index),
					   WUSB.IDs.at(index));
  //----
  if(result<1)
  {
  	//драйвер не установлен
  	::AfxMessageBox(_T("WinUSB driver is not installed!"));
  	return -1;
  }

  /*
  //----
  //result = libusb_open(usb_device_list[index], &hUSB);
  hUSB = libusb_open_device_with_vid_pid(ctx, AdaptersVID.at(index), //USB Vendor ID 
	                                     	  AdaptersPID.at(index));//USB Product ID
  if(hUSB<=0) return -1;
  */
  /* Отсоединить устройство от ядра ОС
  По умолчанию ядро операционной системы захватывает управление USB устройством.
  Перед взаимодействием с устройством его необходимо отсоединить от ядра.
  Отсоединение от ядра выполняется следующим образом:
  1 Проверка захвата USB устройства ядром ОС
  Проверка захвата выполняется при помощи функции:
  int libusb_kernel_driver_active(libusb_device_handle *dev_handle, int interface_number)
  dev_handle - структура доступа к USB устройству
  interface_number - номер отсоединяемого интерфейса USB устройства
  Функция вернет:
  0 - если устройство не захвачено ядром, 
  1 - если устройство захвачено ядром и код ошибки в ином случае.
  //2 Отсоединить устройство от ядра, если оно оказалось захваченным
  Отсоединение устройства от ядра осуществляется следующей функцией:
  int libusb_detach_kernel_driver(libusb_device_handle * dev_handle, int interface_number)
  dev_handle - структура доступа к отсоединяемому USB устройству
  interface_number - номер отсоединяемого интерфейса USB устройства
  Функция вернет 0 в случае успешного отсоединения, иначе - код ошибки */
  
  /*
  result = libusb_kernel_driver_active(hUSB, usb_interface); 
  if(result < 0)
  {
	Show_Errors(result);

	//detach if busy
	result = libusb_detach_kernel_driver(hUSB, usb_interface); 
	if(result < 0)
	 {
	   Show_Errors(result);
	   AdapterClose();
	   TRACE("Error open USB dongle\n");
	   return -1;
	 }
  }	*/
      /*              
  //Reset dongle to initial state
  result = libusb_reset_device(hUSB);
  if(result < 0)
  {
	Show_Errors(result);
	AdapterClose();
	return -1;
  }

  //wait while device hardware rebooted
  Sleep(1000);

  //===================================================================  
  //GET DESCRIPTOR`s
  //===================================================================
  result = libusb_get_descriptor(hUSB, LIBUSB_DT_DEVICE, 0, 
	                            (unsigned char*)&descr_dev, 
								 LIBUSB_DT_DEVICE_SIZE);

  if(result < 0)
   {
	 Show_Errors(result);
	 AdapterClose();
	 return -1;
   }

  result = libusb_get_descriptor(hUSB, LIBUSB_DT_CONFIG, 0, 
	                            (unsigned char*)&descr_conf,
								 LIBUSB_DT_CONFIG_SIZE);
  if(result < 0)
   {
	 Show_Errors(result);
	 AdapterClose();
	 return -1;
   }
  
  //===================================================================
  //SET CONFIGURATION
  //===================================================================
  //const int configuration = 1;
  result = libusb_set_configuration(hUSB, usb_configuration);//1 
  if(result < 0)
  {
	Show_Errors(result);
	AdapterClose();
	return -1;
  }

  //====================================================================
  //SET INTERFACE  reserve access to device
  //====================================================================
  result = libusb_claim_interface(hUSB, usb_interface); //0
  if(result < 0)
  {
	Show_Errors(result);
	AdapterClose();
	return -1;
  }
  */

  result = WUSB.OpenDevice(index);
  if(result<1) return -1; 

  #ifdef BLE_DEBUG
  TRACE1("%s OK\n", __FUNCTION__);
  #endif
  
  //----
  usb_opened = 1;
  adapter_vid = WUSB.VIDs.at(index);
  adapter_pid = WUSB.PIDs.at(index);

  return 1;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::Dongle_BLEIni(void)
{  
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
  
///  struct bt_hci_cmd_le_create_conn le_conn;
  struct bt_hci_cmd_le_set_adv_parameters le_param;
///  struct bt_hci_cmd_set_slave_broadcast cmd;

  int result, rx_accepted;
///  unsigned long addr; 
  unsigned char sspmode = 0x01;
///  long long mask;
  unsigned char buf[256];
///  unsigned char byte;  nafxcwd.lib;
///  char *pString;		 nafxcwd.lib;libcmt.lib;

  /////////////////////////////////////////////////////////////////////

  //HCI_Reset cmd:0x0C03 ret:0e 04 01 03 0c 00
  result = HCI_CMD(BT_HCI_CMD_RESET, NULL, 0, buf, -1, 500);
  if(result<0) return -1;
  Sleep(1000);
  
  //
  //
  //

  //HCI_Read_Buffer_Size,   HCI Opcode: 0x1005
  result = HCI_CMD(BT_HCI_CMD_READ_BUFFER_SIZE, NULL, 0, buf, -1, 100);
  if(result<0) return -1;
  Sleep(20);

  //HCI_LE_Read_Buffer_Size  HCI Opcode: 0x2002
  result = HCI_CMD(BT_HCI_CMD_LE_READ_BUFFER_SIZE, NULL, 0, buf, -1, 100);
  if(result<0) return -1;
  Sleep(20);
 
  //HCI Opcode: 0x1003  "Read_Local_Supported_Features"
  result = HCI_CMD(BT_HCI_CMD_READ_LOCAL_FEATURES, NULL, 0, buf, -1, 100);
  if(result<0) return -1;
  Sleep(20);

  //HCI_LE_Read_Local_Supported_Features, HCI Opcode: 0x2003
  result = HCI_CMD(BT_HCI_CMD_LE_READ_LOCAL_FEATURES, NULL, 0, buf, -1, 40);
  if(result<0) return -1;
  Sleep(20);
    
  //HCI_Read_BD_ADDR HCI Opcode: 0x1009
  memset(buf,0,sizeof(buf));
  result = HCI_CMD(BT_HCI_CMD_READ_BD_ADDR, NULL, 0, buf, -1, 40);
  if(result<0) return -1;
  //========= сохраняем полученный адрес хоста
  //0x 3f f5 7c 00 23 70 e4
  memcpy(HOST_BT_ADDR, &buf[6], 6); 
  TRACE("Host BT_ADDR: 0x%012I64x\n", HOST_BT_ADDR); 
  //=============================================  
  Sleep(20);

  //HCI_Host_Buffer_Size  HCI Opcode: 0x0C33
  buf[0] = 0x9B; buf[1] = 0x06; buf[2] = 0xFF; buf[3] = 0x14; 
  buf[4] = 0x00; buf[5] = 0x0A; buf[6] = 0x00;
  result = HCI_CMD(BT_HCI_CMD_HOST_BUFFER_SIZE, buf, 7, buf, -1, 40);
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
  result = HCI_CMD(BT_HCI_CMD_LE_SET_ADV_PARAMETERS, &le_param, sizeof(le_param), buf, -1, 40);
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
  struct Beacon_tx_struct beacon_tx;
  memset(&beacon_tx, 0, sizeof(beacon_tx));
  beacon_tx.cmd_length = 14; //not transmitted to device
  ///beacon_tx.cmd_header = CMD_BEACON;
  
  //ОТПРАВЛЯТЬ ВЕСЬ ТРЕБУЕМЫЙ ПАКЕТ = 32 октет!
  //int data_length;
  //if(usb_dev_vid==0x0A12) data_length = 32;
  //else data_length = sizeof(dat);
  
  //Персонально для CSR - ОТПРАВЛЯТЬ ВЕСЬ ТРЕБУЕМЫЙ ПАКЕТ = 32 октет!
  result = HCI_CMD(BT_HCI_CMD_LE_SET_ADV_DATA, &beacon_tx, 32, buf, -1, 40); 
  if(result<0) return -1;
  Sleep(20);
 /* 
  //HCI_LE_Set_Event_Mask
  SetEventMaskLE(mask);
  result = HCI_CMD(BT_HCI_CMD_LE_SET_EVENT_MASK, &mask, 8, buf, -1, 40);
  if(result<0) return -1;
  Sleep(20);
*/

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
  //выгрести из контроллера все сообщения из канала 
  //Read answer packet from dongle
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										   buf, 250, &rx_accepted, 1000);  */
  #ifdef BLE_DEBUG
  TRACE("Bluetooth dongle ready\n");
  #endif

  dongle_ready = 1;
  return 1;  
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
void CBTDONGLE::FifoCLR(void)
{ 
  unsigned char buf[256]; int rx_accepted;
  //выгрести из контроллера все сообщения из канала 
  //Read answer packet from dongle
  /*libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                              LIBUSB_ENDPOINT_IN,
								  buf, 256, &rx_accepted, 1000); */
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::WaitEvent(int event_id)
{  
  unsigned char buf[16]; 
  int rx_accepted;
  unsigned char id = (unsigned char) event_id;

  for(int i=0; i<10; i++)
  {
  
	memset(buf, 0, sizeof(buf));
    //выгрести из контроллера все сообщения из канала 
    //Read answer packet from dongle

	WUSB.ControlRead(WUSB.hUSB, (char*)buf, sizeof(buf), rx_accepted, 100);

	//----
	if(buf[0]==id)
	{
	   TRACE1(" > Device Event: 0x%02X OK\n", id);
	   print_buffer((char*)buf, 0, rx_accepted);
	   return 1;
	}
  }
	
  TRACE(" > Error: No Device Event 0x%02X\n", id);
  return -1;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::Read_Message(void) 
{
  return 0;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::Wait_Response(void) 
{ 
  return 1;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------ 
inline void CBTDONGLE::Error(void) 
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
void CBTDONGLE::Show_Errors(int error_code)
{	/*
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
			error_name = 0;/// libusb_strerror((libusb_error)error_code);
			break;
	}

  TRACE("LibUSB err: %d, %s\n", error_code, error_name);    */
  //fprintf(stderr,"libUSB Error: %s: %s (%d)\n", error_name, error_hint,);
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::CheckResponse(int tx_opcode, unsigned char *pBuf, int length)
{
  ///int result, answer_length;
  unsigned short opcode;
  int status;
  if(length<4) 
	 return -1; 

  status = pBuf[2];
  
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
		  char *error = hci_error_code_str[pBuf[5]];
          TRACE3("HCI CMD: 0x%04X, Opcode: 0x%04X, Err: %s\n", 
			      tx_opcode, opcode, error);
		  //----
		  print_buffer((char*)pBuf, 0, length);
		  return 1;
	   }  
	}
  
  //----------------------------------------------------------------------
  //HCI controller event replay, 7.7.15 Command Status Event
  //----------------------------------------------------------------------
  //--- Check Event Code
  if(pBuf[0]==0x0F)//HCI_command_Status event
   { /*
     length = Buf[1]; //parameter length
     opcode = Buf[5];
     opcode <<=8;
     opcode |= Buf[4];                      
	 */
	 status = pBuf[2];

	 //Read CMD opcode
     opcode = pBuf[5];
     opcode <<=8;
     opcode |= pBuf[4];
	 
	 //0x00 Command currently in pending.
     //0x01-0xFF Command failed. See Part ERROR CODES
	 CString err ="Undef satatus";
	 if(status==0x00) err = "Command currently in pending";
	 if(status==0x12) err = "Invalid HCI Command Parameters";

	 TRACE(" > Controller set Status code: 0x%02X to Command 0x%04X\n", status, opcode);
	 TRACE(" > %s", err);
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

      return 1; 
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
  //Известные HCI controller events
  //----------------------------------------------------------------------
  switch(pBuf[0])
  {
     case 0x03:
     case 0x04:
     case 0x05:
     case 0x3E://BLE all events
     return  1;
     
     default:
     break;
  }

  //----------------------------------------------------------------------
  //HCI other controller events
  //----------------------------------------------------------------------
  TRACE1("HCI CMD: 0x%04X", tx_opcode);
  TRACE1(" > Unknown Controller Event: 0x%02X\n\n", pBuf[0]);

  /*
  //Exit OK for unknown rx Length 
  if(result==0 && any_accept_length==1) 
	 return rx_accepted;

  //Exit OK
  if(result==0 && rx_accepted>0) 
	 return rx_accepted;
  */
  //Exit error
  return  1;
}
/*
  //длина данных в переданном пакете  msg.c_str()
  answer_length = pBuf[1];
  if(answer_length<4 &&pBuf[0]!=0xFF) //Vendor answ
  { 
	 TRACE("CheckResponse: paket length Error, Opcode: 0x=%04X!\n\n", tx_opcode);
	 ::MessageBox(NULL, "CheckResponse paket length Error","Error:", 
		          MB_ICONERROR|MB_TASKMODAL);   
	 return -1;
  }*/
//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
/* Исходящие конечные точки имеют адреса 0x00-0x7f, входящие 0x80-0xff.
Помимо этого в Transfer Type можно обнаружить метод передачи конечной точки.
По указаной информации необходимо определить через какую конечную точку вести 
передачу данных и какой у нее адрес. Для каждого вида передачи USB в библиотеке 
существует соответствующая функция, используемая как для передачи данных, 
так и для приёма. Решение о передаче или приёме данных принимается исходя из 
адреса конечной точки, через которую будет выполняться информационный
обмен. 
Если адрес конечной точки исходящий (0x00-0x7f), то будет выполняться передача,
в ином случае - прием. Функции для передачи/приёма данных через USB:

int libusb_interrupt_transfer(struct libusb_device_handle *dev_handle,
unsigned char endpoint,
unsigned char *data,
int length,
int *transferred,
unsigned int timeout)

int libusb_bulk_transfer(struct libusb_device_handle *dev_handle,
unsigned char endpoint,
unsigned char *data,
int length,
int *transferred,
unsigned int timeout)

где endpoint - адрес конечной точки, используемой для передачи/приема данных
data - адрес данных. В случае приема - адрес для записи принимаемых данных,
в случае передачи - адрес отправляемых данных.
length - В случае приема - размер приемного буфера (максимальное количество
данных, которое можно принять указаный в data буфер), в случае передачи -
количество байт, которые нужно отправить.
transferred - ссылка на целочисленную переменную, в которую будет записано
количество переданных/принятых байт.
timeout - максимальное время ожидания выполнения транзакции в миллисекундах.

Если за это время транзакция не выполнена, функция вернет код ошибки по таймауту.
Функции возввращают 0 в случае успешного приёма/передачи, иначе - код ошибки.

Для Control приема/передачи.
Так как формат передачи типа Control строго регламентирован, то для него
используется собственная функция приёма/передачи данных:

int libusb_control_transfer(libusb_device_handle *dev_handle,
uint8_t bmRequestType,
uint8_t bRequest,
uint16_t wValue,
uint16_t wIndex,
unsigned char *data,
uint16_t wLength,
unsigned int timeout)
где поля соответствуют формату стандартных запросов USB.
data - адрес данных. В случае приема - адрес для записи принимаемых данных,
в случае передачи - адрес отправляемых данных.
timeout - максимальное время ожидания выполнения транзакции в миллисекундах.
Если за это время транзакция не выполнена, функция вернет код ошибки по таймауту.
Функции возввращают 0 в случае успешного приёма/передачи, иначе - код ошибки.
*/
/*
http://microsin.net/programming/arm-working-with-usb/usb-in-a-nutshell-part1.html
https://webhamster.ru/mytetrashare/index/mtb186/1518688323fmuismqn9n
https://www.cyberforum.ru/kip-sensors/thread957379-page2.html
https://radiohlam.ru/usb3/
-------------------------------------------------------
BCM20702 Interface 2
Contains Bulk In and Bulk Out endpoints 
(Endpoints 0x84 and 0x04) used for proprietary testing and 
debugging purposes. These endpoints can be ignored during normal operation.
-------------------------------------------------------
*/
// HID Class-Specific Requests values. See section 7.2 of the HID specifications
#define HID_GET_REPORT                0x01
#define HID_GET_IDLE                  0x02
#define HID_GET_PROTOCOL              0x03
#define HID_SET_REPORT                0x09
#define HID_SET_IDLE                  0x0A
#define HID_SET_PROTOCOL              0x0B
#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03

//------------------------------------------------------------------------------
//Function: Write value by handle
//------------------------------------------------------------------------------
int CBTDONGLE::DataTx(char handle, char *pBuf, int length)
{
  return 1;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------   
int CBTDONGLE::Disconnect2(void)
{ 
  //HCI_Reset cmd:0x0C03 ret:0e 04 01 03 0c 00
  HCI_CMD(BT_HCI_CMD_RESET, NULL, 0, NULL, 0, 0);
  
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
//Default Value: 0x00 00 1F FF FF FF FF FF 
//------------------------------------------------------------------------------
void CBTDONGLE::SetEventMask(long long &mask)
{ 
  mask = 0;
  
  //---- 1
  mask |= 0x0000000000000001; //Inquiry Complete Event
  mask |= 0x0000000000000002; //Inquiry Result Event
  mask |= 0x0000000000000004; //Connection Complete Event
  mask |= 0x0000000000000008; //Connection Request Event
  mask |= 0x0000000000000010; //Disconnection Complete Event
  mask |= 0x0000000000000020; //Authentication Complete Event
  mask |= 0x0000000000000040; //Remote Name Request Complete Event
  mask |= 0x0000000000000080; //Encryption Change Event
  
  //---- 2
  mask |= 0x0000000000000100; //Change Connection Link Key Complete Event
  mask |= 0x0000000000000200; //Master Link Key Complete Event
  mask |= 0x0000000000000400; //Read Remote Supported Features Complete Event
  mask |= 0x0000000000000800; //Read Remote Version Information Complete Event
  mask |= 0x0000000000001000; //QoS Setup Complete Event
  mask |= 0x0000000000002000; //Reserved = 1
  mask |= 0x0000000000004000; //Reserved = 1
  mask |= 0x0000000000008000; //Hardware Error Event
  
  //---- 3
  mask |= 0x0000000000010000; //Flush Occurred Event
  mask |= 0x0000000000020000; //Role Change Event
  mask |= 0x0000000000040000; //Reserved = 1
  mask |= 0x0000000000080000; //Mode Change Event
  mask |= 0x0000000000100000; //Return Link Keys Event
  mask |= 0x0000000000200000; //PIN Code Request Event
  mask |= 0x0000000000400000; //Link Key Request Event
  mask |= 0x0000000000800000; //Link Key Notification Event
  
  //---- 4
  mask |= 0x0000000001000000; //Loopback Command Event
  mask |= 0x0000000002000000; //Data Buffer Overflow Event
  mask |= 0x0000000004000000; //Max Slots Change Event
  mask |= 0x0000000008000000; //Read Clock Offset Complete Event
  mask |= 0x0000000010000000; //Connection Packet Type Changed Event
  mask |= 0x0000000020000000; //QoS Violation Event
  mask |= 0x0000000040000000; //Page Scan Mode Change Event [deprecated]
  mask |= 0x0000000080000000; //Page Scan Repetition Mode Change Event
  
  //---- 5
  mask |= 0x0000000100000000; //Flow Specification Complete Event
  mask |= 0x0000000200000000; //Inquiry Result with RSSI Event
  mask |= 0x0000000400000000; //Read Remote Extended Features Complete Event
  mask |= 0x0000000800000000; //Reserved = 1
  mask |= 0x0000001000000000; //Reserved = 1
  mask |= 0x0000002000000000; //Reserved = 1
  mask |= 0x0000004000000000; //Reserved = 1
  mask |= 0x0000008000000000; //Reserved = 1
  
  //---- 6
  mask |= 0x0000010000000000; //Reserved = 1
  mask |= 0x0000020000000000; //Reserved = 1
  mask |= 0x0000040000000000; //Reserved = 1
  mask |= 0x0000080000000000; //Synchronous Connection Complete Event
  mask |= 0x0000100000000000; //Synchronous Connection Changed Event
  mask |= 0x0000200000000000; //Sniff Subrating Event
  mask |= 0x0000400000000000; //Extended Inquiry Result Event
  mask |= 0x0000800000000000; //Encryption Key Refresh Complete Event
  
  //---- 7
  mask |= 0x0001000000000000; //IO Capability Request Event
  mask |= 0x0002000000000000; //IO Capability Request Reply Event
  mask |= 0x0004000000000000; //User Confirmation Request Event
  mask |= 0x0008000000000000; //User Passkey Request Event
  mask |= 0x0010000000000000; //Remote OOB Data Request Event
  mask |= 0x0020000000000000; //Simple Pairing Complete Event
  mask |= 0x0040000000000000; //Reserved = 1
  mask |= 0x0080000000000000; //Link Supervision Timeout Changed Event
  
  //---- 8
  mask |= 0x0100000000000000; //Enhanced Flush Complete Event
  //mask |= 0x0200000000000000; //Reserved = 1
  mask |= 0x0400000000000000; //User Passkey Notification Event
  mask |= 0x0800000000000000; //Keypress Notification Event
  //mask |= 0x1000000000000000; //Remote Host Supported Features Notification Event
  //mask |= 0x2000000000000000; //LE Meta-Event
  //mask |= 0xC000000000000000; //Reserved for future use
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------     
void CBTDONGLE::SetEventMaskPage2(long long &mask)
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
void CBTDONGLE::SetEventMaskLE(long long &mask)
{
 mask = 0; //No LE events specified
 mask |= 0x0000000000000001; //LE Connection Complete Event
 mask |= 0x0000000000000002; //LE Advertising Report Event
 //mask |= 0x0000000000000004; //LE Connection Update Complete Event
 //mask |= 0x0000000000000008; //LE Read Remote Used Features Complete Event
 //mask |= 0x0000000000000010; //LE Long Term Key Request Event
 //mask |= 0x000000000000001F; //Default
}
  
//------------------------------------------------------------------------------
//Function: Bluetooth L2CAP Protocol parser
//------------------------------------------------------------------------------
void CBTDONGLE::L2CAP_Decoder(int acl_length)
{
  int l2cap_length, data_length, CID;
  l2cap_length = L2CAP_buf[1];
  l2cap_length <<= 8;
  l2cap_length |= L2CAP_buf[0];
    
  //----
  CID = L2CAP_buf[3];
  CID <<= 8;
  CID |= L2CAP_buf[2];
  
  //----
  unsigned char opcode = L2CAP_buf[4];

  switch(CID)
  {
  	//CID: Attribute Protocol (0x0004)
    case 0x0004: 
	//Opcode: Write Response (0x13)
	if(opcode==0x13){/*TRACE("Tx OK\n");*/}
	
	//Opcode: Handle Value Notification (0x1b)
	if(opcode==0x1b)
	{
	  //вероятно длина блока данных
	  unsigned short handle = L2CAP_buf[6]; 
	  handle <<= 8;
	  handle |=	L2CAP_buf[5];
	  
      //----
      data_length = acl_length - 7;

	  //передать данные приложению
	  if(data_length>0)
	  { 
		TRACE("RX < "); print_buffer((char*)L2CAP_buf, 0, acl_length);
		if(pRXhandler!=NULL)
		{
		  pRXhandler((char*)&L2CAP_buf[7], data_length); 
	    }
	  }
	}

	break;

	default:
	TRACE("ACL RX < "); print_buffer((char*)L2CAP_buf, 0, acl_length);  
	break;
  }

  ev_DATA.PulseEvent();
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------  
void CBTDONGLE::L2CAP_Encoder(unsigned char *pBuf, int length)
{


}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CBTDONGLE::BT_AddrToTxt1(CString *pTxt, long long Addr) 
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
void CBTDONGLE::BT_AddrToStr1(char *pAddr, long long Addr) 
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